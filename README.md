# A2650-TextLibrary
This is a simple C library providing basic functions for the A2650's text mode

## What's an A2650?
The A2650 is a newly recreated retro computer based around a Motorolla 680x0. It is not a new Amiga, Atari ST, Mac, NeXT: it is a new thing, designed for hobbyist users who want to experience coding and using the "simpler" computers of the late 1980s/early 1990s. It is part of a family of computers that share much of the same architecture. Predecessors include the C256 Foenix, running a 65816 (fast) 8-bit processor. 
Web site: https://c256foenix.com/?v=3e8d115eb4b3

## About this project
This is a learning project for me, to build experience on the A2650. Aside from learning, the goal is to be able to build a library of functions that can be used to place characters, colors, strings, etc., on the screen, and to be able to query the screen about what is at a particular location. Think of it as a basic curses for the A2650 (but not at all related to curses, and using a completely different interface). 

## Status
Non-functional, 5% complete.
I do not have any hardware yet. I'm doing all development against the "morfe" emulator, but that itself is also a tool in development. 

## Why you might want this:
If you have, or are considering acquiring an A2650 or GenX (680x0 + 65816), and want a simple library to put text on the screen in your C programs. 
Note: **NO ONE SHOULD USE THIS, OR EVEN LOOK IN ITS DIRECTION!** (until I get some real hardware and can complete the job).

## How to use it
The A2650 kernel/OS doesn't support dynamically linked libraries yet, so for now, you have you just compile this into your app. 

## Expected final functionality
 * x work with either channel A or channel B
 * x clear / fill an entire screen of text characters
 * x clear / fill an entire screen of text attributes
 * invert the colors of a screen
 * clear / fill a smaller-than-screen rectangular area of text/attrs
 * Draw a string to a specified x, y coord (no wrap)
 * x Draw a char to a specified x, y coord
 * x Get the currently displayed character at the specified coord
 * x Set the foreground and background colors at the specified coord
 * x Set the attribute value at the specified coord
 * x Get the attribute value at the specified coord
 * x Get the foreground or background color at the specified coord
 * draw a line using "graphic" characters
 * draw a box using "graphic" characters
 * copy a full screen of text or attr from an off-screen buffer
 * copy a full screen of text or attr TO an off-screen buffer
 * copy a rectangular area of text or attr TO/FROM an off-screen buffer
 * display a string in a rectangular block on the screen, with wrap
 * display a string in a rectangular block on the screen, with wrap, taking a hook for a "display more" event, and scrolling text vertically up after hook func returns 'continue' (or exit, returning control to calling func, if hook returns 'stop')
 * replace current text font with another, loading from specified ram loc.
 
## Completed
 * work with either channel A or channel B
 * clear / fill an entire screen of text characters
 * clear / fill an entire screen of text attributes
 * Draw a char to a specified x, y coord
 * Get the currently displayed character at the specified coord
 * Set the foreground and background colors at the specified coord
 * Set the attribute value at the specified coord
 * Get the attribute value at the specified coord
 * Get the foreground or background color at the specified coord
