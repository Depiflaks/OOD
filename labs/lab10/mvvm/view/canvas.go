package view

import (
	"image/color"

	"fyne.io/fyne/v2"
	"fyne.io/fyne/v2/canvas"
)

type ShapeView struct {
	obj  fyne.CanvasObject
	pos  fyne.Position
	size fyne.Size
}

func NewRect(pos fyne.Position, size fyne.Size, c color.Color) *ShapeView {
	r := canvas.NewRectangle(c)
	r.Move(pos)
	r.Resize(size)
	return &ShapeView{obj: r, pos: pos, size: size}
}

func (s *ShapeView) Object() fyne.CanvasObject {
	return s.obj
}

func (s *ShapeView) Contains(p fyne.Position) bool {
	return p.X >= s.pos.X &&
		p.Y >= s.pos.Y &&
		p.X <= s.pos.X+s.size.Width &&
		p.Y <= s.pos.Y+s.size.Height
}
