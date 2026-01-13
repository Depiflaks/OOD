package model

import (
	"encoding/xml"
	"fmt"
	"os"
	"path/filepath"
	"vector-editor/src/geometry"
	"vector-editor/src/types"
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

func (w *workspaceModel) SaveAs(path string) {
	fmt.Printf("SaveAs(%s)\n", path)
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
			Position: xmlPoint{
				X: float32(sh.GetPosition().X),
				Y: float32(sh.GetPosition().Y),
			},
			Size: xmlBounds{
				W: float32(sh.GetBounds().Width),
				H: float32(sh.GetBounds().Height),
			},
			Style: styleToXML(st),
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
	fmt.Println("OPEN?")
	xmlPath := path
	if filepath.Ext(path) != ".xml" {
		xmlPath = filepath.Join(path, workspaceFileName)
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

	c := NewCanvas()

	c.SetBackground(fromRGBA8(xw.Background))

	for _, xs := range xw.Shapes {
		style := styleFromXML(xs.Style)
		t := shapeTypeFromString(xs.Type)
		c.NewShape(t, style)
		if style.BackgroundImagePath != nil {
			abs := filepath.Join(dir, *style.BackgroundImagePath)
			style.BackgroundImagePath = &abs
		}

		id := types.ShapeId(xs.ID)

		sh := NewShape(t, id, style)
		sh.UpdateRect(
			geometry.Point{X: float64(xs.Position.X), Y: float64(xs.Position.Y)},
			geometry.Bounds{Width: float64(xs.Size.W), Height: float64(xs.Size.H)},
		)
		//cc.shapes[id] = sh
	}

	//cc.nextId = ShapeId(xw.NextID)

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
