package history

type Command interface {
	Execute()
	Unexecute()
}

type Disposable interface {
	Dispose()
}

type ExecuteFunc func()
type UnexecuteFunc func()
type DisposeFunc func()

type funcCommand struct {
	onExecute   ExecuteFunc
	onUnexecute UnexecuteFunc
	onDispose   DisposeFunc
}

func NewCommand(
	onExecute ExecuteFunc,
	onUnexecute UnexecuteFunc,
	onDispose DisposeFunc,
) Command {
	return &funcCommand{
		onExecute:   onExecute,
		onUnexecute: onUnexecute,
		onDispose:   onDispose,
	}
}

func (c *funcCommand) Execute() {
	if c.onExecute != nil {
		c.onExecute()
	}
}

func (c *funcCommand) Unexecute() {
	if c.onUnexecute != nil {
		c.onUnexecute()
	}
}

func (c *funcCommand) Dispose() {
	if c.onDispose != nil {
		c.onDispose()
	}
}
