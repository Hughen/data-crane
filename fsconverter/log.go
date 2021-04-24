package main

import (
	"sync"

	"go.uber.org/zap"
)

var o sync.Once
var log *logWrapper

type logWrapper struct {
	logger *zap.Logger
	sugar  *zap.SugaredLogger
}

func initLog() {
	o.Do(func() {
		logger, _ := zap.NewProduction()
		log = &logWrapper{logger, logger.Sugar()}
	})
}

func Logger() *zap.SugaredLogger {
	return log.sugar
}

func LogSync() {
	log.logger.Sync()
}
