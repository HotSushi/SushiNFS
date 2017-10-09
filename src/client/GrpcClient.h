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

	private:
		std::unique_ptr<Grpc::Stub> stub_;
};

#endif /* GRPC_CLIENT_H */