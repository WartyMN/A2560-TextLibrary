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

void TestResolution(void);

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


void TestResolution(void)
{
// #define VIDEO_MODE_BYTE			0x01	//!> for all VICKYs, the byte offset from system control register that holds the video mode bits
// #define VIDEO_MODE_BIT1			0x00	//!> for all VICKYs, the bits in the 2nd byte of the system control register that define video mode (resolution)
// #define VIDEO_MODE_BIT2			0x01	//!> for all VICKYs, the bits in the 2nd byte of the system control register that define video mode (resolution)
// 
// // VICKY RESOLUTION FLAGS Per A2560K_UM_Rev0.0.1.pdf and A2560U_UM_Rev0.0.2.pdf
// // VICKY II / VICKY III Chan B
// // 640x480  @ 60FPS > 0 0
// // 800x600  @ 60FPS > 0 1
// // reserved         > 1 0
// // 640x400  @ 70FPS > 1 1
// 
// // VICKY III Chan A
// // 800x600  @ 60FPS > 0 0
// // 1024x768 @ 60FPS > 0 1
// // reserved         > 1 0
// // reserved         > 1 1
// 
// #define VICKY_II_RES_640X480_FLAGS		0x00	// 0b00000000
// #define VICKY_II_RES_800X600_FLAGS		0x01	// 0b00000001
// #define VICKY_II_RES_640X400_FLAGS		0x03	// 0b00000011
// 
// #define VICKY_IIIB_RES_640X480_FLAGS	0x00	// 0b00000000
// #define VICKY_IIIB_RES_800X600_FLAGS	0x01	// 0b00000001
// #define VICKY_IIIB_RES_640X400_FLAGS	0x03	// 0b00000011
// 
// #define VICKY_IIIA_RES_800X600_FLAGS	0x00	// 0b00000000
// #define VICKY_IIIA_RES_1024X768_FLAGS	0x01	// 0b00000001

	unsigned long*	the_vicky_register;
	unsigned long	the_vicky_value;
	unsigned char	the_video_mode_offset;
	unsigned char	the_video_mode_value;
	unsigned char	the_video_mode_bits;
	int				i;
	char			msg_buffer[80*3];
	char*			the_message = msg_buffer;
	
// 	the_video_mode_offset = VIDEO_MODE_BYTE;
// 	
// 	// test channel A
// 	the_vicky_register = (unsigned long*)VICKY_A2560K_A;
// 	the_vicky_value = *the_vicky_register;
// 	DEBUG_OUT(("%s %d: chan A vicky value=%lu before shifting right", __func__, __LINE__, the_vicky_value));
// 	
// 	for (i = 0; i < VIDEO_MODE_BYTE; i++)
// 	{
// 		the_vicky_value = the_vicky_value >> 8;
// 	}
// 	DEBUG_OUT(("%s %d: chan A vicky value=%lu after shifting right", __func__, __LINE__, the_vicky_value));
// 	
// 	the_video_mode_value = the_vicky_value & 0xFF;
// 	DEBUG_OUT(("%s %d: chan A video mode byte=%u, vicky reg=%lu, vicky value=%lu", __func__, __LINE__, the_video_mode_value, *the_vicky_register, the_vicky_value));
	
	//R32(VICKY_CTRL) = (R32(VICKY_CTRL) & ~VICKY_MODE_MASK) | ((mode & 0x07) << 8);
	
	// try setting different mode
// 	the_video_mode_value = (char)VICKY_IIIA_RES_800X600_FLAGS; 
// 	the_vicky_value = *the_vicky_register;
// 	DEBUG_OUT(("%s %d: chan A vicky value=%lu before change, video byte before=%u", __func__, __LINE__, the_vicky_value, the_video_mode_value));
// 	the_vicky_value = the_vicky_value & 0xFFFF00FF;
// 	the_video_mode_value = the_video_mode_value << 8;
// 	DEBUG_OUT(("%s %d: chan A vicky value=%lu after masking, vid byte after shift=%u", __func__, __LINE__, the_vicky_value, the_video_mode_value));
// 	the_vicky_value = the_vicky_value | (the_video_mode_value);
// 	//the_vicky_value = (the_vicky_value & 0x0000FF00) | (the_video_mode_value << 8);
// 	DEBUG_OUT(("%s %d: chan A vicky value=%lu after remasking", __func__, __LINE__, the_vicky_value));
// 
// 	*the_vicky_register = the_vicky_value;
// 	Text_ShowFontChars(&global_screen[ID_CHANNEL_A], 10);
// 	
// 	// test channel B
// 	the_vicky_register = (unsigned long*)VICKY_A2560K_B;
// 	//the_vicky_value = *the_vicky_register;
// 	the_vicky_value = *global_screen[ID_CHANNEL_B].vicky_;
// 	DEBUG_OUT(("%s %d: chan B vicky value=%lu before shifting right", __func__, __LINE__, the_vicky_value));
// 	
// 	for (i = 0; i < VIDEO_MODE_BYTE; i++)
// 	{
// 		the_vicky_value = the_vicky_value >> 8;
// 	}
// 	DEBUG_OUT(("%s %d: chan B vicky value=%lu after shifting right", __func__, __LINE__, the_vicky_value));
// 	
// 	the_video_mode_value = the_vicky_value & 0xFF;
// 	DEBUG_OUT(("%s %d: chan B video mode byte=%u, vicky reg=%lu, vicky value=%lu", __func__, __LINE__, the_video_mode_value, *the_vicky_register, the_vicky_value));
// 	
// 	// try setting different mode
// 	getchar();
// 	the_video_mode_value = (unsigned short)VICKY_IIIB_RES_640X400_FLAGS; 
// 	//the_vicky_value = *the_vicky_register;
// 	the_vicky_value = *global_screen[ID_CHANNEL_B].vicky_;
// 	the_vicky_value = (the_vicky_value & VIDEO_MODE_MASK | (the_video_mode_value << 8));
// 	DEBUG_OUT(("%s %d: chan B vicky value=%lu after remasking", __func__, __LINE__, the_vicky_value));
// 	*the_vicky_register = the_vicky_value;
// 	Text_ShowFontChars(&global_screen[ID_CHANNEL_B], 10);
// 	getchar();
// 	
// 	the_video_mode_value = (unsigned short)VICKY_IIIB_RES_800X600_FLAGS; 
// 	//the_vicky_value = *the_vicky_register;
// 	the_vicky_value = *global_screen[ID_CHANNEL_B].vicky_;
// 	the_vicky_value = (the_vicky_value & VIDEO_MODE_MASK | (the_video_mode_value << 8));
// 	DEBUG_OUT(("%s %d: chan B vicky value=%lu after remasking", __func__, __LINE__, the_vicky_value));
// 	*the_vicky_register = the_vicky_value;
// 	Text_ShowFontChars(&global_screen[ID_CHANNEL_B], 10);
// 	getchar();
		
// 	the_video_mode_value = (unsigned short)VICKY_IIIB_RES_640X480_FLAGS; 
// 	the_vicky_value = *the_vicky_register;
// 	the_vicky_value = (the_vicky_value & VIDEO_MODE_MASK | (the_video_mode_value << 8));
// 	DEBUG_OUT(("%s %d: chan B vicky value=%lu after remasking", __func__, __LINE__, the_vicky_value));
// 	*the_vicky_register = the_vicky_value;
// 	Text_ShowFontChars(&global_screen[ID_CHANNEL_B], 10);

// 	the_video_mode_value = (unsigned short)VICKY_IIIB_RES_640X480_FLAGS; 
// 	*the_vicky_register = (*the_vicky_register & VIDEO_MODE_MASK | (the_video_mode_value << 8));
	
	// test the function call version
// 	getchar();
// 	Text_SetVideoMode(&global_screen[ID_CHANNEL_B], RES_640X400);
// 	Text_FillCharMem(&global_screen[ID_CHANNEL_B], ' ');
// 	Text_FillAttrMem(&global_screen[ID_CHANNEL_B], 159);
// 	sprintf(the_message, "640x400 should now be showing. Measured: %i x %i, %i x %i text, %i x %i visible text", 
// 		global_screen[ID_CHANNEL_B].width_, 
// 		global_screen[ID_CHANNEL_B].height_, 
// 		global_screen[ID_CHANNEL_B].text_mem_cols_, 
// 		global_screen[ID_CHANNEL_B].text_mem_rows_, 
// 		global_screen[ID_CHANNEL_B].text_cols_vis_, 
// 		global_screen[ID_CHANNEL_B].text_rows_vis_
// 		);
// 	Text_DrawStringAtXY(&global_screen[ID_CHANNEL_B], 0, 0, the_message, FG_COLOR_BLACK, BG_COLOR_GREEN);
// 	Text_DrawStringAtXY(&global_screen[ID_CHANNEL_B], 0, 1, (char*)"01234567890123456789012345678901234567890123456789012345678901234567890123456789", FG_COLOR_DK_BLUE, BG_COLOR_YELLOW);
// 	Text_DrawStringAtXY(&global_screen[ID_CHANNEL_B], 0, 2, (char*)"<-START OF LINE", FG_COLOR_BLACK, BG_COLOR_GREEN);
// 	Text_DrawStringAtXY(&global_screen[ID_CHANNEL_B], 0, 45, (char*)"ROW45", FG_COLOR_BLACK, BG_COLOR_GREEN);
// 	Text_DrawStringAtXY(&global_screen[ID_CHANNEL_B], 0, 46, (char*)"ROW46", FG_COLOR_BLACK, BG_COLOR_GREEN);
// 	Text_DrawStringAtXY(&global_screen[ID_CHANNEL_B], 0, 48, (char*)"ROW48", FG_COLOR_BLACK, BG_COLOR_GREEN);
// 	Text_DrawStringAtXY(&global_screen[ID_CHANNEL_B], 0, 49, (char*)"ROW49", FG_COLOR_BLACK, BG_COLOR_GREEN);	
// 	Text_ShowFontChars(&global_screen[ID_CHANNEL_B], 10);

	getchar();
	Text_SetVideoMode(&global_screen[ID_CHANNEL_B], RES_800X600);
	Text_FillCharMem(&global_screen[ID_CHANNEL_B], ' ');
	Text_FillAttrMem(&global_screen[ID_CHANNEL_B], 159);
	sprintf(the_message, "800x600 should now be showing. Measured: %i x %i, %i x %i text, %i x %i visible text", 
		global_screen[ID_CHANNEL_B].width_, 
		global_screen[ID_CHANNEL_B].height_, 
		global_screen[ID_CHANNEL_B].text_mem_cols_, 
		global_screen[ID_CHANNEL_B].text_mem_rows_, 
		global_screen[ID_CHANNEL_B].text_cols_vis_, 
		global_screen[ID_CHANNEL_B].text_rows_vis_
		);
	Text_DrawStringAtXY(&global_screen[ID_CHANNEL_B], 0, 0, the_message, FG_COLOR_BLACK, BG_COLOR_GREEN);
	Text_DrawStringAtXY(&global_screen[ID_CHANNEL_B], 0, 1, (char*)"0123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789", FG_COLOR_DK_BLUE, BG_COLOR_YELLOW);
	Text_DrawStringAtXY(&global_screen[ID_CHANNEL_B], 0, 2, (char*)"<-START OF LINE", FG_COLOR_BLACK, BG_COLOR_GREEN);
	Text_DrawStringAtXY(&global_screen[ID_CHANNEL_B], 0, 66, (char*)"ROW66", FG_COLOR_BLACK, BG_COLOR_GREEN);
	Text_DrawStringAtXY(&global_screen[ID_CHANNEL_B], 0, 70, (char*)"ROW70", FG_COLOR_BLACK, BG_COLOR_GREEN);
	Text_ShowFontChars(&global_screen[ID_CHANNEL_B], 10);

	getchar();
	Text_SetVideoMode(&global_screen[ID_CHANNEL_B], RES_640X480);
	Text_FillCharMem(&global_screen[ID_CHANNEL_B], ' ');
	Text_FillAttrMem(&global_screen[ID_CHANNEL_B], 159);
	sprintf(the_message, "640x480 should now be showing. Measured: %i x %i, %i x %i text, %i x %i visible text", 
		global_screen[ID_CHANNEL_B].width_, 
		global_screen[ID_CHANNEL_B].height_, 
		global_screen[ID_CHANNEL_B].text_mem_cols_, 
		global_screen[ID_CHANNEL_B].text_mem_rows_, 
		global_screen[ID_CHANNEL_B].text_cols_vis_, 
		global_screen[ID_CHANNEL_B].text_rows_vis_
		);
	Text_DrawStringAtXY(&global_screen[ID_CHANNEL_B], 0, 0, the_message, FG_COLOR_BLACK, BG_COLOR_GREEN);
	Text_DrawStringAtXY(&global_screen[ID_CHANNEL_B], 0, 1, (char*)"01234567890123456789012345678901234567890123456789012345678901234567890123456789", FG_COLOR_DK_BLUE, BG_COLOR_YELLOW);
	Text_DrawStringAtXY(&global_screen[ID_CHANNEL_B], 0, 1, (char*)"<-START OF LINE", FG_COLOR_BLACK, BG_COLOR_GREEN);
	Text_DrawStringAtXY(&global_screen[ID_CHANNEL_B], 0, 46, (char*)"ROW46", FG_COLOR_BLACK, BG_COLOR_GREEN);
	Text_DrawStringAtXY(&global_screen[ID_CHANNEL_B], 0, 51, (char*)"ROW51", FG_COLOR_BLACK, BG_COLOR_GREEN);
	Text_DrawStringAtXY(&global_screen[ID_CHANNEL_B], 0, 55, (char*)"ROW55", FG_COLOR_BLACK, BG_COLOR_GREEN);
	Text_ShowFontChars(&global_screen[ID_CHANNEL_B], 10);

	getchar();
	Text_SetVideoMode(&global_screen[ID_CHANNEL_A], RES_800X600);
	Text_FillCharMem(&global_screen[ID_CHANNEL_A], ' ');
	Text_FillAttrMem(&global_screen[ID_CHANNEL_A], 159);
	sprintf(the_message, "800x600 should now be showing. Measured: %i x %i, %i x %i text, %i x %i visible text", 
		global_screen[ID_CHANNEL_A].width_, 
		global_screen[ID_CHANNEL_A].height_, 
		global_screen[ID_CHANNEL_A].text_mem_cols_, 
		global_screen[ID_CHANNEL_A].text_mem_rows_, 
		global_screen[ID_CHANNEL_A].text_cols_vis_, 
		global_screen[ID_CHANNEL_A].text_rows_vis_
		);
	Text_DrawStringAtXY(&global_screen[ID_CHANNEL_A], 0, 0, the_message, FG_COLOR_BLACK, BG_COLOR_GREEN);
	Text_DrawStringAtXY(&global_screen[ID_CHANNEL_B], 0, 5, (char*)"800x600 should now be showing on Channel A", FG_COLOR_RED, BG_COLOR_GREEN);
	Text_DrawStringAtXY(&global_screen[ID_CHANNEL_A], 0, 1, (char*)"0123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789", FG_COLOR_DK_BLUE, BG_COLOR_YELLOW);
	Text_DrawStringAtXY(&global_screen[ID_CHANNEL_A], 0, 2, (char*)"<-START OF LINE", FG_COLOR_BLACK, BG_COLOR_GREEN);
	Text_DrawStringAtXY(&global_screen[ID_CHANNEL_A], 0, 66, (char*)"ROW66", FG_COLOR_BLACK, BG_COLOR_GREEN);
	Text_DrawStringAtXY(&global_screen[ID_CHANNEL_A], 0, 70, (char*)"ROW70", FG_COLOR_BLACK, BG_COLOR_GREEN);
	Text_ShowFontChars(&global_screen[ID_CHANNEL_A], 10);

	getchar();
	Text_SetVideoMode(&global_screen[ID_CHANNEL_A], RES_1024X768);
	Text_FillCharMem(&global_screen[ID_CHANNEL_A], ' ');
	Text_FillAttrMem(&global_screen[ID_CHANNEL_A], 159);
	sprintf(the_message, "1024x768 should now be showing. Measured: %i x %i, %i x %i text, %i x %i visible text", 
		global_screen[ID_CHANNEL_A].width_, 
		global_screen[ID_CHANNEL_A].height_, 
		global_screen[ID_CHANNEL_A].text_mem_cols_, 
		global_screen[ID_CHANNEL_A].text_mem_rows_, 
		global_screen[ID_CHANNEL_A].text_cols_vis_, 
		global_screen[ID_CHANNEL_A].text_rows_vis_
		);
	Text_DrawStringAtXY(&global_screen[ID_CHANNEL_A], 0, 0, the_message, FG_COLOR_BLACK, BG_COLOR_GREEN);
	Text_DrawStringAtXY(&global_screen[ID_CHANNEL_B], 0, 5, (char*)"1024x768 should now be showing on Channel A", FG_COLOR_RED, BG_COLOR_GREEN);
	Text_DrawStringAtXY(&global_screen[ID_CHANNEL_A], 0, 1, (char*)"0123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567", FG_COLOR_DK_BLUE, BG_COLOR_YELLOW);
	Text_DrawStringAtXY(&global_screen[ID_CHANNEL_A], 0, 2, (char*)"<-START OF LINE", FG_COLOR_BLACK, BG_COLOR_GREEN);
	Text_DrawStringAtXY(&global_screen[ID_CHANNEL_A], 0, 87, (char*)"ROW87", FG_COLOR_BLACK, BG_COLOR_GREEN);
	Text_DrawStringAtXY(&global_screen[ID_CHANNEL_A], 0, 89, (char*)"ROW89", FG_COLOR_BLACK, BG_COLOR_GREEN);
	Text_DrawStringAtXY(&global_screen[ID_CHANNEL_A], 0, 91, (char*)"ROW91", FG_COLOR_BLACK, BG_COLOR_GREEN);
	Text_ShowFontChars(&global_screen[ID_CHANNEL_A], 10);

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
	
	TestResolution();
	
	//play_GraphicsModes(&global_screen[ID_CHANNEL_B]);
	
	//General_LogInitialize();

	//keyboard_test();
	   
	#if defined(RUN_TESTS)
//		Text_RunTests();
	#endif
	
}