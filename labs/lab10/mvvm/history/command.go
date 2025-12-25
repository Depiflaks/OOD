package history

type Command interface {
	Execute()
	Undo()
	Redo()
}

type Disposable interface {
	Dispose()
}
