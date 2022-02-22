/*
 * lib_text.c
 *
 *  Created on: Feb 19, 2022
 *      Author: micahbly
 */





/*****************************************************************************/
/*                                Includes                                   */
/*****************************************************************************/

// project includes
#include "lib_text.h"
#include "lib_general.h"
#include "lib_general_a2560.h"

// C includes
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// A2650 includes


/*****************************************************************************/
/*                               Definitions                                 */
/*****************************************************************************/



/*****************************************************************************/
/*                               Enumerations                                */
/*****************************************************************************/



/*****************************************************************************/
/*                             Global Variables                              */
/*****************************************************************************/

extern Screen global_screen[2];



/*****************************************************************************/
/*                       Private Function Prototypes                         */
/*****************************************************************************/

// validate screen id, x, y, and colors
boolean Text_ValidateAll(ScreenID the_screen_id, signed int x, signed int y, unsigned char fore_color, unsigned char back_color);

// validate the coordinates are within the bounds of the specified screen
boolean Text_ValidateXY(ScreenID the_screen_id, signed int x, signed int y);

// calculate the VRAM location of the specified coordinate
unsigned char* Text_GetMemLocForXY(ScreenID the_screen_id, signed int x, signed int y, boolean for_attr);

// Fill attribute or text char memory. Writes to char memory if for_attr is false.
// calling function must validate the screen ID before passing!
// returns false on any error/invalid input.
boolean Text_FillMemory(ScreenID the_screen_id, boolean for_attr, unsigned char the_fill);



/*****************************************************************************/
/*                       Private Function Definitions                        */
/*****************************************************************************/

// **** NOTE: all functions in private section REQUIRE pre-validated parameters. 
// **** NEVER call these from your own functions. Always use the public interface. You have been warned!


// validate screen id, x, y, and colors
boolean Text_ValidateAll(ScreenID the_screen_id, signed int x, signed int y, unsigned char fore_color, unsigned char back_color)
{
	if (the_screen_id != CHANNEL_A && the_screen_id != CHANNEL_B)
	{
		LOG_INFO(("%s %d: illegal screen id", __func__, __LINE__));
		return false;
	}
			
	if (Text_ValidateXY(the_screen_id, x, y) == false)
	{
		LOG_INFO(("%s %d: illegal coordinates %li, %li", __func__, __LINE__, x, y));
		return false;
	}

	if (fore_color > 15 || back_color > 15)
	{
		LOG_INFO(("%s %d: illegal foreground (%u) or background (%u) color", __func__, __LINE__, fore_color, back_color));
		return false;
	}
	
	return true;
}


// validate the coordinates are within the bounds of the specified screen
boolean Text_ValidateXY(ScreenID the_screen_id, signed int x, signed int y)
{
	signed int		max_row;
	signed int		max_col;
	signed int		num_rows;
	signed int		num_cols;
	
	max_col = global_screen[the_screen_id].text_cols_ - 1;
	max_row = global_screen[the_screen_id].text_rows_ - 1;
	
	if (x < 0 || x > max_col || y < 0 || y > max_row)
	{
		return false;
	}

	return true;
}


// calculate the VRAM location of the specified coordinate
unsigned char* Text_GetMemLocForXY(ScreenID the_screen_id, signed int x, signed int y, boolean for_attr)
{
	unsigned char*	the_write_loc;
	//signed int		num_cols;
	
	// LOGIC:
	//   For plotting the VRAM, A2560 (or morfe?) uses a fixed with of 80 cols. 
	//   So even if only 72 are showing, the screen is arranged from 0-71 for row 1, then 80-151 for row 2, etc. 
	
	//num_cols = global_screen[the_screen_id].text_cols_;
	
//	the_write_loc = global_screen[the_screen_id].text_ram_ + (TEXT_COL_WIDTH_FOR_PLOTTING * y) + x;
	//the_write_loc = global_screen[the_screen_id].text_ram_ + (num_cols * y) + x;
	//DEBUG_OUT(("%s %d: screen=%i, x=%i, y=%i, num_cols=%i, calc=%i", __func__, __LINE__, (signed int)the_screen_id, x, y, num_cols, (num_cols * y) + x));
	
	if (for_attr)
	{
//		the_write_loc += global_screen[the_screen_id].text_attr_ram_ - global_screen[the_screen_id].text_ram_;
		the_write_loc = global_screen[the_screen_id].text_attr_ram_ + (TEXT_COL_WIDTH_FOR_PLOTTING * y) + x;
	}
	else
	{
		the_write_loc = global_screen[the_screen_id].text_ram_ + (TEXT_COL_WIDTH_FOR_PLOTTING * y) + x;
	}
	
	//DEBUG_OUT(("%s %d: screen=%i, x=%i, y=%i, for-attr=%i, calc=%i, loc=%p", __func__, __LINE__, (signed int)the_screen_id, x, y, for_attr, (TEXT_COL_WIDTH_FOR_PLOTTING * y) + x, the_write_loc));
	
	return the_write_loc;
}


