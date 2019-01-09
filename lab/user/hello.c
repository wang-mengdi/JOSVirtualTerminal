// hello, world
#include <inc/lib.h>

void
umain(int argc, char **argv)
{
	cprintf("hello, world\n");
	cprintf("i am environment %08x\n", thisenv->env_id);
}

/*void umain(int argc,char **argv){
    for(int i=1;i<=5;i++){
        if(fork_with_priority(i)==0){
			cprintf("child %x is now living!\n", i);
			for (int j = 0; j < 5; ++j) {
				cprintf("child %x is yielding!\n", i);
				sys_yield();
			}
			break;
        }
    }
}*/
