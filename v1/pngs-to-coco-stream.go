package main

import (
	"bufio"
	"flag"
	"image"
	"image/png"
	"image/color"
	"log"
	"io"
	"os"
)

var AUDIO = flag.String("audio", "", "Filename with pcm_u8 audio")

var colors = [][3]byte {
	{3, 3, 3, }, // white
	{2, 2, 2, }, // light gray
	{1, 1, 1, }, // gray
	{0, 0, 0, }, // black

	// lighter:
	{3, 1, 1, }, // red
	{0, 3, 3, }, // anti-red

	{1, 3, 1, }, // green
	{3, 0, 3, }, // anti-green

	{1, 1, 3, }, // blue
	{3, 3, 0, }, // anti-blue

	// darker:
	{2, 0, 0, }, // red
	{0, 1, 1, }, // anti-red

	{0, 2, 0, }, // green
	{1, 0, 1, }, // anti-green

	{0, 0, 2, }, // blue
	{1, 1, 0, }, // anti-blue
}

func main() {
	flag.Parse()
	var err error
	var fd *os.File

	if *AUDIO != "" {
		fd, err = os.Open(*AUDIO)
		if err != nil { panic(err) }
		Send1600(nil)
	}

	for _, a := range flag.Args() {
		if *AUDIO != "" {
			Send1600(fd)
		}
		SendPng(a)
	}

	// Clear the audio buffer at the end.
	if *AUDIO != "" {
		Send1600(nil)
	}
}

func Send1600(fd *os.File) {
	buf := make([]byte, 1600)
	for i := range buf {
		buf[i] = 128
	}

	if fd != nil {
		io.ReadFull(fd, buf)
	}
	n, err := os.Stdout.Write(buf)
	if err != nil { panic(err) }
	if n != len(buf) { panic("short") }
}

func SendPng(filename string) {
	fd, err := os.Open(filename)
	if err != nil { panic(err) }
	r := bufio.NewReader(fd)
	img, err := png.Decode(r)
	if err != nil { panic(err) }

	log.Printf("image %q bounds %v model %v", filename, img.Bounds(), img.ColorModel())

	SerializePng(img)
}

func SerializePng(img image.Image) {
	sz := img.Bounds().Size()

	var zz []byte
	for y := 0; y < sz.Y; y++ {
		for x := 0; x < sz.X; x+=2 {
			left := NormalizeColor(img.At(x,y))
			right := NormalizeColor(img.At(x+1,y))

			zz = append(zz, (left<<4) | right)
		}
	}
	os.Stdout.Write(zz)
}

func NormalizeColor(c color.Color) byte {
	r, g, b, _ := c.RGBA()

	r_ := byte((r+2)/81)
	g_ := byte((g+2)/81)
	b_ := byte((b+2)/81)

	for i, e := range colors {
		if r_ == e[0] && g_ == e[1] && b_ == e[2] {
			return byte(i)
		}
	}
	panic(c)
}
