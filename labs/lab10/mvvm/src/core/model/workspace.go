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
const sourcesDirName = ".sources"

type Workspace interface {
	Canvas() Canvas

	Save()
	SaveAs(path string)
	Open(path string)
}

type workspaceModel struct {
	sourcePath string
	canvas     Canvas
}

func NewWorkspaceModel() Workspace {
	return &workspaceModel{
		canvas: NewCanvas(),
	}
}

func (w *workspaceModel) Canvas() Canvas {
	return w.canvas
}

func (w *workspaceModel) Save() {
	if w.sourcePath == "" {
		return
	}
	w.SaveAs(w.sourcePath)
}

func (w *workspaceModel) SaveAs(dir string) {
	fmt.Printf("SaveAs(%s)\n", dir)
	ensureDir(dir)
	filePath := filepath.Join(dir, workspaceFileName)
	sourcesDir := filepath.Join(dir, sourcesDirName)
	ensureDir(sourcesDir)

	var xw xmlWorkspace
	xw.Version = 1

	bg := w.canvas.GetBackground()
	xw.Background = toRGBA8(bg)

	// TODO: искать по существующим фигурам
	shapes := w.canvas.Shapes()
	xw.Shapes = make([]xmlShape, 0)

	for id, sh := range shapes {
		st := sh.GetStyle()

		if st.BackgroundImagePath != nil {
			src := *st.BackgroundImagePath
			dstName := filepath.Join(sourcesDirName,
				formatImageName(int64(id), safeBaseName(src)),
			)
			dstAbs := filepath.Join(dir, dstName)

			copyFile(src, dstAbs)
			st.BackgroundImagePath = &dstName
		}

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
	c.shapes = make(map[types.ShapeId]Shape)
	c.notifyShapesChanged(oldIds)

	c.background = fromRGBA8(xw.Background)
	shapeIds := make([]types.ShapeId, 0)
	for _, xs := range xw.Shapes {
		style := styleFromXML(xs.Style)
		t := shapeTypeFromString(xs.Type)
		id := xs.ID
		shapeIds = append(shapeIds, types.ShapeId(id))
		sh := NewShape(t, types.ShapeId(id), style)
		c.shapes[types.ShapeId(id)] = sh
		// TODO: прикрутить поддержку изображений
		//if style.BackgroundImagePath != nil {
		//	abs := filepath.Join(dir, *style.BackgroundImagePath)
		//	style.BackgroundImagePath = &abs
		//}
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

func formatImageName(id int64, base string) string {
	return fmt.Sprintf("%d_%s", id, base)
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