// Fill attribute or text char memory. Writes to char memory if for_attr is false.
// calling function must validate the screen ID before passing!
// returns false on any error/invalid input.
boolean Text_FillMemory(ScreenID the_screen_id, boolean for_attr, unsigned char the_fill)
{
	unsigned char*	the_write_loc;
	unsigned long	the_write_len;
	
	if (for_attr)
	{
		the_write_loc = global_screen[the_screen_id].text_attr_ram_;
	}
	else
	{
		the_write_loc = global_screen[the_screen_id].text_ram_;
	}

	the_write_len = global_screen[the_screen_id].text_cols_ * global_screen[the_screen_id].text_rows_;
	
	memset(the_write_loc, the_fill, the_write_len);

	return true;
}




/*****************************************************************************/
/*                        Public Function Definitions                        */
/*****************************************************************************/

// ** NOTE: there is no destructor or constructor for this library, as it does not track any allocated memory. It works on the basis of a screen ID, which corresponds to the text memory for Vicky's Channel A and Channel B video memory.


// Fill attribute memory for the passed screen
// returns false on any error/invalid input.
boolean Text_FillAttrMem(ScreenID the_screen_id, unsigned char the_fill)
{
	if (the_screen_id != CHANNEL_A && the_screen_id != CHANNEL_B)
	{
		LOG_ERR(("%s %d: illegal screen id", __func__, __LINE__));
		return false;
	}
	
 	return Text_FillMemory(the_screen_id, SCREEN_FOR_TEXT_ATTR, the_fill);
}

// Fill character memory for the passed screen
// returns false on any error/invalid input.
boolean Text_FillCharMem(ScreenID the_screen_id, unsigned char the_fill)
{
	if (the_screen_id != CHANNEL_A && the_screen_id != CHANNEL_B)
	{
		LOG_ERR(("%s %d: illegal screen id", __func__, __LINE__));
		return false;
	}
	
	return Text_FillMemory(the_screen_id, SCREEN_FOR_TEXT_CHAR, the_fill);
}


// **** FONT RELATED *****

// replace the current font data with the data at the passed memory buffer
boolean Text_UpdateFontData(ScreenID the_screen_id, unsigned char* new_font_data)
{
	// TEST: check the mem locs I have work in morfe.
	memset(FONT_MEMORY_BANK0, 55, 8*256*1);
	// NO EFFECT: memset(FONT_MEMORY_BANK1, 255, 8*256*1);
	// seems to be expected: apparently the second font is no longer a thing

	return true;
}

// test function to display all 256 font characters
boolean Text_ShowFontChars(ScreenID the_screen_id)
{
	unsigned char	the_char = 1;
	unsigned char*	the_write_loc;

	if (the_screen_id != CHANNEL_A && the_screen_id != CHANNEL_B)
	{
		LOG_ERR(("%s %d: illegal screen id", __func__, __LINE__));
		return false;
	}

	the_write_loc = global_screen[the_screen_id].text_ram_;

	//printf("\n");

	do
	{
		*the_write_loc++ = the_char;
		//printf("%c", the_char++);
	} while (the_char > 0);

	//printf("\n");
	
	return true;
}



// **** Set char/attr functions *****


// Set a char at a specified x, y coord
boolean Text_SetCharAtXY(ScreenID the_screen_id, signed int x, signed int y, unsigned char the_char)
{
	unsigned char*	the_write_loc;
	
	if (!Text_ValidateAll(the_screen_id, x, y, 0, 0))
	{
		LOG_ERR(("%s %d: illegal screen id or coordinate", __func__, __LINE__));
		return false;
	}
	
	the_write_loc = Text_GetMemLocForXY(the_screen_id, x, y, SCREEN_FOR_TEXT_CHAR);	
 	*the_write_loc = the_char;
	
	return true;
}


