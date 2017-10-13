
#include "GRPCServer.h"

// Logic and data behind the server's behavior.
class GrpcServiceImpl final : public Grpc::Service {

  Status GetAttributes(ServerContext* context, GetAttributesRequestObject* request, 
                GetAttributesResponseObject* response) override {

    
    struct stat st;

    toCstat(request->st(), &st);

    res = lstat(request->path(), &st);
    if (res == -1) {

      response.set_status(-errno);
    } else {

      // char* temp = new char[sizeof(st)];
      // memcpy(temp, &st, sizeof(st));

      // StructData newData;

      // newData->set_binary(temp);
      // newData->set_length(sizeof(temp));

      response.set_status(0);
      response.set_st(toGstat(st));
    }

    return Status::OK;

  }

  Status ReadDirectory(ServerContext* context, ReadDirectoryRequestObject* request, 
                ReadDirectoryResponseObject* response) override {




    DIR *dp;
    struct dirent *de;

    dp = opendir(request->path());
    if (dp == NULL) {
      response.set_status(-errno);
    } else {
      while ((de = readdir(dp)) != NULL) {
        struct stat st;
        ReadDirectorySingleObject rd;

        memset(&st, 0, sizeof(st));
        st.st_ino = de->d_ino;
        st.st_mode = de->d_type << 12;

        rd.set_name(de->d_name);
        rd.set_st(toGstat(st));
        response.add_objects(rd);
      }

      response.set_status(0);
    }

    

    closedir(dp);
    return Status::OK;

  }

  Status Read(ServerContext* context, ReadRequestObject* request, 
                ReadResponseObject* response) override {


      int fileDir;
      int res;
      char* buffer = new char[request->size()];

      fileDir = open(request->path(), O_RDONLY);
      
      if (fileDir == -1) {
        response.set_status(-errno);
      } else {
        res = pread(fileDir, buffer, request->size(), request->offset());
        if (res == -1) {
          response.set_status(res = -errno);
        }
      }
      response.set_data(buffer);
      close(fileDir);
      return Status::OK;
  }
};
  1
