package main

import (
	"context"
	"syscall"
	"testing"
	"time"

	"go.mongodb.org/mongo-driver/bson/primitive"
)

var db = NewDB("mongodb://admin:123456@127.0.0.1:27017/mldb?authSource=admin")
var datasetID, _ = primitive.ObjectIDFromHex("608409dfd784d1aa47ac797f")

func getFiles() []File {

	return []File{
		{
			Dataset: datasetID,
			Parent:  "/train",
			Name:    "n34234545",
			Type:    syscall.S_IFDIR,
		}, {
			Dataset: datasetID,
			Parent:  "/train/n34234545",
			Name:    "img_12423454.jpg",
			Type:    syscall.S_IFREG,
			Meta: Meta{
				Name:          "/train/n34234545/img_12423454.jpg",
				Digest:        "1817a1b982623721f486d62906c05e0f",
				ContentLength: 300513,
				ETag:          "1817a1b982623721f486d62906c05e0f",
				ContentType:   "image/jpeg",
				URL:           "mldb://raw/" + datasetID.Hex() + "/objects/train/n34234545/img_12423454.jpg",
				CTime:         1619257967,
				Creator:       "kindle.wang",
				Completed:     true,
				Type:          "rawfile",
			},
		}, {
			Dataset: datasetID,
			Parent:  "/val",
			Name:    "n278923545",
			Type:    syscall.S_IFDIR,
		},
	}
}

func TestDB_InsertIntoFS(t *testing.T) {
	files := getFiles()

	ctx, cancel := context.WithTimeout(context.Background(), 10*time.Second)
	defer cancel()
	err := db.InsertIntoFS(ctx, files)
	t.Logf("insert into fs error: %v", err)

	db.Close()
}
