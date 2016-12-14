# ncwb (NCurses Window Bounce) v0.1
A rather pointless application bouncing a message around the terminal window. Or - my first attempt at getting familiar with ncurses

## Written by
Björn Westerberg Nauclér (mail@bnaucler.se) 2016

## Installation
`sudo make all install` - The binary is installed in /usr/bin unless otherwise specified with DESTDIR

## Usage
Output of `ncwb -h`:  
```
Usage: ncwb [-dsp] [message] - NCurses Window Bounce
Bounces [message] around the screen

Options:
	-d: Show debug information
	-h: Displays this text
	-s [ms]: Sleep per cycle (default: 100)
	-t: Use current time as message
	-p [size]: # of chars padding (default: 2)
```

## License
MIT (Do whatever you want)
