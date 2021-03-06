/*
 * lib_general.c
 *
 *  Created on: Feb 19, 2022
 *      Author: micahbly
 *
 *  - adapted from Amiga WB2K project's lib_general.c
 */





/*****************************************************************************/
/*                                Includes                                   */
/*****************************************************************************/

// project includes
#include "lib_general.h"

// C includes
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>
#include <errno.h>
#include <math.h>

// A2560 includes
#include <mcp/syscalls.h>
#include <mb/a2560_platform.h>
#include <mb/lib_sys.h>


/*****************************************************************************/
/*                               Definitions                                 */
/*****************************************************************************/




/*****************************************************************************/
/*                             Global Variables                              */
/*****************************************************************************/

static char				debug_buffer[256];	// create once, use for every debug and logging function
static const char*		kDebugFlag[5] = {
							"[ERROR]",
							"[WARNING]",
							"[INFO]",
							"[DEBUG]",
							"[ALLOC]"
						};

static FILE*			global_log_file;


/*****************************************************************************/
/*                       Private Function Prototypes                         */
/*****************************************************************************/

//! \cond PRIVATE

// Convert a (positive-only) string integer to an unsigned long integer. returns false in event of error
boolean General_StringToUnsignedLong(const char* the_string_value, unsigned long* the_conversion);

// PRIVATE - no checking of parameters
// copy the specified length of text from this_line_start into the write buffer, and overwrite the null terminator with a line break character
// advance the read and write buffers to the next position
void General_WrapParaWriteLine(char** source, char** target, signed int write_len);

// PRIVATE - no checking of parameters
// passed a string with no line breaks in it, and a buffer to write into, copies the string contents into the target buffer, performing line breaks as it goes
// stops when all characters have been processed, or when all available vertical space has been used up.
signed int General_WrapPara(char* this_line_start, char* formatted_text, signed int remaining_len, signed int max_width, signed int remaining_v_pixels, signed int one_char_width, signed int one_row_height, Font* the_font, signed int (* measure_function)(Font*, char*, signed int, signed int, signed int));

//! \endcond



/*****************************************************************************/
/*                       Private Function Definitions                        */
/*****************************************************************************/

//! \cond PRIVATE


// PRIVATE - no checking of parameters
// copy the specified length of text from this_line_start into the write buffer, and overwrite the null terminator with a line break character
// advance the read and write buffers to the next position
void General_WrapParaWriteLine(char** source, char** target, signed int write_len)
{
	General_Strlcpy(*target, *source, write_len);
	*(*target + write_len - 1) = '\n'; // overwrite the \0 from strlcpy				
	*target += write_len; // move formatted_text up to the next spot for writing
	*source += write_len; // move unformatted_text up to the next spot for reading
}


