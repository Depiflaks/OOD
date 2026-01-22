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
	close()
}

type storage struct {
}

func NewStorage() FileStorage {
	return &storage{}
}

func (s *storage) close() {
	tmpDir := ".tmp"
	err := os.RemoveAll(tmpDir)
	if err != nil {
		fmt.Println("Failed to cleanup temp dir:", err)
	}
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

	dstPath := filepath.Join(tmpDir, fmt.Sprintf("%d", time.Now().UnixNano()))

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
