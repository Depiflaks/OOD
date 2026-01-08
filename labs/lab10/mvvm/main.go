package main

import (
	"image/color"
	"vector-editor/src/geometry"

	"fyne.io/fyne/v2/app"

	"vector-editor/src/manager"
	"vector-editor/src/model"
	"vector-editor/src/modelview"
	"vector-editor/src/view"
)

func main() {
	a := app.New()

	// model
	canvasModel := model.NewCanvas()

	// manager
	canvasManager := manager.NewCanvasManager()

	// modelview
	canvasMV := modelview.NewCanvasModelView(canvasModel, canvasManager)
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

	// test
	toolbarMV.NewRectangle()
	sh := canvasMV.GetShape(0)
	sh.StartResizing()
	sh.Resize(geometry.Vector{
		X: 10,
		Y: 10,
	}, geometry.Scale{
		ScaleX: 1.1,
		ScaleY: 1.1,
	})
	canvasMV.GetShape(0)

	a.Run()
}
