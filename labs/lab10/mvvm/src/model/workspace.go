package model

import (
	"encoding/xml"
	"fmt"
	"os"
	"path/filepath"
	"vector-editor/src/geometry"
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
		panic("Save(): sourcePath is empty; call SaveAs(path) first")
	}
	w.SaveAs(w.sourcePath)
}

func (w *workspaceModel) SaveAs(path string) {
	dir := path
	if filepath.Ext(path) != ".xml" {
		dir = path
		path = filepath.Join(path, workspaceFileName)
	} else {
		dir = filepath.Dir(path)
	}

	ensureDir(dir)

	sourcesDir := filepath.Join(dir, sourcesDirName)
	ensureDir(sourcesDir)

	var xw xmlWorkspace
	xw.Version = 1

	bg := w.canvas.GetBackground()
	xw.Background = toRGBA8(bg)

	shapes := w.canvas.Shapes()
	xw.Shapes = make([]xmlShape, 0, len(shapes))

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
			Pos: xmlPoint{
				X: float32(sh.GetPosition().X),
				Y: float32(sh.GetPosition().Y),
			},
			Size: xmlBounds{
				W: float32(sh.GetBounds().Width),
				H: float32(sh.GetBounds().Height),
			},
			Sty: styleToXML(st),
		}
		xw.Shapes = append(xw.Shapes, xs)
	}

	xw.NextID = inferNextID(w.canvas)

	data, err := xml.MarshalIndent(xw, "", "  ")
	if err != nil {
		panic(err)
	}
	data = append([]byte(xml.Header), data...)

	if err := os.WriteFile(path, data, 0o644); err != nil {
		panic(err)
	}

	w.sourcePath = path
}

func (w *workspaceModel) Open(path string) {
	xmlPath := path
	if filepath.Ext(path) != ".xml" {
		xmlPath = filepath.Join(path, workspaceFileName)
	}
	dir := filepath.Dir(xmlPath)

	raw, err := os.ReadFile(xmlPath)
	if err != nil {
		panic(err)
	}

	var xw xmlWorkspace
	if err := xml.Unmarshal(raw, &xw); err != nil {
		panic(err)
	}

	c := NewCanvas()

	c.SetBackground(fromRGBA8(xw.Background))

	cc, ok := c.(*canvas)
	if !ok {
		for _, xs := range xw.Shapes {
			st := styleFromXML(xs.Sty)
			if st.BackgroundImagePath != nil {
				abs := filepath.Join(dir, *st.BackgroundImagePath)
				st.BackgroundImagePath = &abs
			}
			newID := c.NewShape(shapeTypeFromString(xs.Type), st)
			sh := c.GetShape(newID)
			sh.UpdateRect(
				geometry.Point{X: float64(xs.Pos.X), Y: float64(xs.Pos.Y)},
				geometry.Bounds{Width: float64(xs.Size.W), Height: float64(xs.Size.H)},
			)
		}
		w.canvas = c
		w.sourcePath = xmlPath
		return
	}

	cc.shapes = make(map[ShapeId]Shape, len(xw.Shapes))
	for _, xs := range xw.Shapes {
		st := styleFromXML(xs.Sty)
		if st.BackgroundImagePath != nil {
			abs := filepath.Join(dir, *st.BackgroundImagePath)
			st.BackgroundImagePath = &abs
		}

		id := ShapeId(xs.ID)
		t := shapeTypeFromString(xs.Type)

		sh := NewShape(t, id, st)
		sh.UpdateRect(
			geometry.Point{X: float64(xs.Pos.X), Y: float64(xs.Pos.Y)},
			geometry.Bounds{Width: float64(xs.Size.W), Height: float64(xs.Size.H)},
		)
		cc.shapes[id] = sh
	}

	cc.nextId = ShapeId(xw.NextID)

	w.canvas = c
	w.sourcePath = xmlPath
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
