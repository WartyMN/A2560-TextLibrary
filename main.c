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
#include "lib_general.h"
#include "lib_general_a2560.h"

#include "lib_text.h"


// C includes
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


// A2560 includes
#include <mcp/syscalls.h>


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

/*****************************************************************************/
/*                       Private Function Prototypes                         */
/*****************************************************************************/


/*****************************************************************************/
/*                       Private Function Definitions                        */
/*****************************************************************************/

// test using sys_kbd_scancode() instead of a channel driver
boolean keyboard_test_2(void);

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

/*****************************************************************************/
/*                        Public Function Definitions                        */
/*****************************************************************************/

boolean keyboard_test(void);

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


int main(int argc, char* argv[])
{

#define __f68 1	// undefine '__f68' if running in morfe or addresses will be wrong for morfe

	// move to startup function
	global_screen[ID_CHANNEL_A].id_ = ID_CHANNEL_A;
	global_screen[ID_CHANNEL_A].text_temp_buffer_1_[0] = '\0';
	global_screen[ID_CHANNEL_A].text_temp_buffer_2_[0] = '\0';
	global_screen[ID_CHANNEL_A].text_font_width_ = TEXT_FONT_WIDTH_A2650;
	global_screen[ID_CHANNEL_A].text_font_height_ = TEXT_FONT_HEIGHT_A2650;
	global_screen[ID_CHANNEL_A].rect_.MinX = 0;
	global_screen[ID_CHANNEL_A].rect_.MinY = 0;
	
	#ifdef __f68
		global_screen[ID_CHANNEL_A].width_ = 1024;	
		global_screen[ID_CHANNEL_A].height_ = 768;
		global_screen[ID_CHANNEL_A].text_ram_ = TEXTA_RAM_A2560K;
		global_screen[ID_CHANNEL_A].text_attr_ram_ = TEXTA_ATTR_A2560K;
		global_screen[ID_CHANNEL_A].text_font_ram_ = FONT_MEMORY_BANKA_A2560K;
		global_screen[ID_CHANNEL_A].text_mem_cols_ = global_screen[ID_CHANNEL_A].width_ / global_screen[ID_CHANNEL_A].text_font_width_;
		global_screen[ID_CHANNEL_A].text_mem_rows_ = global_screen[ID_CHANNEL_A].height_ / global_screen[ID_CHANNEL_A].text_font_height_;
		global_screen[ID_CHANNEL_A].text_cols_vis_ = global_screen[ID_CHANNEL_A].text_mem_cols_ - 0; // how morfe has it defined. just guessing for f68
		global_screen[ID_CHANNEL_A].text_rows_vis_ = global_screen[ID_CHANNEL_A].text_mem_rows_ - 0; // how morfe has it defined. just guessing for f68
	#else
		global_screen[ID_CHANNEL_A].width_ = 640;	
		global_screen[ID_CHANNEL_A].height_ = 480;	
		global_screen[ID_CHANNEL_A].text_ram_ = TEXTA_RAM_A2560_MORFE;
		global_screen[ID_CHANNEL_A].text_attr_ram_ = TEXTA_ATTR_A2560_MORFE;
		global_screen[ID_CHANNEL_A].text_font_ram_ = FONT_MEMORY_BANKA_A2560_MORFE;
		global_screen[ID_CHANNEL_A].text_mem_cols_ = global_screen[ID_CHANNEL_A].width_ / global_screen[ID_CHANNEL_A].text_font_width_;
		global_screen[ID_CHANNEL_A].text_mem_rows_ = global_screen[ID_CHANNEL_A].height_ / global_screen[ID_CHANNEL_A].text_font_height_;
		global_screen[ID_CHANNEL_A].text_cols_vis_ = global_screen[ID_CHANNEL_A].text_mem_cols_ - 8; // how morfe has it defined
		global_screen[ID_CHANNEL_A].text_rows_vis_ = global_screen[ID_CHANNEL_A].text_mem_rows_ - 4; // how morfe has it defined
	#endif	

	global_screen[ID_CHANNEL_A].rect_.MaxX = global_screen[ID_CHANNEL_A].width_;
	global_screen[ID_CHANNEL_A].rect_.MaxY = global_screen[ID_CHANNEL_A].height_;	

	global_screen[ID_CHANNEL_B].id_ = ID_CHANNEL_B;
	global_screen[ID_CHANNEL_B].text_temp_buffer_1_[0] = '\0';
	global_screen[ID_CHANNEL_B].text_temp_buffer_2_[0] = '\0';
	global_screen[ID_CHANNEL_B].text_font_width_ = TEXT_FONT_WIDTH_A2650;
	global_screen[ID_CHANNEL_B].text_font_height_ = TEXT_FONT_HEIGHT_A2650;
	global_screen[ID_CHANNEL_B].rect_.MinX = 0;
	global_screen[ID_CHANNEL_B].rect_.MinY = 0;
	
	#ifdef __f68
		global_screen[ID_CHANNEL_B].width_ = 640;	
		global_screen[ID_CHANNEL_B].height_ = 480;	
		global_screen[ID_CHANNEL_B].text_ram_ = TEXTB_RAM_A2560K;
		global_screen[ID_CHANNEL_B].text_attr_ram_ = TEXTB_ATTR_A2560K;
		global_screen[ID_CHANNEL_B].text_font_ram_ = FONT_MEMORY_BANKB_A2560K;
		global_screen[ID_CHANNEL_B].text_mem_cols_ = global_screen[ID_CHANNEL_B].width_ / global_screen[ID_CHANNEL_B].text_font_width_;
		global_screen[ID_CHANNEL_B].text_mem_rows_ = global_screen[ID_CHANNEL_B].height_ / global_screen[ID_CHANNEL_B].text_font_height_;
		global_screen[ID_CHANNEL_B].text_cols_vis_ = global_screen[ID_CHANNEL_B].text_mem_cols_ - 0; // how morfe has it defined. just guessing for f68
		global_screen[ID_CHANNEL_B].text_rows_vis_ = global_screen[ID_CHANNEL_B].text_mem_rows_ - 0; // how morfe has it defined. just guessing for f68
	#else
		global_screen[ID_CHANNEL_B].width_ = 640;	
		global_screen[ID_CHANNEL_B].height_ = 480;	
		global_screen[ID_CHANNEL_B].text_ram_ = TEXTB_RAM_A2560_MORFE;
		global_screen[ID_CHANNEL_B].text_attr_ram_ = TEXTB_ATTR_A2560_MORFE;
		global_screen[ID_CHANNEL_B].text_font_ram_ = FONT_MEMORY_BANKB_A2560_MORFE;
		global_screen[ID_CHANNEL_B].text_mem_cols_ = global_screen[ID_CHANNEL_B].width_ / global_screen[ID_CHANNEL_B].text_font_width_;
		global_screen[ID_CHANNEL_B].text_mem_rows_ = global_screen[ID_CHANNEL_B].height_ / global_screen[ID_CHANNEL_B].text_font_height_;
		global_screen[ID_CHANNEL_B].text_cols_vis_ = global_screen[ID_CHANNEL_B].text_mem_cols_ - 0; // how morfe has it defined
		global_screen[ID_CHANNEL_B].text_rows_vis_ = global_screen[ID_CHANNEL_B].text_mem_rows_ - 0; // how morfe has it defined
	#endif	

	//General_LogInitialize();

	//keyboard_test();
	   
	#if defined(RUN_TESTS)
		Text_RunTests();
	#endif
	
}