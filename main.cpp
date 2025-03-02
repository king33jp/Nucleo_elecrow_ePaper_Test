/**
 * 
 */
#include "mbed.h"
#include "ssd1680.h"
#include "epd_images.h"

//SSD1680( sck, mosi, miso, res, dc, cs, lcd33, busy);
// PA_5 : sck
// PA_7 : mosi
// PA_6 : miso (NC)
// PB_6 : res
// PB_9 : dc
// PB_8 : cs
// PA_8 : lcd33 (NC)
// PB_10: busy
SSD1680 epd( PA_5 , PA_7 , PA_6 , PB_6 , PB_9 , PB_8 , PA_8 , PB_10 );
//DigitalOut led(LED1);

extern uint8_t ImageBW[ALLSCREEN_BYTES];

int main()
{
	//led=0;
	epd.initialize();
	epd.clear(0,0,296,128,WHITE);
	epd.all_fill(WHITE);
	epd.update();

	epd.clear_R26H();

	epd.showPicture(0,0,296,128,gImage_kamakiri2_gray,BLACK);
	epd.displayimage(ImageBW);
	epd.fastupdate();

	epd.sleep();
	thread_sleep_for(5000);
	
	while(true) 
	{
		printf("Hello world from Mbed CE!\n");
		ThisThread::sleep_for(1s);
		//led = !led;
	}

	// main() is expected to loop forever.
	// If main() actually returns the processor will halt
	return 0;
}