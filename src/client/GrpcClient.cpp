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