// PRIVATE - no checking of parameters
// passed a string with no line breaks in it, and a buffer to write into, copies the string contents into the target buffer, performing line breaks as it goes
// stops when all characters have been processed, or when all available vertical space has been used up.
signed int General_WrapPara(char* this_line_start, char* formatted_text, signed int remaining_len, signed int max_width, signed int remaining_v_pixels, signed int one_char_width, signed int one_row_height, Font* the_font, signed int (* measure_function)(Font*, char*, signed int, signed int, signed int))
{
	signed int		v_pixels = 0;
	boolean			line_complete;
	signed int		next_line_len;
// 	char*			start_of_para = this_line_start;
// 	char*			start_of_formatted = formatted_text;

	// Initial condition is next line and this line are the same thing. 
	// Next line start will move word by word to the right until what's left no longer fits on a line
	// then this_line is written out, and next_line and this line are equal again
	next_line_len = remaining_len;
	
	//DEBUG_OUT(("%s %d: remaining_v_pixels=%i", __func__ , __LINE__, remaining_v_pixels));
	
	// outer loop: continue until the entire string has been copied to formatted text, or until we have exceeded the v pixel budget
	do
	{
		signed int		this_line_len = 0;
		char*			next_line_start = this_line_start;

		line_complete = false;

		// inner loop: Each pass is one word. Process until the end of a line is reached
		while (line_complete == false)
		{
			signed int			next_soft_break_pos;
		
			next_soft_break_pos = General_StrFindNextWordEnd(next_line_start, next_line_len);
			
			if (next_soft_break_pos < 0)
			{
				signed int		chars_that_fit;

				// there are no more word breaks in this string. 2 possible conditions:
				//   A. We previously started a new line, and have at least 1 word. there's ONE more word then end of string
				//   B. We are at the start of processing a line, and came to the end of the string. There is one word, it may be larger than will fit. 
				// The word may fit on the current line, or it may not. We haven't measured it. 
				//   if it's longer than the allowed line width, it must be broken mid-word.
				
				// check if the word will fit on the line, and if so, add it and exit the function.
				//   if it will not fit:
				//   A) if already have 1+ words on the line, wrap at this point, add the word, exit the function
				//   B) if no words on line, force break the word at max width and continue.
				
				signed int	proposed_new_line_len = this_line_len + next_line_len;
				
				chars_that_fit = (*measure_function)(the_font, this_line_start, proposed_new_line_len, max_width, one_char_width);
				
				if (chars_that_fit >= proposed_new_line_len)
				{
					// the upcoming word will fit on current line
					// extend length of current line; push start of next line to position past the word
					General_WrapParaWriteLine(&this_line_start, &formatted_text, proposed_new_line_len);

					this_line_len = 0;

					this_line_start = NULL;
					next_line_start = NULL;
					next_line_len = 0;
				}
				else
				{
					// either we got an error, or all the characters will not fit on this line
					// if we have a current line, end it and add line break + remaining string. if no current line, force break the rest of the string at max width and continue.

					// any words on the current line?
					if (this_line_len > 0)
					{
						// we already have some words, so end the current line and continue
						General_WrapParaWriteLine(&this_line_start, &formatted_text, this_line_len);
						this_line_len = 0;
					}
					else
					{
						// the whole string is word-break-less: we must force a break
						General_WrapParaWriteLine(&this_line_start, &formatted_text, chars_that_fit);
					}
				}
				
				line_complete = true;	
			}
			else
			{
				// we found the next word break. test if we can fit everything to that point. if so, continue on. if not, back up to last pos and break there

				signed int	proposed_new_line_len = this_line_len + next_soft_break_pos;
				signed int	chars_that_fit;
			
				chars_that_fit = (*measure_function)(the_font, this_line_start, proposed_new_line_len, max_width, one_char_width);
				
				if (chars_that_fit >= proposed_new_line_len)
				{
					// the upcoming word will fit on current line
					// extend length of current line; push start of next line to position past the word
					this_line_len = proposed_new_line_len;
					next_line_start += next_soft_break_pos;
					next_line_len -= next_soft_break_pos;
				}
				else
				{
					// either we got an error, or all the characters will not fit on this line
					// in either case, end the current line, write it to formatted

					General_WrapParaWriteLine(&this_line_start, &formatted_text, this_line_len);

					this_line_len = 0;

					line_complete = true;

					//DEBUG_OUT(("%s %d: print out of formatted_text after copy...", __func__ , __LINE__));
					//General_PrintBufferCharacters(formatted_text-8, (unsigned short)remaining_len);
					//DEBUG_OUT(("%s %d: line complete. next_line_len=%i, this_line_start='%s'", __func__ , __LINE__, next_line_len, this_line_start));		

					if (chars_that_fit == -1)
					{
						// handle error condition: having completed the current line, force function to exit with an error.
						LOG_ERR(("%s %d: next_line_len=%i, this_line_start='%s'", __func__, __LINE__, next_line_len, this_line_start));	
						*(formatted_text) = '\0';
						return -1;
					}
				}
			}
		}
		
		v_pixels += one_row_height;
		remaining_v_pixels -= one_row_height;
		
	} while (this_line_start != NULL && remaining_v_pixels > 0);
	
// 	DEBUG_OUT(("%s %d: print out of final version of para", __func__ , __LINE__));
// 	General_PrintBufferCharacters(start_of_formatted, (unsigned short)80);
// 	
// 	DEBUG_OUT(("%s %d: remaining_v_pixels=%i, v_pixels=%i", __func__ , __LINE__, remaining_v_pixels, v_pixels));
	
	return v_pixels;
}


// Convert a (positive-only) string integer to an unsigned long integer. returns false in event of error
boolean General_StringToUnsignedLong(const char* the_string_value, unsigned long* the_conversion)
{
	char*			temp;
	unsigned long	val = 0;
	boolean			safe_conversion = true;
	
	errno = 0;
	// LOGIC: errno will be changed by strtol if an overrun or other error occurs
	

	// do conversion... carefully

	val = strtol((char*)the_string_value, (char**)&temp, 0);

	if (temp == the_string_value || *temp != '\0' || ((val == LONG_MIN || val == LONG_MAX) && errno == ERANGE))
	{
		//printf("General_StringToUnsignedLong: Could not convert '%s' to long and leftover string is: '%s' \n", the_string_value, temp);
		val = LONG_MAX;
		safe_conversion = false;
	}

	if (val > INT_MAX)
	{
		//printf("General_StringToUnsignedLong: Could not convert '%s' to int: greater than allowable value.\n", the_string_value);
		val = LONG_MAX;
		safe_conversion = false;
	}

	*the_conversion = (unsigned long)val;
	return safe_conversion;
}