// Set the attribute value at a specified x, y coord
boolean Text_SetAttrAtXY(ScreenID the_screen_id, signed int x, signed int y, unsigned char fore_color, unsigned char back_color)
{
	unsigned char*	the_write_loc;
	unsigned char	the_attribute_value;
	
	if (!Text_ValidateAll(the_screen_id, x, y, fore_color, back_color))
	{
		LOG_ERR(("%s %d: illegal screen id, coordinate, or color", __func__, __LINE__));
		return false;
	}

	// calculate attribute value from passed fore and back colors
	// LOGIC: text mode only supports 16 colors. lower 4 bits are back, upper 4 bits are foreground
	the_attribute_value = ((fore_color << 4) | back_color);
	
	the_write_loc = Text_GetMemLocForXY(the_screen_id, x, y, SCREEN_FOR_TEXT_ATTR);	
 	*the_write_loc = the_attribute_value;
	
	return true;
}


// Draw a char at a specified x, y coord, also setting the color attributes
boolean Text_SetCharAndColorAtXY(ScreenID the_screen_id, signed int x, signed int y, unsigned char the_char, unsigned char fore_color, unsigned char back_color)
{
	unsigned char*	the_write_loc;
	unsigned char	the_attribute_value;
	
	if (!Text_ValidateAll(the_screen_id, x, y, fore_color, back_color))
	{
		LOG_ERR(("%s %d: illegal screen id, coordinate, or color", __func__, __LINE__));
		return false;
	}
		
	// calculate attribute value from passed fore and back colors
	// LOGIC: text mode only supports 16 colors. lower 4 bits are back, upper 4 bits are foreground
	the_attribute_value = ((fore_color << 4) | back_color);
	
	// write character memory
	the_write_loc = Text_GetMemLocForXY(the_screen_id, x, y, SCREEN_FOR_TEXT_CHAR);	
	*the_write_loc = the_char;
	
	// write attribute memory (reuse same calc, just add attr ram delta)
	the_write_loc += global_screen[the_screen_id].text_attr_ram_ - global_screen[the_screen_id].text_ram_;
	*the_write_loc = the_attribute_value;
	
	return true;
}


// **** Get char/attr functions *****


// Get the char at a specified x, y coord
unsigned char Text_GetCharAtXY(ScreenID the_screen_id, signed int x, signed int y)
{
	unsigned char*	the_read_loc;
	unsigned char	the_char;
	
	if (!Text_ValidateAll(the_screen_id, x, y, 0, 0))
	{
		LOG_ERR(("%s %d: illegal screen id or coordinate", __func__, __LINE__));
		return false;
	}
	
	the_read_loc = Text_GetMemLocForXY(the_screen_id, x, y, SCREEN_FOR_TEXT_CHAR);	
 	the_char = (unsigned char)*the_read_loc;
	
	return the_char;
}


// Get the attribute value at a specified x, y coord
unsigned char Text_GetAttrAtXY(ScreenID the_screen_id, signed int x, signed int y)
{
	unsigned char*	the_read_loc;
	unsigned char	the_value;
	
	if (!Text_ValidateAll(the_screen_id, x, y, 0, 0))
	{
		LOG_ERR(("%s %d: illegal screen id or coordinate", __func__, __LINE__));
		return false;
	}
	
	the_read_loc = Text_GetMemLocForXY(the_screen_id, x, y, SCREEN_FOR_TEXT_ATTR);	
 	the_value = (unsigned char)*the_read_loc;
	
	return the_value;
}


// Get the foreground color at a specified x, y coord
unsigned char Text_GetForeColorAtXY(ScreenID the_screen_id, signed int x, signed int y)
{
	unsigned char*	the_read_loc;
	unsigned char	the_value;
	unsigned char	the_color;
	
	if (!Text_ValidateAll(the_screen_id, x, y, 0, 0))
	{
		LOG_ERR(("%s %d: illegal screen id or coordinate", __func__, __LINE__));
		return false;
	}
	
	the_read_loc = Text_GetMemLocForXY(the_screen_id, x, y, SCREEN_FOR_TEXT_ATTR);	
 	the_value = (unsigned char)*the_read_loc;
	the_color = (the_value & 0xF0) >> 4;
	
	return the_color;
}


// Get the background color at a specified x, y coord
unsigned char Text_GetBackColorAtXY(ScreenID the_screen_id, signed int x, signed int y)
{
	unsigned char*	the_read_loc;
	unsigned char	the_value;
	unsigned char	the_color;
	
	if (!Text_ValidateAll(the_screen_id, x, y, 0, 0))
	{
		LOG_ERR(("%s %d: illegal screen id or coordinate", __func__, __LINE__));
		return false;
	}
	
	the_read_loc = Text_GetMemLocForXY(the_screen_id, x, y, SCREEN_FOR_TEXT_ATTR);	
 	the_value = (unsigned char)*the_read_loc;
	the_color = (the_value & 0x0F);
	
	return the_color;
}
