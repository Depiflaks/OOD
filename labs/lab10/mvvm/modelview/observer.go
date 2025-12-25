package modelview

import (
	"vector-editor/geometry"
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
	UpdateRect(position geometry.Point, bounds geometry.Bounds)
	UpdateStyle(style geometry.Style)
}

type ToolbarModelViewObserver interface {
	// TODO: добавить что-то сюда
	Update()
}
