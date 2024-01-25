# tv-coco-wiznet
Audio/Video player for Coco3

Currently works on coco3 with CocoIO.
This is all a work-on-progress.

Hints:

    $ cd v1
    $ sh movie-to-lemma.sh  ~/Videos/Good\ Morning\ Sunshine\ _\ Original\ Song-W7q1bHK8te0.mp4

Input:  A video file in (say) your ~/Videos directory.

Output:  A lemma file in the `lemma/` subdirectory of the input directory.

( A lemma file is something that can be booted from
https://github.com/strickyak/frobio/tree/main/frob3/lemma
if added to lib/pages.go )

Requires:

   * netpbm
   * ffmpeg
   * golang
   * https://github.com/strickyak/coco-shelf (clone this repo *in* the coco-shelf repo.)

For use with the CocoIO Ethernet cartridge:  https://computerconect.com/products/cocoio-prom