//! \endcond


/*****************************************************************************/
/*                        Public Function Definitions                        */
/*****************************************************************************/


//! Format a string by wrapping and trimming to fit the passed width and height. 
//! If the string cannot be displayed in the specified height and width, processing will stop, but it is not an error condition
//! @param	orig_string: pointer to a string pointer that holds the text to be formatted. Upon return, this pointer will point to the next character after the last processed character (if the string was too long to fit). If the entire string fits, this pointer will not be adjusted.
//! @param	formatted_string: pointer to a string pointer to an empty string buffer that will be filled with the formatted version of the text.
//! @param	max_chars_to_format: the length of the string to format (in characters). If max_chars_to_format is less than the length of string, processing will stop after that many characters.
//! @param	max_width: the width into which the text must fit, in pixels. 
//! @param	max_height: the height into which the text must fit, in pixels. Pass a 0 to disable the governor on vertical space. 
//! @param	one_char_width: the width in pixels, of one character. NOTE: This is only used for fixed-width, text mode operations. 
//! @param	one_row_height: the height in pixels, of one row of text, including any leading. 
//! @param	the_font: the font object to be used in measuring width. This is optional and ignore if called for text mode operations.
//! @param	measure_function: pointer to the function responsible for measuring the graphical width of a string 
//! @return Returns number of vertical pixels required. Returns -1 in any error condition.
signed int General_WrapAndTrimTextToFit(char** orig_string, char** formatted_string, signed int max_chars_to_format, signed int max_width, signed int max_height, signed int one_char_width, signed int one_row_height, Font* the_font, signed int (* measure_function)(Font*, char*, signed int, signed int, signed int))
{
	char*			formatted_text;
	signed int		v_pixels = 0;
	signed int		new_v_pixels_used;
	char*			remaining_text;
	signed int		remaining_len = max_chars_to_format;
	boolean			format_complete = false;
	signed int		remaining_v_pixels;
	static char		para_buff[1024];
	char*			the_para = para_buff;

	remaining_v_pixels = max_height;
	
	remaining_text = *orig_string;
	formatted_text = *formatted_string;
		
	// Outer Loop: each pass is one line
	do
	{
		boolean			line_complete = false;
		signed int		dist_to_next_hard_break;
		signed int		len_to_process;
		
		
		dist_to_next_hard_break = General_StrFindNextLineBreak(remaining_text, remaining_len);
		
		if (dist_to_next_hard_break == 1)
		{
			*(formatted_text) = '\n';
			// the first char in the string is a line break - skip over and continue
			new_v_pixels_used = one_row_height;
			// account for the line break char we are skipping past
			len_to_process = 1;
		}
		else
		{
			char*			para_to_process;
			
			if (dist_to_next_hard_break < 1)
			{
				// there are no more line breaks in the string. process the entire string
				para_to_process = remaining_text;
				len_to_process = remaining_len + 1;	// +1 because there is no line break we want snipped off. 
			}
			else
			{
				// there is a line break. Send off that paragraph for processing
				para_to_process = para_buff;
				len_to_process = dist_to_next_hard_break; // not +1 because we don't want to add the line break; WrapPara will always add one at the end
				General_Strlcpy(the_para, remaining_text, dist_to_next_hard_break);
			}		
				
			// process one paragraph
			new_v_pixels_used = General_WrapPara(para_to_process, formatted_text, len_to_process, max_width, remaining_v_pixels, one_row_height, one_char_width, the_font, measure_function);		
		}
	
		if (new_v_pixels_used == -1)
		{
		}
		else
		{
			v_pixels += new_v_pixels_used;
			remaining_v_pixels -= new_v_pixels_used;
			remaining_len -= len_to_process;
			formatted_text += len_to_process;
			remaining_text += len_to_process;
			
			if (remaining_v_pixels < 0 || remaining_len < 1)
			{
				format_complete = true;
			}
			
		}		
	} while ( format_complete == false);

	//DEBUG_OUT(("%s %d: print out of formatted_text after processing...", __func__ , __LINE__));
	//General_PrintBufferCharacters(*formatted_string, (unsigned short)max_chars_to_format+10);
	//DEBUG_OUT(("%s %d: v pixels used=%i", __func__ , __LINE__, v_pixels));
	
	// update the original string pointer passed so that it now points to any remaining text (if any)
	if (remaining_len > 0)
	{
		*orig_string = remaining_text;
	}
	
	return v_pixels;
}


