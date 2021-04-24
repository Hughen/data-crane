package main

import (
	"go.mongodb.org/mongo-driver/bson/primitive"
)

type Job struct {
	ID        primitive.ObjectID `json:"_id" bson:"_id"`
	LastTime  int64              `json:"last" bson:"last"`
	CvtCount  int                `json:"cvt_count" bson:"cvt_count"`
	DataCount int                `json:"data_count" bson:"data_count"`
	Cursor    primitive.ObjectID `json:"cursor" bson:"cursor"`
}

type XData struct {
	ID primitive.ObjectID `json:"_id" bson:"_id"`
}

type Record struct {
	ID            primitive.ObjectID `json:"_id" bson:"_id"`
	BatchID       primitive.ObjectID `json:"batchID" bson:"batchID"`
	Name          string             `bson:"name" bson:"name"`
	Completed     bool               `bson:"completed" bson:"completed"`
	ContentLength int64              `json:"contentLength" bson:"contentLength"`
	ContentType   string             `json:"contentType" bson:"contentType"`
	Creator       string             `json:"creator" bson:"creator"`
	CTime         int64              `json:"ctime" bson:"ctime"`
	Digest        string             `json:"digest" bson:"digest"`
	ETag          string             `json:"eTag" bson:"eTag"`
	Type          string             `json:"type" bson:"type"`
	URL           string             `json:"url" bson:"url"`
}

type File struct {
	// ID      primitive.ObjectID `bson:"_id"`
	Dataset primitive.ObjectID `bson:"dataset"`
	Parent  string             `bson:"parent"`
	Name    string             `bson:"name"`
	Type    int                `bson:"ftype"`
	Meta    Meta               `bson:"meta,omitempty"`
}

type Meta struct {
	Name          string `bson:"name"`
	Digest        string `bson:"digest"`
	ContentLength int64  `bson:"contentLength"`
	ETag          string `bson:"eTag"`
	ContentType   string `bson:"contentType"`
	URL           string `bson:"url"`
	CTime         int64  `bson:"ctime"`
	Creator       string `bson:"creator"`
	Completed     bool   `bson:"completed"`
	Type          string `bson:"type"`
}

func getConvertJobsCollection(cType CvtType) string {
	return "converter_jobs_" + string(cType)
}
