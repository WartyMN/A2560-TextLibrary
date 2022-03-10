/*
 * test5.c
 *
 *  Created on: Feb 19, 2022
 *      Author: micahbly
 */


/*****************************************************************************/
/*                                Includes                                   */
/*****************************************************************************/


// project includes


// C includes
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


// A2560 includes
#include <mcp/syscalls.h>
#include "a2560_platform.h"
#include "lib_general.h"
#include "lib_text.h"


/*****************************************************************************/
/*                               Definitions                                 */
/*****************************************************************************/

#define RUN_TESTS	1	// define "RUN_TESTS" to include ability to run tests. If undefined, this class will not be aware of tests.

#if defined(RUN_TESTS)
	#include "test_base.h"
#endif


/*****************************************************************************/
/*                             Global Variables                              */
/*****************************************************************************/

unsigned long ditherData = 0x5555AAAA;

Screen	global_screen[2];
unsigned short	global_num_screens;
unsigned short	global_main_screen;
unsigned short	global_alt_screen;


/*****************************************************************************/
/*                       Private Function Prototypes                         */
/*****************************************************************************/

// test using sys_kbd_scancode() instead of a channel driver
boolean keyboard_test_2(void);

boolean keyboard_test(void);

// simple function for testing passing a function hook for "do something to see another page of text" for the drawstringinbox stuff. 
boolean Test_MyGetUserResponseFunc(void);


/*****************************************************************************/
/*                       Private Function Definitions                        */
/*****************************************************************************/


boolean keyboard_test_2(void)
{
	// LOGIC: 
	//   page 34 of FoenixMCP Manual
	//   try a simple loop that just keeps banging on the keyboard scanner
	
	//   keys return their keycode when pushed, and their keyboard + 128 when released.
	
	unsigned short	the_code;
	
	do
	{
		the_code = sys_kbd_scancode();
		
		if (the_code > 127)
		{
			DEBUG_OUT(("%s %d: key released: code=%u, keycode=%u", __func__, __LINE__, the_code, the_code & 0xF0));
			// handle_event_key_up()
			
		}
		else
		{
			DEBUG_OUT(("%s %d: key pressed: code=%u", __func__, __LINE__, the_code));
		}
		
	} while (the_code != 0);
	
	return true;
}


