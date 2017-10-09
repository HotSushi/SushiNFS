#include <sys/types.h>
#include <sys/stat.h>
#include "build/GRPC.grpc.pb.h"

struct DirEntry {
  struct stat st;
  std::string name;
} ;

void toCstat(Stat gstat, struct stat *st);

Stat toGstat(struct stat *st);