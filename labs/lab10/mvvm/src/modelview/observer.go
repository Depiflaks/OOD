package modelview

import (
	"vector-editor/src/geometry"
	"vector-editor/src/model"
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
	UpdateDeleted(deleted bool)
}

type ToolbarModelViewObserver interface {
	// TODO: добавить что-то сюда
	Update()
}
