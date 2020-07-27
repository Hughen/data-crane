package lrd

const (
	// ref: https://en.wikipedia.org/wiki/Fowler%E2%80%93Noll%E2%80%93Vo_hash_function#FNV-1a_hash
	fnvOffset64 = 14695981039346656037
	fnvPrime64  = 1099511628211
)

func hashSum(v string) uint64 {
	var hash uint64 = fnvOffset64
	for i := 0; i < len(v); i++ {
		hash ^= uint64(v[i])
		hash *= fnvPrime64
	}

	return hash
}
