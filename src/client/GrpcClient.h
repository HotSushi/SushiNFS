#ifndef GRPC_CLIENT_H
#define GRPC_CLIENT_H 
#include <grpc++/grpc++.h>
#include "build/GRPC.grpc.pb.h"
#include "src/helpers/GeneralHelpers.h"


using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;

class Datastore{
	
	std::string data;
	bool isDirty;
	int originalOffset;
public:
	Datastore(void) {
		data = "";
		isDirty = false;
		originalOffset = 0;
	}

	Datastore(std::string d, int offset, bool status) {
		data = d;
		isDirty = status;
		originalOffset = offset;
	}

	std::string getData(void) {
		return data;
	}
	bool getIsDirty(void) {
		return isDirty;
	}
	int getOriginalOffset(void) {
		return originalOffset;
	}
	void setValues(std::string d, int offset, bool status = false) {
		data = d;
		originalOffset = offset;
		isDirty = status;
	}
};

class GrpcClient {

	public:
		GrpcClient(std::shared_ptr<Channel> channel);
		int getAttributes(std::string path, struct stat *st);
		std::list<DirEntry> readDirectory(std::string path, int &responseCode);
		int read(std::string path, char* buffer,int offset, int size, struct fuse_file_info *fi);
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
		int write(std::string path, const char *buf, int size, int offset, struct fuse_file_info* fi);
		int flush(std::string path, struct fuse_file_info *fi);
		int utimens(std::string path,const struct timespec *ts, struct fuse_file_info *fi);
	private:
		std::unique_ptr<Grpc::Stub> stub_;
};

#endif /* GRPC_CLIENT_H */
