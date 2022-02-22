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

#define RUN_TESTSx	1	// define "RUN_TESTS" to include ability to run tests. If undefined, this class will not be aware of tests.

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
	//Text_ShowFontChars(CHANNEL_B);
	//Text_UpdateFontData(CHANNEL_B, (unsigned int*)0x000000);
}


int main(int argc, char* argv[])
{
	
	// move to startup function
	global_screen[CHANNEL_A].id_ = CHANNEL_A;
	global_screen[CHANNEL_A].rect_.MinX = 0;
	global_screen[CHANNEL_A].rect_.MinY = 0;
	global_screen[CHANNEL_A].rect_.MaxX = CHANNEL_A_WIDTH;
	global_screen[CHANNEL_A].rect_.MaxY = CHANNEL_A_HEIGHT;	
	global_screen[CHANNEL_A].text_cols_ = TEXTA_WIDTH;
	global_screen[CHANNEL_A].text_rows_ = TEXTA_HEIGHT;
	global_screen[CHANNEL_A].text_ram_ = TEXTA_RAM;
	global_screen[CHANNEL_A].text_attr_ram_ = TEXTA_ATTR;

	global_screen[CHANNEL_B].id_ = CHANNEL_B;
	global_screen[CHANNEL_B].rect_.MinX = 0;
	global_screen[CHANNEL_B].rect_.MinY = 0;
	global_screen[CHANNEL_B].rect_.MaxX = CHANNEL_B_WIDTH;
	global_screen[CHANNEL_B].rect_.MaxY = CHANNEL_B_HEIGHT;	
	global_screen[CHANNEL_B].text_cols_ = TEXTB_WIDTH;
	global_screen[CHANNEL_B].text_rows_ = TEXTB_HEIGHT;
	global_screen[CHANNEL_B].text_ram_ = TEXTB_RAM;
	global_screen[CHANNEL_B].text_attr_ram_ = TEXTB_ATTR;
	
#if defined(RUN_TESTS)
//	Text_RunTests();
#endif

	// test colors
	if (1)
	{
		Text_FillCharMem(CHANNEL_B, 4); // 4 = diamond. good mix of fore/back color
		Text_FillAttrMem(CHANNEL_B, 148);
		// 31=black on white
		// 64=dark blue on black
		// 96=dark cyan on black
		// 112=medium gray on black
		// 128=medium gray on black
		// 138=black on light green
		// 139=black on bright yellow
		// 140=gray? on medium blue
		// 141=gray? on pink
		// 142=gray? on light cyan

		// 143=black/gray? on white
		// 15=black on white
	
		// 144=red on black
		// 16=dark red on black
	
		// 145=light red on dark red
		// 17=dark red on dark red
	
		// 146=light red on medium green
		// 18=dark red on medium green
	
		// 147=light red on olive
		// 19=dark red on medium green?
	
		// 148=light red on dark blue
	}
	
	
//	if (!font_test())
	{
		//DEBUG_OUT(("%s %d: font test failed", __func__, __LINE__));
	}

	// muck with char font memory to prove we can
	if (0)
	{
		if (!Text_UpdateFontData(CHANNEL_B, 0))
		{
			//printf("%s %d: Text_ShowFontChars returned error\n", __func__, __LINE__);
		}
	}
	
	
	// test show fonts. 256 chars / 80 cols screen = 3.2 rows needed. 
	if (0)
	{
		if (!Text_ShowFontChars(CHANNEL_B))
		{
			//printf("%s %d: Text_ShowFontChars returned error\n", __func__, __LINE__);
		}
	}
	
	// test char placement
	if (0)
	{
		signed int	x;
		signed int	y;
		
		for (y = 4; y < 40; y = y+2)
		{
			for (x = 0; x < 50; x++)
			{
				if (!Text_SetCharAtXY(CHANNEL_B, 'X', x, y))
				{
					printf("Oh, no!\n");
				}
			}
		}
	}

// WHY does running TWO of these work (any 2), but a third causes a morfe crash???? 

	// test char and color placement
	if (1)
	{
// 		// white char on black back
// 		Text_SetCharAndColorAtXY(CHANNEL_B, 30, COLOR_WHITE, COLOR_BLACK, 0, 4);
//  		
//  		// black char on black back
//  		Text_SetCharAndColorAtXY(CHANNEL_B, 30, COLOR_BLACK, COLOR_BLACK, 2, 4);
 		
 		// red char on black back
 //		Text_SetCharAndColorAtXY(CHANNEL_B, 30, COLOR_RED+1, COLOR_BLACK, 2, 6);
 		
 		// cyan char on black back
 //		Text_SetCharAndColorAtXY(CHANNEL_B, 30, COLOR_CYAN+1, COLOR_BLACK, 2, 8);
 		
 		// violet char on black back
// 		Text_SetCharAndColorAtXY(CHANNEL_B, 30, COLOR_VIOLET+1, COLOR_BLACK, 2, 10);
 		
//  		// green char on black back
//  		Text_SetCharAndColorAtXY(CHANNEL_B, 30, FG_COLOR_GREEN, BG_COLOR_GREEN, 2, 12);
//  		
//  		// blue char on black back
//  		Text_SetCharAndColorAtXY(CHANNEL_B, 30, FG_COLOR_MED_GRAY, BG_COLOR_DK_GRAY, 2, 14);
//  		
//  		// yellow char on black back
//		Text_SetCharAndColorAtXY(CHANNEL_B, 30, FG_COLOR_WHITE, BG_COLOR_BLACK_RED, 2, 16);
//  		
//  		// orange char on black back
  		Text_SetCharAndColorAtXY(CHANNEL_B, 30, COLOR_ORANGE, COLOR_BLACK, 2, 18);
 		
//  		// brown char on black back
  		Text_SetCharAndColorAtXY(CHANNEL_B, 30, COLOR_BROWN, COLOR_BLACK, 2, 20);
//  		
//  		// lt red char on black back
//  		Text_SetCharAndColorAtXY(CHANNEL_B, 30, COLOR_LIGHTRED, COLOR_BLACK, 2, 22);
//  		
//  		// dk gray char on black back
//  		Text_SetCharAndColorAtXY(CHANNEL_B, 30, COLOR_GRAY1, COLOR_BLACK, 2, 24);
//  		
//  		// med gray char on black back
//  		Text_SetCharAndColorAtXY(CHANNEL_B, 30, COLOR_GRAY2, COLOR_BLACK, 2, 26);
//  		
//  		// lt green char on black back
//  		Text_SetCharAndColorAtXY(CHANNEL_B, 30, COLOR_LIGHTGREEN, COLOR_BLACK, 2, 28);
//  		
//  		// lt blue char on black back
//  		Text_SetCharAndColorAtXY(CHANNEL_B, 30, COLOR_LIGHTBLUE, COLOR_BLACK, 2, 30);
//  		
//  		// lt gray char on black back
//  		Text_SetCharAndColorAtXY(CHANNEL_B, 30, COLOR_GRAY3, COLOR_BLACK, 2, 32);
		
//  		// white char on green back
//  		Text_SetCharAndColorAtXY(CHANNEL_B, 30, COLOR_WHITE, COLOR_GREEN, 0, 5);
//  		
//  		// light blue char on green back
//  		Text_SetCharAndColorAtXY(CHANNEL_B, 30, COLOR_LIGHTBLUE, COLOR_GREEN, 2, 5);

// #define FG_COLOR_BLACK			0x00
// #define FG_COLOR_DK_RED			0x01
// #define FG_COLOR_GREEN			0x02
// #define FG_COLOR_BLUE			0x03
// #define FG_COLOR_OLIVE			0x04
// #define FG_COLOR_MED_CYAN		0x05
// #define FG_COLOR_VIOLET			0x06
// #define FG_COLOR_LT_GRAY		0x07
// #define FG_COLOR_ORANGE			0x08
// #define FG_COLOR_BROWN			0x09
// #define FG_COLOR_BLACK_RED		0x0A
// #define FG_COLOR_BLACK_GREEN	0x0B
// #define FG_COLOR_BLACK_BLUE		0x0C
// #define FG_COLOR_DK_GRAY		0x0D
// #define FG_COLOR_MED_GRAY		0x0E
// #define FG_COLOR_WHITE			0x0F
// 
// #define BG_COLOR_BLACK			0x00
// #define BG_COLOR_DK_RED			0x01
// #define BG_COLOR_GREEN			0x02
// #define BG_COLOR_BLUE			0x03
// #define BG_COLOR_DK_OLIVE		0x04
// #define BG_COLOR_DK_CYAN		0x05
// #define BG_COLOR_DK_VIOLET		0x06
// #define BG_COLOR_DK_GRAY		0x07
// #define BG_COLOR_DK_ORANGE		0x08
// #define BG_COLOR_BROWN			0x09
// #define BG_COLOR_BLACK_RED		0x0A
// #define BG_COLOR_BLACK_GREEN	0x0B
// #define BG_COLOR_BLACK_BLUE		0x0C
// #define BG_COLOR_MED_GRAY		0x0D
// #define BG_COLOR_LT_GRAY		0x0E
// #define BG_COLOR_WHITE			0x0F
	}
	
	printf("Hello 21st time, A2560 World!\n");
}