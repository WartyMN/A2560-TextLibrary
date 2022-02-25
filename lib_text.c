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
boolean Text_ValidateAll(signed int the_screen_id, signed int x, signed int y, unsigned char fore_color, unsigned char back_color);

// validate the coordinates are within the bounds of the specified screen
boolean Text_ValidateXY(signed int the_screen_id, signed int x, signed int y);

// calculate the VRAM location of the specified coordinate
unsigned char* Text_GetMemLocForXY(signed int the_screen_id, signed int x, signed int y, boolean for_attr);

// Fill attribute or text char memory. Writes to char memory if for_attr is false.
// calling function must validate the screen ID before passing!
// returns false on any error/invalid input.
boolean Text_FillMemory(signed int the_screen_id, boolean for_attr, unsigned char the_fill);

// Fill character and attribute memory for a specific box area
// calling function must validate screen id, coords, attribute value before passing!
// returns false on any error/invalid input.
boolean Text_FillMemoryBoxBoth(signed int the_screen_id, signed int x, signed int y, signed int width, signed int height, unsigned char the_char, unsigned char the_attribute_value);

// Fill character OR attribute memory for a specific box area
// calling function must validate screen id, coords, attribute value before passing!
// returns false on any error/invalid input.
boolean Text_FillMemoryBox(signed int the_screen_id, signed int x, signed int y, signed int width, signed int height, boolean for_attr, unsigned char the_fill);


/*****************************************************************************/
/*                       Private Function Definitions                        */
/*****************************************************************************/

// **** NOTE: all functions in private section REQUIRE pre-validated parameters. 
// **** NEVER call these from your own functions. Always use the public interface. You have been warned!


