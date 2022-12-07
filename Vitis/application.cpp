//#define _DEBUG
#include "chu_init.h"
#include "gpio_cores.h"
#include "vga_core.h"
#include "sseg_core.h"
#include "ps2_core.h"
#include "xadc_core.h"

int spectrumColor(XadcCore *adc_p) { //get 9bit color from adc input
	int red, green, blue;

	double in = adc_p->read_adc_in(0); //read adc
	int value = (int)(84*in); //scale 0->1 to 0->84

	if(value < 14){ //first segment adc mode : 0 < value < 14
		red = 7; 	 			//red = 3'b111
		green = value/2;  		//green = adc (ramp up)
		blue = 0;  				//blue = 3'b000
	}
	else if (value < 28) { //second segment adc mode : 14 < value < 28
		red = (-value+28)/2; 	 	//red = -adc (ramp down)
		green = 7; 				//green = 3'b111
		blue = 0;  				//blue = 3'b000
	}
	else if (value < 42) { //third  segment adc mode : 28 < value < 42
	red = 0;	 				//red = 3'b000
		green = 7; 				//green = 3'b111
		blue = (value - 28)/2;	//blue = adc (ramp up)
	}
	else if (value < 56) { //fourth segment adc mode : 42 < value < 56
		red = 0;	 			//red = 3'b000
		green = (-value+56)/2; 	//green = -adc (ramp down)
		blue = 7;  				//blue = 3'b111
	}
	else if (value < 70) { //fifth segment adc mode : 56 < value < 70
		red = (value-56)/2;	 	//red = adc (ramp up)
		green = 0; 				//green = 3'b000
		blue = 7;  				//blue = 3'b111
	}
	else if (value < 84) { //sixth segment adc mode : 70 < value < 84
		red = 7;	 			//red = 3'b111
		green = 0; 				//green = 3'b000
		blue = (-value+84)/2;  	//blue = -adc (ramp down)
	}
	else{ //default : value < 0 | adc > 1
		red = 0;   				//all leds off
		green = 0; 				//
		blue = 0;  				//
	} //if(adc)...elseif...else

	return (red << 6) | (green << 3) | blue; //pack values into 9 bits and return
} //spectrumColor


void drawPixel(FrameCore *frame_p, int x, int y, int color, int size) { //draw a square at (x,y) with specified color and size
	int xp, yp;
	for(int i = 0; i < size; i++) { //x-axis
		for(int j = 0; j < size; j++) { //y-axis
			xp = x + i;
			yp = y + j;
			frame_p->wr_pix(xp, yp, color);
		} //for(j)
	} //for(i)
} //drawPixel

