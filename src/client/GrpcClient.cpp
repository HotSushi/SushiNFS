#include <src/client/GrpcClient.h>
#define LOG false

GrpcClient::GrpcClient(std::shared_ptr<Channel> channel)
      : stub_(Grpc::NewStub(channel)) {}

static std::map<int, Datastore> mainDataStore;

static int bufferLimit = 65536;

int GrpcClient::getAttributes(std::string path, struct stat *st){

	if(LOG) std::cout <<"------------------------------------------------\n";
    if(LOG) std::cout << "getAttributes : path passed - " << path << "\n";
	// Container request
	GetAttributesRequestObject getAttributesRequestObject;
	getAttributesRequestObject.set_path(path);
	*getAttributesRequestObject.mutable_st() = toGstat(st);
	ClientContext context;

	// Container response
	GetAttributesResponseObject getAttributesResponseObject;
	if(LOG) std::cout << "getAttributes : Calling server \n";
	// Actual call
	Status status = stub_->GetAttributes(&context, getAttributesRequestObject, &getAttributesResponseObject);
	if(LOG) std::cout << "getAttributes : Response from server \n";
	if(status.ok()){
		if(LOG) std::cout << "getAttributes : converting response \n";
		toCstat(getAttributesResponseObject.st(), st);
		if(LOG) std::cout << "getAttributes : returning resposne \n";
		return getAttributesResponseObject.status();
	}
	else {
		if(LOG) std::cout << "getAttributes : Failed \n";
		if(LOG) std::cout << status.error_code() << ": " << status.error_message()
              << std::endl;

    if(LOG) std::cout <<"------------------------------------------------\n\n";
    return -1;
	}
}

std::list<DirEntry> GrpcClient::readDirectory(std::string path, int &responseCode){
	// Container request
	ReadDirectoryRequestObject readDirectoryRequestObject;
	readDirectoryRequestObject.set_path(path);
	ClientContext context;

	// Container response
	ReadDirectoryResponseObject readDirectoryResponseObject;

	// Call
	Status status = stub_->ReadDirectory(&context, readDirectoryRequestObject, &readDirectoryResponseObject);
	std::list<DirEntry> entries;
	if(status.ok()){
		responseCode = readDirectoryResponseObject.status();
		for(int i=0; i<readDirectoryResponseObject.objects_size(); i++) {
			DirEntry dirEntry;
			toCstat(readDirectoryResponseObject.objects(i).st(),&dirEntry.st);
			dirEntry.name = readDirectoryResponseObject.objects(i).name();
			entries.push_back(dirEntry);
		}
		return entries;
	}
	else {
		if(LOG) std::cout << status.error_code() << ": " << status.error_message()
              << std::endl;
    return entries;
	}
}

int GrpcClient::read(std::string path, char* buffer, int offset, int size, struct fuse_file_info *fi){
	// Container request
	ReadRequestObject readRequestObject;
	readRequestObject.set_path(path);
	readRequestObject.set_offset(offset);
	readRequestObject.set_size(size);
	*readRequestObject.mutable_fileinfo() = toGFileInfo(fi);
	ClientContext context;

	// Container response
	ReadResponseObject readResponseObject;

	// Call
	Status status = stub_->Read(&context, readRequestObject, &readResponseObject);

	toCFileInfo(readResponseObject.fileinfo(), fi);

	if(status.ok()){
		// bcopy(readResponseObject.data().c_str(), buffer, readResponseObject.data().length());
		strncpy(buffer, readResponseObject.data().c_str(), size);
		return readResponseObject.size();
	}
	else {
		if(LOG) std::cout << status.error_code() << ": " << status.error_message()
              << std::endl;
        return -1;
	}

}

int GrpcClient::makeNode(std::string path, mode_t mode, dev_t rdev){
	MknodRequestObject mknodRequestObject;
	mknodRequestObject.set_path(path);
	mknodRequestObject.set_mode(mode);
	mknodRequestObject.set_rdev(rdev);
	ClientContext context;

		// Container response
	MknodResponseObject mknodResponseObject;

	// Call
	Status status = stub_->Mknod(&context, mknodRequestObject, &mknodResponseObject);

	if(status.ok()){
		return mknodResponseObject.status();
	}
	else {
		if(LOG) std::cout << status.error_code() << ": " << status.error_message()
              << std::endl;
    return -1;
	}
}


int GrpcClient::makeDir(std::string path, mode_t mode){
	MkDirRequestObject mkDirRequestObject;
	mkDirRequestObject.set_path(path);
	mkDirRequestObject.set_mode(mode);
	ClientContext context;

	// Container response
	MkDirResponseObject mkDirResponseObject;

	// Call
	Status status = stub_->MkDir(&context, mkDirRequestObject, &mkDirResponseObject);

	if(status.ok()){
		return mkDirResponseObject.status();
	}
	else {
		if(LOG) std::cout << status.error_code() << ": " << status.error_message()
              << std::endl;
    return -1;
	}
}

