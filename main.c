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

//! Find out what kind of machine the software is running on, and configure global screens accordingly
//! @return	Returns false if the machine is known to be incompatible with this software. 
boolean AutoConfigureScreens(void);

//! Change video mode to the one passed.
//! @param new_mode: Must correspond to a valid VICKY video mode. See VICKY_IIIA_RES_800X600_FLAGS, etc. defined in a2560_platform.h
//! @return	returns false on any error/invalid input.
boolean Text_SetVideoMode(Screen* the_screen, screen_resolution new_mode);

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


//! Find out what kind of machine the software is running on, and configure global screens accordingly
//! @return	Returns false if the machine is known to be incompatible with this software. 
boolean AutoConfigureScreens(void)
{
	struct s_sys_info	the_sys_info;
	unsigned short		the_model_number;
	
	sys_get_info(&the_sys_info);
	
	DEBUG_OUT(("%s %d: Machine: '%s'; has %u screens", __func__, __LINE__, the_sys_info.model_name, the_sys_info.screens));

	// configure screen settings, RAM addresses, etc. based on known info about machine types
	
	the_model_number = the_sys_info.model;
	
	switch (the_model_number)
	{
		case MACHINE_C256_FMX:
		case MACHINE_C256_U:
		case MACHINE_C256_GENX:
		case MACHINE_C256_UPLUS:
			DEBUG_OUT(("%s %d: this application is not compatible with the %s.", __func__, __LINE__, the_sys_info.model_name));
			return false;
			break;
			
		case MACHINE_A2560U_PLUS:
		case MACHINE_A2560U:
			global_num_screens = 1;
			global_main_screen = ID_CHANNEL_A;
			global_alt_screen = ID_CHANNEL_A; // there is no alt screen for this hardware.

			global_screen[global_main_screen].id_ = ID_CHANNEL_B;
			global_screen[ID_CHANNEL_A].vicky_ = VICKY_A2560U;
			global_screen[global_main_screen].text_temp_buffer_1_[0] = '\0';
			global_screen[global_main_screen].text_temp_buffer_2_[0] = '\0';
			global_screen[global_main_screen].text_font_width_ = TEXT_FONT_WIDTH_A2650;
			global_screen[global_main_screen].text_font_height_ = TEXT_FONT_HEIGHT_A2650;
			global_screen[global_main_screen].rect_.MinX = 0;
			global_screen[global_main_screen].rect_.MinY = 0;	
			global_screen[global_main_screen].width_ = 640;	
			global_screen[global_main_screen].height_ = 480;	
			global_screen[global_main_screen].text_ram_ = TEXT_RAM_A2560U;
			global_screen[global_main_screen].text_attr_ram_ = TEXT_ATTR_A2560U;
			global_screen[global_main_screen].text_font_ram_ = FONT_MEMORY_BANK_A2560U;
			global_screen[global_main_screen].text_mem_cols_ = global_screen[global_main_screen].width_ / global_screen[global_main_screen].text_font_width_;
			global_screen[global_main_screen].text_mem_rows_ = global_screen[global_main_screen].height_ / global_screen[global_main_screen].text_font_height_;
			global_screen[global_main_screen].text_cols_vis_ = global_screen[global_main_screen].text_mem_cols_ - TEXTB_NUM_COLS_BORDER;
			global_screen[global_main_screen].text_rows_vis_ = global_screen[global_main_screen].text_mem_rows_ - TEXTB_NUM_COLS_BORDER;	// not measured
			global_screen[global_main_screen].rect_.MaxX = global_screen[global_main_screen].width_;
			global_screen[global_main_screen].rect_.MaxY = global_screen[global_main_screen].height_;	
			break;
			
		case MACHINE_A2560X:
		case MACHINE_A2560K:
			global_num_screens = 2;
			global_main_screen = ID_CHANNEL_B;
			global_alt_screen = ID_CHANNEL_A;
			
			global_screen[ID_CHANNEL_A].id_ = ID_CHANNEL_A;
			global_screen[ID_CHANNEL_A].vicky_ = VICKYA_A2560K;
			global_screen[ID_CHANNEL_A].text_temp_buffer_1_[0] = '\0';
			global_screen[ID_CHANNEL_A].text_temp_buffer_2_[0] = '\0';
			global_screen[ID_CHANNEL_A].text_font_width_ = TEXT_FONT_WIDTH_A2650;
			global_screen[ID_CHANNEL_A].text_font_height_ = TEXT_FONT_HEIGHT_A2650;
			global_screen[ID_CHANNEL_A].rect_.MinX = 0;
			global_screen[ID_CHANNEL_A].rect_.MinY = 0;
	
			global_screen[ID_CHANNEL_A].width_ = 1024;	
			global_screen[ID_CHANNEL_A].height_ = 768;
			global_screen[ID_CHANNEL_A].text_ram_ = TEXTA_RAM_A2560K;
			global_screen[ID_CHANNEL_A].text_attr_ram_ = TEXTA_ATTR_A2560K;
			global_screen[ID_CHANNEL_A].text_font_ram_ = FONT_MEMORY_BANKA_A2560K;
			global_screen[ID_CHANNEL_A].text_mem_cols_ = global_screen[ID_CHANNEL_A].width_ / global_screen[ID_CHANNEL_A].text_font_width_;
			global_screen[ID_CHANNEL_A].text_mem_rows_ = global_screen[ID_CHANNEL_A].height_ / global_screen[ID_CHANNEL_A].text_font_height_;
			global_screen[ID_CHANNEL_A].text_cols_vis_ = global_screen[ID_CHANNEL_A].text_mem_cols_ - TEXTA_NUM_COLS_BORDER; // not measured
			global_screen[ID_CHANNEL_A].text_rows_vis_ = global_screen[ID_CHANNEL_A].text_mem_rows_ - TEXTA_NUM_COLS_BORDER; // not measured 

			global_screen[ID_CHANNEL_A].rect_.MaxX = global_screen[ID_CHANNEL_A].width_;
			global_screen[ID_CHANNEL_A].rect_.MaxY = global_screen[ID_CHANNEL_A].height_;	

			global_screen[ID_CHANNEL_B].id_ = ID_CHANNEL_B;
			global_screen[ID_CHANNEL_B].vicky_ = VICKYB_A2560K;
			global_screen[ID_CHANNEL_B].text_temp_buffer_1_[0] = '\0';
			global_screen[ID_CHANNEL_B].text_temp_buffer_2_[0] = '\0';
			global_screen[ID_CHANNEL_B].text_font_width_ = TEXT_FONT_WIDTH_A2650;
			global_screen[ID_CHANNEL_B].text_font_height_ = TEXT_FONT_HEIGHT_A2650;
			global_screen[ID_CHANNEL_B].rect_.MinX = 0;
			global_screen[ID_CHANNEL_B].rect_.MinY = 0;
	
			global_screen[ID_CHANNEL_B].width_ = 640;	
			global_screen[ID_CHANNEL_B].height_ = 480;	
			global_screen[ID_CHANNEL_B].text_ram_ = TEXTB_RAM_A2560K;
			global_screen[ID_CHANNEL_B].text_attr_ram_ = TEXTB_ATTR_A2560K;
			global_screen[ID_CHANNEL_B].text_font_ram_ = FONT_MEMORY_BANKB_A2560K;
			global_screen[ID_CHANNEL_B].text_mem_cols_ = global_screen[ID_CHANNEL_B].width_ / global_screen[ID_CHANNEL_B].text_font_width_;
			global_screen[ID_CHANNEL_B].text_mem_rows_ = global_screen[ID_CHANNEL_B].height_ / global_screen[ID_CHANNEL_B].text_font_height_;
			global_screen[ID_CHANNEL_B].text_cols_vis_ = global_screen[ID_CHANNEL_B].text_mem_cols_ - TEXTB_NUM_COLS_BORDER;
			global_screen[ID_CHANNEL_B].text_rows_vis_ = global_screen[ID_CHANNEL_B].text_mem_rows_ - TEXTB_NUM_COLS_BORDER;	// not measured

			global_screen[ID_CHANNEL_B].rect_.MaxX = global_screen[ID_CHANNEL_B].width_;
			global_screen[ID_CHANNEL_B].rect_.MaxY = global_screen[ID_CHANNEL_B].height_;	
		
			break;

	}
	
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
	
// 	the_video_mode_offset = VIDEO_MODE_BYTE;
// 	
// 	// test channel A
// 	the_vicky_register = (unsigned long*)VICKYA_A2560K;
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
// 	the_vicky_register = (unsigned long*)VICKYB_A2560K;
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
	getchar();
	Text_FillCharMem(&global_screen[ID_CHANNEL_B], ' ');
	Text_FillAttrMem(&global_screen[ID_CHANNEL_B], 159);
	Text_SetVideoMode(&global_screen[ID_CHANNEL_B], RES_640X400);
	Text_DrawStringAtXY(&global_screen[ID_CHANNEL_B], 0, 0, (char*)"640x400 should now be showing. assuming 2 cols/rows borders, 76x46 visible", FG_COLOR_BLACK, BG_COLOR_GREEN);
	Text_DrawStringAtXY(&global_screen[ID_CHANNEL_B], 0, 1, (char*)"01234567890123456789012345678901234567890123456789012345678901234567890123456789", FG_COLOR_DK_BLUE, BG_COLOR_YELLOW);
	Text_DrawStringAtXY(&global_screen[ID_CHANNEL_B], 0, 2, (char*)"<-START OF LINE", FG_COLOR_BLACK, BG_COLOR_GREEN);
	Text_DrawStringAtXY(&global_screen[ID_CHANNEL_B], 0, 45, (char*)"ROW45", FG_COLOR_BLACK, BG_COLOR_GREEN);
	Text_DrawStringAtXY(&global_screen[ID_CHANNEL_B], 0, 46, (char*)"ROW46", FG_COLOR_BLACK, BG_COLOR_GREEN);
	Text_DrawStringAtXY(&global_screen[ID_CHANNEL_B], 0, 48, (char*)"ROW48", FG_COLOR_BLACK, BG_COLOR_GREEN);
	Text_DrawStringAtXY(&global_screen[ID_CHANNEL_B], 0, 49, (char*)"ROW49", FG_COLOR_BLACK, BG_COLOR_GREEN);

	getchar();
	Text_FillCharMem(&global_screen[ID_CHANNEL_B], ' ');
	Text_FillAttrMem(&global_screen[ID_CHANNEL_B], 159);
	Text_SetVideoMode(&global_screen[ID_CHANNEL_B], RES_800X600);
	Text_DrawStringAtXY(&global_screen[ID_CHANNEL_B], 0, 0, (char*)"800x600 should now be showing. assuming 2 cols/rows borders, 96x71 visible", FG_COLOR_BLACK, BG_COLOR_GREEN);
	Text_DrawStringAtXY(&global_screen[ID_CHANNEL_B], 0, 1, (char*)"0123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789", FG_COLOR_DK_BLUE, BG_COLOR_YELLOW);
	Text_DrawStringAtXY(&global_screen[ID_CHANNEL_B], 0, 2, (char*)"<-START OF LINE", FG_COLOR_BLACK, BG_COLOR_GREEN);
	Text_DrawStringAtXY(&global_screen[ID_CHANNEL_B], 0, 70, (char*)"ROW70", FG_COLOR_BLACK, BG_COLOR_GREEN);

	getchar();
	Text_FillCharMem(&global_screen[ID_CHANNEL_B], ' ');
	Text_FillAttrMem(&global_screen[ID_CHANNEL_B], 159);
	Text_SetVideoMode(&global_screen[ID_CHANNEL_B], RES_640X480);
	Text_DrawStringAtXY(&global_screen[ID_CHANNEL_B], 0, 0, (char*)"640x480 should now be showing. assuming 2 cols/rows borders, 76x56 visible", FG_COLOR_BLACK, BG_COLOR_GREEN);
	Text_DrawStringAtXY(&global_screen[ID_CHANNEL_B], 0, 1, (char*)"01234567890123456789012345678901234567890123456789012345678901234567890123456789", FG_COLOR_DK_BLUE, BG_COLOR_YELLOW);
	Text_DrawStringAtXY(&global_screen[ID_CHANNEL_B], 0, 1, (char*)"<-START OF LINE", FG_COLOR_BLACK, BG_COLOR_GREEN);
	Text_DrawStringAtXY(&global_screen[ID_CHANNEL_B], 0, 46, (char*)"ROW46", FG_COLOR_BLACK, BG_COLOR_GREEN);
	Text_DrawStringAtXY(&global_screen[ID_CHANNEL_B], 0, 55, (char*)"ROW55", FG_COLOR_BLACK, BG_COLOR_GREEN);

	getchar();
	Text_FillCharMem(&global_screen[ID_CHANNEL_A], ' ');
	Text_FillAttrMem(&global_screen[ID_CHANNEL_A], 159);
	Text_SetVideoMode(&global_screen[ID_CHANNEL_A], RES_800X600);
	Text_DrawStringAtXY(&global_screen[ID_CHANNEL_A], 0, 0, (char*)"800x600 should now be showing. assuming 2 cols/rows borders, 96x71 visible", FG_COLOR_BLACK, BG_COLOR_GREEN);
	Text_DrawStringAtXY(&global_screen[ID_CHANNEL_A], 0, 1, (char*)"0123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789", FG_COLOR_DK_BLUE, BG_COLOR_YELLOW);
	Text_DrawStringAtXY(&global_screen[ID_CHANNEL_A], 0, 2, (char*)"<-START OF LINE", FG_COLOR_BLACK, BG_COLOR_GREEN);
	Text_DrawStringAtXY(&global_screen[ID_CHANNEL_A], 0, 70, (char*)"ROW70", FG_COLOR_BLACK, BG_COLOR_GREEN);

	getchar();
	Text_FillCharMem(&global_screen[ID_CHANNEL_A], ' ');
	Text_FillAttrMem(&global_screen[ID_CHANNEL_A], 159);
	Text_SetVideoMode(&global_screen[ID_CHANNEL_A], RES_1024X768);
	Text_DrawStringAtXY(&global_screen[ID_CHANNEL_A], 0, 0, (char*)"1024x768 should now be showing. assuming 2 cols/rows borders, 124x92 visible", FG_COLOR_BLACK, BG_COLOR_GREEN);
	Text_DrawStringAtXY(&global_screen[ID_CHANNEL_A], 0, 1, (char*)"0123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567", FG_COLOR_DK_BLUE, BG_COLOR_YELLOW);
	Text_DrawStringAtXY(&global_screen[ID_CHANNEL_A], 0, 2, (char*)"<-START OF LINE", FG_COLOR_BLACK, BG_COLOR_GREEN);
	Text_DrawStringAtXY(&global_screen[ID_CHANNEL_A], 0, 91, (char*)"ROW91", FG_COLOR_BLACK, BG_COLOR_GREEN);

}

