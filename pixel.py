import cv2

img = cv2.imread('frog.png')
img1 = cv2.resize(img,(17*2,16*2))
cv2.imwrite("frogsmall.png",img1)
print(img1)
def f((r,g,b)):
    if r==255 and g==255 and b==255:
        return hex(0x0f)
    elif r==0 and g==0 and b==0:
        return hex(0x00)
    else:
        return hex(0x02)
lattice = map(f,img1.reshape(-1,3))
print(",".join(lattice))
print(len(lattice))