// validate screen id, x, y, and colors
boolean Text_ValidateAll(signed int the_screen_id, signed int x, signed int y, unsigned char fore_color, unsigned char back_color)
{
	if (the_screen_id != ID_CHANNEL_A && the_screen_id != ID_CHANNEL_B)
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
boolean Text_ValidateXY(signed int the_screen_id, signed int x, signed int y)
{
	signed int		max_row;
	signed int		max_col;
	
	max_col = global_screen[the_screen_id].text_cols_ - 1;
	max_row = global_screen[the_screen_id].text_rows_ - 1;
	
	if (x < 0 || x > max_col || y < 0 || y > max_row)
	{
		return false;
	}

	return true;
}


// calculate the VRAM location of the specified coordinate
unsigned char* Text_GetMemLocForXY(signed int the_screen_id, signed int x, signed int y, boolean for_attr)
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
boolean Text_FillMemory(signed int the_screen_id, boolean for_attr, unsigned char the_fill)
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


// Fill character and attribute memory for a specific box area
// calling function must validate screen id, coords, attribute value before passing!
// returns false on any error/invalid input.
boolean Text_FillMemoryBoxBoth(signed int the_screen_id, signed int x, signed int y, signed int width, signed int height, unsigned char the_char, unsigned char the_attribute_value)
{
	unsigned char*	the_char_loc;
	unsigned char*	the_attr_loc;
	signed int		max_row;
	
	// set up char and attribute memory initial loc
	the_char_loc = Text_GetMemLocForXY(the_screen_id, x, y, SCREEN_FOR_TEXT_CHAR);
	the_attr_loc = the_char_loc + (global_screen[the_screen_id].text_attr_ram_ - global_screen[the_screen_id].text_ram_);
	
	max_row = y + height;
	
	for (; y <= max_row; y++)
	{
		memset(the_char_loc, the_char, width);
		memset(the_attr_loc, the_attribute_value, width);
		the_char_loc += TEXT_COL_WIDTH_FOR_PLOTTING;
		the_attr_loc += TEXT_COL_WIDTH_FOR_PLOTTING;
	}
			
	return true;
}


// Fill character OR attribute memory for a specific box area
// calling function must validate screen id, coords, attribute value before passing!
// returns false on any error/invalid input.
boolean Text_FillMemoryBox(signed int the_screen_id, signed int x, signed int y, signed int width, signed int height, boolean for_attr, unsigned char the_fill)
{
	unsigned char*	the_write_loc;
	signed int		max_row;
	
	// set up initial loc
	the_write_loc = Text_GetMemLocForXY(the_screen_id, x, y, for_attr);
	
	max_row = y + height;
	
	for (; y <= max_row; y++)
	{
		memset(the_write_loc, the_fill, width);
		the_write_loc += TEXT_COL_WIDTH_FOR_PLOTTING;
	}
			
	return true;
}




/*****************************************************************************/
/*                        Public Function Definitions                        */
/*****************************************************************************/

// ** NOTE: there is no destructor or constructor for this library, as it does not track any allocated memory. It works on the basis of a screen ID, which corresponds to the text memory for Vicky's Channel A and Channel B video memory.


// Fill attribute memory for the passed screen
// returns false on any error/invalid input.
boolean Text_FillAttrMem(signed int the_screen_id, unsigned char the_fill)
{
	if (the_screen_id != ID_CHANNEL_A && the_screen_id != ID_CHANNEL_B)
	{
		LOG_ERR(("%s %d: illegal screen id", __func__, __LINE__));
		return false;
	}
	
 	return Text_FillMemory(the_screen_id, SCREEN_FOR_TEXT_ATTR, the_fill);
}

// Fill character memory for the passed screen
// returns false on any error/invalid input.
boolean Text_FillCharMem(signed int the_screen_id, unsigned char the_fill)
{
	if (the_screen_id != ID_CHANNEL_A && the_screen_id != ID_CHANNEL_B)
	{
		LOG_ERR(("%s %d: illegal screen id", __func__, __LINE__));
		return false;
	}
	
	return Text_FillMemory(the_screen_id, SCREEN_FOR_TEXT_CHAR, the_fill);
}


// Fill character and/or attribute memory for a specific box area
// returns false on any error/invalid input.
// this version uses char-by-char functions, so it is very slow.
boolean Text_FillBoxSlow(signed int the_screen_id, signed int x1, signed int y1, signed int x2, signed int y2, unsigned char the_char, unsigned char fore_color, unsigned char back_color, text_draw_choice the_draw_choice)
{
	unsigned char dx;
	
	if (!Text_ValidateAll(the_screen_id, x1, y1,fore_color, back_color))
	{
		LOG_ERR(("%s %d: illegal screen id, coordinate, or color", __func__, __LINE__));
		return false;
	}
	
	if (!Text_ValidateXY(the_screen_id, x2, y2))
	{
		LOG_ERR(("%s %d: illegal coordinate", __func__, __LINE__));
		return false;
	}

	if (x1 > x2 || y1 > y2)
	{
		LOG_ERR(("%s %d: illegal coordinates", __func__, __LINE__));
		return false;
	}

	// add 1 to line len, because desired behavior is a box that connects fully to the passed coords
	dx = x2 - x1 + 1;
	
	for (; y1 <= y2; y1++)
	{
		if (!Text_DrawHLine(the_screen_id, x1, y1, dx, the_char, fore_color, back_color, the_draw_choice))
		{
			LOG_ERR(("%s %d: fill failed", __func__, __LINE__));
			return false;
		}
	}
			
	return true;
}


// Fill character and attribute memory for a specific box area
// returns false on any error/invalid input.
boolean Text_FillBox(signed int the_screen_id, signed int x1, signed int y1, signed int x2, signed int y2, unsigned char the_char, unsigned char fore_color, unsigned char back_color)
{
	unsigned char	dy;
	unsigned char	dx;
	unsigned char	the_attribute_value;
	
	if (!Text_ValidateAll(the_screen_id, x1, y1, fore_color, back_color))
	{
		LOG_ERR(("%s %d: illegal screen id, coordinate, or color", __func__, __LINE__));
		return false;
	}
	
	if (!Text_ValidateXY(the_screen_id, x2, y2))
	{
		LOG_ERR(("%s %d: illegal coordinate", __func__, __LINE__));
		return false;
	}

	if (x1 > x2 || y1 > y2)
	{
		LOG_ERR(("%s %d: illegal coordinates", __func__, __LINE__));
		return false;
	}

	// add 1 to line len, because desired behavior is a box that connects fully to the passed coords
	dx = x2 - x1 + 1;
	dy = y2 - y1 + 1;

	// calculate attribute value from passed fore and back colors
	// LOGIC: text mode only supports 16 colors. lower 4 bits are back, upper 4 bits are foreground
	the_attribute_value = ((fore_color << 4) | back_color);

	return Text_FillMemoryBoxBoth(the_screen_id, x1, y1, dx, dy, the_char, the_attribute_value);
}


// Fill character memory for a specific box area
// returns false on any error/invalid input.
boolean Text_FillBoxCharOnly(signed int the_screen_id, signed int x1, signed int y1, signed int x2, signed int y2, unsigned char the_char)
{
	unsigned char	dy;
	unsigned char	dx;
	
	if (!Text_ValidateAll(the_screen_id, x1, y1, 0, 0))
	{
		LOG_ERR(("%s %d: illegal screen id, coordinate, or color", __func__, __LINE__));
		return false;
	}
	
	if (!Text_ValidateXY(the_screen_id, x2, y2))
	{
		LOG_ERR(("%s %d: illegal coordinate", __func__, __LINE__));
		return false;
	}

	if (x1 > x2 || y1 > y2)
	{
		LOG_ERR(("%s %d: illegal coordinates", __func__, __LINE__));
		return false;
	}

	// add 1 to line len, because desired behavior is a box that connects fully to the passed coords
	dx = x2 - x1 + 1;
	dy = y2 - y1 + 1;

	return Text_FillMemoryBox(the_screen_id, x1, y1, dx, dy, SCREEN_FOR_TEXT_CHAR, the_char);
}


// Fill attribute memory for a specific box area
// returns false on any error/invalid input.
boolean Text_FillBoxAttrOnly(signed int the_screen_id, signed int x1, signed int y1, signed int x2, signed int y2, unsigned char fore_color, unsigned char back_color)
{
	unsigned char	dy;
	unsigned char	dx;
	unsigned char	the_attribute_value;
	
	if (!Text_ValidateAll(the_screen_id, x1, y1, 0, 0))
	{
		LOG_ERR(("%s %d: illegal screen id, coordinate, or color", __func__, __LINE__));
		return false;
	}
	
	if (!Text_ValidateXY(the_screen_id, x2, y2))
	{
		LOG_ERR(("%s %d: illegal coordinate", __func__, __LINE__));
		return false;
	}

	if (x1 > x2 || y1 > y2)
	{
		LOG_ERR(("%s %d: illegal coordinates", __func__, __LINE__));
		return false;
	}

	// add 1 to line len, because desired behavior is a box that connects fully to the passed coords
	dx = x2 - x1 + 1;
	dy = y2 - y1 + 1;

	// calculate attribute value from passed fore and back colors
	// LOGIC: text mode only supports 16 colors. lower 4 bits are back, upper 4 bits are foreground
	the_attribute_value = ((fore_color << 4) | back_color);

	return Text_FillMemoryBox(the_screen_id, x1, y1, dx, dy, SCREEN_FOR_TEXT_ATTR, the_attribute_value);
}


// **** FONT RELATED *****

// replace the current font data with the data at the passed memory buffer
boolean Text_UpdateFontData(signed int the_screen_id, unsigned char* new_font_data)
{
	// TEST: check the mem locs I have work in morfe.
	memset(FONT_MEMORY_BANK0, 55, 8*256*1);
	// NO EFFECT: memset(FONT_MEMORY_BANK1, 255, 8*256*1);
	// seems to be expected: apparently the second font is no longer a thing

	return true;
}

// test function to display all 256 font characters
boolean Text_ShowFontChars(signed int the_screen_id)
{
	unsigned char	the_char = 0;
	unsigned char*	the_write_loc;
	unsigned short	i;

	if (the_screen_id != ID_CHANNEL_A && the_screen_id != ID_CHANNEL_B)
	{
		LOG_ERR(("%s %d: illegal screen id", __func__, __LINE__));
		return false;
	}

	the_write_loc = global_screen[the_screen_id].text_ram_;

	//printf("\n");

	for (i = 0; i < 256; i++)
	{
		*the_write_loc++ = the_char++;
		//printf("%c", the_char++);
	}

	//printf("\n");
	
	return true;
}



// **** Set char/attr functions *****


// Set a char at a specified x, y coord
boolean Text_SetCharAtXY(signed int the_screen_id, signed int x, signed int y, unsigned char the_char)
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
boolean Text_SetAttrAtXY(signed int the_screen_id, signed int x, signed int y, unsigned char fore_color, unsigned char back_color)
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
boolean Text_SetCharAndColorAtXY(signed int the_screen_id, signed int x, signed int y, unsigned char the_char, unsigned char fore_color, unsigned char back_color)
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
unsigned char Text_GetCharAtXY(signed int the_screen_id, signed int x, signed int y)
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
unsigned char Text_GetAttrAtXY(signed int the_screen_id, signed int x, signed int y)
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
unsigned char Text_GetForeColorAtXY(signed int the_screen_id, signed int x, signed int y)
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
unsigned char Text_GetBackColorAtXY(signed int the_screen_id, signed int x, signed int y)
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



// **** Drawing functions *****


// draws a horizontal line from specified coords, for n characters, using the specified char and/or attribute
boolean Text_DrawHLine(signed int the_screen_id, signed int x, signed int y, signed int the_line_len, unsigned char the_char, unsigned char fore_color, unsigned char back_color, text_draw_choice the_draw_choice)
{
	signed int	dx;
	
	if (!Text_ValidateAll(the_screen_id, x, y, fore_color, back_color))
	{
		LOG_ERR(("%s %d: illegal screen id, coordinate, or color", __func__, __LINE__));
		return false;
	}

	switch (the_draw_choice)
	{
		case char_only:
			for (dx = 0; dx < the_line_len; dx++)
			{
				Text_SetCharAtXY(the_screen_id, x + dx, y, the_char);
			}
			break;
			
		case attr_only:
			for (dx = 0; dx < the_line_len; dx++)
			{
				Text_SetAttrAtXY(the_screen_id, x + dx, y, fore_color, back_color);
			}
			break;
			
		case char_and_attr:
		default:
			for (dx = 0; dx < the_line_len; dx++)
			{
				Text_SetCharAndColorAtXY(the_screen_id, x + dx, y, the_char, fore_color, back_color);		
			}
			break;			
	}

	return true;
}


// draws a vertical line from specified coords, for n characters, using the specified char and/or attribute
boolean Text_DrawVLine(signed int the_screen_id, signed int x, signed int y, signed int the_line_len, unsigned char the_char, unsigned char fore_color, unsigned char back_color, text_draw_choice the_draw_choice)
{
	unsigned char dy;
	
	if (!Text_ValidateAll(the_screen_id, x, y, fore_color, back_color))
	{
		LOG_ERR(("%s %d: illegal screen id, coordinate, or color", __func__, __LINE__));
		return false;
	}

	switch (the_draw_choice)
	{
		case char_only:
			for (dy = 0; dy < the_line_len; dy++)
			{
				Text_SetCharAtXY(the_screen_id, x, y + dy, the_char);
			}
			break;
			
		case attr_only:
			for (dy = 0; dy < the_line_len; dy++)
			{
				Text_SetAttrAtXY(the_screen_id, x, y + dy, fore_color, back_color);
			}
			break;
			
		case char_and_attr:
		default:
			for (dy = 0; dy < the_line_len; dy++)
			{
				Text_SetCharAndColorAtXY(the_screen_id, x, y + dy, the_char, fore_color, back_color);		
			}
			break;			
	}
	
	return true;
}


// draws a basic box based on 2 sets of coords, using the specified char and/or attribute for all cells
boolean Text_DrawBoxCoords(signed int the_screen_id, signed int x1, signed int y1, signed int x2, signed int y2, unsigned char the_char, unsigned char fore_color, unsigned char back_color, text_draw_choice the_draw_choice)
{
	unsigned char dy;
	unsigned char dx;
	
	if (!Text_ValidateAll(the_screen_id, x1, y1,fore_color, back_color))
	{
		LOG_ERR(("%s %d: illegal screen id, coordinate, or color", __func__, __LINE__));
		return false;
	}
	
	if (!Text_ValidateXY(the_screen_id, x2, y2))
	{
		LOG_ERR(("%s %d: illegal coordinate", __func__, __LINE__));
		return false;
	}

	if (x1 > x2 || y1 > y2)
	{
		LOG_ERR(("%s %d: illegal coordinates", __func__, __LINE__));
		return false;
	}

	// add 1 to line len, because desired behavior is a box that connects fully to the passed coords
	dx = x2 - x1 + 1;
	dy = y2 - y1 + 1;
	
	if (!Text_DrawHLine(the_screen_id, x1, y1, dx, the_char, fore_color, back_color, the_draw_choice))
	{
		LOG_ERR(("%s %d: draw box failed", __func__, __LINE__));
		return false;
	}
	
	if (!Text_DrawVLine(the_screen_id, x2, y1, dy, the_char, fore_color, back_color, the_draw_choice))
	{
		LOG_ERR(("%s %d: draw box failed", __func__, __LINE__));
		return false;
	}
	
	if (!Text_DrawHLine(the_screen_id, x1, y2, dx, the_char, fore_color, back_color, the_draw_choice))
	{
		LOG_ERR(("%s %d: draw box failed", __func__, __LINE__));
		return false;
	}
	
	if (!Text_DrawVLine(the_screen_id, x1, y1, dy, the_char, fore_color, back_color, the_draw_choice))
	{
		LOG_ERR(("%s %d: draw box failed", __func__, __LINE__));
		return false;
	}
		
	return true;
}


// draws a box based on 2 sets of coords, using the predetermined line and corner "graphics", and the passed colors
boolean Text_DrawBoxCoordsFancy(signed int the_screen_id, signed int x1, signed int y1, signed int x2, signed int y2, unsigned char fore_color, unsigned char back_color)
{
	unsigned char	dy;
	unsigned char	dx;
	
	if (!Text_ValidateAll(the_screen_id, x1, y1,fore_color, back_color))
	{
		LOG_ERR(("%s %d: illegal screen id, coordinate, or color", __func__, __LINE__));
		return false;
	}
	
	if (!Text_ValidateXY(the_screen_id, x2, y2))
	{
		LOG_ERR(("%s %d: illegal coordinate", __func__, __LINE__));
		return false;
	}

	if (x1 > x2 || y1 > y2)
	{
		LOG_ERR(("%s %d: illegal coordinates", __func__, __LINE__));
		return false;
	}

	// add 1 to line len, because desired behavior is a box that connects fully to the passed coords
	dx = x2 - x1;
	dy = y2 - y1;
	
	// draw all lines one char shorter on each end so that we don't overdraw when we do corners
	
	if (!Text_DrawHLine(the_screen_id, x1+1, y1, dx, CH_WALL_H, fore_color, back_color, char_and_attr))
	{
		LOG_ERR(("%s %d: draw box failed", __func__, __LINE__));
		return false;
	}
	
	if (!Text_DrawHLine(the_screen_id, x1+1, y2, dx, CH_WALL_H, fore_color, back_color, char_and_attr))
	{
		LOG_ERR(("%s %d: draw box failed", __func__, __LINE__));
		return false;
	}
	
	if (!Text_DrawVLine(the_screen_id, x2, y1+1, dy, CH_WALL_V, fore_color, back_color, char_and_attr))
	{
		LOG_ERR(("%s %d: draw box failed", __func__, __LINE__));
		return false;
	}
	
	if (!Text_DrawVLine(the_screen_id, x1, y1+1, dy, CH_WALL_V, fore_color, back_color, char_and_attr))
	{
		LOG_ERR(("%s %d: draw box failed", __func__, __LINE__));
		return false;
	}
	
	// draw the 4 corners with dedicated corner pieces
	Text_SetCharAndColorAtXY(the_screen_id, x1, y1, CH_WALL_UL, fore_color, back_color);		
	Text_SetCharAndColorAtXY(the_screen_id, x2, y1, CH_WALL_UR, fore_color, back_color);		
	Text_SetCharAndColorAtXY(the_screen_id, x2, y2, CH_WALL_LR, fore_color, back_color);		
	Text_SetCharAndColorAtXY(the_screen_id, x1, y2, CH_WALL_LL, fore_color, back_color);		
	
	return true;
}


// draws a basic box based on start coords and width/height, using the specified char and/or attribute for all cells
boolean Text_DrawBox(signed int the_screen_id, signed int x, signed int y, signed int the_width, signed int the_height, unsigned char the_char, unsigned char fore_color, unsigned char back_color, text_draw_choice the_draw_choice)
{	
	if (!Text_ValidateAll(the_screen_id, x, y,fore_color, back_color))
	{
		LOG_ERR(("%s %d: illegal screen id, coordinate, or color", __func__, __LINE__));
		return false;
	}
	
	if (!Text_ValidateXY(the_screen_id, x + the_width - 1, y + the_height - 1))
	{
		LOG_ERR(("%s %d: illegal coordinate", __func__, __LINE__));
		return false;
	}

	if (!Text_DrawHLine(the_screen_id, x, y, the_width, the_char, fore_color, back_color, the_draw_choice))
	{
		LOG_ERR(("%s %d: draw box failed", __func__, __LINE__));
		return false;
	}
	
	if (!Text_DrawVLine(the_screen_id, x + the_width - 1, y, the_height, the_char, fore_color, back_color, the_draw_choice))
	{
		LOG_ERR(("%s %d: draw box failed", __func__, __LINE__));
		return false;
	}
	
	if (!Text_DrawHLine(the_screen_id, x, y + the_height - 1, the_width, the_char, fore_color, back_color, the_draw_choice))
	{
		LOG_ERR(("%s %d: draw box failed", __func__, __LINE__));
		return false;
	}
	
	if (!Text_DrawVLine(the_screen_id, x, y, the_height, the_char, fore_color, back_color, the_draw_choice))
	{
		LOG_ERR(("%s %d: draw box failed", __func__, __LINE__));
		return false;
	}
		
	return true;
}



// **** Draw string functions *****


// Draw a string at a specified x, y coord, also setting the color attributes
// Truncate, but still draw the string if it is too long to display on the line it started.
// No word wrap is performed. 
boolean Text_DrawStringAtXY(signed int the_screen_id, signed int x, signed int y, unsigned char* the_string, unsigned char fore_color, unsigned char back_color)
{
	unsigned char*	the_char_loc;
	unsigned char*	the_attr_loc;
	unsigned char	the_attribute_value;
	signed int		i;
	signed int		max_col;
	signed int		draw_len;
	
	if (!Text_ValidateAll(the_screen_id, x, y, fore_color, back_color))
	{
		LOG_ERR(("%s %d: illegal screen id, coordinate, or color", __func__, __LINE__));
		return false;
	}
	
	draw_len = General_Strnlen(the_string, TEXT_COL_WIDTH_FOR_PLOTTING); // can't be wider than the screen anyway
	max_col = global_screen[the_screen_id].text_cols_ - 1;
	
	if ( x + draw_len > max_col)
	{
		draw_len = (max_col - x) + 1;
	}
	
	//DEBUG_OUT(("%s %d: draw_len=%i, max_col=%i, x=%i", __func__, __LINE__, draw_len, max_col, x));
	
	// calculate attribute value from passed fore and back colors
	// LOGIC: text mode only supports 16 colors. lower 4 bits are back, upper 4 bits are foreground
	the_attribute_value = ((fore_color << 4) | back_color);

	// set up char and attribute memory initial loc
	the_char_loc = Text_GetMemLocForXY(the_screen_id, x, y, SCREEN_FOR_TEXT_CHAR);
	the_attr_loc = the_char_loc + (global_screen[the_screen_id].text_attr_ram_ - global_screen[the_screen_id].text_ram_);
	
	// draw the string
	for (i = 0; i < draw_len; i++)
	{
		*the_char_loc++ = the_string[i];
		*the_attr_loc++ = the_attribute_value;
	}
	
	return true;
}