//! Extract file extension into the passed char pointer, as new lowercased string pointer, if any found.
//! @param	the_file_name: the file name to extract an extension from
//! @param	the_extension: a pre-allocated buffer that will contain the extension, if any is detected. Must be large enough to hold the extension! No bounds checking is done. 
//! @return	Returns false if no file extension found.
boolean General_ExtractFileExtensionFromFilename(const char* the_file_name, char* the_extension)
{
	// LOGIC: 
	//   if the first char is the first dot from right, we'll count the whole thing as an extension
	//   if no dot char, then don't set extension, and return false
	
    char*	dot = strrchr((char*)the_file_name, '.');
    int		i;

    // (re) set the file extension to "" in case we have to return. It may have a value from whatever previous use was
    the_extension[0] = '\0';

	if(!dot)
    {
    	return false;
    }

	for (i = 1; dot[i]; i++)
	{
		the_extension[i-1] = General_ToLower(dot[i]);
	}

	the_extension[i-1] = '\0';

	return true;
}


// replacement for tolower() in c library, which doesn't seem to work here for some reason.
char General_ToLower(char the_char)
{
    char	lowered_value;
    
    lowered_value = (the_char >='A' && the_char<='Z') ? (the_char + 32) : (the_char);
    
    return lowered_value;
}

// convert a file size in bytes to a human readable format using "10 bytes", "1.4 kb", "1 MB", etc. 
//   NOTE: formatted_file_size string must have been allocated before passing here
void General_MakeFileSizeReadable(unsigned long size_in_bytes, char* formatted_file_size)
{
	double			final_size;
	
	// convert to float before doing any operations on it, to prevent integer weirdness
	final_size = (double)size_in_bytes;
	
	if (size_in_bytes < 1024) // 1.0k
	{
		// show size in bytes, precisely
		sprintf((char*)formatted_file_size, "%lu b", size_in_bytes);
	}
	else if (size_in_bytes < 10240) // 10k
	{
		// show size in .1k chunks (eg, 9.4k)
		final_size /= 1024;
		sprintf((char*)formatted_file_size, "%.1f kb", final_size);
	}
	else if (size_in_bytes < 1048576) // 1 MB
	{
		// show size in 1k chunks
		final_size /= 1024;
		size_in_bytes = General_Round(final_size);
		sprintf((char*)formatted_file_size, "%lu kb", size_in_bytes);
	}
	else if (size_in_bytes < 10485760) // 10MB
	{
		// show size in .1M chunks (eg, 1.4MB)
		final_size /= 1048576;
		sprintf((char*)formatted_file_size, "%.1f Mb", final_size);
	}
	else
	{
		// show size in 1M chunks (eg, 1536 MB)
		final_size /= 1048576;
		size_in_bytes = General_Round(final_size);
		sprintf((char*)formatted_file_size, "%lu Mb", size_in_bytes);
	}
	
	return;
}


// cheapo way to do round. THINK C's and SAS/C's math.h don't include round()
// from: https://stackoverflow.com/questions/4572556/concise-way-to-implement-round-in-c
int General_Round(double the_float)
{
    if (the_float < 0.0)
        return (int)(the_float - 0.5);
    else
        return (int)(the_float + 0.5);
}


// allocates memory and copies the passed string into it. one stop shop for getting a copy of a string
char* General_StrlcpyWithAlloc(const char* the_source, unsigned long max_len)
{
	char* the_target;
	size_t	alloc_len = General_Strnlen(the_source, max_len) + 1;
	
	DEBUG_OUT(("%s %d: here. the_source=%p, max_len=%lu, alloc_len=%lu", __func__, __LINE__, the_source, max_len, alloc_len));

	if ( (the_target = (char*)f_calloc(alloc_len, sizeof(char), MEM_STANDARD) ) == NULL)
	{
		return NULL;
	}
	else
	{
		General_Strlcpy(the_target, the_source, max_len);
	}
	//LOG_ALLOC(("%s %d:	__ALLOC__	the_target	%p	size	%i, string='%s'", __func__ , __LINE__, the_target, General_Strnlen(the_source, max_len) + 1, the_target));

	return the_target;
}


