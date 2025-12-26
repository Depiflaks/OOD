package main

import (
	"image/color"

	"fyne.io/fyne/v2/app"

	"vector-editor/src/appmodel"
	"vector-editor/src/model"
	"vector-editor/src/modelview"
	"vector-editor/src/view"
)

func main() {
	a := app.New()

	// model
	canvasModel := model.NewCanvas()

	// manager
	canvasManager := appmodel.NewCanvasManager()

	// modelview
	canvasMV := modelview.NewCanvasModelView(canvasModel, canvasManager)
	canvasManager.RegisterCanvas(canvasMV)
	toolbarMV := modelview.NewToolbarModelView(canvasManager)
	wsMV := modelview.NewWorkspaceModelView(canvasMV, toolbarMV)

	files := view.FileActions{
		Open:   func() {},
		Save:   func() {},
		SaveAs: func() {},
	}

	var currentFill color.Color = color.RGBA{R: 255, G: 255, B: 255, A: 255}
	var currentStroke color.Color = color.RGBA{R: 0, G: 0, B: 0, A: 255}

	colors := view.ColorActions{
		SetFill:   func(c color.Color) { currentFill = c; _ = currentFill },
		SetStroke: func(c color.Color) { currentStroke = c; _ = currentStroke },
	}

	_ = currentFill
	_ = currentStroke

	w := view.NewWorkspaceView(a, wsMV, files, colors)
	w.Show()

	a.Run()
}
