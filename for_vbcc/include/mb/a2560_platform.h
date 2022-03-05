/*
 * a2650_platform.h
 *
 *  Created on: Feb 19, 2022
 *      Author: micahbly
 */

#ifndef A2650_PLATFORM_H_
#define A2650_PLATFORM_H_


/* about this class
 *
 *
 *
 *** things this class needs to be able to do
 * nothing - this is not a functional class, but a set of useful headers providing structs and defines useful for developing on the A2650 platform
 *
 *** things objects of this class have
 *
 *
 */


/*****************************************************************************/
/*                                Includes                                   */
/*****************************************************************************/



/*****************************************************************************/
/*                            Macro Definitions                              */
/*****************************************************************************/

// A2560 OTHER
#define EA_USER				(char*)0x020000	// start of user space. ie, put your program here.
#define EA_STACK			(char*)0x080000	// stack location

// A2560 VICKY III
#define VICKY						(char*)0xc40000	// vicky registers?
#define VICKYA_A2560K				(char*)0xfec40000	// vicky III channel A offset
#define VICKYA_CURSOR_CTRL_A2560K	(char*)0xfec40010	// vicky III channel A cursor control register
#define VICKYA_CURSOR_POS_A2560K	(char*)0xfec40014	// vicky III channel A cursor position register (x pos is lower word, y pos is upper word)
#define VICKYB_A2560K				(char*)0xfec80000	// vicky III channel B offset
#define VICKYB_CURSOR_CTRL_A2560K	(char*)0xfec80010	// vicky III channel B cursor control register
#define VICKYB_CURSOR_POS_A2560K	(char*)0xfec80014	// vicky III channel B cursor position register
#define TEXTA_RAM_A2560_MORFE		(char*)0xc60000	// channel A text
#define TEXTA_ATTR_A2560_MORFE		(char*)0xc68000	// channel A attr
#define TEXTB_RAM_A2560_MORFE		(char*)0xca0000	// channel B text
#define TEXTB_ATTR_A2560_MORFE		(char*)0xca8000	// channel B attr
#define TEXTA_RAM_A2560U			(char*)0xb60000	// text (A2560U only has one video channel)
#define TEXTA_ATTR_A2560U			(char*)0xb68000	// attr (A2560U only has one video channel)
#define TEXTB_RAM_A2560U			(char*)0xb60000	// text (A2560U only has one video channel)
#define TEXTB_ATTR_A2560U			(char*)0xb68000	// attr (A2560U only has one video channel)
#define TEXTA_RAM_A2560K			(char*)0xfec60000	// channel A text
#define TEXTA_ATTR_A2560K			(char*)0xfec68000	// channel A attr
#define TEXTB_RAM_A2560K			(char*)0xfeca0000	// channel B text
#define TEXTB_ATTR_A2560K			(char*)0xfeca8000	// channel B attr

// subtract 0xfe000000 from the UM map for Vicky (to get the old/morfe addresses)
// size of some areas changed too:
//   channel-A text went 0xc6:0000 -> 0xFEC6:0000, but channel-A color went 0xc6:8000 -> 0xFEC6:4000
//   channel-B text went 0xca:0000-> 0xFECA:0000, channel-B color went 0xca:8000 -> 0xFECA:4000
// btw, one thing to keep in mind is device-mem access granularity -- while in morfe you can do 8-32bit accesses, on the actual hw you will need to adhere to the area access granularity
//  see user manual, the "SIZE" columns

// c256foenix on 2/27:
// I will have to look into that. Suffice to say that Channel A, has 2 Video Modes, 800x600 and 1024x768 with no doubling. Channel B has 6 modes, 640x480@60, 800x600@60 and 640x400@70 and with the equivalent Pixel Doubling, 320x240, 400x300 and 320x200. 
// Now, in the K, for Channel A, to not be confusing (although I might have created what I was trying to avoid) and to not have competing Regiters bit with different function (from Channel B and A), I moved the Resolution selection bit to bit# (Something I have to check), but it is farther down the Control Register.
// the Video mode bit for Channel A (that I call Auxiliary) would be bit 11 of the Control Register
// assign Mstr_Ctrl_Video_Mode_PLL_Aux_o = VICKY_MASTER_REG1_RESYNC_AUX[2][11];
// So, that is the bit that selects either 800x600 or 1024x768 and bit[9:8] are ignored.

