
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

  Status GrpcServiceImpl::Write(ServerContext* context, const WriteRequestObject* request, 
              WriteResponseObject* response) {

      int fileDir;
      char *buffer = new char[request->size()];
      char *path =new char[request->path().length()+1];
      strcpy(path, request->path().c_str());
      FuseFileInfo fi = request->fileinfo();

      if(fi.fh() == 0) {
        fileDir = open(path, O_WRONLY);
      } else {
        fileDir = fi.fh();
      }
      
      if (fileDir == -1) {
        response->set_status(-errno);
      } else {
        int res = pwrite(fileDir, buffer, request->size(), request->offset());
        if (res == -1) {
          response->set_status(-errno);
        }
      }
      
      close(fileDir);
      response->set_status(0);
      *response->mutable_fileinfo() = fi;
      return Status::OK;
  }

  Status GrpcServiceImpl::Mknod(ServerContext* context, const MknodRequestObject* request, 
                MknodResponseObject* response) {
      int res;

      char *path =new char[request->path().length()+1];
      strcpy(path, request->path().c_str());

      mode_t mode = request->mode();
      dev_t rdev = request->rdev();
      /* On Linux this could just be 'mknod(path, mode, rdev)' but this
         is more portable */
      // if (S_ISREG(mode)) {

      //   res = open(path, O_CREAT | O_EXCL | O_WRONLY, mode);

      //   if (res >= 0) {
      //     res = close(res);
      //   }

      // } else 
      if (S_ISFIFO(mode)) {

          res = mkfifo(path, mode);
      } else {

          res = mknod(path, mode, rdev);
      }

      if (res == -1) {
          response->set_status(-errno);
      }

      response->set_status(0);
      return Status::OK;
  }

  Status GrpcServiceImpl::MkDir(ServerContext* context, const MkDirRequestObject* request, 
                MkDirResponseObject* response) {

      char *path =new char[request->path().length()+1];
      strcpy(path, request->path().c_str());

      mode_t mode = request->mode();
      int res = mkdir(path, mode);

      if (res == -1) {
        response->set_status(-errno);
      }

      response->set_status(0);
      return Status::OK;
  }

  Status GrpcServiceImpl::RmDir(ServerContext* context, const RmDirRequestObject* request, 
                RmDirResponseObject* response) {
    
      char *path =new char[request->path().length()+1];
      strcpy(path, request->path().c_str());

      int res = rmdir(path);

      if (res == -1) {
        response->set_status(-errno);
      }

      response->set_status(0);
      return Status::OK;
  }

  Status GrpcServiceImpl::Rename(ServerContext* context, const RenameRequestObject* request, 
                RenameResponseObject* response) {

      // if (flags)
      //   return -EINVAL

      char *to_path =new char[request->to().length()+1];
      strcpy(to_path, request->to().c_str());

      char *from_path =new char[request->from().length()+1];
      strcpy(from_path, request->from().c_str());

      int res = rename(from_path, to_path);
      if (res == -1) {
        response->set_status(-errno);
      }

      response->set_status(0);

      return Status::OK;
  }

  Status GrpcServiceImpl::Truncate(ServerContext* context, const TruncateRequestObject* request, 
                TruncateResponseObject* response) {

        int res;
        char *path =new char[request->path().length()+1];
        strcpy(path, request->path().c_str());
        FuseFileInfo fi = request->fileinfo();

        if (fi.fh() != 0) {
          res = ftruncate(fi.fh(), request->size());
        } else {
          res = truncate(path, request->size());
        }

        if (res == -1) {
          response->set_status(-errno);
        }

        response->set_status(0);
        *response->mutable_fileinfo() = fi;
        return Status::OK;
  }

  Status GrpcServiceImpl::Create(ServerContext* context, const CreateRequestObject* request, 
                CreateResponseObject* response) {

        char *path =new char[request->path().length()+1];
        strcpy(path, request->path().c_str());
        FuseFileInfo fi = request->fileinfo();
        mode_t mode = request->mode();
        //fi->flags as 0
        int res = open(path, 0, mode);
        if (res == -1) {
          response->set_status(-errno);
        } else {
          fi.set_fh(res);
          response->set_status(0);
          *response->mutable_fileinfo() = fi;
        }

        return Status::OK;
  }

  Status GrpcServiceImpl::Open(ServerContext* context, const OpenRequestObject* request, 
              OpenResponseObject* response) {

      char *path =new char[request->path().length()+1];
      strcpy(path, request->path().c_str());
      FuseFileInfo fi = request->fileinfo();
      //fi->flags as 0
      int res = open(path, 0);
      if (res == -1) {
        response->set_status(-errno);
      }
      fi.set_fh(res);
      response->set_status(0);
      *response->mutable_fileinfo() = fi;
      
      return Status::OK;
  }

  Status GrpcServiceImpl::Release(ServerContext* context, const ReleaseRequestObject* request, 
            ReleaseResponseObject* response) {

      char *path =new char[request->path().length()+1];
      strcpy(path, request->path().c_str());
      FuseFileInfo fi = request->fileinfo();

      (void) path;
      close(fi.fh());
      response->set_status(0);
      *response->mutable_fileinfo() = fi;
      return Status::OK;
  }

  Status GrpcServiceImpl::Fsync(ServerContext* context, const FsyncRequestObject* request, 
            FsyncResponseObject* response) {

      char *path =new char[request->path().length()+1];
      strcpy(path, request->path().c_str());
      FuseFileInfo fi = request->fileinfo();
      int isdatasync = request->isdatasync();
      (void) path;
      (void) isdatasync;
      (void) fi;
      response->set_status(0);
      *response->mutable_fileinfo() = fi;
      return Status::OK;
  }

  Status GrpcServiceImpl::Unlink(ServerContext* context, const UnlinkRequestObject* request, 
            UnlinkResponseObject* response) {
      
      char *path =new char[request->path().length()+1];
      strcpy(path, request->path().c_str());

      int res = unlink(path);

      if (res == -1) {
        response->set_status(-errno);
      }

      response->set_status(0);
      return Status::OK;
  }
// };

