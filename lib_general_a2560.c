/*
 * lib_general_a2560.c
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
//#include "localize.h"

// C includes
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>
#include <errno.h>

// A2560 includes



/*****************************************************************************/
/*                               Definitions                                 */
/*****************************************************************************/




/*****************************************************************************/
/*                             Global Variables                              */
/*****************************************************************************/

static char				message_buffer[ALERT_MAX_MESSAGE_LEN];	// used for alert dialogs sprintfs




/*****************************************************************************/
/*                       Private Function Prototypes                         */
/*****************************************************************************/




/*****************************************************************************/
/*                       Private Function Definitions                        */
/*****************************************************************************/



/*****************************************************************************/
/*                        Public Function Definitions                        */
/*****************************************************************************/




/*
// calculates how many characters of the passed string will fit into the passed rectangle
unsigned int General_TextFit(struct RastPort* the_rastport, unsigned char* the_string, unsigned int the_len, unsigned int the_width)
{
	struct TextExtent	the_resulting_extent;
	unsigned int		fit_count;
	unsigned int		the_height = the_rastport->Font->tf_YSize;
	
	fit_count = TextFit( the_rastport, the_string, the_len, &the_resulting_extent, NULL, 1, the_width, the_height);
	
	return fit_count;	
}
*/


/*
// format a string by wrapping and trimming to fit the passed width and height. returns number of vertical pixels required. passing a 0 for height disables the governor on allowed vertical space. if max_chars_to_format is less than len of string, processing will stop after that many characters.
unsigned int General_WrapAndTrimTextToFit(unsigned char** orig_string, unsigned char** formatted_string, unsigned int max_chars_to_format, struct RastPort* the_rastport, unsigned int max_width, unsigned int max_height)
{
	unsigned int		font_height = the_rastport->Font->tf_YSize;
	unsigned char*		trimmed = *formatted_string;
	unsigned int		v_pixels = 0;
	unsigned char*		this_line = *orig_string;
	signed int			remaining_len = max_chars_to_format;
	boolean				format_complete = false;
	
	if (max_chars_to_format == 0)
	{
		*(trimmed) = '\0';
		return 0;
	}
	
	//DEBUG_OUT(("%s %d: max_chars_to_format=%i, remaining_len=%i, this line='%s', maxwidth=%lu", __func__ , __LINE__, max_chars_to_format, remaining_len, this_line, max_width));
	//DEBUG_OUT(("%s %d: orig string='%s'", __func__ , __LINE__, *orig_string));
	//DEBUG_OUT(("%s %d: formatted string='%s'", __func__ , __LINE__, trimmed));
	
	// Outer Loop: each pass is one line
	while (!format_complete && v_pixels <= max_height)
	{
		boolean			line_complete = false;
		unsigned int	this_line_len = 0;
		unsigned int	chars_that_fit;
		unsigned int	dist_to_next_hard_break;
		unsigned char*	this_phrase;
		
		this_phrase = this_line;
		
		dist_to_next_hard_break = General_StrFindNextLineBreak(this_phrase, remaining_len);
		
		//DEBUG_OUT(("%s %d: dist_to_next_hard_break=%u", __func__ , __LINE__, dist_to_next_hard_break));
		
		if (dist_to_next_hard_break == 0)
		{
			// there are no more line breaks in the string. Try first if entire string will fit in H space. if so, done. if not, need to go word by word.
			chars_that_fit = General_TextFit(the_rastport, this_line, remaining_len, max_width);
		
			//DEBUG_OUT(("%s %d: chars_that_fit=%u, remaining_len=%i", __func__ , __LINE__, chars_that_fit, remaining_len));
			
			if (chars_that_fit >= remaining_len)
			{
				// the entire string fits. copy this line and stop processing				
				General_Strlcat(trimmed, this_line, remaining_len + 1);

				*(trimmed + remaining_len) = '\0';
				line_complete = true;
				format_complete = true;
				
				//DEBUG_OUT(("%s %d: formatting complete, as follows...", __func__ , __LINE__));
				//General_PrintBufferCharacters(*formatted_string, (unsigned short)max_chars_to_format+5);
				//remaining_len = 0;
			}
			else
			{
				// remaining string is too long to fit without breaking at least once
			}
		}
		else
		{
			// there is a line break. before trying to split up further, first check if the string up to the hard line break is small enough to fit
			
			//DEBUG_OUT(("%s %d: line break found at position %u", __func__ , __LINE__, dist_to_next_hard_break));		

			chars_that_fit = General_TextFit(the_rastport, this_line, dist_to_next_hard_break, max_width);
			
			//DEBUG_OUT(("%s %d: chars_that_fit=%u, dist_to_next_hard_break=%u, all-fit=%i, break-numfit=%i", __func__ , __LINE__, chars_that_fit, dist_to_next_hard_break, (chars_that_fit >= dist_to_next_hard_break), dist_to_next_hard_break - chars_that_fit ));		
			
			if (chars_that_fit >= dist_to_next_hard_break )
			{
				// the hard-coded line fits. copy this line and start processing the next one		
				//DEBUG_OUT(("%s %d: chars before line break fit on one line!", __func__ , __LINE__));		
				
				General_Strlcpy(trimmed, this_line, dist_to_next_hard_break + 1);
				//DEBUG_OUT(("%s %d: print out of trimmed after copy...", __func__ , __LINE__));
 				//General_PrintBufferCharacters(trimmed, (unsigned short)dist_to_next_hard_break+5);
				//DEBUG_OUT(("%s %d: line break found; fitted='%s'", __func__ , __LINE__, trimmed));		

				trimmed += dist_to_next_hard_break - 0; // move trimmed up to the next spot for writing
				this_line += dist_to_next_hard_break; // move untrimmed up to the next spot for reading
				remaining_len -= dist_to_next_hard_break;
				line_complete = true;
				
				//DEBUG_OUT(("%s %d: line break found; remaining_len=%i, this_line now='%s'", __func__ , __LINE__, remaining_len, this_line));		
			}
			else
			{
				// string up to the hard-coded line break is too long to fit without breaking at least once
				//DEBUG_OUT(("%s %d: string up to the hard-coded line break can't fit without breaking", __func__ , __LINE__));
			}
		}
		
		// LOGIC:
		//   if line_complete is false at this point, we know we haven't been able to use a hard-coded line break
		//   we also know the entire remaining string didn't fit
		//   we need to go word-by-word.
		
		// Inner Loop: each pass is one word
		while (line_complete == false)
		{
			signed int			dist_to_next_soft_break_option;
			
			dist_to_next_soft_break_option = General_StrFindNextWordEnd(this_phrase, remaining_len);
			
			//DEBUG_OUT(("%s %d: dist_to_next_soft_break_option=%i", __func__ , __LINE__, dist_to_next_soft_break_option));		
			//DEBUG_OUT(("%s %d: remaining_len=%i, this_phrase='%s'", __func__ , __LINE__, remaining_len, this_phrase));		
			//DEBUG_OUT(("%s %d: remaining_len=%i", __func__ , __LINE__, remaining_len));		
			//Delay(10);
			
			if (dist_to_next_soft_break_option < 0)
			{
				// there are no more word breaks in this string
				// make sure that we don't have one long string that we have to force break
				if (this_line_len == 0)
				{
					// the whole string is word-break-less: we must force a break
					//DEBUG_OUT(("%s %d: this line len is 0, so must break word in middle...", __func__ , __LINE__));
					chars_that_fit = General_TextFit(the_rastport, this_line, remaining_len, max_width);
					General_Strlcpy(trimmed, this_line, chars_that_fit + 0);
					//DEBUG_OUT(("%s %d: print out of trimmed after copy...", __func__ , __LINE__));
					//General_PrintBufferCharacters(trimmed, (unsigned short)dist_to_next_hard_break+5);

					*(trimmed + chars_that_fit - 2) = '\n'; // need to overwrite the \0 that strlcpy added!
					trimmed += chars_that_fit - 1; // move trimmed up to the next spot for writing
					this_line += chars_that_fit; // move untrimmed up to the next spot for reading
				}
				else
				{
					// we have hit a long word, but previously we got at least 1 word break, so we can go back to that spot and break the line
					//DEBUG_OUT(("%s %d: must do word wrap. wrapping @ this_line_len=%i", __func__ , __LINE__, this_line_len));
					General_Strlcpy(trimmed, this_line, this_line_len - 0);
					*(trimmed + this_line_len - 1) = '\n';
					trimmed += this_line_len + 0; // move trimmed up to the next spot for writing
					this_line += this_line_len; // move untrimmed up to the next spot for reading
					remaining_len -= this_line_len;
					//DEBUG_OUT(("%s %d: this line now '%s'", __func__ , __LINE__, this_line));
					this_line_len = 0;
				}
				
				line_complete = true;
			}
			else
			{
				// we found the next word break. test if we can fit everything to that point. if so, continue on. if not, back up to last pos and break there
	
				//DEBUG_OUT(("%s %d: found a new word break. this word='%s'", __func__ , __LINE__, this_line, General_StrlcpyWithAlloc(this_line, dist_to_next_soft_break_option-this_line_len)));
				//DEBUG_OUT(("%s %d: found a new word break", __func__ , __LINE__, this_line));

				chars_that_fit = General_TextFit(the_rastport, this_line, this_line_len + dist_to_next_soft_break_option, max_width);
			
				//DEBUG_OUT(("%s %d: chars_that_fit=%i", __func__ , __LINE__, chars_that_fit));		
			
				if (chars_that_fit >= (this_line_len + dist_to_next_soft_break_option) )
				{
					// the next word fits. continue on
					this_phrase += dist_to_next_soft_break_option;
					this_line_len += dist_to_next_soft_break_option;
					remaining_len -= dist_to_next_soft_break_option;
				}
				else
				{
					// the new word doesn't fit, but previously we got at least 1 word break, so we can go back to that spot and break the line
					General_Strlcpy(trimmed, this_line, this_line_len + 0);
					//DEBUG_OUT(("%s %d: print out of trimmed after copy...", __func__ , __LINE__));
					//General_PrintBufferCharacters(trimmed, (unsigned short)dist_to_next_hard_break+5);
					//DEBUG_OUT(("%s %d: line complete. remaining_len=%i, this_line='%s'", __func__ , __LINE__, remaining_len, this_line));		

					*(trimmed + this_line_len - 1) = '\n'; // overwrite the \0 from strlcpy
					trimmed += this_line_len + 0; // move trimmed up to the next spot for writing
					this_line += this_line_len; // move untrimmed up to the next spot for reading
					line_complete = true;
					this_line_len = 0;
				}
			}
		}
		
		v_pixels += font_height;
	}

	//DEBUG_OUT(("%s %d: print out of trimmed after processing...", __func__ , __LINE__));
	//General_PrintBufferCharacters(*formatted_string, (unsigned short)max_chars_to_format+10);
	//DEBUG_OUT(("%s %d: v pixels used=%i", __func__ , __LINE__, v_pixels));
	
	return v_pixels;
}
*/


