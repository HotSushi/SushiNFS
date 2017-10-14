#define FUSE_USE_VERSION 30

#include <fuse.h>
#include "src/client/GrpcClient.h"

static GrpcClient grpcClient  = grpc::CreateChannel("localhost:50051", grpc::InsecureChannelCredentials());

static int do_getattr( const char *path, struct stat *st, struct fuse_file_info *fi )
{
	std::string pathstr(path);
	return grpcClient.getAttributes(pathstr, st);
}

static int do_readdir( const char *path, void *buffer, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi, fuse_readdir_flags flags)
{
	int responseCode;
	std::string pathstr(path);
	std::list<DirEntry> dirEntries = grpcClient.readDirectory(pathstr, responseCode);
	for (auto const& dirEntry : dirEntries) {
    	filler(buffer, dirEntry.name.c_str(), &dirEntry.st, 0, FUSE_FILL_DIR_PLUS);
	}
	return responseCode;
}

static int do_read( const char *path, char *buffer, size_t size, off_t offset, struct fuse_file_info *fi )
{
	std::string pathstr(path);
	std::string readData = grpcClient.read(pathstr, offset, size);
	memcpy(buffer, readData.c_str(), size);
	return strlen(readData.c_str());
}

static struct fuse_operations operations;

void setFuseOperations(struct fuse_operations &fo){
	//memset(fo, 0, sizeof(struct fuse_operations));
	fo.getattr = &do_getattr;
	fo.readdir = &do_readdir;
	fo.read = &do_read; 
}

int main( int argc, char *argv[] )
{
	setFuseOperations(operations);
	return fuse_main( argc, argv, &operations, NULL );
}