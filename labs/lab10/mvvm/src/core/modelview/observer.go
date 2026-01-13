package modelview

import (
	"vector-editor/src/types"
	"vector-editor/src/types/draw"
)

type WorkspaceModelViewObserver interface {
	// TODO: добавить что-то сюда
	Update()
}

type CanvasModelViewObserver interface {
	OnShapesChanged(ids []types.ShapeId)
	OnBackgroundChanged()
}

type ShapeModelViewObserver interface {
	Update()
}

type ToolbarModelViewObserver interface {
	OnSelectionChange(style draw.Style, selectedCount int)
}
