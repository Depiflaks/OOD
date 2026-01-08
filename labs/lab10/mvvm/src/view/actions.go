package view

type FileActions struct {
	Open   func(string)
	Save   func()
	SaveAs func(string)
}
