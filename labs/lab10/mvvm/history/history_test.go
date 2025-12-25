package history

import (
	"os"
	"path/filepath"
	"testing"
)

type mockCommand struct {
	execCount   int
	unexecCount int
}

func (m *mockCommand) Execute() {
	m.execCount++
}

func (m *mockCommand) Unexecute() {
	m.unexecCount++
}

type mockDisposableCommand struct {
	path     string
	disposed bool
}

func (m *mockDisposableCommand) Execute() {
	os.WriteFile(m.path, []byte("data"), 0644)
}

func (m *mockDisposableCommand) Unexecute() {
	os.Remove(m.path)
}

func (m *mockDisposableCommand) Dispose() {
	os.Remove(m.path)
	m.disposed = true
}

func TestAppendThreeCommands(t *testing.T) {
	h := New(0)
	c1 := &mockCommand{}
	c2 := &mockCommand{}
	c3 := &mockCommand{}

	h.AppendAndExecute(c1)
	h.AppendAndExecute(c2)
	h.AppendAndExecute(c3)

	if c1.execCount != 1 || c2.execCount != 1 || c3.execCount != 1 {
		t.Fail()
	}
}

func TestAppendThreeUndoOne(t *testing.T) {
	h := New(0)
	c1 := &mockCommand{}
	c2 := &mockCommand{}
	c3 := &mockCommand{}

	h.AppendAndExecute(c1)
	h.AppendAndExecute(c2)
	h.AppendAndExecute(c3)
	h.Undo()

	if c3.unexecCount != 1 {
		t.Fail()
	}
}

func TestUndoAllThenRedo(t *testing.T) {
	h := New(0)
	c1 := &mockCommand{}
	c2 := &mockCommand{}
	c3 := &mockCommand{}

	h.AppendAndExecute(c1)
	h.AppendAndExecute(c2)
	h.AppendAndExecute(c3)

	h.Undo()
	h.Undo()
	h.Undo()

	h.Redo()

	if c1.execCount != 2 {
		t.Fail()
	}
}

func TestUndoMoreThanExistsThenRedo(t *testing.T) {
	h := New(0)
	c1 := &mockCommand{}
	c2 := &mockCommand{}
	c3 := &mockCommand{}

	h.AppendAndExecute(c1)
	h.AppendAndExecute(c2)
	h.AppendAndExecute(c3)

	h.Undo()
	h.Undo()
	h.Undo()
	h.Undo()

	h.Redo()

	if c1.execCount != 2 {
		t.Fail()
	}
}

func TestRedoWithoutUndo(t *testing.T) {
	h := New(0)
	c1 := &mockCommand{}
	c2 := &mockCommand{}
	c3 := &mockCommand{}

	h.AppendAndExecute(c1)
	h.AppendAndExecute(c2)
	h.AppendAndExecute(c3)

	h.Redo()

	if c3.execCount != 1 {
		t.Fail()
	}
}

func TestLimitOverflow(t *testing.T) {
	h := New(5)
	var cmds []*mockCommand

	for i := 0; i < 6; i++ {
		c := &mockCommand{}
		cmds = append(cmds, c)
		h.AppendAndExecute(c)
	}

	for i := 0; i < 6; i++ {
		h.Undo()
	}

	if cmds[0].unexecCount != 0 {
		t.Fail()
	}
}

func TestUndoThenNewCommandThenUndoRedo(t *testing.T) {
	h := New(0)
	c1 := &mockCommand{}
	c2 := &mockCommand{}
	c3 := &mockCommand{}
	c4 := &mockCommand{}

	h.AppendAndExecute(c1)
	h.AppendAndExecute(c2)
	h.AppendAndExecute(c3)

	h.Undo()
	h.AppendAndExecute(c4)
	h.Undo()
	h.Redo()

	if c4.execCount != 2 {
		t.Fail()
	}
}

func TestDisposableCommand(t *testing.T) {
	dir := t.TempDir()
	path := filepath.Join(dir, "file.txt")

	h := New(0)
	c := &mockDisposableCommand{path: path}

	h.AppendAndExecute(c)
	h.Undo()
	h.AppendAndExecute(&mockCommand{})

	if _, err := os.Stat(path); err == nil {
		t.Fail()
	}
}

func TestBranchingHistory(t *testing.T) {
	h := New(0)
	c1 := &mockCommand{}
	c2 := &mockCommand{}
	c3 := &mockCommand{}
	c4 := &mockCommand{}

	h.AppendAndExecute(c1)
	h.AppendAndExecute(c2)
	h.AppendAndExecute(c3)

	h.Undo()
	h.Undo()

	h.AppendAndExecute(c4)

	if c3.unexecCount != 1 || c4.execCount != 1 {
		t.Fail()
	}
}
