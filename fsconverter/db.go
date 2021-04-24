package main

import (
	"context"
	"time"

	"go.mongodb.org/mongo-driver/bson/primitive"

	"go.mongodb.org/mongo-driver/bson"

	mgoptions "go.mongodb.org/mongo-driver/mongo/options"

	"go.mongodb.org/mongo-driver/mongo"
)

const (
	CollectionRawData   = "rawdata"
	CollectionDataBatch = "databatch"
	CollectionRawFile   = "rawfile"
	CollectionTFRecord  = "tfrecord"
	CollectionFS        = "fs"
)

type queryCount struct {
	ID    primitive.ObjectID `bson:"_id"`
	Count int64              `bson:"count"`
}

type DB struct {
	client *mongo.Client
}

func NewDB(uri string) *DB {
	ctx, cancel := context.WithTimeout(context.Background(), 5*time.Second)
	defer cancel()

	client, err := mongo.Connect(ctx, mgoptions.Client().ApplyURI(uri))
	if err != nil {
		panic(err)
	}

	return &DB{client: client}
}

func (db *DB) Close() {
	ctx, cancel := context.WithTimeout(context.Background(), 5*time.Second)
	defer cancel()
	db.client.Disconnect(ctx)
}

func (db *DB) getCollection(collection string) *mongo.Collection {
	return db.client.Database("mldb").Collection(collection)
}

func (db *DB) GetXData(ctx context.Context, cType CvtType) ([]XData, error) {
	c := db.getCollection(string(cType))
	var result []XData
	cursor, err := c.Find(ctx, bson.D{})
	if err != nil {
		return result, err
	}
	defer cursor.Close(ctx)
	for cursor.Next(ctx) {
		var tmp []XData
		if err := cursor.Decode(&tmp); err != nil {
			return result, err
		}
		result = append(result, tmp...)
	}
	if err := cursor.Err(); err != nil {
		return []XData{}, err
	}

	return result, nil
}

func (db *DB) CalculateTasks(ctx context.Context, cType CvtType, listID []XData) error {
	if len(listID) == 0 {
		return nil
	}
	c := db.getCollection(string(cType))

	inQuery := make(bson.A, len(listID))
	for i, v := range listID {
		inQuery[i] = v.ID
	}

	cursor, err := c.Aggregate(ctx, bson.D{
		{"$match", bson.M{"_id": bson.M{"$in": inQuery}}},
		{"$group", bson.M{
			"_id":   "$batchID",
			"count": bson.M{"$sum": 1},
		}},
	})
	if err != nil {
		return err
	}
	defer cursor.Close(ctx)
	for cursor.Next(ctx) {
		var result []queryCount
		if err := cursor.Decode(&result); err != nil {
			return err
		}

		for _, item := range result {
			if err := db.insertConverterJobs(ctx, cType, item.ID, item.Count); err != nil {
				Logger().Warnw("can not create or update a job", "taskID", item.ID.Hex(), "count", item.Count, "error", err)
			}
		}
	}
	if err := cursor.Err(); err != nil {
		return err
	}

	return nil
}

func (db *DB) insertConverterJobs(ctx context.Context, cType CvtType, taskID primitive.ObjectID, count int64) error {
	c := db.getCollection(getConvertJobsCollection(cType))

	update := bson.M{
		"$set": bson.M{"data_count": count},
		"$setOnInsert": bson.M{
			"last":       time.Now().Second(),
			"cvt_count":  0,
			"data_count": count,
			"cursor":     primitive.NilObjectID,
		},
	}
	upsert := true
	_, err := c.UpdateOne(ctx, bson.M{"_id": taskID}, update, &mgoptions.UpdateOptions{Upsert: &upsert})
	return err
}

func (db *DB) GetConverterJobs(ctx context.Context, cType CvtType) ([]Job, error) {
	c := db.getCollection(getConvertJobsCollection(cType))

	var result []Job
	filter := bson.M{"$where": "this.data_count != this.cvt_count"}
	cursor, err := c.Find(ctx, filter)
	if err != nil {
		return result, err
	}
	defer cursor.Close(ctx)
	for cursor.Next(ctx) {
		var tmp []Job
		if err := cursor.Decode(&tmp); err != nil {
			return []Job{}, err
		}
		result = append(result, tmp...)
	}
	if err := cursor.Err(); err != nil {
		return []Job{}, err
	}

	return result, nil
}

func (db *DB) NextRecords(ctx context.Context, cType CvtType, taskID, offset primitive.ObjectID) ([]Record, error) {
	c := db.getCollection(string(cType))

	var result []Record
	filter := bson.D{
		{"batchID", taskID},
		{"_id", bson.M{"$gt": offset}},
	}
	var limit int64 = 5000
	cursor, err := c.Find(ctx, filter, &mgoptions.FindOptions{Sort: bson.M{"_id": 1}, Limit: &limit})
	if err != nil {
		return result, err
	}
	defer cursor.Close(ctx)
	for cursor.Next(ctx) {
		var tmp []Record
		if err := cursor.Decode(&tmp); err != nil {
			return []Record{}, err
		}
		result = append(result, tmp...)
	}
	if err := cursor.Err(); err != nil {
		return []Record{}, err
	}

	return result, nil
}

// UpdateJobStatus
// 注意：CvtCount 为累加
func (db *DB) UpdateJobStatus(ctx context.Context, cType CvtType, job Job) error {
	c := db.getCollection(getConvertJobsCollection(cType))
	update := bson.M{
		"$set": bson.M{
			"last":   time.Now().Second(),
			"cursor": job.Cursor,
		},
		"$inc": bson.M{"cvt_count": job.CvtCount},
	}
	_, err := c.UpdateOne(ctx, bson.M{"_id": job.ID}, update)
	return err
}

func (db *DB) InsertIntoFS(ctx context.Context, files []File) error {
	if len(files) == 0 {
		return nil
	}
	c := db.getCollection(CollectionFS)

	values := make([]interface{}, len(files))
	for i, file := range files {
		values[i] = file
	}
	orderd := false
	_, err := c.InsertMany(ctx, values, &mgoptions.InsertManyOptions{Ordered: &orderd})
	if mongo.IsDuplicateKeyError(err) {
		return nil
	}
	return err
}
