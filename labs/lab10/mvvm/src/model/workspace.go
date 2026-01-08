package model

import (
	"encoding/xml"
	"image/color"
	"os"
	"vector-editor/src/geometry"
)

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

func (w workspaceModel) Canvas() Canvas {
	return w.canvas
}

func (w *workspaceModel) Save() {
	w.SaveAs(w.sourcePath)
}

func (w *workspaceModel) SaveAs(path string) {
	type rgbaXML struct {
		R uint8 `xml:"r,attr"`
		G uint8 `xml:"g,attr"`
		B uint8 `xml:"b,attr"`
		A uint8 `xml:"a,attr"`
	}
	type pointXML struct {
		X float64 `xml:"x,attr"`
		Y float64 `xml:"y,attr"`
	}
	type boundsXML struct {
		W float64 `xml:"w,attr"`
		H float64 `xml:"h,attr"`
	}
	type styleXML struct {
		Fill   *rgbaXML `xml:"fill,omitempty"`
		Stroke *rgbaXML `xml:"stroke,omitempty"`
	}
	type shapeXML struct {
		ID       int64     `xml:"id,attr"`
		Type     int       `xml:"type,attr"`
		Position pointXML  `xml:"position"`
		Bounds   boundsXML `xml:"bounds"`
		Style    styleXML  `xml:"style"`
	}
	type canvasXML struct {
		Background rgbaXML    `xml:"background"`
		Shapes     []shapeXML `xml:"shapes>shape"`
	}
	type workspaceXML struct {
		XMLName xml.Name  `xml:"workspace"`
		Canvas  canvasXML `xml:"canvas"`
	}

	toRGBA := func(c color.Color) rgbaXML {
		r, g, b, a := c.RGBA()
		return rgbaXML{R: uint8(r >> 8), G: uint8(g >> 8), B: uint8(b >> 8), A: uint8(a >> 8)}
	}
	styleToXML := func(st geometry.Style) styleXML {
		var out styleXML
		if st.Fill != nil && *st.Fill != nil {
			rgba := toRGBA(*st.Fill)
			out.Fill = &rgba
		}
		if st.Stroke != nil && *st.Stroke != nil {
			rgba := toRGBA(*st.Stroke)
			out.Stroke = &rgba
		}
		return out
	}

	// background
	bg := rgbaXML{R: 255, G: 255, B: 255, A: 255}
	if cc, ok := w.canvas.(*canvas); ok && cc.background != nil {
		bg = toRGBA(cc.background)
	}

	// shapes
	shapesMap := w.canvas.Shapes()
	shapes := make([]shapeXML, 0, len(shapesMap))
	for id, s := range shapesMap {
		pos := s.GetPosition()
		b := s.GetBounds()
		shapes = append(shapes, shapeXML{
			ID:       int64(id),
			Type:     int(s.GetShapeType()),
			Position: pointXML{X: pos.X, Y: pos.Y},
			Bounds:   boundsXML{W: b.Width, H: b.Height},
			Style:    styleToXML(s.GetStyle()),
		})
	}

	doc := workspaceXML{
		Canvas: canvasXML{
			Background: bg,
			Shapes:     shapes,
		},
	}

	data, err := xml.MarshalIndent(doc, "", "  ")
	if err != nil {
		panic(err)
	}
	data = append([]byte(xml.Header), data...)

	if err := os.WriteFile(path, data, 0o644); err != nil {
		panic(err)
	}
}

func (w *workspaceModel) Open(path string) {
	type rgbaXML struct {
		R uint8 `xml:"r,attr"`
		G uint8 `xml:"g,attr"`
		B uint8 `xml:"b,attr"`
		A uint8 `xml:"a,attr"`
	}
	type pointXML struct {
		X float64 `xml:"x,attr"`
		Y float64 `xml:"y,attr"`
	}
	type boundsXML struct {
		W float64 `xml:"w,attr"`
		H float64 `xml:"h,attr"`
	}
	type styleXML struct {
		Fill   *rgbaXML `xml:"fill,omitempty"`
		Stroke *rgbaXML `xml:"stroke,omitempty"`
	}
	type shapeXML struct {
		ID       int64     `xml:"id,attr"`
		Type     int       `xml:"type,attr"`
		Position pointXML  `xml:"position"`
		Bounds   boundsXML `xml:"bounds"`
		Style    styleXML  `xml:"style"`
	}
	type canvasXML struct {
		Background rgbaXML    `xml:"background"`
		Shapes     []shapeXML `xml:"shapes>shape"`
	}
	type workspaceXML struct {
		XMLName xml.Name  `xml:"workspace"`
		Canvas  canvasXML `xml:"canvas"`
	}

	data, err := os.ReadFile(path)
	if err != nil {
		panic(err)
	}

	var doc workspaceXML
	if err := xml.Unmarshal(data, &doc); err != nil {
		panic(err)
	}

	// preserve observers, if current canvas is the concrete type
	var observers []CanvasObserver
	if old, ok := w.canvas.(*canvas); ok {
		observers = old.observers
	}

	fromRGBA := func(x rgbaXML) color.Color {
		return color.RGBA{R: x.R, G: x.G, B: x.B, A: x.A}
	}
	styleFromXML := func(x styleXML) geometry.Style {
		var st geometry.Style
		if x.Fill != nil {
			var c color.Color = fromRGBA(*x.Fill)
			st.Fill = &c
		}
		if x.Stroke != nil {
			var c color.Color = fromRGBA(*x.Stroke)
			st.Stroke = &c
		}
		return st
	}

	newShapes := make(map[ShapeId]Shape, len(doc.Canvas.Shapes))
	var maxID ShapeId = -1
	changed := make([]ShapeId, 0, len(doc.Canvas.Shapes))

	for _, sx := range doc.Canvas.Shapes {
		id := ShapeId(sx.ID)
		if id > maxID {
			maxID = id
		}
		st := styleFromXML(sx.Style)
		newShapes[id] = &shape{
			id:        id,
			shapeType: ShapeType(sx.Type),
			position:  geometry.Point{X: sx.Position.X, Y: sx.Position.Y},
			size:      geometry.Bounds{Width: sx.Bounds.W, Height: sx.Bounds.H},
			style:     st,
		}
		changed = append(changed, id)
	}

	c := &canvas{
		background: fromRGBA(doc.Canvas.Background),
		shapes:     newShapes,
		observers:  observers,
		nextId:     maxID + 1,
	}
	w.canvas = c

	// notify about loaded shapes
	if len(changed) > 0 {
		c.notify(changed)
	}
}