/*
// draw a line in an intuition window
void General_DrawLine(struct RastPort* the_rastport, signed long x1, signed long y1, signed long x2, signed long y2, unsigned char the_color_pen)
{
	// move the pen to the first coordinate, or it will draw a line from where it is, to there
	Move( the_rastport, x1, y1 );

	// draw the line
	SetAPen( the_rastport, the_color_pen );
	Draw( the_rastport, x2, y2 );
}
*/


/*
// draw a poly in an intuition window
void General_DrawPoly(struct RastPort* the_rastport, short num_coords, short* the_coordinates, unsigned char the_color_pen)
{
	short	x = *(the_coordinates + 0);
	short	y = *(the_coordinates + 1);

	// move the pen to the first coordinate, or it will draw a line from where it is, to there
	Move( the_rastport, x, y );

	// draw the poly
	SetAPen( the_rastport, the_color_pen );
	PolyDraw( the_rastport, num_coords, the_coordinates );
}
*/


/*
// draw a rectangle in the rastport passed. If do_undraw is TRUE, try to undraw it (unimplemented TODO)
void General_DrawBox(struct RastPort* the_rastport, signed short x1, signed short y1, signed short x2, signed short y2, boolean do_undraw, unsigned char the_color_pen)
{
	short				coordinates[10];

	// COMPLEMENT mode will simply inverse pixels. This makes it possible to draw once, then draw again, to restore what was there
	SetDrMd(the_rastport, COMPLEMENT);

	// need 10 coords to use Intuitions PolyDraw for a box
	coordinates[0] = x1;
	coordinates[1] = y1;

	coordinates[2] = x2;
	coordinates[3] = y1;

	coordinates[4] = x2;
	coordinates[5] = y2;

	coordinates[6] = x1;
	coordinates[7] = y2;

	coordinates[8] = x1;
	coordinates[9] = y1;
	
	//DEBUG_OUT(("%s %d: %i, %i to %i, %i", __func__ , __LINE__, x1, y1, x2, y2));

	// set a dotted line pattern
	//SetDrPt(the_rastport, 0xAAAA);
	SetDrPt(the_rastport, 0xFFFF);

	// draw the surrounding box
	General_DrawPoly(the_rastport, 5, &coordinates[0], the_color_pen);

	// turn off pattern
	//SetDrPt(the_rastport, 0xFFFF);

	// reset draw mode in case we need it for plotting/replotting/etc.
	SetDrMd(the_rastport, JAM1);
}
*/