boolean keyboard_test(void)
{
	signed short			bytes_read = 0;
	signed short			bytes_requested = 5;
	static unsigned char	keyboard_buff[256];
	unsigned char*			the_keyboard_buff = keyboard_buff;
	signed short			the_channel_id;
	signed short			the_device_id = 0;
	boolean					stop = false;
	signed int				y = 30;
	
// 	Text_DrawStringAtXY(ID_CHANNEL_B, 0, y++, (char*)"Trying to open keyboard device...", FG_COLOR_DK_BLUE, BG_COLOR_YELLOW);
	DEBUG_OUT(("%s %d: Trying to open keyboard device...", __func__, __LINE__));
	
	// open keyboard console for reading. Console is on device 0 and 1. 
	the_channel_id = sys_chan_open(the_device_id, (unsigned char*)"", 1);
	
	if (the_channel_id < 0)
	{
		//DEBUG_OUT(("%s %d: Failed to open channel for device %i. Error# %i", __func__, __LINE__, the_device_id, the_channel_id));
		//Text_DrawStringAtXY(ID_CHANNEL_A, 0, y++, (char*)"Failed to open keyboard device", FG_COLOR_DK_BLUE, BG_COLOR_YELLOW);
		DEBUG_OUT(("%s %d: Failed to open keyboard device. proceeding anyway...", __func__, __LINE__));
// 		return false;
	}
	else
	{
		//Text_DrawStringAtXY(ID_CHANNEL_A, 0, y++, (char*)"Opened keyboard device", FG_COLOR_DK_BLUE, BG_COLOR_YELLOW);
		DEBUG_OUT(("%s %d: Opened keyboard device", __func__, __LINE__));
	}
	
	sys_chan_flush(the_channel_id);
	
	//Text_DrawStringAtXY(ID_CHANNEL_A, 0, y++, (char*)"flushed channel", FG_COLOR_DK_BLUE, BG_COLOR_YELLOW);
	DEBUG_OUT(("%s %d: Flushed channel", __func__, __LINE__));

	if ( ((sys_chan_status(the_channel_id) & CDEV_STAT_ERROR) == 1) )
	{
		//Text_DrawStringAtXY(ID_CHANNEL_A, 0, y++, (char*)"channel status had error (0x01)", FG_COLOR_DK_BLUE, BG_COLOR_YELLOW);
		DEBUG_OUT(("%s %d: channel status had error (0x01)", __func__, __LINE__));
		return false;
	}
	else
	{
		//Text_DrawStringAtXY(ID_CHANNEL_A, 0, y++, (char*)"channel status says no error condition", FG_COLOR_DK_BLUE, BG_COLOR_YELLOW);
		DEBUG_OUT(("%s %d: channel status says no error condition", __func__, __LINE__));
	}
	
	// read and type characters to screen until there is an channel error, or the char typed is tab
	while ( ((sys_chan_status(the_channel_id) & CDEV_STAT_ERROR) == 0) && !stop)
	{
		unsigned char	the_char;
		
		//bytes_read = sys_chan_read(the_channel_id, the_keyboard_buff, bytes_requested);
		the_char = sys_chan_read_b(the_channel_id);
		bytes_read++;
		
		if (the_char == '\t')
		{
			stop = true;
		}
		else
		{
			Text_SetCharAtXY(ID_CHANNEL_A, bytes_read, 40, the_char);
		}
		
		//Text_DrawStringAtXY(ID_CHANNEL_A, 0, 40, (char*)the_keyboard_buff, FG_COLOR_DK_BLUE, BG_COLOR_YELLOW);
	}

	// close channel
	sys_chan_close(the_channel_id);
		
	return true;
}



// simple function for testing passing a function hook for "do something to see another page of text" for the drawstringinbox stuff. 
//! @return	returns true if the user wants to continue, or false if the user wants to stop the current action.
boolean Test_MyGetUserResponseFunc(void)
{
	unsigned char	c;
	
	c = getchar();
	
	if (c == 'q')
	{
		return false;
	}
	
	return true;
}

void play_GraphicsModes(Screen* the_screen);

void play_GraphicsModes(Screen* the_screen)
{
	unsigned long*	abs_vram_addr;
	unsigned char*	vram_addr;
	unsigned long	my_vram_addr;
	unsigned long*	vram_addr_control;
	int				x;
	
	DEBUG_OUT(("%s %d: chanB ctr reg value=%#x", __func__, __LINE__, *the_screen->vicky_));
	*the_screen->vicky_ = (*the_screen->vicky_ & GRAPHICS_MODE_MASK | (GRAPHICS_MODE_GRAPHICS) | GRAPHICS_MODE_EN_BITMAP);
	DEBUG_OUT(("%s %d: chanB ctr reg value=%#x", __func__, __LINE__, *the_screen->vicky_));
	DEBUG_OUT(("%s %d: chanB VRAM addr value=%#x", __func__, __LINE__, R32(BITMAP_VRAM_ADDR_A2560_0)));
	vram_addr_control = (unsigned long*)BITMAP_VRAM_ADDR_A2560_0;
	DEBUG_OUT(("%s %d: chanB VRAM addr value=%#x", __func__, __LINE__, *vram_addr_control));
	my_vram_addr = (unsigned long)(*vram_addr_control);
	abs_vram_addr = (unsigned long*)R32(BITMAP_VRAM_ADDR_A2560_0 + VRAM_BUFFER_A);
	vram_addr = (char*)abs_vram_addr;
	*vram_addr_control = ++my_vram_addr;
	
// 	for (x=0; x < (640*3000); x = x + 2)
// 	{
// 		*vram_addr = 0xaa;
// 	}
}

