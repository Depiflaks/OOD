package model

import (
	"encoding/base64"
	"encoding/xml"
	"image/color"
	"os"
	"path/filepath"
	"strings"
	"testing"
	"vector-editor/src/types"
	"vector-editor/src/types/draw"
	"vector-editor/src/types/geometry"
)

func createDummyImage(t *testing.T, dir string, content string) string {
	path := filepath.Join(dir, "test_image.png")
	err := os.WriteFile(path, []byte(content), 0644)
	if err != nil {
		t.Fatalf("Failed to create dummy image: %v", err)
	}
	return path
}

func TestWorkspace_SaveAs(t *testing.T) {
	tmpDir := t.TempDir()
	savePath := filepath.Join(tmpDir, "saved_workspace.xml")

	ws := NewWorkspaceModel()
	canvas := ws.Canvas()

	bgColor := color.RGBA{R: 255, G: 0, B: 0, A: 255}
	canvas.SetBackground(bgColor)

	style := draw.Style{
		Fill:   &color.RGBA{R: 0, G: 255, B: 0, A: 255},
		Stroke: &color.RGBA{R: 0, G: 0, B: 255, A: 255},
	}
	shapeId := canvas.NewShape(types.Rect, style)
	shape := canvas.GetShape(shapeId)

	pos := geometry.Point{X: 10, Y: 20}
	bounds := geometry.Bounds{Width: 100, Height: 50}
	shape.UpdateRect(pos, bounds)

	ws.SaveAs(savePath, []types.ShapeId{shapeId})

	if _, err := os.Stat(savePath); os.IsNotExist(err) {
		t.Fatalf("File was not created at %s", savePath)
	}

	data, err := os.ReadFile(savePath)
	if err != nil {
		t.Fatalf("Failed to read saved file: %v", err)
	}

	var xw xmlWorkspace
	if err := xml.Unmarshal(data, &xw); err != nil {
		t.Fatalf("Failed to unmarshal saved XML: %v", err)
	}

	if xw.Background.R != 255 || xw.Background.G != 0 {
		t.Errorf("Expected background R=255, got %d", xw.Background.R)
	}

	if len(xw.Shapes) != 1 {
		t.Fatalf("Expected 1 shape, got %d", len(xw.Shapes))
	}

	xs := xw.Shapes[0]
	if xs.ID != int64(shapeId) {
		t.Errorf("Expected ID %d, got %d", shapeId, xs.ID)
	}
	if xs.Type != "rect" {
		t.Errorf("Expected type 'rect', got '%s'", xs.Type)
	}
	if xs.Position.X != 10 || xs.Position.Y != 20 {
		t.Errorf("Wrong position: got (%d, %d)", xs.Position.X, xs.Position.Y)
	}
	if xs.Size.W != 100 || xs.Size.H != 50 {
		t.Errorf("Wrong bounds: got %dx%d", xs.Size.W, xs.Size.H)
	}

	if xs.Style.Fill == nil || xs.Style.Fill.G != 255 {
		t.Error("Fill color mismatch in XML")
	}
}

func TestWorkspace_Open(t *testing.T) {
	xmlContent := `
<workspace version="1">
  <background r="10" g="10" b="10" a="255"/>
  <shapes>
    <shape id="42" type="ellipse">
      <position x="50" y="50"/>
      <bounds w="30" h="30"/>
      <style>
        <fill r="100" g="100" b="100" a="255"/>
      </style>
    </shape>
  </shapes>
  <nextId>100</nextId>
</workspace>`

	tmpDir := t.TempDir()
	filePath := filepath.Join(tmpDir, "test_open.xml")
	if err := os.WriteFile(filePath, []byte(xmlContent), 0644); err != nil {
		t.Fatal(err)
	}

	ws := NewWorkspaceModel()
	ws.Open(filePath)
	defer ws.Close()

	canvas := ws.Canvas()

	bg := canvas.GetBackground()
	r, _, _, _ := bg.RGBA()
	if uint8(r>>8) != 10 {
		t.Errorf("Background color not loaded correctly")
	}

	shapes := canvas.Shapes()
	if len(shapes) != 1 {
		t.Fatalf("Expected 1 shape, got %d", len(shapes))
	}

	var loadedShape Shape
	for _, s := range shapes {
		loadedShape = s
		break
	}

	if loadedShape.GetShapeId() != 42 {
		t.Errorf("Expected ID 42, got %d", loadedShape.GetShapeId())
	}
	if loadedShape.GetShapeType() != types.Ellipse {
		t.Errorf("Expected type Ellipse, got %v", loadedShape.GetShapeType())
	}

	bounds := loadedShape.GetBounds()
	if bounds.Width != 30 || bounds.Height != 30 {
		t.Errorf("Bounds mismatch: got %v", bounds)
	}
}

func TestWorkspace_ImageRoundTrip(t *testing.T) {

	tmpDir := t.TempDir()
	srcImgPath := createDummyImage(t, tmpDir, "im-data")
	savePath := filepath.Join(tmpDir, "image_ws.xml")

	ws1 := NewWorkspaceModel()
	c1 := ws1.Canvas()

	style := draw.Style{
		BackgroundImagePath: &srcImgPath,
	}
	id := c1.NewShape(types.Rect, style)

	ws1.SaveAs(savePath, []types.ShapeId{id})
	ws1.Close()

	xmlBytes, _ := os.ReadFile(savePath)
	xmlStr := string(xmlBytes)
	encodedData := base64.StdEncoding.EncodeToString([]byte("im-data"))

	if !contains(xmlStr, encodedData) {
		t.Errorf("XML does not contain base64 encoded image data")
	}

	ws2 := NewWorkspaceModel()
	ws2.Open(savePath)
	defer ws2.Close()

	c2 := ws2.Canvas()

	loadedShape := c2.GetShape(id)
	if loadedShape == nil {
		t.Fatalf("Shape %d not found after loading", id)
	}

	loadedStyle := loadedShape.GetStyle()
	if loadedStyle.BackgroundImagePath == nil {
		t.Fatal("BackgroundImagePath is nil after loading")
	}

	newPath := *loadedStyle.BackgroundImagePath

	if _, err := os.Stat(newPath); os.IsNotExist(err) {
		t.Errorf("Restored image file does not exist at: %s", newPath)
	}

	content, _ := os.ReadFile(newPath)
	if string(content) != "im-data" {
		t.Errorf("Restored image content mismatch. Want 'im-data', got '%s'", string(content))
	}
}

func contains(s, substr string) bool {
	return len(s) >= len(substr) && (s == substr || len(substr) == 0 || (len(s) > 0 && search(s, substr)))
}

func search(s, substr string) bool {
	return strings.Contains(s, substr)
}