int GrpcClient::rmDir(std::string path){
	RmDirRequestObject rmDirRequestObject;
	rmDirRequestObject.set_path(path);
	ClientContext context;

	// Container response
	RmDirResponseObject rmDirResponseObject;

	// Call
	Status status = stub_->RmDir(&context, rmDirRequestObject, &rmDirResponseObject);

	if(status.ok()){
		return rmDirResponseObject.status();
	}
	else {
		if(LOG) std::cout << status.error_code() << ": " << status.error_message()
              << std::endl;
    return -1;
	}
}

int GrpcClient::rename(std::string from, std::string to){
	RenameRequestObject renameRequestObject;
	renameRequestObject.set_from(from);
	renameRequestObject.set_to(to);
	ClientContext context;

	// Container response
	RenameResponseObject renameResponseObject;

	// Call
	Status status = stub_->Rename(&context, renameRequestObject, &renameResponseObject);

	if(status.ok()){
		return renameResponseObject.status();
	}
	else {
		if(LOG) std::cout << status.error_code() << ": " << status.error_message()
              << std::endl;
    return -1;
	}
}

int GrpcClient::truncate(std::string path, off_t size, struct fuse_file_info *fi){
	TruncateRequestObject truncateRequestObject;
	truncateRequestObject.set_path(path);
	truncateRequestObject.set_size(size);
	*truncateRequestObject.mutable_fileinfo() = toGFileInfo(fi);
	ClientContext context;

	// Container response
	TruncateResponseObject truncateResponseObject;

	// Call
	Status status = stub_->Truncate(&context, truncateRequestObject, &truncateResponseObject);

	toCFileInfo(truncateResponseObject.fileinfo(), fi);

	if(status.ok()){
		return truncateResponseObject.status();
	}
	else {
		if(LOG) std::cout << status.error_code() << ": " << status.error_message()
              << std::endl;
    return -1;
	}
}

int GrpcClient::create(std::string path, mode_t mode, struct fuse_file_info *fi){
	CreateRequestObject createRequestObject;
	createRequestObject.set_path(path);
	createRequestObject.set_mode(mode);
	*createRequestObject.mutable_fileinfo() = toGFileInfo(fi);
	ClientContext context;

	// Container response
	CreateResponseObject createResponseObject;

	// Call
	Status status = stub_->Create(&context, createRequestObject, &createResponseObject);
	if(LOG) std::cout <<" Success creating file \n";
	toCFileInfo(createResponseObject.fileinfo(), fi);
	if(LOG) std::cout <<" Success storing response object \n";
	if(status.ok()){
		return createResponseObject.status();
	}
	else {
		if(LOG) std::cout << status.error_code() << ": " << status.error_message()
              << std::endl;
    return -1;
	}
}

int GrpcClient::open(std::string path, struct fuse_file_info *fi){
	OpenRequestObject openRequestObject;
	openRequestObject.set_path(path);
	*openRequestObject.mutable_fileinfo() = toGFileInfo(fi);
	ClientContext context;

	// Container response
	OpenResponseObject openResponseObject;

	// Call
	Status status = stub_->Open(&context, openRequestObject, &openResponseObject);

	toCFileInfo(openResponseObject.fileinfo(), fi);

	if(status.ok()){
		return openResponseObject.status();
	}
	else {
		if(LOG) std::cout << status.error_code() << ": " << status.error_message()
              << std::endl;
    return -1;
	}
}

int GrpcClient::release(std::string path, struct fuse_file_info *fi){
	ReleaseRequestObject releaseRequestObject;
	releaseRequestObject.set_path(path);
	*releaseRequestObject.mutable_fileinfo() = toGFileInfo(fi);
	ClientContext context;

	// Container response
	ReleaseResponseObject releaseResponseObject;

	// Call
	Status status = stub_->Release(&context, releaseRequestObject, &releaseResponseObject);

	toCFileInfo(releaseResponseObject.fileinfo(), fi);

	if(status.ok()){
		mainDataStore.clear();
		return releaseResponseObject.status();
	}
	else {
		if(LOG) std::cout << status.error_code() << ": " << status.error_message()
              << std::endl;
    return -1;
	}
}

int GrpcClient::fsync(std::string path, int isdatasync, struct fuse_file_info* fi){
	FsyncRequestObject fsyncRequestObject;
	fsyncRequestObject.set_path(path);
	fsyncRequestObject.set_isdatasync(isdatasync);
	*fsyncRequestObject.mutable_fileinfo() = toGFileInfo(fi);
	ClientContext context;

	// Container response
	FsyncResponseObject fsyncResponseObject;

	// Call
	Status status = stub_->Fsync(&context, fsyncRequestObject, &fsyncResponseObject);

	toCFileInfo(fsyncResponseObject.fileinfo(), fi);

	if(status.ok()){
		return fsyncResponseObject.status();
	}
	else {
		if(LOG) std::cout << status.error_code() << ": " << status.error_message()
              << std::endl;
    return -1;
	}
}

