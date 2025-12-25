package history

type Command interface {
	Execute()
	Unexecute()
}

type Disposable interface {
	Dispose()
}
