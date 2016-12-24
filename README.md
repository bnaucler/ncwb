# ncwb (NCurses Window Bounce) v0.2
A rather pointless application bouncing a message around the terminal window. Or - my first attempt at getting familiar with ncurses

## Written by
Björn Westerberg Nauclér (mail@bnaucler.se) 2016

Compiled and tested on Mac OS Sierra and Arch Linux (amd64)

## Installation
`sudo make all install` - The binary is installed in /usr/bin unless otherwise specified with DESTDIR

## Usage
Output of `ncwb -h`:  
```
NCurses Window Bounce - Bounces [message] around the screen

Usage: ncwb [-cdfhlnrstp] [message]
Options:
	-c [num]: Color pair 1-6 (default: 1)
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

## Notes
Due to (probably) a bug in ncurses, an ugly border will be drawn if combining -f and -t.

## License
MIT (Do whatever you want)
