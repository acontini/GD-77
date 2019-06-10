/*
 * Copyright (C)2019 Roger Clark. VK3KYY / G4KYF
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *   1. Redistributions of source code must retain the above copyright notice,
 *      this list of conditions and the following disclaimer.
 *   2. Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in the
 *      documentation and/or other materials provided with the distribution.
 *   3. The name of the author may not be used to endorse or promote products
 *      derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
 * EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#include "menu/menuSystem.h"
#include "UC1701.h"
#include "fw_settings.h"

static void updateScreen();
static void handleEvent(int buttons, int keys, int events);

static uint8_t originalBrightness;
static uint8_t contrast;
static uint8_t inverseVideo;
static int8_t 	backLightTimeout;// used int_8 to save space

const int BACKLIGHT_MAX_TIMEOUT = 30;
const int CONTRAST_MAX_VALUE = 30;// Maximum value which still seems to be readable
const int CONTRAST_MIN_VALUE = 12;// Minimum value which still seems to be readable

int menuDisplayOptions(int buttons, int keys, int events, bool isFirstRun)
{
	if (isFirstRun)
	{
		gMenusCurrentItemIndex=0;
		originalBrightness = nonVolatileSettings.displayBacklightPercentage;
		contrast = nonVolatileSettings.displayContrast;
		inverseVideo = nonVolatileSettings.displayInverseVideo;
		backLightTimeout = 	nonVolatileSettings.backLightTimeout;
		updateScreen();
	}
	else
	{
		if (events!=0 && keys!=0)
		{
			handleEvent(buttons, keys, events);
		}
	}
	return 0;
}

static void updateScreen()
{
	char buf[20];
	UC1701_clearBuf();
	UC1701_printCentered(0, "Display options",UC1701_FONT_GD77_8x16);

	if (gMenusCurrentItemIndex==0)
	{
		UC1701_fillRect(0,16,128,16,false);
	}
	sprintf(buf,"Brightness %d%%",nonVolatileSettings.displayBacklightPercentage);
	UC1701_printCore(0,16,buf,UC1701_FONT_GD77_8x16,0,(gMenusCurrentItemIndex==0));

	if (gMenusCurrentItemIndex==1)
	{
		UC1701_fillRect(0,32,128,16,false);
	}
	sprintf(buf,"Contrast %d",contrast);
	UC1701_printCore(0,32,buf,UC1701_FONT_GD77_8x16,0,(gMenusCurrentItemIndex==1));

	if (gMenusCurrentItemIndex==2)
	{
		UC1701_fillRect(0,48,128,16,false);
	}
	sprintf(buf,"Timeout %d",backLightTimeout);
	UC1701_printCore(0,48,buf,UC1701_FONT_GD77_8x16,0,(gMenusCurrentItemIndex==2));

	UC1701_render();
	displayLightTrigger();
}

static void handleEvent(int buttons, int keys, int events)
{
	if ((keys & KEY_DOWN)!=0 && gMenusEndIndex!=0)
	{
		gMenusCurrentItemIndex++;
		if (gMenusCurrentItemIndex > 2)
		{
			gMenusCurrentItemIndex = 0;
		}
	}
	else if ((keys & KEY_UP)!=0)
	{
		gMenusCurrentItemIndex--;
		if (gMenusCurrentItemIndex < 0)
		{
			gMenusCurrentItemIndex=2;
		}
	}
	else if ((keys & KEY_RIGHT)!=0)
	{
		switch(gMenusCurrentItemIndex)
		{
			case 0:
				if (nonVolatileSettings.displayBacklightPercentage<10)
				{
					nonVolatileSettings.displayBacklightPercentage += 1;
				}
				else
				{
					nonVolatileSettings.displayBacklightPercentage += 10;
				}

				if (nonVolatileSettings.displayBacklightPercentage>100)
				{
					nonVolatileSettings.displayBacklightPercentage=100;
				}
				break;
			case 1:
				contrast++;
				if (contrast > CONTRAST_MAX_VALUE)
				{
					contrast = CONTRAST_MAX_VALUE;
				}
				UC1701_setContrast(contrast);
				break;
			case 2:
				backLightTimeout += 5;
				if (backLightTimeout > BACKLIGHT_MAX_TIMEOUT)
				{
					backLightTimeout=0;
				}
				break;
		}
	}
	else if ((keys & KEY_LEFT)!=0)
	{
		switch(gMenusCurrentItemIndex)
		{
			case 0:
				if (nonVolatileSettings.displayBacklightPercentage <= 10)
				{
					nonVolatileSettings.displayBacklightPercentage -= 1;
				}
				else
				{
					nonVolatileSettings.displayBacklightPercentage -= 10;
				}

				if (nonVolatileSettings.displayBacklightPercentage<0)
				{
					nonVolatileSettings.displayBacklightPercentage=0;
				}
				break;
			case 1:
				contrast--;
				if (contrast < CONTRAST_MIN_VALUE)
				{
					contrast = CONTRAST_MIN_VALUE;
				}
				UC1701_setContrast(contrast);
				break;
			case 2:
				backLightTimeout -= 5;
				if (backLightTimeout < 0)
				{
					backLightTimeout = BACKLIGHT_MAX_TIMEOUT;
				}
				break;
		}
	}
	else if ((keys & KEY_GREEN)!=0)
	{
		nonVolatileSettings.displayInverseVideo = inverseVideo;
		nonVolatileSettings.displayContrast = contrast;
		nonVolatileSettings.backLightTimeout = backLightTimeout;
		menuSystemPopAllAndDisplayRootMenu();
	}
	else if ((keys & KEY_RED)!=0)
	{
		if (nonVolatileSettings.displayContrast != contrast || nonVolatileSettings.displayInverseVideo != inverseVideo)
		{
			UC1701_begin(nonVolatileSettings.displayInverseVideo);
		}
		nonVolatileSettings.displayBacklightPercentage = originalBrightness;

		menuSystemPopPreviousMenu();
		return;
	}
	updateScreen();
}
