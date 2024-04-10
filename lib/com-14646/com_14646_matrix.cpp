#include "com_14646_matrix.h"
#include "gpio.h"
#include "tim2.h"

#define DISPLAY_FRAME_BUFFER_LENGTH DISPLAY_WIDTH*DISPLAY_HEIGHT

void displayInit(void) {
		disable_output();
	init_gpio();
	init_tim2();
}

// Yes we are strapped for memory atm because we want to store as much as we can in the frame buffer
// I'm not taking the time to refactor my refresh display code to handle a different frame buffer format that's more compressed tho
static bool display_frame_buffer [DISPLAY_FRAME_BUFFER_LENGTH * 3];

void bufferFrame(uint8_t frame[128]) {
	for (int i = 0; i < 128; i++) {
		uint8_t byte = frame[i];

		for (int j = 0; j < 8; j++) {
			int baseIndex = (i*8+j)*3;

			if ((byte & (0x1 << j)) == 0x0) {
				display_frame_buffer[baseIndex] = false;
				display_frame_buffer[baseIndex+1] = false;
				display_frame_buffer[baseIndex+2] = false;
			} else {
				display_frame_buffer[baseIndex] = true;
				display_frame_buffer[baseIndex+1] = true;
				display_frame_buffer[baseIndex+2] = true;
			}
		}
	}
}

void bufferPixel(struct pixel p, int x, int y) {
	int index = y*DISPLAY_HEIGHT + (DISPLAY_WIDTH - (x+1));

	display_frame_buffer[index*3] = p.r;
	display_frame_buffer[index*3+1] = p.g;
	display_frame_buffer[index*3+2] = p.b;
	
	//display_frame_buffer[1] = true;
}

void clearBuffer() {
	for (int i = 0; i < DISPLAY_FRAME_BUFFER_LENGTH*3; i++) {
		display_frame_buffer[i] = false;
	}
}

void refreshDisplay() {
	int mult = 0;
	bool* currBottomRow = (bool*)&display_frame_buffer;
	bool* currTopRow = currBottomRow + 16*3*32;
	struct rgb_data data;
	multiplex(0);
	flip_latch();
	
	for (mult = 0; mult < 16; mult++) {
		
		int count = 0;
	
		
		disable_output();
		
		multiplex(mult);
	
		
		for (int i = 0; i < 32*3; i += 3) {
			data.r1 = currBottomRow[i];
			data.g1 = currBottomRow[i+1];
			data.b1 = currBottomRow[i+2];
			
			data.r2 = currTopRow[i];
			data.g2 = currTopRow[i+1];
			data.b2 = currTopRow[i+2];
			
			output_rgb_data(data);
			flip_clock();
		}
		enable_output();
		
		
		flip_latch();
		
		start_tim2();
		
		while (!isTim2Updated()) {}
		
		stop_tim2();
		
		//for (int x = 0; x < 1000; x++) {}
		
		
		currBottomRow += 32*3;
		currTopRow += 32*3;
		
		
		
	}
	
	disable_output();
		
}
