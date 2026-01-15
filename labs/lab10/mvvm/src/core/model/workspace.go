package model

import (
	"encoding/xml"
	"fmt"
	"os"
	"path/filepath"
	"vector-editor/src/types"
	"vector-editor/src/types/geometry"
)

const workspaceFileName = "workspace.xml"

type Workspace interface {
	Canvas() Canvas

	Save(visibleShapes []types.ShapeId)
	SaveAs(path string, visibleShapes []types.ShapeId)
	Open(path string)
	Close()
}

type workspaceModel struct {
	sourcePath string
	canvas     Canvas

	fileStorage FileStorage
}

func NewWorkspaceModel() Workspace {
	return &workspaceModel{
		canvas:      NewCanvas(),
		fileStorage: NewStorage(),
	}
}

func (w *workspaceModel) Canvas() Canvas {
	return w.canvas
}

func (w *workspaceModel) Save(visibleShapes []types.ShapeId) {
	if w.sourcePath == "" {
		return
	}
	w.SaveAs(w.sourcePath, visibleShapes)
}

func (w *workspaceModel) SaveAs(path string, visibleShapes []types.ShapeId) {
	filePath := path
	if filepath.Ext(path) != ".xml" {
		fmt.Println("error: file extension not supported")
		return
	}
	dir := filepath.Dir(filePath)
	fmt.Printf("SaveAs(%s)\n", dir)
	ensureDir(dir)

	var xw xmlWorkspace
	xw.Version = 1

	bg := w.canvas.GetBackground()
	xw.Background = toRGBA8(bg)

	xw.Shapes = make([]xmlShape, 0)

	for _, id := range visibleShapes {
		sh := w.canvas.GetShape(id)
		st := sh.GetStyle()

		xs := xmlShape{
			ID:   int64(sh.GetShapeId()),
			Type: shapeTypeToString(sh.GetShapeType()),
			Position: xmlPoint{
				X: sh.GetPosition().X,
				Y: sh.GetPosition().Y,
			},
			Size: xmlBounds{
				W: sh.GetBounds().Width,
				H: sh.GetBounds().Height,
			},
			Style: styleToXML(st),
		}
		xw.Shapes = append(xw.Shapes, xs)
	}

	xw.NextID = inferNextID(w.canvas)

	storeFile(filePath, xw)

	w.sourcePath = dir
}

func storeFile(filePath string, xw xmlWorkspace) {
	data, err := xml.MarshalIndent(xw, "", "  ")
	if err != nil {
		panic(err)
	}
	data = append([]byte(xml.Header), data...)

	if err := os.WriteFile(filePath, data, 0o644); err != nil {
		panic(err)
	}
}

func (w *workspaceModel) Open(path string) {
	xmlPath := path
	if filepath.Ext(path) != ".xml" {
		fmt.Println("error: file extension not supported")
		return
	}
	fmt.Println("XML:", xmlPath)
	dir := filepath.Dir(xmlPath)

	raw, err := os.ReadFile(xmlPath)
	if err != nil {
		panic(err)
	}

	var xw xmlWorkspace
	if err := xml.Unmarshal(raw, &xw); err != nil {
		panic(err)
	}

	c := w.canvas.(*canvas)
	oldIds := make([]types.ShapeId, 0)
	for id := range c.shapes {
		oldIds = append(oldIds, id)
	}
	c.DeleteShapes(oldIds)

	c.background = fromRGBA8(xw.Background)
	shapeIds := make([]types.ShapeId, 0)
	for _, xs := range xw.Shapes {
		style := styleFromXML(xs.Style, w.fileStorage)
		t := shapeTypeFromString(xs.Type)
		id := xs.ID
		shapeIds = append(shapeIds, types.ShapeId(id))
		sh := NewShape(t, types.ShapeId(id), style)
		c.shapes[types.ShapeId(id)] = sh
		sh.UpdateRect(
			geometry.Point{X: xs.Position.X, Y: xs.Position.Y},
			geometry.Bounds{Width: xs.Size.W, Height: xs.Size.H},
		)
	}

	c.nextId = types.ShapeId(xw.NextID)

	c.notifyShapesChanged(shapeIds)
	c.notifyBackgroundChanged()
	w.sourcePath = dir
}

func (w *workspaceModel) Close() {
	w.fileStorage.close()
}

func inferNextID(c Canvas) int64 {
	if cc, ok := c.(*canvas); ok {
		return int64(cc.nextId)
	}
	var maxi int64 = -1
	for id := range c.Shapes() {
		if int64(id) > maxi {
			maxi = int64(id)
		}
	}
	return maxi + 1
}