//! Change video mode to the one passed.
//! @param new_mode: One of the enumerated screen_resolution values. Must correspond to a valid VICKY video mode for the host machine. See VICKY_IIIA_RES_800X600_FLAGS, etc. defined in a2560_platform.h
//! @return	returns false on any error/invalid input.
boolean Text_SetVideoMode(Screen* the_screen, screen_resolution new_mode)
{
	unsigned char	new_mode_flag = 0xFF;
	
	if (the_screen == NULL)
	{
		LOG_ERR(("%s %d: passed screen was NULL", __func__, __LINE__));
		return false;
	}

	// TODO: figure out smarter way of knowing which video modes are legal for the machine being run on
	if (the_screen->vicky_ == VICKYA_A2560K)
	{
		DEBUG_OUT(("%s %d: vicky identified as VICKYA_A2560K", __func__, __LINE__));
		
		if (new_mode == RES_800X600)
		{
			new_mode_flag = VICKY_IIIA_RES_800X600_FLAGS;
		}
		else if (new_mode == RES_1024X768)
		{
			new_mode_flag = VICKY_IIIA_RES_1024X768_FLAGS;
		}
	}
	else if (the_screen->vicky_ == VICKYB_A2560K)
	{
		DEBUG_OUT(("%s %d: vicky identified as VICKYB_A2560K", __func__, __LINE__));
		
		if (new_mode == RES_640X400)
		{
			new_mode_flag = VICKY_IIIB_RES_640X400_FLAGS;
		}
		else if (new_mode == RES_640X480)
		{
			new_mode_flag = VICKY_IIIB_RES_640X480_FLAGS;
		}
		else if (new_mode == RES_800X600)
		{
			new_mode_flag = VICKY_IIIB_RES_800X600_FLAGS;
		}
	}
	else if (the_screen->vicky_ == VICKY_A2560U)
	{
		DEBUG_OUT(("%s %d: vicky identified as VICKY_A2560U", __func__, __LINE__));
		if (new_mode == RES_640X400)
		{
			new_mode_flag = VICKY_II_RES_640X400_FLAGS;
		}
		else if (new_mode == RES_640X480)
		{
			new_mode_flag = VICKY_II_RES_640X480_FLAGS;
		}
		else if (new_mode == RES_800X600)
		{
			new_mode_flag = VICKY_II_RES_800X600_FLAGS;
		}
	}
	DEBUG_OUT(("%s %d: specified video mode = %u, flag=%u", __func__, __LINE__, new_mode, new_mode_flag));
	
	if (new_mode_flag == 0xFF)
	{
		LOG_ERR(("%s %d: specified video mode is not legal for this screen %u", __func__, __LINE__, new_mode));
		return false;
	}
	
	*the_screen->vicky_ = (*the_screen->vicky_ & VIDEO_MODE_MASK | (new_mode << 8));
	
	// teach screen about the new settings
	switch (new_mode)
	{
		case RES_640X400:
			the_screen->width_ = 640;	
			the_screen->height_ = 400;
			break;
			
		case RES_640X480:
			the_screen->width_ = 640;	
			the_screen->height_ = 480;
			break;
			
		case RES_800X600:
			the_screen->width_ = 800;	
			the_screen->height_ = 600;
			break;
			
		case RES_1024X768:
			the_screen->width_ = 1024;	
			the_screen->height_ = 768;
			break;		
	}
	
	the_screen->text_mem_cols_ = the_screen->width_ / the_screen->text_font_width_;
	the_screen->text_mem_rows_ = the_screen->height_ / the_screen->text_font_height_;
	the_screen->text_cols_vis_ = the_screen->text_mem_cols_ - TEXTA_NUM_COLS_BORDER; // not measured
	the_screen->text_rows_vis_ = the_screen->text_mem_rows_ - TEXTA_NUM_COLS_BORDER; // not measured 
	the_screen->rect_.MaxX = the_screen->width_;
	the_screen->rect_.MaxY = the_screen->height_;	
	
	Text_ShowFontChars(the_screen, 10);

	// tell the MCP that we changed res so it can update it's internal col sizes, etc. 
	//sys_text_setsizes();
	
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


/*****************************************************************************/
/*                        Public Function Definitions                        */
/*****************************************************************************/



int main(int argc, char* argv[])
{
	
	// find out what kind of machine the software is running on, and configure global screens accordingly
	AutoConfigureScreens();
	
	TestResolution();
	
	// move to startup function

	//General_LogInitialize();

	//keyboard_test();
	   
	#if defined(RUN_TESTS)
//		Text_RunTests();
	#endif
	
}