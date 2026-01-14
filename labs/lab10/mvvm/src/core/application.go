package core

import (
	"log"
	"os"
	"vector-editor/src/core/modelview"
	"vector-editor/src/core/view"
	"vector-editor/src/samples"

	"gioui.org/app"
)

func Run(
	mv modelview.WorkspaceModelView,
) {
	go func() {
		window := new(app.Window)
		_ = view.NewWorkspaceView(mv, window)
		loop := func() error {
			//return wv.Run()
			return samples.ToolbarLoop(window)
		}
		if err := loop(); err != nil {
			log.Fatal(err)
		}
		os.Exit(0)
	}()
	app.Main()
}

func loop(workspaceView view.WorkspaceView) {
	workspaceView.Run()
}
