/*
 * lib_text.h
 *
*  Created on: Feb 19, 2022
 *      Author: micahbly
 */

#ifndef LIB_TEXT_H_
#define LIB_TEXT_H_


/* about this library: TextDisplay
 *
 * This handles writing and reading information to/from the VICKY's text mode memory
 *
 *** things this library needs to be able to do
 * x work with either channel A or channel B
 * x clear / fill an entire screen of text characters
 * x clear / fill an entire screen of text attributes
 * x invert the colors of a screen
 * x clear / fill a smaller-than-screen rectangular area of text/attrs
 * x Draw a char to a specified x, y coord
 * x Get the currently displayed character at the specified coord
 * x Set the foreground and background colors at the specified coord
 * x Set the attribute value at the specified coord
 * x Get the attribute value at the specified coord
 * x Get the foreground or background color at the specified coord
 * x draw a line using "graphic" characters
 * x draw a box using "graphic" characters
 * copy a full screen of text or attr from an off-screen buffer
 * copy a full screen of text or attr TO an off-screen buffer
 * copy a full screen of text and attr between channel A and B
 * copy a rectangular area of text or attr TO/FROM an off-screen buffer
 * x display a string at a specified x, y coord (no wrap)
 * display a string in a rectangular block on the screen, with wrap
 * display a string in a rectangular block on the screen, with wrap, taking a hook for a "display more" event, and scrolling text vertically up after hook func returns 'continue' (or exit, returning control to calling func, if hook returns 'stop')
 * replace current text font with another, loading from specified ram loc.
 */


/*****************************************************************************/
/*                                Includes                                   */
/*****************************************************************************/

// project includes
#include "lib_general_a2560.h"

// A2650 includes


/*****************************************************************************/
/*                            Macro Definitions                              */
/*****************************************************************************/

#define SCREEN_FOR_TEXT_ATTR	true	// param for functions with is_attr
#define SCREEN_FOR_TEXT_CHAR	false	// param for functions with is_attr

// these are from my C-128 code, but they match up to what is shown in the VICKY II wiki page
//   https://wiki.c256foenix.com/index.php?title=VICKY_II
//   probably makes them the default colors
#define COLOR_BLACK             0x00
#define COLOR_WHITE             0x01
#define COLOR_RED               0x02
#define COLOR_CYAN              0x03
#define COLOR_VIOLET            0x04
#define COLOR_GREEN				0x05
#define COLOR_BLUE              0x06
#define COLOR_YELLOW            0x07
#define COLOR_ORANGE            0x08
#define COLOR_BROWN             0x09
#define COLOR_LIGHTRED          0x0A
#define COLOR_GRAY1             0x0B
#define COLOR_GRAY2             0x0C
#define COLOR_LIGHTGREEN        0x0D
#define COLOR_LIGHTBLUE         0x0E
#define COLOR_GRAY3             0x0F
//.... BUT... at least in morfe, vicky2/text_lug.go is defined differently:
#define FG_COLOR_BLACK			0x00
#define FG_COLOR_DK_RED			0x01
#define FG_COLOR_GREEN			0x02
#define FG_COLOR_BLUE			0x03
#define FG_COLOR_OLIVE			0x04
#define FG_COLOR_MED_CYAN		0x05
#define FG_COLOR_VIOLET			0x06
#define FG_COLOR_LT_GRAY		0x07
#define FG_COLOR_ORANGE			0x08
#define FG_COLOR_BROWN			0x09
#define FG_COLOR_BLACK_RED		0x0A
#define FG_COLOR_BLACK_GREEN	0x0B
#define FG_COLOR_BLACK_BLUE		0x0C
#define FG_COLOR_DK_GRAY		0x0D
#define FG_COLOR_MED_GRAY		0x0E
#define FG_COLOR_WHITE			0x0F

#define BG_COLOR_BLACK			0x00
#define BG_COLOR_DK_RED			0x01
#define BG_COLOR_GREEN			0x02
#define BG_COLOR_BLUE			0x03
#define BG_COLOR_DK_OLIVE		0x04
#define BG_COLOR_DK_CYAN		0x05
#define BG_COLOR_DK_VIOLET		0x06
#define BG_COLOR_DK_GRAY		0x07
#define BG_COLOR_DK_ORANGE		0x08
#define BG_COLOR_BROWN			0x09
#define BG_COLOR_BLACK_RED		0x0A
#define BG_COLOR_BLACK_GREEN	0x0B
#define BG_COLOR_BLACK_BLUE		0x0C
#define BG_COLOR_MED_GRAY		0x0D
#define BG_COLOR_LT_GRAY		0x0E
#define BG_COLOR_WHITE			0x0F

// update: the numbers shown in vicky2 file in morfe don't match up to what's shown on screen, at least with a2650 config. eg, 20/00/00 is not a super dark blue, it's some totally bright thing. need to spend some time mapping these out better. But since user configurable, will wait until real machine comes and I can make sure of what's in flash rom. 

/*****************************************************************************/
/*                  Character-codes (IBM Page 437 charset)                   */
/*****************************************************************************/
// https://en.wikipedia.org/wiki/Code_page_437

#define CH_CHECKERED1	0xB0
#define CH_CHECKERED2	0xB1
#define CH_CHECKERED3	0xB2
#define CH_SOLID		0xDB	// inverse space
#define CH_WALL_H		0xCD
#define CH_WALL_V		0xBA
#define CH_WALL_UL		0xC9
#define CH_WALL_UR		0xBB
#define CH_WALL_LL		0xC8
#define CH_WALL_LR		0xBC
#define CH_INTERSECT	0xCE
#define CH_SMILEY1		0x01 // 
#define CH_SMILEY2		0x02 // 
#define CH_HEART		0x03 // 
#define CH_DIAMOND		0x04 // 
#define CH_CLUB			0x05 // 
#define CH_SPADE		0x06 // 
#define CH_MIDDOT		0x07 // 
#define CH_RIGHT		0x10 // Triangle pointing right
#define CH_LEFT			0x11 // Triangle pointing left
#define CH_UP			0x1E // Triangle pointing up
#define CH_DOWN			0x1F // Triangle pointing down


