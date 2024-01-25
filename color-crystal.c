#include <stdio.h>


/*

$ pnmcolormap 16 /tmp/axel.pnm | hd
00000000  50 36 0a 31 36 20 31 0a  36 33 0a 02 03 03 06 04  |P6.16 1.63......|
00000010  03 0e 03 03 1c 13 0c 17  09 0b 17 18 0a 1c 13 29  |...............)|
00000020  31 17 2a 2c 29 1b 09 09  07 2b 2d 30 0d 0d 0a 2f  |1.*,)....+-0.../|
00000030  17 0d 0e 0f 20 12 09 07  0e 0a 03                 |.... ......|

$ pnmcolormap 16 /tmp/axel.pnm | cat -nev
     1	P6$
     2	16 1$
     3	63$
     4	^B^C^C^F^D^C^N^C^C^\^S^L^W	^K^W^X$
     5	^\^S)1^W*,)^[		^G+-0^M^M$
     6	/^W^M^N^O ^R	^G^N$
     7	^C
*/

char colors[] = {
	3, 3, 3, // white
	2, 2, 2, // light gray
	1, 1, 1, // gray
	0, 0, 0, // black

	// lighter:
	3, 1, 1, // red
	0, 3, 3, // anti-red

	1, 3, 1, // green
	3, 0, 3, // anti-green

	1, 1, 3, // blue
	3, 3, 0, // anti-blue

	// darker:
	2, 0, 0, // red
	0, 1, 1, // anti-red

	0, 2, 0, // green
	1, 0, 1, // anti-green

	0, 0, 2, // blue
	1, 1, 0, // anti-blue
};

int main() {
	printf("P6\n16 1\n63\n");

	for (int i = 0; i < sizeof colors; i++) {
		putchar(20 * colors[i]);
	}
}

/*

$ jpegtopnm axel.jpg > /tmp/axel.pnm
( $ pnmcolormap 16 axel.jpg )

$ ./color-crystal > /tmp/cc
$ pnmscale -xysize 128 96  /tmp/axel.pnm > /tmp/a.pnm
$ pnmremap -mapfile=/tmp/cc /tmp/a.pnm > /tmp/acc.pnm

-----------------

 2415  jpegtopnm ~/Pictures/teletubbies.jpg > /tmp/tt.pnm
 2416  pnmscale -xysize 128 96 /tmp/tt.pnm > /tmp/t.pnm
 2417  pnmremap -mapfile=/tmp/cc /tmp/t.pnm > /tmp/tcc.pnm

*/
