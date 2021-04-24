package main

import (
	"math/rand"
	"time"
)

func main() {
	rand.Seed(time.Now().Unix())

	initLog()
	defer LogSync()
}
