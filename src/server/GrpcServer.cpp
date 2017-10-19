
#include <src/server/GrpcServer.h>
#define LOG false

// Logic and data behind the server's behavior.
// class GrpcServiceImpl final : public Grpc::Service {

  Status GrpcServiceImpl::GetAttributes(ServerContext* context, const GetAttributesRequestObject* request, 
                GetAttributesResponseObject* response) {

    
    struct stat st;

    toCstat(request->st(), &st);
    if(LOG) std::cout <<"------------------------------------------------\n";
    if(LOG) std::cout << "GetAttributes : path passed - " << request->path() << "\n";
    std::string adjustedPath = mountpoint + request->path();
    char *path =new char[adjustedPath.length()+1];
    strcpy(path, adjustedPath.c_str());
    int res = lstat(path, &st);
    if (res == -1) {
      if(LOG) std::cout << "GetAttributes : Error getting stat -  " << errno << " Error message - " << std::strerror(errno) <<"\n";
      response->set_status(-errno);
    } else {
      // if(LOG) std::cout << "GetAttributes : Success getting stat \n";
      // char* temp = new char[sizeof(st)];
      // memcpy(temp, &st, sizeof(st));

      // StructData newData;

      // newData->set_binary(temp);
      // newData->set_length(sizeof(temp));
      response->set_status(0);
      *response->mutable_st() = toGstat(&st);
      // response->st = toGstat(&st);
    }
      
    
    if(LOG) std::cout <<"------------------------------------------------\n\n";

    return Status::OK;

  }

  Status GrpcServiceImpl::ReadDirectory(ServerContext* context, const ReadDirectoryRequestObject* request, 
                ReadDirectoryResponseObject* response) {




    DIR *dp;
    struct dirent *de;
    if(LOG) std::cout <<"------------------------------------------------\n";
    if(LOG) std::cout << "ReadDirectory : path passed - " << request->path() << "\n";
    std::string adjustedPath = mountpoint + request->path();
    char *path =new char[adjustedPath.length()+1];
    strcpy(path, adjustedPath.c_str());

    dp = opendir(path);
    if (dp == NULL) {
      if(LOG) std::cout << "ReadDirectory : Error getting directory path -  " << errno << "\n";
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

    if(LOG) std::cout <<"------------------------------------------------\n\n";

    closedir(dp);
    return Status::OK;

  }

  Status GrpcServiceImpl::Read(ServerContext* context, const ReadRequestObject* request, 
                ReadResponseObject* response) {

      if(LOG) std::cout <<"------------------------------------------------\n";

      if(LOG) std::cout << "Read : path passed - " << request->path() <<"\n";

      int fileDir;
      int res;
      char* buffer = new char[request->size()];  
    std::string adjustedPath = mountpoint + request->path();
    char *path =new char[adjustedPath.length()+1];
    strcpy(path, adjustedPath.c_str());
      struct fuse_file_info fi;
      toCFileInfo(request->fileinfo(), &fi);
    
      fileDir = fi.fh;
      if(LOG) std::cout << "Read: File directory using FH - " << fileDir <<"\n";
      if (fileDir == 0) {
        fileDir = open(path, O_RDONLY);
        fi.fh = fileDir;
        if(LOG) std::cout << "Read: File directory using open - " << fileDir <<"\n";
      }

      if(LOG) std::cout << "Read: got the file directory - " << fileDir <<"\n";

      if (fileDir == -1) {
        if(LOG) std::cout << "Read: Error occured with file directory - " << errno <<"\n";
        response->set_status(-errno);
      } else {
        res = pread(fileDir, buffer, request->size(), request->offset());
        if (res == -1) {
          if(LOG) std::cout << "Read: Error occured with writing to file- " << errno << " Error message - " << std::strerror(errno) <<"\n";
          response->set_status(-errno);
        }
      }
      response->set_data(buffer);
      response->set_size(res);
      *response->mutable_fileinfo() = toGFileInfo(&fi);
      delete[] buffer;
      if(LOG) std::cout <<"------------------------------------------------\n\n";
      // close(fileDir);
      return Status::OK;
  }

  Status GrpcServiceImpl::Write(ServerContext* context, const WriteRequestObject* request, 
              WriteResponseObject* response) {

      if(LOG) std::cout <<"------------------------------------------------\n";
      if(LOG) std::cout << "Write : path passed - " << request->path() <<"\n";
      int fileDir;
      //char *buffer = new char[request->data().length() + 1];
      std::string adjustedPath = mountpoint + request->path();
      char *path =new char[adjustedPath.length()+1];
      strcpy(path, adjustedPath.c_str());
    
      std::string buffer = request->data();
      struct fuse_file_info fi;
      toCFileInfo(request->fileinfo(), &fi);
      if(LOG) std::cout << "Write: got all the inputs sorted. File header - "<< fi.fh << "\n";



      //(void) fi;
      if(fi.fh == 0) {
        fileDir = open(path, O_WRONLY);
      } else {
        fileDir = fi.fh;
      }

      if(LOG) std::cout << "Write: got the file directory - " << fileDir <<"\n";
      int res;
      
      if (fileDir == -1) {
        if(LOG) std::cout << "Write: Error occured with file directory - " << errno <<"\n";
        response->set_status(-errno);
      } else {
        res = pwrite(fileDir, &buffer[0], request->size(), request->offset());
        
        if (res == -1) {
          if(LOG) std::cout << "Write: Error occured with writing to file- " << errno << " Error message - " << std::strerror(errno) <<"\n";
          response->set_status(-errno);
        }

        if (request->flag()) {
           // fsync(fi.fh);
          close(dup(fi.fh));
        }
      }
      if(LOG) std::cout <<"------------------------------------------------\n\n";
      
      //close(fileDir);
      response->set_status(0);
      response->set_datasize(res);
      *response->mutable_fileinfo() = toGFileInfo(&fi);
      return Status::OK;
  }

  Status GrpcServiceImpl::Create(ServerContext* context, const CreateRequestObject* request, 
                CreateResponseObject* response) {
        if(LOG) std::cout <<"------------------------------------------------\n";

        if(LOG) std::cout << "Create : path passed - " << request->path() <<"\n";
        std::string adjustedPath = mountpoint + request->path();
        char *path =new char[adjustedPath.length()+1];
        strcpy(path, adjustedPath.c_str());
        mode_t mode = request->mode();
        if(LOG) std::cout << "Create : mode passed - " << mode << "\n";
        struct fuse_file_info fi;
        toCFileInfo(request->fileinfo(), &fi);
        if(LOG) std::cout << "Create : file flags passed - " << fi.flags <<"\n";
        
        //fi->flags as 0
        int res = open(path, fi.flags, mode);
        if (res == -1) {
          if(LOG) std::cout << "Create : Error file not opened. -  " << errno <<"\n";
          response->set_status(-errno);
        } else {
          fi.fh = res;
          if(LOG) std::cout << "Create : Success file created - " << fi.fh <<"\n";
          response->set_status(0);
          *response->mutable_fileinfo() = toGFileInfo(&fi);
        }
        if(LOG) std::cout <<"------------------------------------------------\n\n";

        return Status::OK;
  }

  Status GrpcServiceImpl::Open(ServerContext* context, const OpenRequestObject* request, 
              OpenResponseObject* response) {

      if(LOG) std::cout <<"------------------------------------------------\n";
      if(LOG) std::cout << "Open : path passed - " << request->path();
      std::string adjustedPath = mountpoint + request->path();
      char *path =new char[adjustedPath.length()+1];
      strcpy(path, adjustedPath.c_str());
      struct fuse_file_info fi;
      toCFileInfo(request->fileinfo(), &fi);

      if(LOG) std::cout << "Open : file flags passed - " << fi.flags <<"\n";
      //fi->flags as 0
      int res = open(path, fi.flags);
      if (res == -1) {
        response->set_status(-errno);
      }
      fi.fh = res;

      if(LOG) std::cout << "Open : file handler passed - " << fi.fh <<"\n";
      if(LOG) std::cout <<"------------------------------------------------\n\n";
      response->set_status(0);
      *response->mutable_fileinfo() = toGFileInfo(&fi);
      
      return Status::OK;
  }

  Status GrpcServiceImpl::Mknod(ServerContext* context, const MknodRequestObject* request, 
                MknodResponseObject* response) {
      int res;

      std::string adjustedPath = mountpoint + request->path();
      char *path =new char[adjustedPath.length()+1];
      strcpy(path, adjustedPath.c_str());
      
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

      std::string adjustedPath = mountpoint + request->path();
      char *path =new char[adjustedPath.length()+1];
      strcpy(path, adjustedPath.c_str());
    
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
    
      std::string adjustedPath = mountpoint + request->path();
      char *path =new char[adjustedPath.length()+1];
      strcpy(path, adjustedPath.c_str());
    
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

      std::string adjustedToPath = mountpoint + request->to();    
      char *to_path =new char[adjustedToPath.length()+1];
      strcpy(to_path, adjustedToPath.c_str());

      std::string adjustedFromPath = mountpoint + request->from();    
      char *from_path =new char[adjustedFromPath.length()+1];
      strcpy(from_path, adjustedFromPath.c_str());

      int res = rename(from_path, to_path);
      if (res == -1) {
        response->set_status(-errno);
      }

      response->set_status(0);

      return Status::OK;
  }

  Status GrpcServiceImpl::Truncate(ServerContext* context, const TruncateRequestObject* request, 
                TruncateResponseObject* response) {

        if(LOG) std::cout <<"------------------------------------------------\n";
        if(LOG) std::cout << "Truncate : path passed - " << request->path() << "\n";
        int res;
        std::string adjustedPath = mountpoint + request->path();
        char *path =new char[adjustedPath.length()+1];
        strcpy(path, adjustedPath.c_str());
        struct fuse_file_info fi;
        toCFileInfo(request->fileinfo(), &fi);
        if(LOG) std::cout << "Truncate : FH received - " << fi.fh << "\n";

        if (fi.fh != 0) {
          res = ftruncate(fi.fh, request->size());
        } else {
          res = truncate(path, request->size());
        }

        if (res == -1) {
          if(LOG) std::cout << "Truncate : Error file not opened. -  " << errno <<"\n";
          response->set_status(-errno);
        }
        if(LOG) std::cout <<"------------------------------------------------\n\n";
        response->set_status(0);
        *response->mutable_fileinfo() = toGFileInfo(&fi);
        return Status::OK;
  }

  Status GrpcServiceImpl::Release(ServerContext* context, const ReleaseRequestObject* request, 
            ReleaseResponseObject* response) {

      if(LOG) std::cout <<"------------------------------------------------\n";
      if(LOG) std::cout << "Release : path passed - " << request->path() << "\n";
      std::string adjustedPath = mountpoint + request->path();
      char *path =new char[adjustedPath.length()+1];
      strcpy(path, adjustedPath.c_str());
      struct fuse_file_info fi;
      toCFileInfo(request->fileinfo(), &fi);
      if(LOG) std::cout << "Release : FH received - " << fi.fh << "\n";

      (void) path;
      close(fi.fh);
      if(LOG) std::cout << "Release : FH closed, fh - " << fi.fh << "\n";
      response->set_status(0);
      *response->mutable_fileinfo() = toGFileInfo(&fi);
      if(LOG) std::cout <<"------------------------------------------------\n\n";
      return Status::OK;
  }

  Status GrpcServiceImpl::Fsync(ServerContext* context, const FsyncRequestObject* request, 
            FsyncResponseObject* response) {

      if(LOG) std::cout <<"------------------------------------------------\n";
      if(LOG) std::cout << "Fsync : path passed - " << request->path() << "\n";
      std::string adjustedPath = mountpoint + request->path();
      char *path =new char[adjustedPath.length()+1];
      strcpy(path, adjustedPath.c_str());
      int isdatasync = request->isdatasync();
      struct fuse_file_info fi;
      toCFileInfo(request->fileinfo(), &fi);

      (void) path;
      (void) isdatasync;
      (void) fi;
      response->set_status(0);
      *response->mutable_fileinfo() = toGFileInfo(&fi);
      if(LOG) std::cout <<"------------------------------------------------\n\n";
      return Status::OK;
  }

  Status GrpcServiceImpl::Unlink(ServerContext* context, const UnlinkRequestObject* request, 
            UnlinkResponseObject* response) {
      
      if(LOG) std::cout <<"------------------------------------------------\n";
      if(LOG) std::cout << "Unlink : path passed - " << request->path() << "\n";
      std::string adjustedPath = mountpoint + request->path();
      char *path =new char[adjustedPath.length()+1];
      strcpy(path, adjustedPath.c_str());
    
      int res = unlink(path);

      if (res == -1) {
        response->set_status(-errno);
      }
      if(LOG) std::cout <<"------------------------------------------------\n\n";
      response->set_status(0);
      return Status::OK;
  }

  Status GrpcServiceImpl::Flush(ServerContext* context, const FlushRequestObject* request, 
            FlushResponseObject* response) {

      if(LOG) std::cout <<"------------------------------------------------\n";
      if(LOG) std::cout << "Flush : path passed - " << request->path() << "\n";
      std::string adjustedPath = mountpoint + request->path();
      char *path =new char[adjustedPath.length()+1];
      strcpy(path, adjustedPath.c_str());
      struct fuse_file_info fi;
      toCFileInfo(request->fileinfo(), &fi);
      if(LOG) std::cout << "Flush : FH received - " << fi.fh << "\n";

      (void) path;
      int res = close(dup(fi.fh));
      response->set_status(0);
      if (res == -1) {
        response->set_status(-errno);
      }
      if(LOG) std::cout << "Flush : FH closed, fh - " << fi.fh << "\n";
      *response->mutable_fileinfo() = toGFileInfo(&fi);
      if(LOG) std::cout <<"------------------------------------------------\n\n";
      return Status::OK;
  }

  Status GrpcServiceImpl::Utimens(ServerContext* context, const UtimensRequestObject* request, 
              UtimensResponseObject* response) {

      std::string adjustedPath = mountpoint + request->path();
      char *path =new char[adjustedPath.length()+1];
      strcpy(path, adjustedPath.c_str());
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

  void GrpcServiceImpl::setMountPoint(std::string mountpoint) {
    GrpcServiceImpl::mountpoint = mountpoint;
  }
// };