//    so: 1024x768 = 128x96
//    800x600 = 100x75
//    640x480 = 80x60
//    640x400 = 80x50
//    400x300 = 50x37.5
//    320x240 = 40x30
//    320x200 = 40x25
//    (these are all maximums, as borders can be configured, which reduces the number of usable rows/cols.)
//
// c256 foenix on 2/27:
// To answer, in the traditional Text Mode (Channel B). When you are double pixel mode, everything is reajusted automatically. The Channel A doesn't have a text doubling mode (anymore). And the text matrix and FONT dimension are all manual (needs to be programmed). This is to allow the usage of different sizes of FONT.


#define TEXTA_NUM_COLS_MORFE		72	// channel A width
#define TEXTA_NUM_ROWS_MORFE		56	// channel A height
#define TEXTB_NUM_COLS_MORFE		80	// channel B width
#define TEXTB_NUM_ROWS_MORFE		60	// channel B height
#define TEXTA_NUM_COLS_A2560K		100	// channel A width
#define TEXTA_NUM_ROWS_A2560K		75	// channel A height
#define TEXTB_NUM_COLS_A2560K		100	// channel B width
#define TEXTB_NUM_ROWS_A2560K		75	// channel B height
#define TEXTA_NUM_COLS_BORDER		4	// need to measure.... 
#define TEXTB_NUM_COLS_BORDER		4	// going on what f68 shows

#define TEXT_COL_COUNT_FOR_PLOTTING_MORFE		80	// regardless of visible cols (between borders), VRAM seems to be fixed at 80 cols across.
#define TEXT_ROW_COUNT_FOR_PLOTTING_MORFE		60	// regardless of visible rows (between borders), VRAM seems to be fixed at 60 rows up/down.
#define TEXT_COL_COUNT_FOR_PLOTTING_A2560K		100	// regardless of visible cols (between borders), VRAM seems to be fixed at 80 cols across.
#define TEXT_ROW_COUNT_FOR_PLOTTING_A2560K		75	// regardless of visible rows (between borders), VRAM seems to be fixed at 60 rows up/down.
#define TEXT_COL_COUNT_FOR_PLOTTING		TEXT_COL_COUNT_FOR_PLOTTING_A2560K	// regardless of visible cols (between borders), VRAM seems to be fixed at 80 cols across.
#define TEXT_ROW_COUNT_FOR_PLOTTING		TEXT_ROW_COUNT_FOR_PLOTTING_A2560K	// regardless of visible rows (between borders), VRAM seems to be fixed at 60 rows up/down.

#define CHANNEL_A_WIDTH		576	// pixels in between the borders
#define CHANNEL_A_HEIGHT	448	// pixels in between the borders
#define CHANNEL_B_WIDTH		640	// pixels in between the borders
#define CHANNEL_B_HEIGHT	480	// pixels in between the borders

#define ID_CHANNEL_A			0	// for use in lib_text() calls, etc. 
#define ID_CHANNEL_B			1	// for use in lib_text() calls, etc.

#define TEXT_FONT_WIDTH_A2650	8	// for text mode, the width of the fixed-sized font chars
#define TEXT_FONT_HEIGHT_A2650	8	// for text mode, the height of the fixed-sized font chars. I believe this is supposed to be 16, but its 8 in morfe at the moment.

//#define FONT_MEMORY_BANK0	(char*)0xAF8000	// $AF8000 - $AF87FF
//#define FONT_MEMORY_BANK1	(char*)0xAF8800	// $AF8800 - $AF8FFF

// from vicky3.go in morfe tho:
//const FONT_MEMORY_BANK0           = 0x8000
#define FONT_MEMORY_BANKA_A2560_MORFE	(char*)0xc48000		// $AF8000 - $AF87FF
#define FONT_MEMORY_BANKB_A2560_MORFE	(char*)0xc48800		// $AF8800 - $AF8FFF
#define FONT_MEMORY_BANKA_A2560U		(char*)0xb48000		// needs update
#define FONT_MEMORY_BANKB_A2560U		(char*)0xb48000		// needs update
#define FONT_MEMORY_BANKA_A2560K		(char*)0xfec48000	// chan A
#define FONT_MEMORY_BANKB_A2560K		(char*)0xfec88000	// chan B

// gadget:
// If it's the same as on the C256 line, each character consists of 8 bytes.  Upper left hand corner is the high-bit of byte zero, upper right is the low bit of byte zero, lower left is the high bit of byte 7, lower right is the low bit of byte 7.  The bytes are placed in memory from character zero to character 255, 0..7, 0..7, 0..7, etc.

// pjw:
// Yeah, C256 and A2560U fonts are the same layout. The A2560K is a little different. The mostly-text-mode screen supports an 8x16 font layout, where the structure is essentially the same, but each character is 16 bytes rather than 8.

