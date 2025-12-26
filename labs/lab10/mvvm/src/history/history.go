package history

type History struct {
	commands []Command
	cursor   int
	limit    int
}

func NewHistory(limit int) *History {
	return &History{
		limit: limit,
	}
}

func (h *History) AppendAndExecute(cmd Command) {
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

func (h *History) Undo() {
	if h.cursor == 0 {
		return
	}
	h.cursor--
	h.commands[h.cursor].Unexecute()
}

func (h *History) Redo() {
	if h.cursor == len(h.commands) {
		return
	}
	h.commands[h.cursor].Execute()
	h.cursor++
}

func (h *History) dispose(cmds []Command) {
	for _, c := range cmds {
		if d, ok := c.(Disposable); ok {
			d.Dispose()
		}
	}
}
