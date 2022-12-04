/*****************************************************************//**
 * @file main_video_test.cpp
 *
 * @brief Basic test of 4 basic i/o cores
 *
 * @author p chu
 * @version v1.0: initial release
 *********************************************************************/

//#define _DEBUG
#include "chu_init.h"
#include "gpio_cores.h"
#include "vga_core.h"
#include "sseg_core.h"
#include "ps2_core.h"
#include "xadc_core.h"

void frame_draw(int x, int y, FrameCore *frame_p, int color, int size) {
	int xp, yp;
	for(int i = 0; i < size; i++) {
		for(int j = 0; j < size; j++) {
			xp = x + i;
			yp = y + j;
			frame_p->wr_pix(xp, yp, color);
		}
	}
}

void osd_init(OsdCore *osd_p, FrameCore *frame_p) {
	osd_p->bypass(0);
	frame_p->bypass(0);

	frame_p->clr_screen(0x001);
	osd_p->set_color(0x0f0, 0xfff);

	for (int i = 0; i < 640; i++) {
		for(int j = 400+12; j < 480; j++) {
			frame_p->wr_pix(i, j, 0xfff);
		}
	}

	int x = 10;
	int y = 27;

	osd_p->wr_char(x-1, y, 30, 0); //up
	osd_p->wr_char(x-1, y+1, 31, 0); //down
	osd_p->wr_char(x, y+1, 16, 0); //left
	osd_p->wr_char(x-2, y+1, 17, 0); //right

	osd_p->wr_char(x+3, y+1, 84, 0);
	osd_p->wr_char(x+4, y+1, 79, 0);
	osd_p->wr_char(x+5, y+1, 0, 0);
	osd_p->wr_char(x+6, y+1, 77, 0);
	osd_p->wr_char(x+7, y+1, 79, 0);
	osd_p->wr_char(x+8, y+1, 86, 0);
	osd_p->wr_char(x+9, y+1, 69, 0);


	x += 14;
	osd_p->wr_char(x, y+1, 65, 0);
	osd_p->wr_char(x+1, y+1, 68, 0);
	osd_p->wr_char(x+2, y+1, 67, 0);
	osd_p->wr_char(x+3, y+1, 61, 0);
	osd_p->wr_char(x+4, y+1, 67, 0);
	osd_p->wr_char(x+5, y+1, 79, 0);
	osd_p->wr_char(x+6, y+1, 76, 0);
	osd_p->wr_char(x+7, y+1, 79, 0);
	osd_p->wr_char(x+8, y+1, 82, 0);

	x += 13;
	osd_p->wr_char(x, y+1, 83, 0);
	osd_p->wr_char(x+1, y+1, 87, 0);
	osd_p->wr_char(x+2, y+1, 48, 0);
	osd_p->wr_char(x+3, y+1, 61, 0);
	osd_p->wr_char(x+4, y+1, 69, 0);
	osd_p->wr_char(x+5, y+1, 82, 0);
	osd_p->wr_char(x+6, y+1, 65, 0);
	osd_p->wr_char(x+7, y+1, 83, 0);
	osd_p->wr_char(x+8, y+1, 69, 0);
	osd_p->wr_char(x+9, y+1, 82, 0);

	x += 14;
	osd_p->wr_char(x, y+1, 82, 0);
	osd_p->wr_char(x+1, y+1, 61, 0);
	osd_p->wr_char(x+2, y+1, 82, 0);
	osd_p->wr_char(x+3, y+1, 69, 0);
	osd_p->wr_char(x+4, y+1, 83, 0);
	osd_p->wr_char(x+5, y+1, 69, 0);
	osd_p->wr_char(x+6, y+1, 84, 0);

	x += 11;
	osd_p->wr_char(x, y+1, 83, 0);
	osd_p->wr_char(x+1, y+1, 87, 0);
	osd_p->wr_char(x+2, y+1, 49, 0);
	osd_p->wr_char(x+3, y+1, 53, 0);
	osd_p->wr_char(x+4, y+1, 61, 0);
	osd_p->wr_char(x+5, y+1, 83, 0);
	osd_p->wr_char(x+6, y+1, 73, 0);
	osd_p->wr_char(x+7, y+1, 90, 0);
	osd_p->wr_char(x+8, y+1, 69, 0);

}

