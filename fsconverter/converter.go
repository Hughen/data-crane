package main

import (
	"context"
	"math/rand"
	"path/filepath"
	"strings"
	"sync"
	"sync/atomic"
	"syscall"
	"time"

	"go.mongodb.org/mongo-driver/bson/primitive"
)

type CvtType string

const (
	CvtRawData   CvtType = CollectionRawData
	CvtDataBatch CvtType = CollectionDataBatch
)

type Converter struct {
	CType  CvtType
	JobNum int

	mu        sync.Mutex
	index     int
	jobs      []Job
	startTime time.Time
	wg        sync.WaitGroup

	success int32

	db *DB
}

func NewConverter(mongoURI string, cType CvtType, convertNum int) *Converter {
	return &Converter{
		CType:     cType,
		JobNum:    convertNum,
		index:     0,
		startTime: time.Now(),
		success:   0,

		db: NewDB(mongoURI),
	}
}

func (c *Converter) Start() {
	c.makeJobs()

	for workerID := 0; workerID < c.JobNum; workerID++ {
		c.wg.Add(1)
		c.startWorker(workerID)
	}

	c.wg.Wait()
	Logger().Info("all finished", "success", c.success)
}

func (c *Converter) makeJobs() {
	ctx, cancel := context.WithTimeout(context.Background(), 1*time.Minute)
	defer cancel()
	data, err := c.db.GetXData(ctx, c.CType)
	if err != nil {
		panic(err)
	}
	if err := c.db.CalculateTasks(ctx, c.CType, data); err != nil {
		panic(err)
	}
	jobs, err := c.db.GetConverterJobs(ctx, c.CType)
	if err != nil {
		panic(err)
	}

	c.index = 0
	c.jobs = jobs
}

func (c *Converter) startWorker(workerID int) {
	defer c.wg.Done()
	for {
		curJob := c.nextJob()
		if curJob == nil {
			Logger().Infow("not any job can be converted", "workerID", workerID)
			return
		}
		var alreadyInsertDir map[string]bool
		jobID := curJob.ID
		cursor := curJob.Cursor
		var files []File
		var count int
		summaryCount := 0
		for {
			files, count, cursor = c.raw2fs(workerID, alreadyInsertDir, jobID, cursor)
			if len(files) == 0 {
				Logger().Infow("finished convert", "workerID", workerID, "jobID", jobID.Hex())
				break
			}
			for {
				err := c.SaveFiles(files)
				if err == nil {
					break
				}
				Logger().Warnw("failed to save fs", "workerID", workerID, "fileCount", len(files), "dataset", curJob.ID.Hex(), "cursor", cursor.Hex(), "error", err)
			}

			for {
				err := c.updateJobStatus(jobID, cursor, count)
				if err == nil {
					break
				}
				Logger().Warnw("failed to update job status", "workerID", workerID, "jobID", jobID.Hex(), "cursor", cursor.Hex(), "count", count, "error", err)
			}
			summaryCount += count
			Logger().Infow("convert summary", "workerID", workerID, "jobID", jobID.Hex(), "accumulativeCount", summaryCount)
		}
		atomic.AddInt32(&c.success, int32(summaryCount))
	}
}

func (c *Converter) nextJob() *Job {
	c.mu.Lock()
	defer c.mu.Unlock()

	if len(c.jobs) <= (c.index + 1) {
		return nil
	}
	c.index++
	return &c.jobs[c.index]
}

func (c *Converter) scrapeRecords(jobID, cursor primitive.ObjectID) ([]Record, error) {
	ctx, cancel := context.WithTimeout(context.Background(), 1*time.Minute)
	defer cancel()
	return c.db.NextRecords(ctx, c.CType, jobID, cursor)
}

func (c *Converter) raw2fs(workerID int, insertedDir map[string]bool, jobID, cursor primitive.ObjectID) ([]File, int, primitive.ObjectID) {
start:
	records, err := c.scrapeRecords(jobID, cursor)
	if err != nil {
		sleepDuration := time.Duration(rand.Intn(5)+1) * time.Second
		Logger().Errorw("unable to scrape files", "workerID", workerID, "collection", c.CType, "sleepDuration", sleepDuration, "error", err)
		time.Sleep(sleepDuration)
		goto start
	}
	var files []File
	total := len(records)
	if total == 0 {
		return files, 0, primitive.NilObjectID
	}
	for _, item := range records {
		arr := strings.Split(strings.TrimPrefix(item.Name, "/"), "/")
		lastIndex := len(arr) - 1
		if lastIndex < 0 {
			lastIndex = 0
		}
		for i := 0; i < lastIndex; i++ {
			path := filepath.Join("/", strings.Join(arr[0:i+1], "/"))
			if _, ok := insertedDir[path]; ok {
				continue
			}
			insertedDir[path] = true
			files = append(files, File{
				Dataset: item.BatchID,
				Parent:  filepath.Join(path, ".."),
				Name:    filepath.Base(path),
				Type:    syscall.S_IFDIR,
			})
		}
		filePath := filepath.Join("/", item.Name)
		files = append(files, File{
			Dataset: item.BatchID,
			Parent:  filepath.Join(filePath, ".."),
			Name:    filepath.Base(filePath),
			Type:    syscall.S_IFREG,
			Meta: Meta{
				Name:          filePath,
				Digest:        item.Digest,
				ContentLength: item.ContentLength,
				ETag:          item.ETag,
				ContentType:   item.ContentType,
				URL:           item.URL,
				CTime:         item.CTime,
				Creator:       item.Creator,
				Completed:     item.Completed,
				Type:          item.Type,
			},
		})
	}

	return files, total, records[total-1].ID
}

func (c *Converter) SaveFiles(files []File) error {
	ctx, cancel := context.WithTimeout(context.Background(), 2*time.Minute)
	defer cancel()
	return c.db.InsertIntoFS(ctx, files)
}

func (c *Converter) updateJobStatus(jobID, cursor primitive.ObjectID, count int) error {
	ctx, cancel := context.WithTimeout(context.Background(), 5*time.Second)
	defer cancel()
	return c.db.UpdateJobStatus(ctx, c.CType, Job{
		ID:       jobID,
		Cursor:   cursor,
		CvtCount: count,
	})
}
