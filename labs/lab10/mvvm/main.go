package main

import (
	"image/color"
	"vector-editor/src/core"
	"vector-editor/src/core/manager"
	"vector-editor/src/core/model"
	"vector-editor/src/core/modelview"
)

func main() {

	// model
	workspaceModel := model.NewWorkspaceModel()

	// manager
	workspaceManager := manager.NewWorkspaceManager()

	// modelview
	wsMV := modelview.NewWorkspaceModelView(workspaceManager, workspaceModel)

	var currentFill color.Color = color.RGBA{R: 255, G: 255, B: 255, A: 255}
	var currentStroke color.Color = color.RGBA{R: 0, G: 0, B: 0, A: 255}

	_ = currentFill
	_ = currentStroke

	core.Run(wsMV)
}
