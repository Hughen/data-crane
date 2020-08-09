package v1

import (
	"bufio"
	"io"
	"os"
	"path/filepath"
	"sync"

	s3types "data-crane/s3"

	"github.com/spf13/pflag"
)

type objFlag struct {
	index    int
	disposed bool
}

type Task struct {
	mu    sync.Mutex
	index int
	list  []string
	lmap  map[string]objFlag
	// prefetching object list
	cur []s3types.Object
}

type Prefetcher struct {
	// the local cache storage path
	lcsRoot string
	// csi => Task
	tasks *sync.Map
}

func NewPrefetcher() *Prefetcher {
	lcs, err := pflag.CommandLine.GetString("lcs-root")
	if err != nil || len(lcs) == 0 {
		panic("the lcs-root flag is not defined")
	}

	// TODO: check directory perm

	return &Prefetcher{
		lcsRoot: lcs,
		tasks:   &sync.Map{},
	}
}

func (pre *Prefetcher) Start() {
	go pre.prepare()

	// TODO: start prefetch worker
}

func (pre *Prefetcher) Prefetch(csiVolume string, listFile string) error {
	task, err := pre.readListFile(listFile)
	if err != nil {
		return err
	}

	v, ok := pre.tasks.Load(csiVolume)
	if ok {
		// prevent from current Task data
		oldTask := v.(*Task)
		oldTask.mu.Lock()
		defer oldTask.mu.Unlock()
	}

	pre.tasks.Store(csiVolume, task)
	return nil
}

func (pre *Prefetcher) prepare() {
	// TODO: scan all prefetch_list in lcs
	// if existed in tasks, it should not be updated
}

func (pre *Prefetcher) readListFile(listFile string) (*Task, error) {
	file, err := os.Open(filepath.Join(pre.lcsRoot, listFile))
	if err != nil {
		return nil, err
	}
	defer file.Close()

	lines := make([]string, 0)
	lmap := make(map[string]objFlag)
	buf := bufio.NewReader(file)
	idx := 0
	for {
		data, _, c := buf.ReadLine()
		if c == io.EOF {
			break
		}
		val := string(data)
		lines = append(lines, val)
		lmap[val] = objFlag{index: idx, disposed: false}
		idx++
	}

	return &Task{
		index: 0,
		list:  lines,
		lmap:  lmap,
		cur:   make([]s3types.Object, 0, 3),
	}, nil
}