// strlcpy implementation from apple/bsd. the Amiga Strlcpy is only in 4.0. Modified with Amiga-isms
unsigned long General_Strlcpy(char* dst, const char* src, unsigned long maxlen)
{
    const unsigned long		srclen = strlen(src);
	DEBUG_OUT(("%s %d: here. dst=%p ('%s'), src=%p ('%s'), max_len=%lu, srclen=%lu", __func__, __LINE__, dst, dst, src, src, maxlen, srclen));

    if (srclen + 1 < maxlen)
    {
        memcpy(dst, src, srclen + 1);
    }
    else if (maxlen != 0)
    {
    	memcpy(dst, src, maxlen - 1);
        dst[maxlen - 1] = '\0';
    }
	DEBUG_OUT(("%s %d: here. dst=%p ('%s'), src=%p ('%s'), max_len=%lu, srclen=%lu", __func__, __LINE__, dst, dst, src, src, maxlen, srclen));

    return srclen;
}

// General_Strlcat implementation from apple/bsd. Modified with Amiga-isms
unsigned long General_Strlcat(char* dst, const char* src, unsigned long maxlen)
{
    const unsigned long		srclen = strlen(src);
    const unsigned long 	dstlen = General_Strnlen(dst, maxlen);

    if (dstlen == maxlen)
    {
    	return maxlen + srclen;
    }

    if (srclen < maxlen-dstlen)
    {
        memcpy(dst + dstlen, src, srclen + 1);
    }
    else
    {
        memcpy(dst + dstlen, src, maxlen - dstlen - 1);
        dst[maxlen - 1] = '\0';
    }

    return dstlen + srclen;
}


// strncmp implementation from Amiga site. Case sensitive
// http://home.snafu.de/kdschem/c.dir/strings.dir/strncmp.c
int General_Strncmp(const char* string_1, const char* string_2, long length)
{
	register char	c;
	
	do ; while( (c = *string_1++) && (c == *string_2++) && --length );
	if (c)
	{
		string_2--;
	}
	return (c - *string_2);
}


// strncasecmp (case insensitive comparison) based on code from slashdot and apple open source
// https://stackoverflow.com/questions/5820810/case-insensitive-string-comparison-in-c
// https://opensource.apple.com/source/tcl/tcl-10/tcl/compat/strncasecmp.c.auto.html
signed int General_Strncasecmp(const char* string_1, const char* string_2, long max_len)
{
	//DEBUG_OUT(("%s %d: s1='%s'; s2='%s'; maxlen=%i", __func__ , __LINE__, string_1, string_2, max_len));

	for (; max_len != 0; max_len--, string_1++, string_2++)
	{
		char	u1 = (char) *string_1;
		char	u2 = (char) *string_2;
		
		if (General_ToLower(u1) != General_ToLower(u2))
		{
			return General_ToLower(u1) - General_ToLower(u2);
		}
		
		if (u1 == '\0')
		{
			return 0;
		}
	}
	
	return 0;	
}


// strnlen implementation from apple/bsd. Modified with Amiga-isms
unsigned long General_Strnlen(const char* s, unsigned long maxlen)
{
	unsigned long	len;
	DEBUG_OUT(("%s %d: here. s=%p, s='%s', maxlen=%lu", __func__, __LINE__, s, s, maxlen));

	for (len = 0; len < maxlen; len++, s++)
	{
		if (!*s)
		{
			break;
		}
	}
	DEBUG_OUT(("%s %d: here. s=%p, s='%s', len=%lu", __func__, __LINE__, s, s, len));
	return (len);
}


// function compatible with List_MergeSortedList(). Compares to strings passed as void pointers, returns true if first string is longer than second. 
// NOTE: compares to a maximum of MAX_STRING_COMP_LEN
boolean General_CompareStringLength(void* first_payload, void* second_payload)
{
	char*		string_1 = (char*)first_payload;
	char*		string_2 = (char*)second_payload;

	if (General_Strnlen(string_1, MAX_STRING_COMP_LEN) > General_Strnlen(string_2, MAX_STRING_COMP_LEN))
	{
		return true;
	}
	else
	{
		return false;
	}
}


