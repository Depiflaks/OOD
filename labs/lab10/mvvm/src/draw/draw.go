package draw

import "image/color"

type Style struct {
	Fill                *color.Color
	Stroke              *color.Color
	BackgroundImagePath *string
}
