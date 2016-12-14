#include	<stdio.h>
#include	<curses.h>
#include	<signal.h>
#include	<string.h>
#include	<termios.h>


#define	TOP	20
#define	BOTTOM	40
#define	LEDGE	20
#define	REDGE	40

#define X_INIT  30
#define Y_INIT  30
#define TICKS_PER_SEC   50

#define	Wstone	'O'
#define	Bstone	'X'
#define	BLANK	' '

#define	UP	'w'
#define	DOWN	's'
#define	LEFT	'a'
#define	RIGHT	'd'
#define	SPACE	32





struct	stone{
	int	x_pos, y_pos;
	char	symbol;
};