// Find the next space, dash, or other word break character and return its position within the string. If none found before end of string or max len, returns -1.
signed int General_StrFindNextWordEnd(const char* the_string, signed int max_search_len)
{
	char*	next_space;
	char*	next_dash;
	char*	first_hit = NULL; // worst case scenario - no word endings found
	
	next_space = strchr((char*)the_string, ' ');
	next_dash = strchr((char*)the_string, '-');
	
	if (next_space)
	{
		first_hit = next_space;
	}
	
	if (next_dash && next_dash < next_space)
	{
		first_hit = next_dash;
	}
	
	if (first_hit)
	{
		return (first_hit - (char*)the_string) +1;
	}
	else
	{
		return -1;
	}
}


// Find the next line break character and return its position within the string (+1: first char is '1'). If none found before end of string or max len, returns 0.
signed int General_StrFindNextLineBreak(const char* the_string, signed int max_search_len)
{
	char*	next_line_break;

	next_line_break = strchr((char*)the_string, '\n');
	
	if (next_line_break)
	{
		return (signed int)((next_line_break - (char*)the_string) + 1);
	}
	else
	{
		return 0;
	}
}


// test if 2 rectangles intersect
boolean General_RectIntersect(Rectangle r1, Rectangle r2)
{
	if	(
		(r1.MinX > r2.MaxX) ||
		(r1.MaxX < r2.MinX) ||
		(r1.MinY > r2.MaxY) ||
		(r1.MaxY < r2.MinY)
		)
	{
		return false;
	}
	else
	{
		return true;
	}
}


// test if a point is within a rectangle
boolean General_PointInRect(signed int x, signed int y, Rectangle r)
{
	if	(
		(x > r.MaxX) ||
		(x < r.MinX) ||
		(y > r.MaxY) ||
		(y < r.MinY)
		)
	{
		return false;
	}
	else
	{
		return true;
	}
}


// Convert a positive or negative string integer to a signed long integer. returns false in event of error
boolean General_StringToSignedLong(const char* the_string_value, signed long* the_conversion)
{
	signed long		signed_val = 0;
	unsigned long	unsigned_val = 0;
	boolean			safe_conversion;
	const char*		start_of_number = the_string_value;
	
	// is this a negative number string?
	if (*the_string_value == '-')
	{
		start_of_number++;
	}
	
	safe_conversion = General_StringToUnsignedLong(start_of_number, &unsigned_val);

	signed_val = (signed long)unsigned_val;
	
	if (*the_string_value == '-')
	{
		signed_val = -signed_val;
	}
	
	*the_conversion = signed_val;
	return safe_conversion;
}


// Position one rect within the bounds of another. Horizontally: centers the hero rect within the left/right of the frame rect; Vertically: centers or or puts at 25% line
// put the frame coords into the frame_rect, and the object to be centered into the hero_rect. ON return, the frame rect will hold the coords to be used.
void General_CenterRectWithinRect(Rectangle* the_frame_rect, Rectangle* the_hero_rect, boolean at_25_percent_v)
{
	signed short	hero_height = the_hero_rect->MaxY - the_hero_rect->MinY;
	signed short	hero_width = the_hero_rect->MaxX - the_hero_rect->MinX;
	signed short	frame_height = the_frame_rect->MaxY - the_frame_rect->MinY;
	
	// horizontal: center left/right
	the_frame_rect->MinX = (the_frame_rect->MaxX - the_frame_rect->MinX - hero_width) / 2 + the_frame_rect->MinX;
	the_frame_rect->MaxX = the_frame_rect->MinX + hero_width;

	if (at_25_percent_v == true)
	{
		signed short	proposed_top;

		// set at 25% of vertical (good for showing an about window, for example)
		proposed_top = frame_height / 4;
		
		// make sure there was actually enough space
		if ((proposed_top + hero_height) > the_frame_rect->MaxY)
		{
			// commented out code below works, but it would be better to just center it vertically at this point
			//the_frame_rect->MinY = proposed_top - ((proposed_top + hero_height) - the_frame_rect->bottom);
			at_25_percent_v = false; // let fall through
		}
		else
		{
			the_frame_rect->MinY = proposed_top;
		}
	}

	if (at_25_percent_v == false)
	{
		// vertical: center top/bottom
		the_frame_rect->MinY = (frame_height - hero_height) / 2 + the_frame_rect->MinY;
	}

	the_frame_rect->MaxY = the_frame_rect->MinY + hero_height;

	//DEBUG_OUT(("%s %d: coords=%i, %i / %i, %i", __func__ , __LINE__, the_frame_rect->MinX, the_frame_rect->MinY, the_frame_rect->MaxX, the_frame_rect->MaxY));
}