// #define GRAPHICS_MODE_MASK		0xFFFFFF00	//!> for all VICKYs, the mask for the system control register that holds the graphics/bitmap/text/sprite mode bits
// #define GRAPHICS_MODE_TEXT		0x01	// 0b00000001	Enable the Text Mode
// #define GRAPHICS_MODE_TEXT_OVER	0x02	// 0b00000010	Enable the Overlay of the text mode on top of Graphic Mode (the Background Color is ignored)
// #define GRAPHICS_MODE_GRAPHICS	0x04	// 0b00000100	Enable the Graphic Mode
// #define GRAPHICS_MODE_EN_BITMAP	0x08	// 0b00001000	Enable the Bitmap Module In Vicky
// #define GRAPHICS_MODE_EN_TILE	0x10	// 0b00010000	Enable the Tile Module in Vicky
// #define GRAPHICS_MODE_EN_SPRITE	0x20	// 0b00100000	Enable the Sprite Module in Vicky
// #define GRAPHICS_MODE_EN_GAMMA	0x40	// 0b01000000	Enable the GAMMA correction - The Analog and DVI have different color values, the GAMMA is great to correct the difference
// #define GRAPHICS_MODE_DIS_VIDEO	0x80	// 0b10000000	This will disable the Scanning of the Video information in the 4Meg of VideoRAM hence giving 100% bandwidth to the CPU
// 
// #define default_start_a2560k_vram	0x00011000	// offset against vicky I think though. add to VICKY_A2560K_B? based on doing peek32 in f68. 
// #define VRAM_BUFFER_A			0x0080000
// #define VRAM_BUFFER_B			0x00C0000

// #define default_start_a2560k_vram	0x00011000	// offset against vicky I think though. add to VICKY_A2560K_B? based on doing peek32 in f68. 
// #define VRAM_BUFFER_A				0x0080000
// #define VRAM_BUFFER_B				0x00C0000
// #define BITMAP_CTRL_REG_A2560_0		VICKY_A2560K_B + 0x0100	//! Bitmap Layer0 Control Register (Foreground Layer)
// #define BITMAP_VRAM_ADDR_A2560_0	VICKY_A2560K_B + 0x0104	//! Bitmap Layer0 VRAM Address Pointer. Offset within the VRAM memory from VICKY’s perspective. VRAM Address begins @ $00:0000 and ends @ $1FFFFF
// #define BITMAP_CTRL_REG_A2560_1		VICKY_A2560K_B + 0x0108	//! Bitmap Layer1 Control Register (Background Layer)
// #define BITMAP_VRAM_ADDR_A2560_1	VICKY_A2560K_B + 0x010C	//! Bitmap Layer0 VRAM Address Pointer. Offset within the VRAM memory from VICKY’s perspective. VRAM Address begins @ $00:0000 and ends @ $1FFFFF

/*****************************************************************************/
/*                        Public Function Definitions                        */
/*****************************************************************************/



int main(int argc, char* argv[])
{
	
	// find out what kind of machine the software is running on, and configure global screens accordingly
	global_screen[ID_CHANNEL_A].id_ = ID_CHANNEL_A;
	global_screen[ID_CHANNEL_B].id_ = ID_CHANNEL_B;

	if (Text_AutoConfigureScreen(&global_screen[ID_CHANNEL_A]) == false)
	{
		DEBUG_OUT(("%s %d: Auto configure failed for screen A", __func__, __LINE__));
		exit(0);
	}
	
	if (Text_AutoConfigureScreen(&global_screen[ID_CHANNEL_B]) == false)
	{
		DEBUG_OUT(("%s %d: Auto configure failed for screen B", __func__, __LINE__));
		exit(0);
	}
	
	//play_GraphicsModes(&global_screen[ID_CHANNEL_B]);
	
	//General_LogInitialize();

	//keyboard_test();
	   
	#if defined(RUN_TESTS)
		Text_RunTests();
	#endif
	
}