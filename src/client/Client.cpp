#define FUSE_USE_VERSION 30

#include <fuse.h>
#include "src/client/GrpcClient.h"

static GrpcClient grpcClient  = grpc::CreateChannel("localhost:50051", grpc::InsecureChannelCredentials());

static int do_getattr( const char *path, struct stat *st, struct fuse_file_info *fi )
{
	std::string pathstr(path);
	return grpcClient.getAttributes(pathstr, st);
}

static int do_readdir( const char *path, void *buffer, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi )
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

struct hello_fuse_operations:fuse_operations
{
    hello_fuse_operations ()
    {
        getattr    = do_getattr;
        readdir    = do_readdir;
        read       = do_read;
    }
};
static struct hello_fuse_operations hello_oper;

int main( int argc, char *argv[] )
{
	return fuse_main( argc, argv, &hello_oper, NULL );
}
int (*)(const char*, void*, fuse_fill_dir_t, off_t, fuse_file_info*, fuse_readdir_flags) {aka int (*)(const char*, void*, int (*)(void*, const char*, const stat*, long int, fuse_fill_dir_flags), long int, fuse_file_info*, fuse_readdir_flags)}
int (*)(const char*, void*, fuse_fill_dir_t, off_t, fuse_file_info*) {aka int (*)(const char*, void*, int (*)(void*, const char*, const stat*, long int, fuse_fill_dir_flags), long int, fuse_file_info*)}’