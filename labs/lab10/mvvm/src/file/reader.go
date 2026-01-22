package file

import (
	"io"
	"os"
)

func Open(path string) io.ReadCloser {
	rc, _ := os.Open(path)
	return rc
}
