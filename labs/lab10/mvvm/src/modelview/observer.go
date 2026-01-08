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
	OnShapesChanged(ids []model.ShapeId)
	OnBackgroundChanged()
}

type ShapeModelViewObserver interface {
	Update()
}

type ToolbarModelViewObserver interface {
	OnSelectionChange(style geometry.Style, selectedCount int)
}
