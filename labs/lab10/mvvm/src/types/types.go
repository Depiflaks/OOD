package types

type ShapeType int

const (
	Rect ShapeType = iota
	Ellipse
	Triangle
)

type ShapeId int64
