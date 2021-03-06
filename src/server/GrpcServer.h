#include <iostream>
#include <memory>
#include <string>

#include <grpc++/grpc++.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <fcntl.h>

#include "src/helpers/GeneralHelpers.h"

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::Status;


class GrpcServiceImpl  final : public Grpc::Service {

	Status GetAttributes(ServerContext* context, const GetAttributesRequestObject* request, 
                GetAttributesResponseObject* response);

	Status ReadDirectory(ServerContext* context, const ReadDirectoryRequestObject* request, 
                ReadDirectoryResponseObject* response);

	Status Read(ServerContext* context, const ReadRequestObject* request, 
                ReadResponseObject* response);

	std::string mountLocation;

public:
	void setMountLocation(std::string mount);
};
