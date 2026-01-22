package view

import (
	"image"

	"gioui.org/layout"
	"gioui.org/op/clip"
)

type ShapeStrategy interface {
	GetOptions(gtx layout.Context, rect image.Rectangle) (fillOp clip.Op, strokeOp clip.Op)
}

type RectStrategy struct{}

func (s RectStrategy) GetOptions(gtx layout.Context, rect image.Rectangle) (clip.Op, clip.Op) {
	obj := clip.Rect(rect)
	fillOp := obj.Op()
	strokePath := obj.Path()

	strokeOp := clip.Stroke{Path: strokePath, Width: strokeWidth}.Op()

	return fillOp, strokeOp
}

type EllipseStrategy struct{}

func (s EllipseStrategy) GetOptions(gtx layout.Context, rect image.Rectangle) (clip.Op, clip.Op) {
	obj := clip.Ellipse(rect)
	fillOp := obj.Op(gtx.Ops)
	strokePath := obj.Path(gtx.Ops)

	strokeOp := clip.Stroke{Path: strokePath, Width: strokeWidth}.Op()

	return fillOp, strokeOp
}

type TriangleStrategy struct{}

func (s TriangleStrategy) GetOptions(gtx layout.Context, rect image.Rectangle) (clip.Op, clip.Op) {
	var path clip.Path
	a, b, c := GetTrianglePoints(rect)

	path.Begin(gtx.Ops)
	path.MoveTo(a)
	path.LineTo(b)
	path.LineTo(c)
	path.Close()

	strokePath := path.End()

	fillOp := clip.Outline{Path: strokePath}.Op()
	strokeOp := clip.Stroke{Path: strokePath, Width: strokeWidth}.Op()

	return fillOp, strokeOp
}