int GrpcClient::unlink(std::string path){
	UnlinkRequestObject unlinkRequestObject;
	unlinkRequestObject.set_path(path);
	ClientContext context;

	// Container response
	UnlinkResponseObject unlinkResponseObject;

	// Call
	Status status = stub_->Unlink(&context, unlinkRequestObject, &unlinkResponseObject);

	if(status.ok()){
		return unlinkResponseObject.status();
	}
	else {
		if(LOG) std::cout << status.error_code() << ": " << status.error_message()
              << std::endl;
    return -1;
	}
}

int GrpcClient::write(std::string path, const char *buf, int size, int offset, struct fuse_file_info* fi)
{

	WriteRequestObject writeRequestObject;
	if(LOG) std::cout <<"------------------------------------------------\n";
    if(LOG) std::cout << "write : path passed - " << path << "\n";
    if(LOG) std::cout << "write : data size is - " << strlen(buf) << "\n";
	
	bool commitFlag = false;
      if (fi->fh != 0) {
        if (mainDataStore.find(fi->fh) != mainDataStore.end()) {
            Datastore ds = mainDataStore.find(fi->fh)->second;
            if (ds.getData().length() > bufferLimit) {
              commitFlag = true;
            }
        }
      }


	writeRequestObject.set_flag(commitFlag);
	writeRequestObject.set_path(path);
	writeRequestObject.set_data(buf);
	writeRequestObject.set_offset(offset);
	writeRequestObject.set_size(size);
	*writeRequestObject.mutable_fileinfo() = toGFileInfo(fi);
	
	ClientContext context;

	// Container response
	WriteResponseObject writeResponseObject;

	// Call
	Status status = stub_->Write(&context, writeRequestObject, &writeResponseObject);

	toCFileInfo(writeResponseObject.fileinfo(), fi);

	if(status.ok()){
		if (commitFlag) {
			if (mainDataStore.find(fi->fh) != mainDataStore.end()) {
	            Datastore ds = mainDataStore.find(fi->fh)->second;
	            ds.setValues("",0,false);
	            mainDataStore.find(fi->fh)->second = ds;
        	}
		} else {
			std::string temp2(buf);
			if (mainDataStore.find(fi->fh) != mainDataStore.end()) {
	            Datastore ds = mainDataStore.find(fi->fh)->second;
	            ds.setValues((ds.getData() + temp2),ds.getOriginalOffset());
	            mainDataStore.find(fi->fh)->second = ds;
        	} else {
        		mainDataStore.insert(std::make_pair<int, Datastore>(fi->fh,Datastore(temp2, offset, false)));
        	}
		}
		return writeResponseObject.datasize();
	} else {

		if(LOG) std::cout << status.error_code() << ": " << status.error_message()
              << std::endl;
    return -1;
	}
	if(LOG) std::cout <<"------------------------------------------------\n\n";
}

int GrpcClient::flush(std::string path, struct fuse_file_info *fi){
	FlushRequestObject flushRequestObject;
	flushRequestObject.set_path(path);
	*flushRequestObject.mutable_fileinfo() = toGFileInfo(fi);
	ClientContext context;

	// Container response
	FlushResponseObject flushResponseObject;

	// Call
	Status status = stub_->Flush(&context, flushRequestObject, &flushResponseObject);

	toCFileInfo(flushResponseObject.fileinfo(), fi);

	if(status.ok()){
		return flushResponseObject.status();
		
	}
	else {
		if(LOG) std::cout << status.error_code() << ": " << status.error_message()
              << std::endl;
    return -1;
	}
}

int GrpcClient::utimens(std::string path,const struct timespec *ts, struct fuse_file_info* fi)
{
	UtimensRequestObject utimensRequestObject;
	utimensRequestObject.set_path(path);
	*utimensRequestObject.mutable_timespec() = toGTimeSpec(ts);
	// *utimensRequestObject.mutable_fileinfo() = toGFileInfo(fi);
	
	ClientContext context;

	// Container response
	UtimensResponseObject utimensResponseObject;

	// Call
	Status status = stub_->Utimens(&context, utimensRequestObject, &utimensResponseObject);

	// toCFileInfo(utimensResponseObject.fileinfo(), fi);

	if(status.ok()){
		return utimensResponseObject.status();
	}
	else {
		if(LOG) std::cout << status.error_code() << ": " << status.error_message()
              << std::endl;
    return -1;
	}
}
	