// return the first char of the last part of a file path
// if no path part detected, returns the original string
// not guaranteed that this is a FILENAME, as if you passed a path to a dir, it would return the DIR name
// amigaDOS compatibility function (see FilePart)
char* General_NamePart(const char* the_file_path)
{
	char*	last_slash;
	
	last_slash = strchr(the_file_path, '/');
	
	if (last_slash && ++last_slash)
	{
		return last_slash;
	}
	
	return (char*)the_file_path;
}


// return everything to the left of the filename in a path. 
// amigaDOS compatibility function
char* General_PathPart(const char* the_file_path)
{
	char*	the_directory_path;
	char*	this_point;
	
	this_point = (char*)the_file_path;
	the_directory_path = this_point; // default to returning start of the string
	
	while (*this_point)
	{
		if (*this_point == '/')
		{
			the_directory_path = this_point;
		}
		
		this_point++;
	}
	
	return the_directory_path;
}


// allocate and return the portion of the path passed, minus the filename. In other words: return a path to the parent file.
// calling method must free the string returned
char* General_ExtractPathToParentFolderWithAlloc(const char* the_file_path)
{
	// LOGIC: 
	//   PathPart includes the : if non-name part is for a volume. but doesn't not include trailing / if not a volume
	//   we want in include the trailing : and /, so calling routine can always just append a file name and get a legit path
	
	unsigned long	path_len;
	char*			the_directory_path;

	// get a string for the directory portion of the filepath
	if ( (the_directory_path = (char*)calloc(FILE_MAX_PATHNAME_SIZE, sizeof(char)) ) == NULL)
	{
		LOG_ERR(("%s %d: could not allocate memory for the directory path", __func__ , __LINE__));
		return NULL;
	}
	
	path_len = (General_PathPart(the_file_path) - the_file_path) - 1;
	
	//DEBUG_OUT(("%s %d: pathlen=%lu; last char='%c'", __func__ , __LINE__, path_len, the_file_path[path_len]));

	if (the_file_path[path_len] != ':')
	{
		// path wasn't to root of a volume, move 1 tick to the right to pick up the / that is already in the full path
		path_len++;
	}

	path_len++;

	General_Strlcpy(the_directory_path, the_file_path, path_len + 1);
	//DEBUG_OUT(("%s %d: pathlen=%lu; parent path='%s'", __func__ , __LINE__, path_len, the_directory_path));
	
	return the_directory_path;
}


// allocate and return the filename portion of the path passed.
// calling method must free the string returned
char* General_ExtractFilenameFromPathWithAlloc(const char* the_file_path)
{
	char*	the_file_name;

	// get a string for the file name portion of the filepath
	if ( (the_file_name = (char*)calloc(FILE_MAX_PATHNAME_SIZE, sizeof(char)) ) == NULL)
	{
		LOG_ERR(("%s %d: could not allocate memory for the filename", __func__ , __LINE__));
		return NULL;
	}
	else
	{
		char*	the_file_name_part = General_NamePart(the_file_path);
		int		filename_len = General_Strnlen(the_file_name_part, FILE_MAX_PATHNAME_SIZE);

		if (filename_len == 0)
		{
			// FilePart() might return a string with no text: that would indicate the file path is for the root of a file system or virtual device
			// in that case, we just use the file path minus : as the name

			// copy the part of the path minus the last char into the file name
			signed int	path_len = General_Strnlen(the_file_path, FILE_MAX_PATHNAME_SIZE);
			General_Strlcpy(the_file_name, the_file_path, path_len);
		}
		else
		{
			General_Strlcpy(the_file_name, the_file_name_part, filename_len + 1);
		}
		LOG_ALLOC(("%s %d:	__ALLOC__	the_file_name	%p	size	%i", __func__ , __LINE__, the_file_name, FILE_MAX_PATHNAME_SIZE));
	}

	return the_file_name;
}


// populates the passed string by safely combining the passed file path and name, accounting for cases where path is a disk root
void General_CreateFilePathFromFolderAndFile(char* the_combined_path, char* the_folder_path, char* the_file_name)
{
	signed int	path_len;
	
	General_Strlcpy(the_combined_path, the_folder_path, FILE_MAX_PATHNAME_SIZE);
	path_len = General_Strnlen(the_combined_path, FILE_MAX_PATHNAME_SIZE);

	// if the filename passed was empty, just return the original folder path. 
	//   otherwise you end up with "mypath" and file "" = "mypath/", which is bad. 
	if (the_file_name[0] == '\0')
	{
		return;
	}
	
	General_Strlcat(the_combined_path, the_file_name, FILE_MAX_PATHNAME_SIZE);

	//DEBUG_OUT(("%s %d: file '%s' and folder '%s' produces path of '%s'", __func__ , __LINE__, the_file_name, the_folder_path, the_combined_path));
}