void initializeGraphics(OsdCore *osd_p, FrameCore *frame_p) { //Draw graphics and text to screen
	osd_p->bypass(0);	//enable osd and frame cores
	frame_p->bypass(0); //

	frame_p->clr_screen(0x001); //make background black (dark-grey)
	osd_p->set_color(0x0f0, 0x000); //set text color to green w/o background

	//draw white box for control text
	for (int i = 0; i < 640; i++) {//x-axis : 0->640
		for(int j = 412; j < 480; j++) {//y-axis : 412->480
			frame_p->wr_pix(i, j, 0xfff);
		} //for(j)
	} //for(i)

	int x = 10; //initial position for move controls
	int y = 27; //

	osd_p->wr_char(x-1, y, 30, 0); 		//up
	osd_p->wr_char(x-1, y+1, 31, 0); 	//down
	osd_p->wr_char(x, y+1, 16, 0); 		//left
	osd_p->wr_char(x-2, y+1, 17, 0); 	//right

	osd_p->wr_char(x+3, y+1, 84, 0); 	//T
	osd_p->wr_char(x+4, y+1, 79, 0); 	//O
	osd_p->wr_char(x+5, y+1, 0, 0); 	//space
	osd_p->wr_char(x+6, y+1, 77, 0); 	//M
	osd_p->wr_char(x+7, y+1, 79, 0); 	//O
	osd_p->wr_char(x+8, y+1, 86, 0); 	//V
	osd_p->wr_char(x+9, y+1, 69, 0); 	//E


	x += 14; //initial position for ADC=COLOR
	osd_p->wr_char(x, y+1, 65, 0);		//A
	osd_p->wr_char(x+1, y+1, 68, 0);	//D
	osd_p->wr_char(x+2, y+1, 67, 0);	//C
	osd_p->wr_char(x+3, y+1, 61, 0);	//=
	osd_p->wr_char(x+4, y+1, 67, 0);	//C
	osd_p->wr_char(x+5, y+1, 79, 0);	//O
	osd_p->wr_char(x+6, y+1, 76, 0);	//L
	osd_p->wr_char(x+7, y+1, 79, 0);	//O
	osd_p->wr_char(x+8, y+1, 82, 0);	//R

	x += 13; //initial position for SW0=ERASER
	osd_p->wr_char(x, y+1, 83, 0);		//S
	osd_p->wr_char(x+1, y+1, 87, 0);	//W
	osd_p->wr_char(x+2, y+1, 48, 0);	//0
	osd_p->wr_char(x+3, y+1, 61, 0);	//=
	osd_p->wr_char(x+4, y+1, 69, 0);	//E
	osd_p->wr_char(x+5, y+1, 82, 0);	//R
	osd_p->wr_char(x+6, y+1, 65, 0);	//A
	osd_p->wr_char(x+7, y+1, 83, 0);	//S
	osd_p->wr_char(x+8, y+1, 69, 0);	//E
	osd_p->wr_char(x+9, y+1, 82, 0);	//R

	x += 14; //initial position for R=RESET
	osd_p->wr_char(x, y+1, 82, 0);		//R
	osd_p->wr_char(x+1, y+1, 61, 0);	//=
	osd_p->wr_char(x+2, y+1, 82, 0);	//R
	osd_p->wr_char(x+3, y+1, 69, 0);	//E
	osd_p->wr_char(x+4, y+1, 83, 0);	//S
	osd_p->wr_char(x+5, y+1, 69, 0);	//E
	osd_p->wr_char(x+6, y+1, 84, 0);	//T

	x += 11; //initial position for SW15=SIZE
	osd_p->wr_char(x, y+1, 83, 0);		//S
	osd_p->wr_char(x+1, y+1, 87, 0);	//W
	osd_p->wr_char(x+2, y+1, 49, 0);	//1
	osd_p->wr_char(x+3, y+1, 53, 0);	//5
	osd_p->wr_char(x+4, y+1, 61, 0);	//=
	osd_p->wr_char(x+5, y+1, 83, 0);	//S
	osd_p->wr_char(x+6, y+1, 73, 0);	//I
	osd_p->wr_char(x+7, y+1, 90, 0);	//Z
	osd_p->wr_char(x+8, y+1, 69, 0);	//E
} //initializeGraphics

int getKBInput(Ps2Core *ps2_p) {
	int cmd; //0 - do nothing; 1 - up; 2 - down; 3 - left; 4 - right; 5 - reset
	char input = 0x00;		  //get keyboard input
	ps2_p->get_kb_ch(&input); //

	switch((int)input) {
		case 50: 	 //up
			cmd = 1; //
			break;
		case 56: 	 //down
			cmd = 2; //
			break;
		case 52: 	 //left
			cmd = 3; //
			break;
		case 54: 	 //right
			cmd = 4; //
			break;
		case 114: 	 //r -> reset
			cmd = 5; //
			break;
		default: cmd = 0; //0 or >5 -> do nothing
	} //switch(input)
	return cmd;
} //getKBInput

int spectrum12bit(int spectrum9bit){ //convert 9 bit color to 12 bit color
	int R, RABC, RA, G, GABC, GA, B, BABC, BA;

	RABC = (spectrum9bit >> 6) & 7; //get 3 bit red value
	RA = (RABC >> 2) & 1; //get msb from red value

	GABC = (spectrum9bit >> 3) & 7; //get 3 bit green value
	GA = (GABC >> 2) & 1; //get msb from green value

	BABC = spectrum9bit & 7; //get 3 bit blue value
	BA = (BABC >> 2) & 1; //get msb from blue value

	R = (RABC << 1) | RA; //form 4 bit red value
	G = (GABC << 1) | GA; //form 4 bit green value
	B = (BABC << 1) | BA; //form 4 bit blue value

	return (R << 8) | (G << 4) | B; //form 12 bit color value and return
}

