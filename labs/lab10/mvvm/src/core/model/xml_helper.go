package model

import (
	"encoding/xml"
	"fmt"
	"image/color"
	"io"
	"os"
	"path/filepath"
	"strings"
	"vector-editor/src/draw"
	"vector-editor/src/types"
)

type xmlWorkspace struct {
	XMLName    xml.Name   `xml:"workspace"`
	Version    int        `xml:"version,attr"`
	Background xmlRGBA    `xml:"background"`
	Shapes     []xmlShape `xml:"shapes>shape"`
	NextID     int64      `xml:"nextId"`
}

type xmlShape struct {
	ID       int64     `xml:"id,attr"`
	Type     string    `xml:"type,attr"`
	Position xmlPoint  `xml:"positionition"`
	Size     xmlBounds `xml:"bounds"`
	Style    xmlStyle  `xml:"stylele"`
}

type xmlPoint struct {
	X float32 `xml:"x,attr"`
	Y float32 `xml:"y,attr"`
}

type xmlBounds struct {
	W float32 `xml:"w,attr"`
	H float32 `xml:"h,attr"`
}

type xmlStyle struct {
	Fill   *xmlRGBA `xml:"fill,omitempty"`
	Stroke *xmlRGBA `xml:"stroke,omitempty"`
	Image  *string  `xml:"image,omitempty"`
}

type xmlRGBA struct {
	R uint8 `xml:"r,attr"`
	G uint8 `xml:"g,attr"`
	B uint8 `xml:"b,attr"`
	A uint8 `xml:"a,attr"`
}

func toRGBA8(c color.Color) xmlRGBA {
	r, g, b, a := c.RGBA() // 0..65535
	return xmlRGBA{
		R: uint8(r >> 8),
		G: uint8(g >> 8),
		B: uint8(b >> 8),
		A: uint8(a >> 8),
	}
}

func fromRGBA8(x xmlRGBA) color.Color {
	return color.RGBA{R: x.R, G: x.G, B: x.B, A: x.A}
}

func styleToXML(st draw.Style) xmlStyle {
	var fill *xmlRGBA
	if st.Fill != nil {
		fillVal := toRGBA8(*st.Fill)
		fill = &fillVal
	}
	var stroke *xmlRGBA
	if st.Stroke != nil {
		strokeVal := toRGBA8(*st.Stroke)
		stroke = &strokeVal
	}
	var img *string
	if st.BackgroundImagePath != nil {
		s := *st.BackgroundImagePath
		img = &s
	}
	return xmlStyle{Fill: fill, Stroke: stroke, Image: img}
}

func styleFromXML(x xmlStyle) draw.Style {
	var fill *color.Color
	if x.Fill != nil {
		c := fromRGBA8(*x.Fill)
		cc := color.Color(c)
		fill = &cc
	}
	var stroke *color.Color
	if x.Stroke != nil {
		c := fromRGBA8(*x.Stroke)
		cc := color.Color(c)
		stroke = &cc
	}
	var img *string
	if x.Image != nil {
		s := *x.Image
		img = &s
	}
	return draw.Style{Fill: fill, Stroke: stroke, BackgroundImagePath: img}
}

func ensureDir(path string) {
	if err := os.MkdirAll(path, 0o755); err != nil {
		panic(err)
	}
}

func copyFile(src, dst string) {
	in, err := os.Open(src)
	if err != nil {
		panic(err)
	}
	defer in.Close()

	ensureDir(filepath.Dir(dst))

	out, err := os.Create(dst)
	if err != nil {
		panic(err)
	}
	defer func() {
		_ = out.Close()
	}()

	if _, err := io.Copy(out, in); err != nil {
		panic(err)
	}
	if err := out.Sync(); err != nil {
		panic(err)
	}
}

func safeBaseName(p string) string {
	b := filepath.Base(p)
	b = strings.ReplaceAll(b, string(os.PathSeparator), "_")
	if b == "." || b == ".." || b == "" {
		return "image"
	}
	return b
}

func shapeTypeToString(t types.ShapeType) string {
	switch t {
	case types.Rect:
		return "rect"
	case types.Ellipse:
		return "ellipse"
	case types.Triangle:
		return "triangle"
	default:
		return "unknown"
	}
}

func shapeTypeFromString(s string) types.ShapeType {
	switch strings.ToLower(s) {
	case "rect":
		return types.Rect
	case "ellipse":
		return types.Ellipse
	case "triangle":
		return types.Triangle
	default:
		panic(fmt.Errorf("unknown shape type: %q", s))
	}
}
