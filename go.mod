module data-crane

go 1.14

require (
	github.com/allegro/bigcache v1.2.1
	github.com/spf13/pflag v1.0.5
	github.com/stretchr/testify v1.6.1 // indirect
	google.golang.org/grpc v1.30.0
)

replace google.golang.org/grpc => github.com/grpc/grpc-go v1.30.0
