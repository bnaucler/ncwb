# ncwb (NCurses Window Bounce) v0.1
A rather pointless application bouncing a message around the terminal window. Or - my first attempt at getting familiar with ncurses

## Written by
Björn Westerberg Nauclér (mail@bnaucler.se) 2016

## Installation
`sudo make all install` - The binary is installed in /usr/bin unless otherwise specified with DESTDIR

## Usage
Output of `ncwb -h`:  
```
NCurses Window Bounce - Bounces [message] around the screen

Usage: ./ncwb [-dfhlnrstp] [message]
Options:
	-c: Color pair [1-6] (default: 1)
	-d: Show debug information
	-f: Use figlet
	-h: Displays this text
	-l: Leave trail
	-n: No window movement
	-r: Random window movement
	-s [ms]: Sleep per cycle (default: 200)
	-t: Use current time as message
	-p [size]: # of chars padding (default: 2)
```

## Disclaimer
It's pretty buggy. Expect it to do kind of what you ask it to, but not exactly.

## License
MIT (Do whatever you want)