int ps2_check(Ps2Core *ps2_p) {
	int dir;
	char input = 0x00;
	ps2_p->get_kb_ch(&input);


	switch((int)input) {
		case 50:
			dir = 1;
			break;
		case 56:
			dir = 2;
			break;
		case 52:
			dir = 3;
			break;
		case 54:
			dir = 4;
			break;
		case 114:
			dir = 5;
			break;
		default: dir = 0;
	}

	//uart.disp((int)dir);
	//uart.disp("\n\r");
	return dir;
}

int read_adc(XadcCore *adc_p) {
	double current_adc = adc_p->read_adc_in(0);

	if(current_adc <= 0.25) return 0;
	else if(current_adc <= 0.50) return 1;
	else if(current_adc <= 0.75) return 2;
	else return 3;

}
void ghost_check(SpriteCore *ghost_p, Ps2Core *ps2_p, XadcCore *adc_p,
		OsdCore *osd_p, FrameCore *frame_p, GpiCore *sw_p) {
  int x, y;
  int dir;
  int color2;

  osd_init(osd_p, frame_p);
   // slowly move mouse pointer
   ghost_p->bypass(0);
   ghost_p->wr_ctrl(0x1B);  //animation; blue ghost
   x = 320;
   y = 200;
   ghost_p->move_xy(x, y); //initial

   while (1) {

		dir = ps2_check(ps2_p);
		if(dir == 5) break;

		int color = read_adc(adc_p);
		int sw0 = sw_p->read(0);
		int size = sw_p->read(15);

		if(sw0) ghost_p->wr_ctrl(color << 3 | 2);
		else ghost_p->wr_ctrl(color << 3 | (1 + 2*size) );


		switch (dir) {
		   case 1:
			   y = y + 4;
			   break;
		   case 2:
			   y = y - 4;
			   break;
		   case 3:
			   x = x - 4;
			   break;
		   case 4:
			   x = x + 4;
			   break;
		   default:;
		}

		y = y % 400;
		x = x % 640;
		ghost_p->move_xy(x, y);

		if(x < 0) x = 636;
		if(y < 0) y = 396;

		switch(color) {
			case 0:
				color2 = 0xF00;
				break;
			case 1:
				color2 = 0xF8B;
				break;
			case 2:
				color2 = 0xFA0;
				break;
			default: color2 = 0x0FF;
		}

		if(sw0) frame_draw(x+4, y+4, frame_p, 0x001, 8);
		else {
			if(size) frame_draw(x-1, y, frame_p, color2, 16);
			else frame_draw(x+4, y+4, frame_p, color2, 8);
		}

   }

}

// external core instantiation
Ps2Core ps2(get_slot_addr(BRIDGE_BASE, S11_PS2));
GpoCore led(get_slot_addr(BRIDGE_BASE, S2_LED));
GpiCore sw(get_slot_addr(BRIDGE_BASE, S3_SW));
FrameCore frame(FRAME_BASE);
GpvCore bar(get_sprite_addr(BRIDGE_BASE, V7_BAR));
GpvCore gray(get_sprite_addr(BRIDGE_BASE, V6_GRAY));
SpriteCore ghost(get_sprite_addr(BRIDGE_BASE, V3_GHOST), 1024);
SpriteCore mouse(get_sprite_addr(BRIDGE_BASE, V1_MOUSE), 1024);
OsdCore osd(get_sprite_addr(BRIDGE_BASE, V2_OSD));
SsegCore sseg(get_slot_addr(BRIDGE_BASE, S8_SSEG));
XadcCore adc(get_slot_addr(BRIDGE_BASE, S5_XDAC));

int main() {
   while (1) {
      //test_start(&led);
      // bypass all cores
      frame.bypass(1);
      bar.bypass(1);
      gray.bypass(1);
      ghost.bypass(1);
      osd.bypass(1);
      mouse.bypass(1);


	  ghost_check(&ghost, &ps2, &adc, &osd, &frame, &sw);

   } // while
} //main
