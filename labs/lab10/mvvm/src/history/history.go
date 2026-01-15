package history

type History interface {
	AppendAndExecute(cmd Command)
	Undo()
	Redo()
	Clear()
}

type commandHistory struct {
	commands []Command
	cursor   int
	limit    int
}

func NewHistory(limit int) History {
	return &commandHistory{
		limit: limit,
	}
}

func (h *commandHistory) AppendAndExecute(cmd Command) {
	if h.cursor < len(h.commands) {
		h.dispose(h.commands[h.cursor:])
		h.commands = h.commands[:h.cursor]
	}

	cmd.Execute()
	h.commands = append(h.commands, cmd)
	h.cursor++

	if h.limit > 0 && len(h.commands) > h.limit {
		h.dispose(h.commands[:1])
		h.commands = h.commands[1:]
		h.cursor--
	}
}

func (h *commandHistory) Undo() {
	if h.cursor == 0 {
		return
	}
	h.cursor--
	h.commands[h.cursor].Unexecute()
}

func (h *commandHistory) Redo() {
	if h.cursor == len(h.commands) {
		return
	}
	h.commands[h.cursor].Execute()
	h.cursor++
}

func (h *commandHistory) Clear() {
	h.commands = h.commands[:0]
}

func (h *commandHistory) dispose(cmds []Command) {
	for _, c := range cmds {
		if d, ok := c.(Disposable); ok {
			d.Dispose()
		}
	}
}
