#include <src/client/GrpcClient.h>

GrpcClient::GrpcClient(std::shared_ptr<Channel> channel)
      : stub_(Grpc::NewStub(channel)) {}

int GrpcClient::getAttributes(std::string path, struct stat *st){
	// Container request
	GetAttributesRequestObject getAttributesRequestObject;
	getAttributesRequestObject.set_path(path);
	getAttributesRequestObject.set_st(toGstat(st));
	ClientContext context;

	// Container response
	GetAttributesResponseObject getAttributesResponseObject;

	// Actual call
	Status status = stub_->GetAttributes(&context, getAttributesRequestObject, &getAttributesResponseObject)

	if(status.ok()){
		toCstat(getAttributesResponseObject.st(), st);
		return getAttributesResponseObject.status();
	}
	else {
		std::cout << status.error_code() << ": " << status.error_message()
              << std::endl;
    return "RPC failed";
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
	Status status = stub_->ReadDirectory(context, readDirectoryRequestObject, &readDirectoryResponseObject);
	
	if(status.ok()){
		std::list<DirEntry>;
		responseCode = readDirectoryResponseObject.status();
		for(int i=0; i<readDirectoryResponseObject.objects_size(); i++) {

		}
		return NULL;
	}
	else {
		std::cout << status.error_code() << ": " << status.error_message()
              << std::endl;
    return -1;
	}
}

int GrpcClient::read(std::string path, int offset, int size){
	// Container request
	ReadRequestObject readRequestObject;
	readRequestObject.set_path(path);
	readRequestObject.set_offset(offset);
	readRequestObject.set_size(size);
	ClientContext context;

	// Container response
	ReadResponseObject readResponseObject;

	// Call
	Status status = stub_->Read(context, readRequestObject, readResponseObject)

	return 1;

}