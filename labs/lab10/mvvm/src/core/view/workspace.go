package view

import (
	"image/color"

	"gioui.org/app"
	"gioui.org/io/key"
	"gioui.org/layout"
	"gioui.org/op"
	"gioui.org/widget/material"
	"github.com/ncruces/zenity"

	"vector-editor/src/core/modelview"
)

type (
	C = layout.Context
	D = layout.Dimensions
)

const (
	WindowWidth  = 1024
	WindowHeight = 768
	WindowTitle  = "Vertical Editor"
)

type WorkspaceView interface {
	Run() error
}

type workspaceView struct {
	workspaceMV modelview.WorkspaceModelView

	window *app.Window

	toolbar ToolbarView
	canvas  CanvasView

	locked bool

	fileActions FileActions
}

func NewWorkspaceView(
	mv modelview.WorkspaceModelView,
	window *app.Window,
) WorkspaceView {
	window.Option(app.Title(WindowTitle), app.Size(WindowWidth, WindowHeight))
	wv := &workspaceView{
		workspaceMV: mv,
		window:      window,
	}
	wv.loadFileActions()

	wv.toolbar = NewToolbarView(wv.window, mv.Toolbar(), wv.fileActions, wv)
	wv.canvas = NewCanvasView(wv.window, mv.Canvas())

	return wv
}

func (v *workspaceView) loadFileActions() {
	v.fileActions = FileActions{
		Open: func() {
			path, err := openFileDialog("config", []string{"*.xml"})
			if err == nil {
				v.workspaceMV.Open(path)
				v.window.Invalidate()
			}
		},
		Save: func() {
			v.workspaceMV.Save()
		},
		SaveAs: func() {
			path, err := openFolderDialog()
			if err == nil {
				v.workspaceMV.SaveAs(path)
				v.window.Invalidate()
			}
		},
	}
}

func openFileDialog(name string, extensions []string) (string, error) {
	return zenity.SelectFile(
		zenity.FileFilters{
			{Name: name, Patterns: extensions},
		})
}

func openFolderDialog() (string, error) {
	return zenity.SelectFile(zenity.Directory())
}

func (v *workspaceView) Run() error {
	th := material.NewTheme()
	th.Bg = color.NRGBA{R: 255, A: 255}
	var ops op.Ops

	for {
		switch e := v.window.Event().(type) {
		case app.DestroyEvent:
			v.workspaceMV.Close()
			return e.Err
		case app.FrameEvent:
			gtx := app.NewContext(&ops, e)

			layout.Stack{}.Layout(gtx,
				layout.Expanded(func(gtx layout.Context) layout.Dimensions {
					return layout.Flex{Axis: layout.Vertical}.Layout(gtx,
						layout.Rigid(func(gtx layout.Context) layout.Dimensions {
							return v.toolbar.Process(gtx)
						}),
						layout.Flexed(1, func(gtx layout.Context) layout.Dimensions {
							return v.canvas.Process(gtx)
						}),
					)
				}),
			)

			v.processWorkspace(gtx)

			e.Frame(gtx.Ops)
		}
	}
}

func (v *workspaceView) processWorkspace(gtx layout.Context) {
	for {
		ev, ok := gtx.Event(key.Filter{
			Optional: key.ModCtrl | key.ModShift,
		})
		if !ok {
			break
		}
		e, ok := ev.(key.Event)
		if !ok {
			break
		}
		if e.State != key.Press {
			break
		}
		switch e.Name {

		case "Z":
			if e.Modifiers.Contain(key.ModCtrl) {
				if e.Modifiers.Contain(key.ModShift) {
					v.workspaceMV.Redo()
				} else {
					v.workspaceMV.Undo()
				}
			}
		case "S":
			if e.Modifiers.Contain(key.ModCtrl) {
				if e.Modifiers.Contain(key.ModShift) {
					go v.fileActions.SaveAs()
				} else {
					v.workspaceMV.Save()
				}
			}

		case "O":
			if e.Modifiers.Contain(key.ModCtrl) {
				go v.fileActions.Open()
			}
		case key.NameDeleteForward:
			v.workspaceMV.Delete()
		}
	}
}

type Locker interface {
	Lock()
	Unlock()
}

func (v *workspaceView) Lock() {
	v.locked = true
}

func (v *workspaceView) Unlock() {
	v.locked = false
}
