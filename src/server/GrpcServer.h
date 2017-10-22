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

	Status Mknod(ServerContext* context, const MknodRequestObject* request, 
                MknodResponseObject* response);

	Status MkDir(ServerContext* context, const MkDirRequestObject* request, 
                MkDirResponseObject* response);

	Status RmDir(ServerContext* context, const RmDirRequestObject* request, 
                RmDirResponseObject* response);

	Status Rename(ServerContext* context, const RenameRequestObject* request, 
                RenameResponseObject* response);

	Status Truncate(ServerContext* context, const TruncateRequestObject* request, 
                TruncateResponseObject* response);

	Status Create(ServerContext* context, const CreateRequestObject* request, 
                CreateResponseObject* response);

	Status Open(ServerContext* context, const OpenRequestObject* request, 
            	OpenResponseObject* response);

	Status Write(ServerContext* context, const WriteRequestObject* request, 
            	WriteResponseObject* response);

	Status Release(ServerContext* context, const ReleaseRequestObject* request, 
        		ReleaseResponseObject* response);

	Status Fsync(ServerContext* context, const FsyncRequestObject* request, 
        		FsyncResponseObject* response);

	Status Unlink(ServerContext* context, const UnlinkRequestObject* request, 
        		UnlinkResponseObject* response);

	Status Flush(ServerContext* context, const FlushRequestObject* request, 
        		FlushResponseObject* response);

  	Status Utimens(ServerContext* context, const UtimensRequestObject* request, 
              	UtimensResponseObject* response);

    std::string mountpoint;

public:

    void setMountPoint(std::string mountpoint);

};