// DEBUG functionality I want:
//   3 levels of logging (err/warn/info)
//   additional debug out function that leaves no footprint in compiled release version of code (calls to it also disappear)
//   able to pass format string and multiple variables when needed

void General_LogError(const char* format, ...)
{
	va_list		args;
	
	va_start(args, format);
	vsprintf(debug_buffer, format, args);
	va_end(args);

	// f68 emulator has a log to console feature:
#ifdef _f68_
	*((long *)-4) = (long)&debug_buffer;
#else
	//fprintf(global_log_file, "%s %s\n", kDebugFlag[LogError], debug_buffer);
	printf("%s %s\n", kDebugFlag[LogError], debug_buffer);
#endif
}

void General_LogWarning(const char* format, ...)
{
	va_list		args;
	
	va_start(args, format);
	vsprintf(debug_buffer, format, args);
	va_end(args);

	// f68 emulator has a log to console feature:
#ifdef _f68_
	*((long *)-4) = (long)&debug_buffer;
#else
	//fprintf(global_log_file, "%s %s\n", kDebugFlag[LogWarning], debug_buffer);
	printf("%s %s\n", kDebugFlag[LogWarning], debug_buffer);
#endif
}

void General_LogInfo(const char* format, ...)
{
	va_list		args;
	
	va_start(args, format);
	vsprintf(debug_buffer, format, args);
	va_end(args);

	// f68 emulator has a log to console feature:
#ifdef _f68_
	*((long *)-4) = (long)&debug_buffer;
#else
	//fprintf(global_log_file, "%s %s\n", kDebugFlag[LogInfo], debug_buffer);
	printf("%s %s\n", kDebugFlag[LogInfo], debug_buffer);
#endif
}

void General_DebugOut(const char* format, ...)
{
	va_list		args;
	
	va_start(args, format);
	vsprintf(debug_buffer, format, args);
	va_end(args);
	
	// f68 emulator has a log to console feature:
#ifdef _f68_
	*((long *)-4) = (long)&debug_buffer;
#else
	//fprintf(global_log_file, "%s %s\n", kDebugFlag[LogDebug], debug_buffer);
	printf("%s %s\n", kDebugFlag[LogDebug], debug_buffer);
#endif
}

void General_LogAlloc(const char* format, ...)
{
	va_list		args;
	
	va_start(args, format);
	vsprintf(debug_buffer, format, args);
	va_end(args);
	
	// f68 emulator has a log to console feature:
#ifdef _f68_
	*((long *)-4) = (long)&debug_buffer;
#else
	//fprintf(global_log_file, "%s %s\n", kDebugFlag[LogAlloc], debug_buffer);
	printf("%s %s\n", kDebugFlag[LogAlloc], debug_buffer);
#endif
}

// initialize log file
// globals for the log file
boolean General_LogInitialize(void)
{
	const char*		the_file_path = "wb2k_log.txt";

	global_log_file = fopen( the_file_path, "w");
	
	if (global_log_file == NULL)
	{
		printf("General_LogInitialize: log file could not be opened! \n");
		return false;
	}
	
	return true;
}

// close the log file
void General_LogCleanUp(void)
{
	if (global_log_file != NULL)
	{
		fclose(global_log_file);
	}
}


// debug function to print out a chunk of memory character by character
void General_PrintBufferCharacters(char* the_data, unsigned short the_len)
{
	unsigned short	i;
	unsigned short	bytes_out = 0;
	char*			temp = the_data;
	char			buffer[512];
	char*			next_field = buffer;
	
	sprintf(next_field, "Buffer Print: ");
	bytes_out += 14;
	next_field +=14;

	for (i = 0; i < the_len; i++, temp++)
	{
		if (*temp == 0)
		{
			sprintf(next_field, "'(0)' = 0,     ");
		}
		else
		{
			sprintf(next_field, "'%c' = %i,     ", (char)(*temp), (char)(*temp));
		}
		
		bytes_out += 10;
		next_field += 10;
		
		if (bytes_out > 235)
		{
			DEBUG_OUT((buffer));
			bytes_out = 0;
			next_field = buffer;
		}
	}
	
	sprintf(next_field, "\n");
	
	DEBUG_OUT((buffer));
}

