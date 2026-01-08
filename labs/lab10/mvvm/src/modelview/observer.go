package modelview

import (
	"vector-editor/src/model"
)

type WorkspaceModelViewObserver interface {
	// TODO: добавить что-то сюда
	Update()
}

type CanvasModelViewObserver interface {
	OnShapesChanged(ids []model.ShapeId)
}

type ShapeModelViewObserver interface {
	Update()
}

type ToolbarModelViewObserver interface {
	// TODO: добавить что-то сюда
	Update()
}
