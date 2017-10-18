
#include <src/server/GrpcServer.h>

// Logic and data behind the server's behavior.
// class GrpcServiceImpl final : public Grpc::Service {

  Status GrpcServiceImpl::GetAttributes(ServerContext* context, const GetAttributesRequestObject* request, 
                GetAttributesResponseObject* response) {

    
    struct stat st;

    toCstat(request->st(), &st);

    char *path =new char[request->path().length()+1];
    strcpy(path, request->path().c_str());
    int res = lstat(path, &st);
    if (res == -1) {

      response->set_status(-errno);
    } else {

      // char* temp = new char[sizeof(st)];
      // memcpy(temp, &st, sizeof(st));

      // StructData newData;

      // newData->set_binary(temp);
      // newData->set_length(sizeof(temp));
      response->set_status(0);
      *response->mutable_st() = toGstat(&st);
      // response->st = toGstat(&st);
    }

    return Status::OK;

  }

  Status GrpcServiceImpl::ReadDirectory(ServerContext* context, const ReadDirectoryRequestObject* request, 
                ReadDirectoryResponseObject* response) {




    DIR *dp;
    struct dirent *de;

    char *path =new char[request->path().length()+1];
    strcpy(path, request->path().c_str());
    
    dp = opendir(path);
    if (dp == NULL) {
      response->set_status(-errno);
    } else {
      while ((de = readdir(dp)) != NULL) {
        struct stat st;
        ReadDirectorySingleObject* rd = response->add_objects();

        memset(&st, 0, sizeof(st));
        st.st_ino = de->d_ino;
        st.st_mode = de->d_type << 12;

        rd->set_name(de->d_name);
        *rd->mutable_st() = toGstat(&st);
      }

      response->set_status(0);
    }

    

    closedir(dp);
    return Status::OK;

  }

  Status GrpcServiceImpl::Read(ServerContext* context, const ReadRequestObject* request, 
                ReadResponseObject* response) {


      int fileDir;
      int res;
      char* buffer = new char[request->size()];
      char *path =new char[request->path().length()+1];
      strcpy(path, request->path().c_str());
    
      fileDir = open(path, O_RDONLY);
      
      if (fileDir == -1) {
        response->set_status(-errno);
      } else {
        res = pread(fileDir, buffer, request->size(), request->offset());
        if (res == -1) {
          response->set_status(res = -errno);
        }
      }
      std::string pathstr(buffer);
      response->set_data(pathstr);
      close(fileDir);
      return Status::OK;
  }

  void GrpcServiceImpl::setMountLocation(std::string mount){
    GrpcServiceImpl::mountLocation = mount;
  }
// };

