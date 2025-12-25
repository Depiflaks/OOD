package graphics

import "image/color"

type Vector struct {
	X float64
	Y float64
}

type Point struct {
	X float64
	Y float64
}

type Scale struct {
	ScaleX float64
	ScaleY float64
}

type Bounds struct {
	Width  float64
	Height float64
}

type Style struct {
	Fill   *color.Color
	Stroke *color.Color
}
