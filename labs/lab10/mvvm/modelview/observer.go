package modelview

import (
	"vector-editor/graphics"
	"vector-editor/model"
)

type WorkspaceModelViewObserver interface {
	// TODO: добавить что-то сюда
	Update()
}

type CanvasModelViewObserver interface {
	UpdateShapes(ids []model.ShapeId)
}

type ShapeModelViewObserver interface {
	UpdateRect(position graphics.Point, bounds graphics.Bounds)
	UpdateStyle(style graphics.Style)
}

type ToolbarModelViewObserver interface {
	// TODO: добавить что-то сюда
	Update()
}
