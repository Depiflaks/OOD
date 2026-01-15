package view

type FileActions struct {
	Open   func()
	Save   func()
	SaveAs func()
}
