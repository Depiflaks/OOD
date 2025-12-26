package view

import "image/color"

type FileActions struct {
	Open   func()
	Save   func()
	SaveAs func()
}

type ColorActions struct {
	SetFill   func(c color.Color)
	SetStroke func(c color.Color)
}
