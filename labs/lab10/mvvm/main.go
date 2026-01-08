package main

import (
	"image/color"

	"fyne.io/fyne/v2/app"

	"vector-editor/src/manager"
	"vector-editor/src/model"
	"vector-editor/src/modelview"
	"vector-editor/src/view"
)

func main() {
	a := app.NewWithID("vector-editor")

	// model
	workspaceModel := model.NewWorkspaceModel()

	// manager
	workspaceManager := manager.NewWorkspaceManager()

	// modelview
	wsMV := modelview.NewWorkspaceModelView(workspaceManager, workspaceModel)

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
	//toolbar := wsMV.Toolbar()
	//canvas := wsMV.Canvas()
	//toolbar.NewRectangle()
	//toolbar.NewRectangle()
	//sh1 := canvas.GetShape(0)
	//sh2 := canvas.GetShape(1)
	//sh1.Select(false)
	//sh1.StartDragging()
	//fmt.Println(sh1.GetPosition().X, sh1.GetPosition().Y)
	//d := geometry.Vector{
	//	X: 100,
	//	Y: 150,
	//}
	//sh1.Drag(d)
	//fmt.Println(sh1.GetPosition().X, sh1.GetPosition().Y)
	//sh1.StopDragging()
	//sh1.Drag(d)
	//
	//sh2.Select(true)
	//sh1.StartResizing()
	//fmt.Println(sh2.GetPosition().X, sh2.GetPosition().Y)
	//sh2.Resize(geometry.Vector{
	//	X: 10,
	//	Y: 10,
	//}, geometry.Scale{
	//	ScaleX: 1.1,
	//	ScaleY: 0.9,
	//})
	//
	//fmt.Println(sh1.GetPosition().X, sh1.GetPosition().Y)
	//fmt.Println(sh2.GetPosition().X, sh2.GetPosition().Y)
	//
	//sh1.StopResizing()
	//sh2.Resize(geometry.Vector{
	//	X: 10,
	//	Y: 10,
	//}, geometry.Scale{
	//	ScaleX: 1.1,
	//	ScaleY: 0.9,
	//})
	//
	//canvas.ClearSelection()
	a.Run()
}
