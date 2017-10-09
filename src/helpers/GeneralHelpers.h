#include <sys/types.h>
#include <sys/stat.h>
#include "build/GRPC.grpc.pb.h"


void toCstat(Stat gstat, struct stat *st);

Stat toGstat(struct stat *st);