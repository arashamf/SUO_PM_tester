#include "st7735.h"
#include "timer.h"
#include "pins.h"
#include "get_image.h"
#include "motor.h"
#include "menu.h"
#include "jam_sensors.h"
#include "protocol.h"
#include "softI2C.h"

// check if the banknote path is opened or closed
void CheckPathOpened(void)
{
	static int pathOpenState=0;
	
	if((PATH_OPEN==0)&&(pathOpenState==0))	{	// if the banknote path is opened
		ClearLcdMemory();
		LCD_SetFont(Arial_15x17,black);	
		LCD_ShowString(40,53,"path is open"); 	
		SetClutzColor(CLUTZ_BLACK); 
		LCD_Refresh();
		pathOpenState=1;
	}
	if((PATH_OPEN==1)&&(pathOpenState==1))	{	// path was opened but now it's closed	
		getImgState=ERR_CHAIN;
		CleanPath();
		LcdShowIntro();
		LCD_Refresh();
		pathOpenState=0;	
		SetClutzColor(CLUTZ_GREEN);			
	}
}


void CheckPathState(int distance, int clutzTimeout)
{
	CheckPathOpened();
	if(PATH_OPEN) { currentAstraState &=~ASTRA_STATE_PATH_OPEN; }	
	else { currentAstraState |=ASTRA_STATE_PATH_OPEN;	}

	if((distance > 0)&&(distance<MINIMUM_CASH_BAG_DISTANCE)&&(isDistanceSensorPresent==1)) { currentAstraState |=ASTRA_STATE_BAG_FULL; }	
	else { currentAstraState &=~ASTRA_STATE_BAG_FULL; }
	
	if(EXIT_JAM_SENSOR){ currentAstraState |=ASTRA_STATE_EXIT_JAM; }
	else { currentAstraState &=~ASTRA_STATE_EXIT_JAM; }
	
	if(currentAstraState&ASTRA_STATE_PATH_OPEN) { 
		SetClutzColor(CLUTZ_BLACK); 
		return;
	}
	
	if(currentAstraState==ASTRA_STATE_READY) {
		if(clutzTimeout==0) {
			SetClutzColor(CLUTZ_GREEN);
		}
		else {
			SetClutzColor(CLUTZ_RED);
		}
	}
	else {
		SetClutzColor(CLUTZ_PURPLE);
	}
}

