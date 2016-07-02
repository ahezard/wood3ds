#include <nds/arm7/input.h>
#include <nds/arm7/touch.h>
#include <nds/system.h>
#include <nds/touch.h>
#include <nds/fifocommon.h>
#include <nds/ipc.h>
#include <nds/ndstypes.h>

#define PACK_INPUT_DATA(px,py,keys) ((px&0xff)|((py&0xff)<<8)|((keys&0xff)<<16))

enum{
	KEY_TOUCH = (1<<6)
}Keys;

void inputGetAndSend(void){

	static bool penDown = false;

	touchPosition tempPos = {0};

	u16 keys= REG_KEYXY;


	if(!touchPenDown()) {
		keys |= KEY_TOUCH;
  	} else {
		keys &= ~KEY_TOUCH;
	}

	if(keys & KEY_TOUCH) {
		penDown = false;	
	} else {
		keys |= KEY_TOUCH;

		if(penDown) {
			touchReadXY(&tempPos);	
			
			if(tempPos.rawx && tempPos.rawy) {
				keys &= ~KEY_TOUCH;
			} else {
				penDown = false;
			}
		} else {
			penDown = true;
		}
	}	

	fifoSendValue32(FIFO_INPUT,PACK_INPUT_DATA(tempPos.px,tempPos.py,keys));
}
