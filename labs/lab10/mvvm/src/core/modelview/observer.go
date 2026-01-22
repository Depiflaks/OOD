package modelview

import (
	"image/color"
	"vector-editor/src/types"
	"vector-editor/src/types/draw"
)

type BackgroundUpdateObserver interface {
	OnBackgroundUpdate(color color.Color)
}

type WorkspaceModelViewObserver interface {
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
