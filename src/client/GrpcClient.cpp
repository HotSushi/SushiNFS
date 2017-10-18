#include <src/client/GrpcClient.h>

GrpcClient::GrpcClient(std::shared_ptr<Channel> channel)
      : stub_(Grpc::NewStub(channel)) {}

int GrpcClient::getAttributes(std::string path, struct stat *st){
	// Container request
	GetAttributesRequestObject getAttributesRequestObject;
	getAttributesRequestObject.set_path(path);
	*getAttributesRequestObject.mutable_st() = toGstat(st);
	ClientContext context;

	// Container response
	GetAttributesResponseObject getAttributesResponseObject;

	// Actual call
	Status status = stub_->GetAttributes(&context, getAttributesRequestObject, &getAttributesResponseObject);

	if(status.ok()){
		toCstat(getAttributesResponseObject.st(), st);
		return getAttributesResponseObject.status();
	}
	else {
		std::cout << status.error_code() << ": " << status.error_message()
              << std::endl;
    return -1;
	}
}

std::list<DirEntry> GrpcClient::readDirectory(std::string path, int &responseCode,  struct fuse_file_info* result){
	// Container request
	ReadDirectoryRequestObject readDirectoryRequestObject;
	readDirectoryRequestObject.set_path(path);
	*readDirectoryRequestObject.mutable_fileinfo() = toGFileInfo(result);
	ClientContext context;

	// Container response
	ReadDirectoryResponseObject readDirectoryResponseObject;

	// Call
	Status status = stub_->ReadDirectory(&context, readDirectoryRequestObject, &readDirectoryResponseObject);
	toCFileInfo(readDirectoryResponseObject.fileinfo(), result);
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
		std::cout << status.error_code() << ": " << status.error_message()
              << std::endl;
    return entries;
	}
}

std::string GrpcClient::read(std::string path, int offset, int size){
	// Container request
	ReadRequestObject readRequestObject;
	readRequestObject.set_path(path);
	readRequestObject.set_offset(offset);
	readRequestObject.set_size(size);
	ClientContext context;

	// Container response
	ReadResponseObject readResponseObject;

	// Call
	Status status = stub_->Read(&context, readRequestObject, &readResponseObject);

	if(status.ok()){
		return readResponseObject.data();
	}
	else {
		std::cout << status.error_code() << ": " << status.error_message()
              << std::endl;
    return "RPC FAILED";
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
		std::cout << status.error_code() << ": " << status.error_message()
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
		std::cout << status.error_code() << ": " << status.error_message()
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
		std::cout << status.error_code() << ": " << status.error_message()
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
		std::cout << status.error_code() << ": " << status.error_message()
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
		std::cout << status.error_code() << ": " << status.error_message()
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

	toCFileInfo(createResponseObject.fileinfo(), fi);

	if(status.ok()){
		return createResponseObject.status();
	}
	else {
		std::cout << status.error_code() << ": " << status.error_message()
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
		std::cout << status.error_code() << ": " << status.error_message()
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
		return releaseResponseObject.status();
	}
	else {
		std::cout << status.error_code() << ": " << status.error_message()
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
		std::cout << status.error_code() << ": " << status.error_message()
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
		std::cout << status.error_code() << ": " << status.error_message()
              << std::endl;
    return -1;
	}
}

int GrpcClient::write(std::string path, std::string buffer, int size, int offset, struct fuse_file_info* fi)
{
	WriteRequestObject writeRequestObject;
	writeRequestObject.set_path(path);
	writeRequestObject.set_data(buffer);
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
		return writeResponseObject.status();
	}
	else {
		std::cout << status.error_code() << ": " << status.error_message()
              << std::endl;
    return -1;
	}
}

int GrpcClient::utimens(std::string path,const struct timespec *ts, struct fuse_file_info* fi)
{
	UtimensRequestObject utimensRequestObject;
	utimensRequestObject.set_path(path);
	*utimensRequestObject.mutable_timespec() = toGTimeSpec(ts);
	*utimensRequestObject.mutable_fileinfo() = toGFileInfo(fi);
	
	ClientContext context;

	// Container response
	UtimensResponseObject utimensResponseObject;

	// Call
	Status status = stub_->Utimens(&context, utimensRequestObject, &utimensResponseObject);

	toCFileInfo(utimensResponseObject.fileinfo(), fi);

	if(status.ok()){
		return utimensResponseObject.status();
	}
	else {
		std::cout << status.error_code() << ": " << status.error_message()
              << std::endl;
    return -1;
	}
}
	
int GrpcClient::lookup(struct fuse_file_info* base, std::string component, struct fuse_file_info* result)
{
	LookUpRequestObject lookUpRequestObject;
	*lookUpRequestObject.mutable_fileinfo() = toGFileInfo(base);
	lookUpRequestObject.set_component(component);

	ClientContext context;

	// Container response
	LookUpResponseObject lookUpResponseObject;

	// Call
	Status status = stub_->LookUp(&context, lookUpRequestObject, &lookUpResponseObject);

	toCFileInfo(lookUpResponseObject.fileinfo(), result);
	if(status.ok()){
		return lookUpResponseObject.status();
	}
	else {
		std::cout << status.error_code() << ": " << status.error_message()
              << std::endl;
    return -1;
	}
}


