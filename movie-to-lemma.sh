#!/bin/sh
set -eux

../bin/gcc6809  -Os -S  --std=gnu99 -I../frobio/ -fwhole-program  tv.c 
lwasm.orig --obj -o tv.o tv.s --list=tv.list 
lwlink --decb --entry=_main --map=tv.map --output=tv.cmd tv.o

cc -o color-crystal color-crystal.c
./color-crystal > /tmp/cc

T=/tmp/movie.tmp
for movie
do
	rm -rf $T
	mkdir -p $T
	# ffmpeg -y -i "$movie" -an -vf "setpts=1.25*PTS,scale=128:96,setsar=1:1" -r 5 $T/out%06d.png
	ffmpeg -y -i "$movie" -an -filter:v fps=fps=5,scale=128:96  $T/out%06d.png

	ffmpeg -y -i "$movie" -ac 1 -ar 8000 -c:a libmp3lame -q:a 5 $T/out.mp3
	ffmpeg -y -i $T/out.mp3 -f u8 -acodec pcm_u8 $T/out.u8

	( cd $T
	  
	  for x in out*.png
	  do
	    y=$(basename $x .png)
	    pngtopnm $x > $y.pnm
	    pnmremap -mapfile=/tmp/cc $y.pnm > new.$y.pnm
	    pnmtopng new.$y.pnm > new.$y.png
	  done
	)

	mkdir -p "$(dirname "$movie")/lemma"
	(
		cat tv.cmd
		go run pngs-to-coco-stream.go --audio=$T/out.u8 $T/new.*.png
	) > "$(dirname "$movie")/lemma/$(basename "$movie").lemma"
done

# rm -rf $T color-crystal tv.[abd-z]*


# ffmpeg -y -i Teletubbies\ say\ \'Eh-oh\!\'\ 🎵\ Music\ Video\ 🎵-DIDAUViRnAU.mkv  -ac 1 -ar 6144 -c:a libmp3lame -q:a 5 out.mp3
