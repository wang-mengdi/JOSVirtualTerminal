// Simple command-line kernel monitor useful for
// controlling the kernel and exploring the system interactively.

#include <inc/stdio.h>
#include <inc/string.h>
#include <inc/memlayout.h>
#include <inc/assert.h>
#include <inc/x86.h>

#include <kern/console.h>
#include <kern/monitor.h>
#include <kern/kdebug.h>
#include <kern/trap.h>

#define CMDBUF_SIZE	80	// enough for one VGA text line


struct Command {
	const char *name;
	const char *desc;
	// return -1 to force monitor to exit
	int (*func)(int argc, char** argv, struct Trapframe* tf);
};

int mon_showmappings(int argc, char **argv, struct Trapframe *tf);
int mon_showvmrange(int argc, char **argv, struct Trapframe *tf);
int mon_setperm(int argc, char **argv, struct Trapframe *tf);

static struct Command commands[] = {
	{ "help", "Display this list of commands", mon_help },
	{ "kerninfo", "Display information about the kernel", mon_kerninfo },
    { "backtrace", "Trace function calls", mon_backtrace},
    { "showmappings", "Show mapping information", mon_showmappings},
    { "showvmrange", "Show a range of virtual memory", mon_showvmrange},
    { "setperm", "Set permission of a page", mon_setperm}
};

/***** Implementations of basic kernel monitor commands *****/

int
mon_help(int argc, char **argv, struct Trapframe *tf)
{
	int i;

	for (i = 0; i < ARRAY_SIZE(commands); i++)
		cprintf("%s - %s\n", commands[i].name, commands[i].desc);
	return 0;
}

int
mon_kerninfo(int argc, char **argv, struct Trapframe *tf)
{
	extern char _start[], entry[], etext[], edata[], end[];

	cprintf("Special kernel symbols:\n");
	cprintf("  _start                  %08x (phys)\n", _start);
	cprintf("  entry  %08x (virt)  %08x (phys)\n", entry, entry - KERNBASE);
	cprintf("  etext  %08x (virt)  %08x (phys)\n", etext, etext - KERNBASE);
	cprintf("  edata  %08x (virt)  %08x (phys)\n", edata, edata - KERNBASE);
	cprintf("  end    %08x (virt)  %08x (phys)\n", end, end - KERNBASE);
	cprintf("Kernel executable memory footprint: %dKB\n",
		ROUNDUP(end - entry, 1024) / 1024);
	return 0;
}

extern pte_t *pgdir_walk(pde_t *pgdir, const void *va, int create);
extern pde_t *kern_pgdir;

int mon_showmappings(int argc, char **argv, struct Trapframe *tf){

    if(argc!=3){
        cprintf("Usage: showmappings 0xbegin_addr 0xend_addr\n");
        return 0;
    }
    uint32_t begin=strtol(argv[1], NULL, 16),
             end=strtol(argv[2], NULL, 16);
    if(!(begin<end)){
        cprintf("show mappings: begin<end not hold\n");
        return 0;
    }
    if(begin%PGSIZE||end%PGSIZE){
        cprintf("show mappings: not aligned\n");
        return 0;
    }
    cprintf("begin: %x, end: %x\n",begin,end);
    for(uint32_t now=begin;now<end;now+=PGSIZE){
        cprintf("page %08x~%08x: ",now,now+PGSIZE);
        pte_t *pte=pgdir_walk(kern_pgdir,(void*)now,0);
        if(!pte){
            cprintf("show mappings: page not found\n");
            return 0;
        }
        cprintf("physical page %08x ",PTE_ADDR(*pte));
        uint32_t p=(*pte)&PTE_P;
        uint32_t w=(*pte)&PTE_W;
        uint32_t u=(*pte)&PTE_U;
        cprintf("PTE_P=%x, PTE_W=%x, PTE_U=%x\n",p,w,u);
    }
    return 0;
}

