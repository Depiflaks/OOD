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

type Bounds struct {
	Width  float64
	Height float64
}

type Style struct {
	fill   color.Color
	stroke color.Color
}

func (s *Style) GetFill() color.Color {
	return s.fill
}

func (s *Style) SetFill(c color.Color) {
	s.fill = c
}

func (s *Style) GetStroke() color.Color {
	return s.stroke
}

func (s *Style) SetStroke(c color.Color) {
	s.stroke = c
}
