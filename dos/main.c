#include <stdio.h>
#include <i86.h>
#include <stdlib.h>
#include <conio.h>
#include <GL/osmesa.h>
#include <GL/gl.h>
#include <string.h>
#include <dos.h>

OSMesaContext ctx;

#define WIDTH 320
#define HEIGHT 200

int rgb(int r, int g, int b){
	int color = 0;

	color += r * 7 / 255;
	color = color << 3;

	color += g * 7 / 255;
	color = color << 3;

	color += b * 3 / 255;

	return color;
}

void(__interrupt *old)();

int quit = 0;

void __interrupt key(){
	unsigned char sc = inp(0x60);
	if(sc == 1) quit = 1;
	outp(0x20, 0x20);
}

/* Where you draw stuff */
double deg = 0;
void draw(void){
	glClear(GL_COLOR_BUFFER_BIT);

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glOrtho(-WIDTH / 2, WIDTH / 2, -HEIGHT / 2, HEIGHT / 2, -1, 1);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();

	glRotatef(deg++, 0, 0, 1);
	glBegin(GL_TRIANGLES);
	glColor3f(1, 0, 0);
		glVertex2f(0, -50);
	glColor3f(0, 1, 0);
		glVertex2f(-50, 50);
	glColor3f(0, 0, 1);
		glVertex2f(50, 50);
	glEnd();

	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
}

int main(){
	unsigned char* buf;
	int i;
	int mode;
	union REGS regs;

	printf("OSMesa DOS Demo\n");

	old = _dos_getvect(0x09);
	_dos_setvect(0x09, key);

	buf = malloc(WIDTH * HEIGHT);

	regs.h.ah = 0x0f;
	int386(0x10, &regs, &regs);
	mode = regs.h.al;

	regs.h.ah = 0;
	regs.h.al = 0x13;
	int386(0x10, &regs, &regs);

	/* do stuff */
	for(i = 0; i < 256; i++){
		/* RRRGGGBB */
		int r = (double)((i >> 5) & 7) / 7 * 255;
		int g = (double)((i >> 2) & 7) / 7 * 255;
		int b = (double)((i >> 0) & 3) / 3 * 255;
		
		regs.h.ah = 0x10;
		regs.h.al = 0x10;

		regs.w.bx = i;
		regs.h.ch = (double)g / 255 * 0x3f;
		regs.h.cl = (double)b / 255 * 0x3f;
		regs.h.dh = (double)r / 255 * 0x3f;
		int386(0x10, &regs, &regs);
	}

	ctx = OSMesaCreateContext(OSMESA_RGB_332, NULL);
	OSMesaMakeCurrent(ctx, (unsigned char*)buf, GL_UNSIGNED_BYTE, WIDTH, HEIGHT);
	while(!quit){
		draw();
		memcpy((unsigned char*)0xa0000, buf, WIDTH * HEIGHT);
	}
	OSMesaDestroyContext(ctx);

	regs.h.ah = 0;
	regs.h.al = mode;
	int386(0x10, &regs, &regs);

	_dos_setvect(0x09, old);

	free(buf);

	return 0;
}
