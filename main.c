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



/*****************************************************************************/
/*                        Public Function Definitions                        */
/*****************************************************************************/

// NOTE: lots of test junk getting put here temporarily until I can figure out why
// minunit and other stuff crashes morfe

boolean font_test(void);

boolean font_test(void)
{
	// alloc a block of memory for overwriting font data
	// need 256 chars * 8 bytes each
	
	unsigned char*	the_new_font_data;
	
	if ( (the_new_font_data = (unsigned char*)calloc(256 * 8, sizeof(char)) ) == NULL)
	{
		return false;
	}

	//DEBUG_OUT(("%s %d: font data chunk allocated ok", __func__, __LINE__));
	
	free(the_new_font_data);
	
	return true;
	//Text_ShowFontChars(ID_CHANNEL_B);
	//Text_UpdateFontData(ID_CHANNEL_B, (unsigned int*)0x000000);
}


int main(int argc, char* argv[])
{
	
	// move to startup function
	global_screen[ID_CHANNEL_A].id_ = ID_CHANNEL_A;
	global_screen[ID_CHANNEL_A].rect_.MinX = 0;
	global_screen[ID_CHANNEL_A].rect_.MinY = 0;
	global_screen[ID_CHANNEL_A].rect_.MaxX = CHANNEL_A_WIDTH;
	global_screen[ID_CHANNEL_A].rect_.MaxY = CHANNEL_A_HEIGHT;	
	global_screen[ID_CHANNEL_A].text_cols_ = TEXTA_WIDTH;
	global_screen[ID_CHANNEL_A].text_rows_ = TEXTA_HEIGHT;
	global_screen[ID_CHANNEL_A].text_ram_ = TEXTA_RAM;
	global_screen[ID_CHANNEL_A].text_attr_ram_ = TEXTA_ATTR;

	global_screen[ID_CHANNEL_B].id_ = ID_CHANNEL_B;
	global_screen[ID_CHANNEL_B].rect_.MinX = 0;
	global_screen[ID_CHANNEL_B].rect_.MinY = 0;
	global_screen[ID_CHANNEL_B].rect_.MaxX = CHANNEL_B_WIDTH;
	global_screen[ID_CHANNEL_B].rect_.MaxY = CHANNEL_B_HEIGHT;	
	global_screen[ID_CHANNEL_B].text_cols_ = TEXTB_WIDTH;
	global_screen[ID_CHANNEL_B].text_rows_ = TEXTB_HEIGHT;
	global_screen[ID_CHANNEL_B].text_ram_ = TEXTB_RAM;
	global_screen[ID_CHANNEL_B].text_attr_ram_ = TEXTB_ATTR;
	
#if defined(RUN_TESTS)
	Text_RunTests();
#endif
	
	
	
	//exit(0);
	//sys_exit(0);
	{
// 		void*	ptr_to_hell = (void*)0x20000;
// 		goto *ptr_to_hell;
		unsigned long int i;
		for (i=1; i>0; i++)
		{
			Text_FillBoxSlow(ID_CHANNEL_B, 0, 10, 50, 55, CH_CHECKERED3, FG_COLOR_BLUE, BG_COLOR_BLACK_BLUE, char_and_attr);
		}
	}
}