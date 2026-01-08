package model

import (
	"fmt"
	"io"
	"os"
	"path/filepath"
	"time"
)

type FileStorage interface {
	store(localPath string) string
	delete(path string)
}

type storage struct {
}

func NewStorage() FileStorage {
	return &storage{}
}

func (s *storage) store(localPath string) string {
	tmpDir := ".tmp"
	if err := os.MkdirAll(tmpDir, 0o755); err != nil {
		panic(err)
	}

	src, err := os.Open(localPath)
	if err != nil {
		panic(err)
	}
	defer src.Close()

	base := filepath.Base(localPath)
	dstPath := filepath.Join(tmpDir, fmt.Sprintf("%d_%s", time.Now().UnixNano(), base))

	dst, err := os.Create(dstPath)
	if err != nil {
		panic(err)
	}
	defer dst.Close()

	if _, err := io.Copy(dst, src); err != nil {
		panic(err)
	}

	return dstPath
}

func (s *storage) delete(path string) {
	_ = os.Remove(path)
}
