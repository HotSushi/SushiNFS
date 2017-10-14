#ifndef GENERAL_HELPERS_H
#define GENERAL_HELPERS_H 
#define FUSE_USE_VERSION 30
#include <sys/types.h>
#include <sys/stat.h>
#include <fuse.h>
#include "build/GRPC.grpc.pb.h"

struct DirEntry {
  struct stat st;
  std::string name;
} ;

void toCstat(Stat gstat, struct stat *st);

Stat toGstat(struct stat *st);

FuseFileInfo toGFileInfo(struct fuse_file_info *fi);

void toCFileInfo(FuseFileInfo fuseFileInfo, struct fuse_file_info *fi);
#endif /* GENERAL_HELPERS_H */
