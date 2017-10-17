
#include <src/server/GrpcServer.h>

// Logic and data behind the server's behavior.
// class GrpcServiceImpl final : public Grpc::Service {

  Status GrpcServiceImpl::GetAttributes(ServerContext* context, const GetAttributesRequestObject* request, 
                GetAttributesResponseObject* response) {

    
    struct stat st;

    toCstat(request->st(), &st);
    std::cout <<"------------------------------------------------\n";
    std::cout << "GetAttributes : path passed - " << request->path() << "\n";
    char *path =new char[request->path().length()+1];
    strcpy(path, request->path().c_str());
    int res = lstat(path, &st);
    if (res == -1) {
      std::cout << "GetAttributes : Error getting stat -  " << errno << " Error message - " << std::strerror(errno) <<"\n";
      response->set_status(-errno);
    } else {
      // std::cout << "GetAttributes : Success getting stat \n";
      // char* temp = new char[sizeof(st)];
      // memcpy(temp, &st, sizeof(st));

      // StructData newData;

      // newData->set_binary(temp);
      // newData->set_length(sizeof(temp));
      response->set_status(0);
      *response->mutable_st() = toGstat(&st);
      // response->st = toGstat(&st);
    }
      
    
    std::cout <<"------------------------------------------------\n\n";

    return Status::OK;

  }

  Status GrpcServiceImpl::ReadDirectory(ServerContext* context, const ReadDirectoryRequestObject* request, 
                ReadDirectoryResponseObject* response) {




    DIR *dp;
    struct dirent *de;
    std::cout <<"------------------------------------------------\n";
    std::cout << "ReadDirectory : path passed - " << request->path() << "\n";
    char *path =new char[request->path().length()+1];
    strcpy(path, request->path().c_str());
    
    dp = opendir(path);
    if (dp == NULL) {
      std::cout << "ReadDirectory : Error getting directory path -  " << errno << "\n";
      response->set_status(-errno);
    } else {
      while ((de = readdir(dp)) != NULL) {
        struct stat st;
        ReadDirectorySingleObject* rd = response->add_objects();

        // memset(&st, 0, sizeof(st));
        st.st_ino = de->d_ino;
        st.st_mode = de->d_type << 12;

        rd->set_name(de->d_name);
        *rd->mutable_st() = toGstat(&st);
      }
      response->set_status(0);
    }

    std::cout <<"------------------------------------------------\n\n";

    closedir(dp);
    return Status::OK;

  }

  Status GrpcServiceImpl::Read(ServerContext* context, const ReadRequestObject* request, 
                ReadResponseObject* response) {

      std::cout <<"------------------------------------------------\n";

      std::cout << "Read : path passed - " << request->path() <<"\n";

      int fileDir;
      int res;
      char* buffer = new char[request->size()];
      char *path =new char[request->path().length()+1];
      strcpy(path, request->path().c_str());
      struct fuse_file_info fi;
      toCFileInfo(request->fileinfo(), &fi);
    
      fileDir = fi.fh;
      std::cout << "Read: File directory using FH - " << fileDir <<"\n";
      if (fileDir == 0) {
        fileDir = open(path, O_RDONLY);
        fi.fh = fileDir;
        std::cout << "Read: File directory using open - " << fileDir <<"\n";
      }

      std::cout << "Read: got the file directory - " << fileDir <<"\n";

      if (fileDir == -1) {
        std::cout << "Read: Error occured with file directory - " << errno <<"\n";
        response->set_status(-errno);
      } else {
        res = pread(fileDir, buffer, request->size(), request->offset());
        if (res == -1) {
          std::cout << "Read: Error occured with writing to file- " << errno << " Error message - " << std::strerror(errno) <<"\n";
          response->set_status(-errno);
        }
      }
      response->set_data(buffer);
      *response->mutable_fileinfo() = toGFileInfo(&fi);
      delete[] buffer;
      std::cout <<"------------------------------------------------\n\n";
      // close(fileDir);
      return Status::OK;
  }

  Status GrpcServiceImpl::Write(ServerContext* context, const WriteRequestObject* request, 
              WriteResponseObject* response) {

      std::cout <<"------------------------------------------------\n";
      std::cout << "Write : path passed - " << request->path() <<"\n";
      int fileDir;
      //char *buffer = new char[request->data().length() + 1];
      char *path =new char[request->path().length()+1];
      std::string buffer = request->data();
      strcpy(path, request->path().c_str());
      struct fuse_file_info fi;
      toCFileInfo(request->fileinfo(), &fi);
      std::cout << "Write: got all the inputs sorted. File header - "<< fi.fh << "\n";



      //(void) fi;
      if(fi.fh == 0) {
        fileDir = open(path, O_WRONLY);
      } else {
        fileDir = fi.fh;
      }

      std::cout << "Write: got the file directory - " << fileDir <<"\n";
      int res;
      
      if (fileDir == -1) {
        std::cout << "Write: Error occured with file directory - " << errno <<"\n";
        response->set_status(-errno);
      } else {
        res = pwrite(fileDir, &buffer[0], request->size(), request->offset());
        
        if (res == -1) {
          std::cout << "Write: Error occured with writing to file- " << errno << " Error message - " << std::strerror(errno) <<"\n";
          response->set_status(-errno);
        }

        if (request->flag()) {
           // fsync(fi.fh);
          close(dup(fi.fh));
        }
      }
      std::cout <<"------------------------------------------------\n\n";
      
      //close(fileDir);
      response->set_status(0);
      response->set_datasize(res);
      *response->mutable_fileinfo() = toGFileInfo(&fi);
      return Status::OK;
  }

  Status GrpcServiceImpl::Create(ServerContext* context, const CreateRequestObject* request, 
                CreateResponseObject* response) {
        std::cout <<"------------------------------------------------\n";

        std::cout << "Create : path passed - " << request->path() <<"\n";
        char *path =new char[request->path().length()+1];
        strcpy(path, request->path().c_str());
        mode_t mode = request->mode();
        std::cout << "Create : mode passed - " << mode << "\n";
        struct fuse_file_info fi;
        toCFileInfo(request->fileinfo(), &fi);
        std::cout << "Create : file flags passed - " << fi.flags <<"\n";
        
        //fi->flags as 0
        int res = open(path, fi.flags, mode);
        if (res == -1) {
          std::cout << "Create : Error file not opened. -  " << errno <<"\n";
          response->set_status(-errno);
        } else {
          fi.fh = res;
          std::cout << "Create : Success file created - " << fi.fh <<"\n";
          response->set_status(0);
          *response->mutable_fileinfo() = toGFileInfo(&fi);
        }
        std::cout <<"------------------------------------------------\n\n";

        return Status::OK;
  }

  Status GrpcServiceImpl::Open(ServerContext* context, const OpenRequestObject* request, 
              OpenResponseObject* response) {

      std::cout <<"------------------------------------------------\n";
      std::cout << "Open : path passed - " << request->path();
      char *path = new char[request->path().length()+1];
      strcpy(path, request->path().c_str());
      struct fuse_file_info fi;
      toCFileInfo(request->fileinfo(), &fi);

      std::cout << "Open : file flags passed - " << fi.flags <<"\n";
      //fi->flags as 0
      int res = open(path, fi.flags);
      if (res == -1) {
        response->set_status(-errno);
      }
      fi.fh = res;

      std::cout << "Open : file handler passed - " << fi.fh <<"\n";
      std::cout <<"------------------------------------------------\n\n";
      response->set_status(0);
      *response->mutable_fileinfo() = toGFileInfo(&fi);
      
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

        std::cout <<"------------------------------------------------\n";
        std::cout << "Truncate : path passed - " << request->path() << "\n";
        int res;
        char *path =new char[request->path().length()+1];
        strcpy(path, request->path().c_str());
        struct fuse_file_info fi;
        toCFileInfo(request->fileinfo(), &fi);
        std::cout << "Truncate : FH received - " << fi.fh << "\n";

        if (fi.fh != 0) {
          res = ftruncate(fi.fh, request->size());
        } else {
          res = truncate(path, request->size());
        }

        if (res == -1) {
          std::cout << "Truncate : Error file not opened. -  " << errno <<"\n";
          response->set_status(-errno);
        }
        std::cout <<"------------------------------------------------\n\n";
        response->set_status(0);
        *response->mutable_fileinfo() = toGFileInfo(&fi);
        return Status::OK;
  }

  Status GrpcServiceImpl::Release(ServerContext* context, const ReleaseRequestObject* request, 
            ReleaseResponseObject* response) {

      std::cout <<"------------------------------------------------\n";
      std::cout << "Release : path passed - " << request->path() << "\n";
      char *path =new char[request->path().length()+1];
      strcpy(path, request->path().c_str());
      struct fuse_file_info fi;
      toCFileInfo(request->fileinfo(), &fi);
      std::cout << "Release : FH received - " << fi.fh << "\n";

      (void) path;
      close(fi.fh);
      std::cout << "Release : FH closed, fh - " << fi.fh << "\n";
      response->set_status(0);
      *response->mutable_fileinfo() = toGFileInfo(&fi);
      std::cout <<"------------------------------------------------\n\n";
      return Status::OK;
  }

  Status GrpcServiceImpl::Fsync(ServerContext* context, const FsyncRequestObject* request, 
            FsyncResponseObject* response) {

      std::cout <<"------------------------------------------------\n";
      std::cout << "Fsync : path passed - " << request->path() << "\n";
      char *path =new char[request->path().length()+1];
      strcpy(path, request->path().c_str());
      int isdatasync = request->isdatasync();
      struct fuse_file_info fi;
      toCFileInfo(request->fileinfo(), &fi);

      (void) path;
      (void) isdatasync;
      (void) fi;
      response->set_status(0);
      *response->mutable_fileinfo() = toGFileInfo(&fi);
      std::cout <<"------------------------------------------------\n\n";
      return Status::OK;
  }

  Status GrpcServiceImpl::Unlink(ServerContext* context, const UnlinkRequestObject* request, 
            UnlinkResponseObject* response) {
      
      std::cout <<"------------------------------------------------\n";
      std::cout << "Unlink : path passed - " << request->path() << "\n";
      char *path =new char[request->path().length()+1];
      strcpy(path, request->path().c_str());

      int res = unlink(path);

      if (res == -1) {
        response->set_status(-errno);
      }
      std::cout <<"------------------------------------------------\n\n";
      response->set_status(0);
      return Status::OK;
  }

  Status GrpcServiceImpl::Flush(ServerContext* context, const FlushRequestObject* request, 
            FlushResponseObject* response) {

      std::cout <<"------------------------------------------------\n";
      std::cout << "Flush : path passed - " << request->path() << "\n";
      char *path =new char[request->path().length()+1];
      strcpy(path, request->path().c_str());
      struct fuse_file_info fi;
      toCFileInfo(request->fileinfo(), &fi);
      std::cout << "Flush : FH received - " << fi.fh << "\n";

      (void) path;
      int res = close(dup(fi.fh));
      response->set_status(0);
      if (res == -1) {
        response->set_status(-errno);
      }
      std::cout << "Flush : FH closed, fh - " << fi.fh << "\n";
      *response->mutable_fileinfo() = toGFileInfo(&fi);
      std::cout <<"------------------------------------------------\n\n";
      return Status::OK;
  }

  Status GrpcServiceImpl::Utimens(ServerContext* context, const UtimensRequestObject* request, 
              UtimensResponseObject* response) {

      char *path =new char[request->path().length()+1];
      strcpy(path, request->path().c_str());
      // struct fuse_file_info fi;
      // toCFileInfo(request->fileinfo(), &fi);

      struct timespec ts;
      toCTimeSpec(request->timespec(), &ts);
      const struct timespec *ts2;
      // bcopy(ts, ts2, sizeof(ts));

      int res = utimensat(0, path, ts2, AT_SYMLINK_NOFOLLOW);
      if (res == -1) {
        response->set_status(-errno);
      }

      response->set_status(0);
      // *response->mutable_fileinfo() = toGFileInfo(&fi);
      
      return Status::OK;
  }
// };