int mon_showvmrange(int argc, char **argv, struct Trapframe *tf){
    if(argc!=3){
        cprintf("Usage: showvmrange 0xaddr 0xdwords\n");
        return 0;
    }
    void** addr=(void*)strtol(argv[1],NULL,16);
    uint32_t nbytes=strtol(argv[2],NULL,16);
    for(uint32_t i=0;i<nbytes;i++){
        cprintf("%x: %x\n",addr+i,addr[i]);
    }
    return 0;
}

int mon_setperm(int argc, char **argv, struct Trapframe *tf){
    if(argc!=4){
        cprintf("Usage: setprem 0xaddr [P|W|U] [0|1]\n");
        return 0;
    }
    uint32_t addr=strtol(argv[1],NULL,16);
    pte_t *pte=pgdir_walk(kern_pgdir,(void*)addr,0);
    if(pte==NULL){
        cprintf("setprem: page not found");
        return 0;
    }
    uint32_t mask=0;
    char bt=argv[2][0];
    if(bt=='P') mask=PTE_P;
    else if(bt=='W') mask=PTE_W;
    else if(bt=='U') mask=PTE_U;
    char st=argv[3][0];
    if(st=='0'){
        *pte=*pte&~mask;
    }
    else{
        *pte=*pte|mask;
    }
    return 0;
}

int
mon_backtrace(int argc, char **argv, struct Trapframe *tf)
{
	// Your code here.
    cprintf("Stack backtrace:\n");
    uint32_t *ebp;
    ebp = (uint32_t*) read_ebp();
    while(ebp!=NULL){
        uint32_t *eip=(uint32_t*)ebp[1];
        cprintf("  ebp %08x  eip %08x  args ",ebp,eip);
        for(int i=0;i<5;i++){
            cprintf("%08x",ebp[i+2]);
            if(i<4) cprintf(" ");
            else cprintf("\n");
        }
        ebp=(uint32_t*)ebp[0];
        struct Eipdebuginfo info;
        debuginfo_eip((uint32_t)eip,&info);
        cprintf("           %s:%d: ",info.eip_file, info.eip_line);
        for(int i=0;i<info.eip_fn_namelen;i++){
            cprintf("%c",info.eip_fn_name[i]);
        }
        cprintf("+");
        cprintf("%d\n",eip-info.eip_fn_addr);
    }
	return 0;
}



/***** Kernel monitor command interpreter *****/

#define WHITESPACE "\t\r\n "
#define MAXARGS 16

static int
runcmd(char *buf, struct Trapframe *tf)
{
	int argc;
	char *argv[MAXARGS];
	int i;

	// Parse the command buffer into whitespace-separated arguments
	argc = 0;
	argv[argc] = 0;
	while (1) {
		// gobble whitespace
		while (*buf && strchr(WHITESPACE, *buf))
			*buf++ = 0;
		if (*buf == 0)
			break;

		// save and scan past next arg
		if (argc == MAXARGS-1) {
			cprintf("Too many arguments (max %d)\n", MAXARGS);
			return 0;
		}
		argv[argc++] = buf;
		while (*buf && !strchr(WHITESPACE, *buf))
			buf++;
	}
	argv[argc] = 0;

	// Lookup and invoke the command
	if (argc == 0)
		return 0;
	for (i = 0; i < ARRAY_SIZE(commands); i++) {
		if (strcmp(argv[0], commands[i].name) == 0)
			return commands[i].func(argc, argv, tf);
	}
	cprintf("Unknown command '%s'\n", argv[0]);
	return 0;
}

void
monitor(struct Trapframe *tf)
{
	char *buf;

	cprintf("Welcome to the JOS kernel monitor!\n");
	cprintf("Type 'help' for a list of commands.\n");

	if (tf != NULL)
		print_trapframe(tf);

	while (1) {
		buf = readline("K> ");
		if (buf != NULL)
			if (runcmd(buf, tf) < 0)
				break;
	}
}