// beethead:
// The 2nd font was removed when the U line came in since it was not being used.  Atleast on the C256's.

#define SYS_TICKS_PER_SEC		60	// per syscalls.h in MCP, "a jiffie is 1/60 of a second."




// amiga rawkey codes
#define KEYCODE_CURSOR_LEFT		79
#define KEYCODE_CURSOR_RIGHT	78
#define KEYCODE_CURSOR_UP		76
#define KEYCODE_CURSOR_DOWN		77

#define KEYCODE_RETURN			0x44					// rawkey code
#define KEYCODE_RETURN_UP		KEY_RAW_RETURN & 0x80
#define KEY_RETURN			13	// vanilla key
#define KEY_ESC				27	// vanilla key

#define CHAR_UMLAUT			0xA8	// ¨ char -- will use in place of ellipsis when truncating strings
#define CHAR_ELLIPSIS		CHAR_UMLAUT

#define KEY_BUFFER_SIZE		16	// unlikely anyone would ever want to type 15 chars to select a file

#define ALERT_MAX_MESSAGE_LEN		256	// 255 chars + terminator. seems long, but with formatting chars, not crazy.

#define ALERT_DIALOG_SHOW_AS_ERROR	true	// parameter for General_ShowAlert()
#define ALERT_DIALOG_SHOW_AS_INFO	false	// parameter for General_ShowAlert()

#define ALERT_DIALOG_INCLUDE_CANCEL	true	// parameter for General_ShowAlert()
#define ALERT_DIALOG_NO_CANCEL_BTN	false	// parameter for General_ShowAlert()

#define ALERT_DIALOG_1ST_BUTTON	0	// return value for General_ShowAlert()
#define ALERT_DIALOG_2ND_BUTTON	1	// return value for General_ShowAlert()
#define ALERT_DIALOG_3RD_BUTTON	2	// return value for General_ShowAlert()
// NOTE: can't define "OK" as 1 or 0, because it depends on passing ok, then cancel (eg). Buttons appear to be numbered from RIGHT to LEFT!

/*****************************************************************************/
/*                               Enumerations                                */
/*****************************************************************************/


/*****************************************************************************/
/*                                 Structs                                   */
/*****************************************************************************/

typedef struct Coordinate
{
    int x;
    int y;
} Coordinate;

typedef struct Rectangle
{
	signed short   MinX, MinY;
	signed short   MaxX, MaxY;
} Rectangle;

typedef struct Screen
{
	signed int		id_;				// 0 for channel A, 1 for channel B. not all foenix's have 2 channels.
	Rectangle		rect_;				// the x1, y1, > x2, y2 coordinates of the screen, taking into account any borders. 
	signed int		width_;				// for the current resolution, the max horizontal pixel count 
	signed int		height_;			// for the current resolution, the max vertical pixel count 
	signed int		text_cols_vis_;		// accounting for borders, the number of visible columns on screen
	signed int		text_rows_vis_;		// accounting for borders, the number of visible rows on screen
	signed int		text_mem_cols_;		// for the current resolution, the total number of columns per row in VRAM. Use for plotting x,y 
	signed int		text_mem_rows_;		// for the current resolution, the total number of rows per row in VRAM. Use for plotting x,y 
	char*			text_ram_;
	char*			text_attr_ram_;
	char*			text_font_ram_;		// 2K of memory holding font definitions.
	signed int		text_font_height_;	// in text mode, the height in pixels for the fixed width font. Should be either 8 or 16, depending on which Foenix. used for calculating text fit.
	signed int		text_font_width_;	// in text mode, the width in pixels for the fixed width font. Unlikely to be other than '8' with Foenix machines. used for calculating text fit.
	char			text_temp_buffer_1_[TEXT_COL_COUNT_FOR_PLOTTING_A2560K * TEXT_ROW_COUNT_FOR_PLOTTING_A2560K + 1];	// todo: replace with pointer, and allocate space on resolution switch. general use temp buffer - do NOT use for real storage - any utility function clobber it
	char			text_temp_buffer_2_[TEXT_COL_COUNT_FOR_PLOTTING_A2560K * TEXT_ROW_COUNT_FOR_PLOTTING_A2560K + 1];	// todo: replace with pointer, and allocate space on resolution switch. general use temp buffer - do NOT use for real storage - any utility function clobber it
} Screen;


typedef enum
{
	false = 0,
	true = 1
} boolean;

/*****************************************************************************/
/*                             Global Variables                              */
/*****************************************************************************/


/*****************************************************************************/
/*                       Public Function Prototypes                          */
/*****************************************************************************/





#endif /* A2650_PLATFORM_H_ */