/*
// checks a file exists without locking the file. tries to get a lock on the dir containing the file, then checks contents until it matches
// SLOW, and probably pointless, but struggling with issue of locks not unlocking when checking for existence of an icon file.
boolean General_CheckFileExists(unsigned char* the_file_path)
{
	BPTR 					the_dir_lock;
	struct FileInfoBlock*	fileInfo;
	LONG					the_io_error;
	unsigned char*			the_file_name_to_check;
	int						filename_len;
	unsigned char*			the_directory_path;

	// get a string for the directory portion of the filepath
	if ( (the_directory_path = General_ExtractPathToParentFolderWithAlloc(the_file_path)) == NULL)
	{
		return false;
	}
	LOG_ALLOC(("%s %d:	__ALLOC__	the_directory_path	%p	size	%i		'%s'", __func__ , __LINE__, the_directory_path, General_Strnlen(the_directory_path, FILE_MAX_PATHNAME_SIZE) + 1, the_directory_path));
	
	// get the name portion of the path, so we can compare it later to each file we find in the parent directory
	the_file_name_to_check = FilePart(the_file_path);
	filename_len = General_Strnlen(the_file_name_to_check, FILE_MAX_PATHNAME_SIZE);

	// FileInfoBlocks have to be byte aligned, so needs AllocDosObject
	fileInfo = AllocDosObject(DOS_FIB, NULL);
	LOG_ALLOC(("%s %d:	__ALLOC__	fileInfo	%p	size	%i		AllocDosObject", __func__ , __LINE__, fileInfo, sizeof(struct FileInfoBlock)));

	// try to get lock on the  directory
	if ( (the_dir_lock = Lock((STRPTR)the_directory_path, SHARED_LOCK)) == 0)
	{
		LOG_INFO(("%s %d: Failed to lock dir %s!", __func__ , __LINE__, the_directory_path));
		LOG_ALLOC(("%s %d:	__FREE__	the_directory_path	%p	size	%i		'%s'", __func__ , __LINE__, the_directory_path, General_Strnlen(the_directory_path, FILE_MAX_PATHNAME_SIZE) + 1, the_directory_path));
		free(the_directory_path);
		the_directory_path = NULL;
		return false;
	}
	//printf("General_CheckFileExists: debug - dir-lock: %li / %p\n", the_dir_lock, BADDR( the_dir_lock ));

	if (Examine(the_dir_lock, fileInfo) != 0)
	{
		while ( ExNext(the_dir_lock, fileInfo) ) // data = ExamineDir(context))
		{
			if (fileInfo->fib_DirEntryType > 0)
			{
				// it's a directory
			}
			else
			{
				// it's a file... is it the we are looking for?
				if (General_Strncmp(the_file_name_to_check, (unsigned char*)fileInfo->fib_FileName, filename_len) == 0)
				{
					// it's the file we were looking for
					//printf("General_CheckFileExists: Confirmed file %s!\n", the_file_path);
					LOG_ALLOC(("%s %d:	__FREE__	the_directory_path	%p	size	%i		'%s'", __func__ , __LINE__, the_directory_path, General_Strnlen(the_directory_path, FILE_MAX_PATHNAME_SIZE) + 1, the_directory_path));
					free(the_directory_path);
					the_directory_path = NULL;
					
					LOG_ALLOC(("%s %d:	__FREE__	fileInfo	%p	size	%i		FreeDosObject", __func__ , __LINE__, fileInfo, sizeof(struct FileInfoBlock)));
					FreeDosObject(DOS_FIB, fileInfo);
					
					UnLock(the_dir_lock);
					return true;
				}
				else
				{
	//				printf("General_CheckFileExists: found |%s|!\n", fileInfo->fib_FileName);
				}
			}
		}
	}


	the_io_error = IoErr();

	if (the_io_error == ERROR_NO_MORE_ENTRIES)
	{
		// last file read: fail condition
	}
	else
	{
		// some other io error happened: just another fail condition
	}

	// free objects allocated in this method
	LOG_ALLOC(("%s %d:	__FREE__	the_directory_path	%p	size	%i", __func__ , __LINE__, the_directory_path, General_Strnlen(the_directory_path, FILE_MAX_PATHNAME_SIZE) + 1));
	free(the_directory_path);
	the_directory_path = NULL;
	
	LOG_ALLOC(("%s %d:	__FREE__	fileInfo	%p	size	%i		FreeDosObject", __func__ , __LINE__, fileInfo, sizeof(struct FileInfoBlock)));
	FreeDosObject(DOS_FIB, fileInfo);
	
	UnLock(the_dir_lock);

	return false;
}
*/


/*
// return current date/time as a timestamp. 
struct DateStamp* General_GetCurrentDateStampWithAlloc(void)
{
	struct DateStamp*	the_datetime;

	if ( (the_datetime = (struct DateStamp*)calloc(1, sizeof(struct DateStamp)) ) == NULL)
	{
		LOG_ERR(("%s %d: could not allocate memory to create new datestamp", __func__ , __LINE__));
		return NULL;
	}
	LOG_ALLOC(("%s %d:	__ALLOC__	the_datetime	%p	size	%i", __func__ , __LINE__, the_datetime, sizeof(struct DateStamp)));
	
	// have Amiga populate with the current time/date
	DateStamp(the_datetime);
	
	return the_datetime;
}
*/

