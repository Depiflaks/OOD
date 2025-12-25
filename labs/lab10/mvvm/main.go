package main

import (
	"image/color"
	"math"
	"vector-editor/history"

	"fyne.io/fyne/v2"
	"fyne.io/fyne/v2/app"
	"fyne.io/fyne/v2/canvas"
	"fyne.io/fyne/v2/container"
	"fyne.io/fyne/v2/widget"
)

type ShapeType int

const (
	Rect ShapeType = iota
	Circle
)

type Shape struct {
	kind   ShapeType
	pos    fyne.Position
	size   fyne.Size
	object fyne.CanvasObject
}

func (s *Shape) Contains(p fyne.Position) bool {
	if s.kind == Rect {
		return p.X >= s.pos.X &&
			p.Y >= s.pos.Y &&
			p.X <= s.pos.X+s.size.Width &&
			p.Y <= s.pos.Y+s.size.Height
	}

	if s.kind == Circle {
		cx := s.pos.X + s.size.Width/2
		cy := s.pos.Y + s.size.Height/2
		r := s.size.Width / 2
		dx := p.X - cx
		dy := p.Y - cy
		return math.Sqrt(float64(dx*dx+dy*dy)) <= float64(r)
	}
	return false
}

// MouseLayer теперь полноценный виджет
type MouseLayer struct {
	widget.BaseWidget
	shapes []*Shape
}

func NewMouseLayer(shapes []*Shape) *MouseLayer {
	m := &MouseLayer{shapes: shapes}
	m.ExtendBaseWidget(m) // Важно для работы событий в кастомных виджетах
	return m
}

func (m *MouseLayer) Tapped(e *fyne.PointEvent) {
	for _, s := range m.shapes {
		if s.Contains(e.Position) {
			switch obj := s.object.(type) {
			case *canvas.Rectangle:
				obj.FillColor = color.RGBA{50, 200, 50, 255}
				obj.Refresh()
			case *canvas.Circle:
				obj.FillColor = color.RGBA{200, 200, 50, 255}
				obj.Refresh()
			}
		}
	}
}

func (m *MouseLayer) TappedSecondary(*fyne.PointEvent) {}

func (m *MouseLayer) CreateRenderer() fyne.WidgetRenderer {
	return widget.NewSimpleRenderer(container.NewStack())
}

func main() {
	h := history.New(0)

	(*h).Redo()
	a := app.New()
	w := a.NewWindow("Fyne Shapes")

	var shapes []*Shape

	// Создаем прямоугольник
	rect := canvas.NewRectangle(color.RGBA{200, 50, 50, 255})
	rect.Move(fyne.NewPos(50, 50))
	rect.Resize(fyne.NewSize(120, 80))

	shapes = append(shapes, &Shape{
		kind:   Rect,
		pos:    rect.Position(),
		size:   rect.Size(),
		object: rect,
	})

	circle := canvas.NewCircle(color.RGBA{50, 50, 200, 255})
	circle.Move(fyne.NewPos(220, 80))
	circle.Resize(fyne.NewSize(100, 100))

	shapes = append(shapes, &Shape{
		kind:   Circle,
		pos:    circle.Position(),
		size:   circle.Size(),
		object: circle,
	})

	// Слой с графикой
	graphicsLayer := container.NewWithoutLayout(rect, circle)

	// Слой для обработки кликов (прозрачный виджет поверх всего)
	clickLayer := NewMouseLayer(shapes)

	// Используем Max, чтобы оба слоя занимали все окно
	root := container.NewMax(
		graphicsLayer,
		clickLayer,
	)

	w.SetContent(root)
	w.Resize(fyne.NewSize(400, 300))
	w.ShowAndRun()
}