/*****************************************************************************/
/*                               Enumerations                                */
/*****************************************************************************/

typedef enum text_draw_choice
{
	char_only		= 0,
	attr_only 		= 1,
	char_and_attr	= 2,
} text_draw_choice;


/*****************************************************************************/
/*                                 Structs                                   */
/*****************************************************************************/


/*****************************************************************************/
/*                             Global Variables                              */
/*****************************************************************************/


/*****************************************************************************/
/*                       Public Function Prototypes                          */
/*****************************************************************************/


// ** NOTE: there is no destructor or constructor for this library, as it does not track any allocated memory. It works on the basis of a screen ID, which corresponds to the text memory for Vicky's Channel A and Channel B video memory.



// **** Block fill functions ****


// Fill attribute memory for the passed screen
// returns false on any error/invalid input.
boolean Text_FillAttrMem(signed int the_screen_id, unsigned char the_fill);

// Fill character memory for the passed screen
// returns false on any error/invalid input.
boolean Text_FillCharMem(signed int the_screen_id, unsigned char the_fill);

// Fill character and/or attribute memory for a specific box area
// returns false on any error/invalid input.
// this version uses char-by-char functions, so it is very slow.
boolean Text_FillBoxSlow(signed int the_screen_id, signed int x1, signed int y1, signed int x2, signed int y2, unsigned char the_char, unsigned char fore_color, unsigned char back_color, text_draw_choice the_draw_choice);

// Fill character and attribute memory for a specific box area
// returns false on any error/invalid input.
boolean Text_FillBox(signed int the_screen_id, signed int x1, signed int y1, signed int x2, signed int y2, unsigned char the_char, unsigned char fore_color, unsigned char back_color);

// Invert the colors of a rectangular block
// As this requires sampling each character cell, it is no faster to for entire screen as opposed to a subset box
boolean Text_InvertBox(signed int the_screen_id, signed int x1, signed int y1, signed int x2, signed int y2);




// **** FONT RELATED *****


// replace the current font data with the data at the passed memory buffer
boolean Text_UpdateFontData(signed int the_screen_id, unsigned char* new_font_data);

// test function to display all 256 font characters
boolean Text_ShowFontChars(signed int the_screen_id);




// **** Set char/attr functions *****


// Set a char at a specified x, y coord
boolean Text_SetCharAtXY(signed int the_screen_id, signed int x, signed int y, unsigned char the_char);

// Set the attribute value at a specified x, y coord
boolean Text_SetAttrAtXY(signed int the_screen_id, signed int x, signed int y, unsigned char fore_color, unsigned char back_color);

// Draw a char at a specified x, y coord, also setting the color attributes
boolean Text_SetCharAndColorAtXY(signed int the_screen_id, signed int x, signed int y, unsigned char the_char, unsigned char fore_color, unsigned char back_color);




// **** Get char/attr functions *****


// Get the char at a specified x, y coord
unsigned char Text_GetCharAtXY(signed int the_screen_id, signed int x, signed int y);

// Get the attribute value at a specified x, y coord
unsigned char Text_GetAttrAtXY(signed int the_screen_id, signed int x, signed int y);

// Get the foreground color at a specified x, y coord
unsigned char Text_GetForeColorAtXY(signed int the_screen_id, signed int x, signed int y);

// Get the background color at a specified x, y coord
unsigned char Text_GetBackColorAtXY(signed int the_screen_id, signed int x, signed int y);



// **** Drawing functions *****


// draws a horizontal line from specified coords, for n characters, using the specified char and/or attribute
boolean Text_DrawHLine(signed int the_screen_id, signed int x, signed int y, signed int the_line_len, unsigned char the_char, unsigned char fore_color, unsigned char back_color, text_draw_choice the_draw_choice);

// draws a vertical line from specified coords, for n characters, using the specified char and/or attribute
boolean Text_DrawVLine(signed int the_screen_id, signed int x, signed int y, signed int the_line_len, unsigned char the_char, unsigned char fore_color, unsigned char back_color, text_draw_choice the_draw_choice);

// draws a basic box based on 2 sets of coords, using the specified char and/or attribute for all cells
boolean Text_DrawBoxCoords(signed int the_screen_id, signed int x1, signed int y1, signed int x2, signed int y2, unsigned char the_char, unsigned char fore_color, unsigned char back_color, text_draw_choice the_draw_choice);

// draws a box based on 2 sets of coords, using the predetermined line and corner "graphics", and the passed colors
boolean Text_DrawBoxCoordsFancy(signed int the_screen_id, signed int x1, signed int y1, signed int x2, signed int y2, unsigned char fore_color, unsigned char back_color);

// draws a basic box based on start coords and width/height, using the specified char and/or attribute for all cells
boolean Text_DrawBox(signed int the_screen_id, signed int x, signed int y, signed int the_width, signed int the_height, unsigned char the_char, unsigned char fore_color, unsigned char back_color, text_draw_choice the_draw_choice);



// **** Draw string functions *****


// Draw a string at a specified x, y coord, also setting the color attributes
// Truncate, but still draw the string if it is too long to display on the line it started.
// No word wrap is performed. 
boolean Text_DrawStringAtXY(signed int the_screen_id, signed int x, signed int y, unsigned char* the_string, unsigned char fore_color, unsigned char back_color);



#endif /* LIB_TEXT_H_ */
