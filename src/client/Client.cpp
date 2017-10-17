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
	
	// memcpy(buffer, readData.c_str(), size);
	return grpcClient.read(pathstr, buffer,offset, size, fi);
}

static int do_mknod(const char *path, mode_t mode, dev_t rdev)
{
	std::string pathstr(path);
	return grpcClient.makeNode(pathstr, mode, rdev);
}

static int do_mkdir(const char *path, mode_t mode)
{
	std::string pathstr(path);
	return grpcClient.makeDir(pathstr, mode);
}

static int do_rmdir(const char *path)
{
	std::string pathstr(path);
	return grpcClient.rmDir(pathstr);
}

static int do_rename(const char *from, const char *to, unsigned int flags)
{
	std::string fromstr(from);
	std::string tostr(to);
	return grpcClient.rename(fromstr, tostr);

}

static int do_truncate(const char *path, off_t size, struct fuse_file_info *fi)
{
	std::string pathstr(path);
	return grpcClient.truncate(pathstr, size, fi);
}

static int do_create(const char *path, mode_t mode, struct fuse_file_info *fi)
{
	std::string pathstr(path);
	return grpcClient.create(pathstr, mode, fi);
}

static int do_open(const char *path, struct fuse_file_info *fi)
{
	std::string pathstr(path);
	return grpcClient.open(pathstr, fi);
}

static int do_release(const char *path, struct fuse_file_info *fi)
{
	std::string pathstr(path);
	return grpcClient.release(pathstr, fi);
}

static int do_fsync(const char *path, int isdatasync, struct fuse_file_info* fi)
{
	std::string pathstr(path);
	return grpcClient.fsync(pathstr, isdatasync, fi);	
}

static int do_unlink(const char *path)
{
	std::string pathstr(path);
	return grpcClient.unlink(pathstr);
}

static int do_write(const char *path, const char *buf, size_t size, off_t offset, struct fuse_file_info *fi)
{
	std::string pathstr(path);
	return grpcClient.write(pathstr, buf, size, offset, fi);
}

static int do_flush(const char *path, struct fuse_file_info *fi)
{
	std::string pathstr(path);
	return grpcClient.flush(pathstr, fi);
}

static int do_utimens(const char *path, const struct timespec ts[2], struct fuse_file_info *fi)
{
	std::string pathstr(path);
    return grpcClient.utimens(pathstr, ts, fi);
}

static struct fuse_operations operations;

void setFuseOperations(struct fuse_operations &fo){
	fo.getattr = &do_getattr;
	fo.readdir = &do_readdir;
	fo.read = &do_read; 
	fo.mknod = &do_mknod;
	fo.mkdir = &do_mkdir;
	fo.rmdir = &do_rmdir;
	fo.rename = &do_rename;
	fo.truncate = &do_truncate;
	fo.create = &do_create;
	fo.open = &do_open;
	fo.release = &do_release;
	fo.fsync = &do_fsync;
	fo.unlink = &do_unlink;
	fo.write = &do_write;
	fo.flush = &do_flush;
	// sfo.utimens = &do_utimens;
}

int main( int argc, char *argv[] )
{
	setFuseOperations(operations);
	return fuse_main( argc, argv, &operations, NULL );
}
