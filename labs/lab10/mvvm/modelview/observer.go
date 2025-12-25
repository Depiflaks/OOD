package modelview

import "vector-editor/model"

type WorkspaceModelViewObserver interface {
	// TODO: добавить что-то сюда
	Update()
}

type CanvasModelViewObserver interface {
	UpdateShapes(ids []model.ShapeId)
}

type ShapeModelViewObserver interface {
	UpdateRect()
	UpdateStyle()
}

type ToolbarModelViewObserver interface {
	// TODO: добавить что-то сюда
	Update()
}
