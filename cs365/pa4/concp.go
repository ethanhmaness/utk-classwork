package main

import (
	"fmt"
	"io"
	"os"
	"path"
	"sync"
)

// Init waitgroup globally
var wg = sync.WaitGroup{}

// Checks if a filename corresponds to an valid file (not a directory)
func IsFile(name string) bool {
	finfo, err := os.Stat(name)
	if err != nil || finfo.IsDir() {
		return false
	}
	return true
}

// Checks if a filename corresponds to an valid directory (not a file)
func IsDir(name string) bool {
	finfo, err := os.Stat(name)
	if err != nil || !finfo.IsDir() {
		return false
	}
	return true
}

// Checks if a list of filenames all correspond to actual files (not directories)
func AssertValidFiles(names []string) error {
	for i := 0; i < len(names); i++ {
		if name := names[i]; !IsFile(name) {
			return fmt.Errorf("%v is not a valid file.", name)
		}
	}
	return nil
}

// Copies a file to a directory 1KB at a time
func CopyFileToDir(filename string, dirname string) error {

	// Open the source file
	rfile, err := os.Open(filename)
	if err != nil {
		return err
	}
	defer rfile.Close()

	// Create/truncate the destination file
	wfname := dirname + "/" + path.Base(filename)
	wfile, err := os.Create(wfname)
	if err != nil {
		return err
	}
	defer wfile.Close()

	//Create 1KB buffer
	buf := make([]byte, 1024)
	var n int
	// Loop infinitely
	for {

		// Read up to 1KB
		n, err = rfile.Read(buf)
		if err != nil && (err != io.EOF || n != 0) {
			return err
		}

		// Write from buffer
		_, err = wfile.Write(buf[:n])
		if err != nil {
			return err
		}

		// If nothing was left to read, return
		if n == 0 {
			return nil
		}

	}
}

// Calls CopyFileToDir concurrently
// Sends the returned error value over a channel
func CopyFileToDirAsync(filename string, dirname string, errch chan error) {
	defer wg.Done()
	errch <- CopyFileToDir(filename, dirname)
}

// Copies a list of files to a directory concurrently
// Will immediately return if a copy fails, without finishing any remaining copies
func CopyFilesToDir(filenames []string, dirname string) error {

	// Make error channel
	ch := make(chan error, len(filenames))
	defer close(ch)

	// Start file copy goroutines
	for _, filename := range filenames {
		wg.Add(1)
		go CopyFileToDirAsync(filename, dirname, ch)
	}

	// Wait for all goroutines to send a value back through the channel
	for i := 0; i < len(filenames); i++ {
		err := <-ch
		// If an error is returned, immediately return
		if err != nil {
			return err
		}
	}

	// The WaitGroup doesn't actually do anything, I'm not sure why the writeup requires it to be present
	// Channel blocking already prevents execution from ever reaching this point until all goroutines are finished
	// I love channels
	wg.Wait()

	return nil
}

// Handles errors, exiting with a non-zero value if one is found
func main() {
	if err := run(); err != nil {
		fmt.Fprintf(os.Stderr, "error: %v\n", err)
		os.Exit(1)
	}
}

// The actual "main"
func run() error {

	// Get number of args, check valid format
	numArgs := len(os.Args)
	if numArgs < 3 {
		fmt.Println("Usage: concp <file1> [file2] ... <directory>")
		return fmt.Errorf("Invalid arguments.")
	}

	// Init string vars
	filenames := os.Args[1 : numArgs-1]
	dirname := os.Args[numArgs-1]

	// Check all files
	if err := AssertValidFiles(filenames); err != nil {
		return err
	}

	// Check directory
	if !IsDir(dirname) {
		fmt.Println()
		return fmt.Errorf("%v is not a valid directory.", dirname)
	}

	// Copy the files, return the first error found
	return CopyFilesToDir(filenames, dirname)
}
