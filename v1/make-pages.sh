#!/bin/sh
set -eux

cat <<~~~
  Add(80, 0, "Video Player for coco3", &Card{})
~~~

p=80
i=801
for x
do

  b=$(basename "$x")

  cat <<~~~
      Add($i, $p, "$(expr substr "$b" 1 24)", &Card{
          Launch: ".$b.lemma",
	  Text: "coco3 Video player:\n  $b",
      })
~~~


  i=$(expr 1 + $i)
  case $i in
    *0)
      i=$(expr 1 + $i)
      n=$(expr 1 + $p)
      cat <<~~~
      Add($n, $p, "MORE", &Card{})
~~~
      p=$n
  esac



done