void etch_a_sketch(SpriteCore *square_p, Ps2Core *ps2_p, XadcCore *adc_p,
		OsdCore *osd_p, FrameCore *frame_p, GpiCore *sw_p) {
	int x, y; //stores cursor position
	int cmd; //stores user input
	int color9bit, color12bit; //stores color values
	int sw0, size; //stores user inputs for cursor

	initializeGraphics(osd_p, frame_p); //Draw graphics and text to screen

	square_p->bypass(0); //enable cursor
	square_p->wr_ctrl(0x1B); //initiate cursor

	x = 320; 				//set cursor to middle of screen
	y = 200; 				//
	square_p->move_xy(x, y); //

	while (1) {
		color9bit = spectrumColor(adc_p);	   //read color values from adc
		color12bit = spectrum12bit(color9bit); //

		cmd = getKBInput(ps2_p); //get user input from ps2 keyboard

		if(cmd == 5) break; //if user inputs reset, break out of infinite while loop

		sw0 = sw_p->read(0); //select cursor : 0 - brush, 1 - eraser
		size = sw_p->read(15); //select size : 0 - 8px, 1 - 16px

		if(sw0) square_p->wr_ctrl(3 << 3 | 2); //display eraser frame
		else square_p->wr_ctrl(3 << 3 | (1 + 2*size) ); //display brush frame of appropriate size

		switch (cmd) { //cursor movements
		   case 1: 		  //up
			   y = y + 4; //
			   break;
		   case 2: 	 	  //down
			   y = y - 4; //
			   break;
		   case 3:		  //left
			   x = x - 4; //
			   break;
		   case 4:		  //right
			   x = x + 4; //
			   break;
		   default:; //do nothing
		}//switch(cmd)

		y = y % 400; //window size
		x = x % 640; //

		square_p->move_xy(x, y); //move cursor

		if(x < 0) x = 636; //moves the cursor to the right of the window if it goes off the left
		if(y < 0) y = 396; //moved the cursor to the top of the window if it goes off the bottom

		osd_p->set_color(spectrum12bit(color9bit), 0x000); //set text color
		square_p->wr_color(color12bit); //set cursor color

		if(sw0) drawPixel(frame_p, x+4, y+4, 0x001, 8); //erase function
		else {
			if(size) drawPixel(frame_p, x-1, y, color9bit, 16); //draw size small
			else drawPixel(frame_p, x+4, y+4, color9bit, 8); //draw size large
		}//if(sw0)...else
   }//while
}//etch_a_sketch

// external core instantiation
Ps2Core ps2(get_slot_addr(BRIDGE_BASE, S11_PS2));
GpoCore led(get_slot_addr(BRIDGE_BASE, S2_LED));
GpiCore sw(get_slot_addr(BRIDGE_BASE, S3_SW));
FrameCore frame(FRAME_BASE);
GpvCore bar(get_sprite_addr(BRIDGE_BASE, V7_BAR));
GpvCore gray(get_sprite_addr(BRIDGE_BASE, V6_GRAY));
SpriteCore square(get_sprite_addr(BRIDGE_BASE, V5_SQUARE), 1024);
SpriteCore mouse(get_sprite_addr(BRIDGE_BASE, V1_MOUSE), 1024);
SpriteCore ghost(get_sprite_addr(BRIDGE_BASE, V3_GHOST), 1024);
OsdCore osd(get_sprite_addr(BRIDGE_BASE, V2_OSD));
SsegCore sseg(get_slot_addr(BRIDGE_BASE, S8_SSEG));
XadcCore adc(get_slot_addr(BRIDGE_BASE, S5_XDAC));

int main() {
   while (1) {
      // bypass all cores
      frame.bypass(1);
      bar.bypass(1);
      gray.bypass(1);
      square.bypass(1);
      osd.bypass(1);
      mouse.bypass(1);
      ghost.bypass(1);

	  etch_a_sketch(&square, &ps2, &adc, &osd, &frame, &sw); //functionality

   } // while
} //main
