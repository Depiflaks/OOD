package graphics

import "image/color"

type Vector2D struct {
	X float64
	Y float64
}

type Bounds struct {
	Width  float64
	Height float64
}

type Color color.Color

type Style struct {
	fill   Color
	stroke Color
}

func (s *Style) GetFill() Color {
	return s.fill
}

func (s *Style) SetFill(c Color) {
	s.fill = c
}

func (s *Style) GetStroke() Color {
	return s.stroke
}

func (s *Style) SetStroke(c Color) {
	s.stroke = c
}
