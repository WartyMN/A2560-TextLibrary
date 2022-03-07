@mainpage Foenix A2650 Text Library
# A2650-TextLibrary
This is a simple C library providing basic functions for the A2650's text mode

## What's an A2650?
The A2650 is a newly recreated retro computer based around a Motorolla 680x0. It is not a new Amiga, Atari ST, Mac, NeXT: it is a new thing, designed for hobbyist users who want to experience coding and using the "simpler" computers of the late 1980s/early 1990s. It is part of a family of computers that share much of the same architecture. Predecessors include the C256 Foenix, running a 65816 (fast) 8-bit processor. 
Web site: https://c256foenix.com/?v=3e8d115eb4b3

## About this project
This is a learning project for me, to build experience on the A2650. Aside from learning, the goal is to be able to build a library of functions that can be used to place characters, colors, strings, etc., on the screen, and to be able to query the screen about what is at a particular location. Think of it as a basic curses for the A2650 (but not at all related to curses, and using a completely different interface). 

## Status
75% complete. Next phase: Bug-fixing, smarter configuration, etc., No optimization done, no testing against hardware.
I do not have any hardware yet. I'm doing all development against the f68 emulator, but that itself is also a tool in development. 

## Why you might want this:
If you have, or are considering acquiring an A2650 or GenX (680x0 + 65816), and want a simple library to put text on the screen in your C programs. 
Note: **NO ONE SHOULD USE THIS, OR EVEN LOOK IN ITS DIRECTION!** (until I get some real hardware and can complete the job).

## How to use it
The A2650 kernel/OS doesn't support dynamically linked libraries yet, so for now, you have you just compile this into your app. 

## Expected final functionality
 * work with either channel A or channel B
 * clear / fill an entire screen of text characters
 * clear / fill an entire screen of text attributes
 * invert the colors of a screen
 * clear / fill a smaller-than-screen rectangular area of text/attrs
 * Draw a char to a specified x, y coord
 * Get the currently displayed character at the specified coord
 * Set the foreground and background colors at the specified coord
 * Set the attribute value at the specified coord
 * Get the attribute value at the specified coord
 * Get the foreground or background color at the specified coord
 * draw a line using "graphic" characters
 * draw a box using "graphic" characters
 * copy a full screen of text or attr from an off-screen buffer
 * copy a full screen of text or attr TO an off-screen buffer
 * copy a full screen of text and attr between channel A and B
 * copy a rectangular area of text or attr TO/FROM an off-screen buffer
 * display a pre-formatted string in a rectangular block on the screen, breaking on \n characters
 * Draw a string to a specified x, y coord (no wrap)
 * format a string for a given height and width, wrapping words as necessary
 * display a formatted string in a rectangular block on the screen, with wrap
 * display a string in a rectangular block on the screen, with wrap, taking a hook for a "display more" event, and scrolling text vertically up after hook func returns 'continue' (or exit, returning control to calling func, if hook returns 'stop')
 * replace current text font with another, loading from specified ram loc.
 * configure at compile time for use in various Foenix machines
 
## Completed
 * work with either channel A or channel B
 * clear / fill an entire screen of text characters
 * clear / fill an entire screen of text attributes
 * invert the colors of a screen
 * clear / fill a smaller-than-screen rectangular area of text/attrs
 * Draw a char to a specified x, y coord
 * Get the currently displayed character at the specified coord
 * Set the foreground and background colors at the specified coord
 * Set the attribute value at the specified coord
 * Get the attribute value at the specified coord
 * Get the foreground or background color at the specified coord
 * draw a line using "graphic" characters
 * draw a box using "graphic" characters
 * copy a full screen of text or attr from an off-screen buffer
 * copy a full screen of text or attr TO an off-screen buffer
 * copy a rectangular area of text or attr TO/FROM an off-screen buffer
 * Draw a string to a specified x, y coord (no wrap)
 * format a string for a given height and width, wrapping words as necessary
 * display a formatted string in a rectangular block on the screen, with wrap
 * display a string in a rectangular block on the screen, with wrap, taking a hook for a "display more" event, and scrolling text vertically up after hook func returns 'continue' (or exit, returning control to calling func, if hook returns 'stop')
 * replace current text font with another, loading from specified ram loc.
 * documentation (for text lib, have not started on general lib)
 * prep and clean up to be a standalone static library

## ToDo
 * copy a full screen of text and attr between channel A and B
 * ~~clean up the A2650U/K vs morfe addresses, default colors, etc.~~
 * optimization
 * add function(s) to switch resolutions, and automatically adjust number of cols/rows
 * add function(s) to adjust border sizes, and automatically adjust number of visible rows/columns
 * configure at compile time for use in various Foenix machines
 * ~~fix bug with word wrap declining to present the final line in the string if string is shorter than box.~~
 * clean up text wrap buffers etc
 * strategy for handling DEBUG macros (build 2 versions of library? one debug mode, one not? TBD)
 * document a2560_platform.h
 * document lib_general.c
 
