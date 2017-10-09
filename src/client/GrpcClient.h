#include <grpc++/grpc++.h>
#include "build/GRPC.grpc.pb.h"
#include "src/helpers/GeneralHelpers.h"


using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;

struct DirEntry {
  struct stat st;
  std::string name;
} ;

class GrpcClient {

	public:
		GrpcClient(std::shared_ptr<Channel> channel);
		int getAttributes(std::string path, struct stat *st);
		std::list<DirEntry> readDirectory(std::string path, int &responseCode);
		int read(std::string path, int offset, int size);

	private:
		std::unique_ptr<Grpc::Stub> stub_;
};