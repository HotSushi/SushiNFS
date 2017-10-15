#ifndef GRPC_CLIENT_H
#define GRPC_CLIENT_H 
#include <grpc++/grpc++.h>
#include "build/GRPC.grpc.pb.h"
#include "src/helpers/GeneralHelpers.h"


using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;

class GrpcClient {

	public:
		GrpcClient(std::shared_ptr<Channel> channel);
		int getAttributes(std::string path, struct stat *st);
		std::list<DirEntry> readDirectory(std::string path, int &responseCode);
		std::string read(std::string path, int offset, int size);
		int makeNode(std::string path, mode_t mode, dev_t rdev);
		int makeDir(std::string path, mode_t mode);
		int rmDir(std::string path);
		int rename(std::string from, std::string to);
		int truncate(std::string path, off_t size, struct fuse_file_info *fi);
		int create(std::string path, mode_t mode, struct fuse_file_info *fi);
		int open(std::string path, struct fuse_file_info *fi);
		int release(std::string path, struct fuse_file_info *fi);
		int fsync(std::string path, int isdatasync, struct fuse_file_info* fi);
		int unlink(std::string path);
		int write(std::string path, std::string buffer, int size, int offset, struct fuse_file_info* fi);
		int utimens(std::string path,const struct timespec *ts, struct fuse_file_info *fi);
	private:
		std::unique_ptr<Grpc::Stub> stub_;
};

#endif /* GRPC_CLIENT_H */
