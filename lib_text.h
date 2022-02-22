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
 * work with either channel A or channel B
 * clear / fill an entire screen of text characters
 * clear / fill an entire screen of text attributes
 * clear / fill a smaller-than-screen rectangular area of text/attrs
 * Draw a string to a specified x, y coord (no wrap)
 * Draw a char to a specified x, y coord
 * Get the currently displayed character at the specified coord
 * Get the attribute value at the specified coord
 * copy a full screen of text or attr from an off-screen buffer
 * copy a full screen of text or attr TO an off-screen buffer
 * copy a rectangular area of text or attr TO/FROM an off-screen buffer
 * invert the colors of a screen
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
/*                               Enumerations                                */
/*****************************************************************************/



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



// Fill attribute memory for the passed screen
// returns false on any error/invalid input.
boolean Text_FillAttrMem(ScreenID the_screen_id, unsigned char the_fill);

// Fill character memory for the passed screen
// returns false on any error/invalid input.
boolean Text_FillCharMem(ScreenID the_screen_id, unsigned char the_fill);

// replace the current font data with the data at the passed memory buffer
boolean Text_UpdateFontData(ScreenID the_screen_id, unsigned char* new_font_data);

// test function to display all 256 font characters
boolean Text_ShowFontChars(ScreenID the_screen_id);


// **** FONT RELATED *****


// **** CHAR put/get functions *****

// Draw a char at a specified x, y coord
boolean Text_SetCharAtXY(ScreenID the_screen_id, char the_char, signed int x, signed int y);

// Draw a char at a specified x, y coord, also setting the color attributes
boolean Text_SetCharAndColorAtXY(ScreenID the_screen_id, char the_char, unsigned char fore_color, unsigned char back_color, signed int x, signed int y);


#endif /* LIB_TEXT_H_ */
