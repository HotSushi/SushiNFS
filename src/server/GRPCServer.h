#include <iostream>
#include <memory>
#include <string>

#include <grpc++/grpc++.h>
#include <fuse.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>

#include "src/helpers/GeneralHelpers.h"

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::Status;
using GRPC::Grpc;


class GrpcServiceImpl final : public Grpc::Service {

	Status GetAttributes(ServerContext* context, GetAttributesRequestObject* request, 
                GetAttributesResponseObject* response);

	Status ReadDirectory(ServerContext* context, ReadDirectoryRequestObject* request, 
                ReadDirectoryResponseObject* response);

	Status Read(ServerContext* context, ReadRequestObject* request, 
                ReadResponseObject* response);

};
