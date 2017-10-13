#
# Copyright 2015 gRPC authors.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#

HOST_SYSTEM = $(shell uname | cut -f 1 -d_)
SYSTEM ?= $(HOST_SYSTEM)
GRPCSOURCEPATH = ${GRPC_SOURCE}
CXX = g++
CPPFLAGS += -I$(GRPCSOURCEPATH)third_party/protobuf/src -I. -pthread `pkg-config --cflags grpc`\
             `pkg-config fuse3 --cflags --libs`
CXXFLAGS += -std=c++11
ifeq ($(SYSTEM),Darwin)
LDFLAGS += -L/usr/local/lib -L$(GRPCSOURCEPATH)libs/opt/protobuf `pkg-config --libs grpc++ grpc`\
            `pkg-config fuse3 --cflags --libs`\
           -lgrpc++_reflection\
           -ldl
else
LDFLAGS += -L/usr/local/lib -L$(GRPCSOURCEPATH)libs/opt/protobuf `pkg-config --libs grpc++ grpc`\
		    `pkg-config fuse3 --cflags --libs`\
           -Wl,--no-as-needed -lgrpc++_reflection -Wl,--as-needed\
           -ldl -std=c++11
endif
MKDIR_P = mkdir -p
PROTOC = protoc
GRPC_CPP_PLUGIN = grpc_cpp_plugin
GRPC_CPP_PLUGIN_PATH ?= `which $(GRPC_CPP_PLUGIN)`

PROTOS_PATH = src/protos
SOURCE_PATH = src
BIN_PATH = bin
BUILD_PATH = build

vpath %.proto $(PROTOS_PATH)

.PHONY: directories googlerpc

all: directories system-check $(BIN_PATH)/Client #$(BIN_PATH)/HelloClient $(BIN_PATH)/HelloServer 

directories: bin_dir build_dir

#googlerpc: $(BUILD_PATH)/GRPC.grpc.pb.cc $(BUILD_PATH)/GRPC.pb.cc $(BUILD_PATH)/GeneralHelpers.o $(BUILD_PATH)/GrpcClient.o

$(BIN_PATH)/Client: $(BUILD_PATH)/GRPC.pb.o $(BUILD_PATH)/GRPC.grpc.pb.o $(BUILD_PATH)/GeneralHelpers.o $(BUILD_PATH)/GrpcClient.o $(BUILD_PATH)/Client.o
	$(CXX) $^ $(LDFLAGS) -o $@

$(BUILD_PATH)/Client.o: $(SOURCE_PATH)/client/Client.cpp
	$(CXX) $(CXXFLAGS) $(CPPFLAGS)  -c -o $(BUILD_PATH)/Client.o $(SOURCE_PATH)/client/Client.cpp

$(BIN_PATH)/Server: $(BUILD_PATH)/GRPC.pb.o $(BUILD_PATH)/GRPC.grpc.pb.o $(BUILD_PATH)/GeneralHelpers.o $(BUILD_PATH)/GrpcServer.o $(BUILD_PATH)/Server.o
	$(CXX) $^ $(LDFLAGS) -o $@

$(BUILD_PATH)/Server.o: $(SOURCE_PATH)/server/Server.cpp
	$(CXX) $(CXXFLAGS) $(CPPFLAGS)  -c -o $(BUILD_PATH)/Server.o $(SOURCE_PATH)/server/Server.cpp

$(BUILD_PATH)/GeneralHelpers.o: $(SOURCE_PATH)/helpers/GeneralHelpers.h $(SOURCE_PATH)/helpers/GeneralHelpers.cpp $(BUILD_PATH)/GRPC.grpc.pb.cc
	$(CXX) $(CXXFLAGS) $(CPPFLAGS)  -c -o $(BUILD_PATH)/GeneralHelpers.o $(SOURCE_PATH)/helpers/GeneralHelpers.cpp

$(BUILD_PATH)/GrpcClient.o: $(SOURCE_PATH)/client/GrpcClient.cpp $(SOURCE_PATH)/client/GrpcClient.h $(BUILD_PATH)/GRPC.grpc.pb.cc
	$(CXX) $(CXXFLAGS) $(CPPFLAGS)  -c -o $(BUILD_PATH)/GrpcClient.o $(SOURCE_PATH)/client/GrpcClient.cpp

$(BUILD_PATH)/GrpcServer.o: $(SOURCE_PATH)/server/GrpcServer.cpp $(SOURCE_PATH)/server/GrpcServer.h $(BUILD_PATH)/GRPC.grpc.pb.cc
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -c -o $(BUILD_PATH)/GrpcServer.o $(SOURCE_PATH)/server/GrpcServer.cpp

$(BIN_PATH)/HelloClient: $(BUILD_PATH)/HelloWorld.pb.o $(BUILD_PATH)/HelloWorld.grpc.pb.o $(BUILD_PATH)/HelloClient.o
	$(CXX) $^ $(LDFLAGS) -o $@

$(BIN_PATH)/HelloServer: $(BUILD_PATH)/HelloWorld.pb.o $(BUILD_PATH)/HelloWorld.grpc.pb.o $(BUILD_PATH)/HelloServer.o
	$(CXX) $^ $(LDFLAGS) -o $@

$(BUILD_PATH)/HelloClient.o: $(SOURCE_PATH)/client/HelloClient.cpp
	$(CXX) $(CXXFLAGS) $(CPPFLAGS)  -c -o $(BUILD_PATH)/HelloClient.o $(SOURCE_PATH)/client/HelloClient.cpp

$(BUILD_PATH)/HelloServer.o: $(SOURCE_PATH)/server/HelloServer.cpp
	$(CXX) $(CXXFLAGS) $(CPPFLAGS)  -c -o $(BUILD_PATH)/HelloServer.o $(SOURCE_PATH)/server/HelloServer.cpp

$(BUILD_PATH)/%.grpc.pb.cc: %.proto
	$(PROTOC) -I $(PROTOS_PATH) --grpc_out=$(BUILD_PATH) --plugin=protoc-gen-grpc=$(GRPC_CPP_PLUGIN_PATH) $<

$(BUILD_PATH)/%.pb.cc: %.proto
	$(PROTOC) -I $(PROTOS_PATH) --cpp_out=$(BUILD_PATH) $<

bin_dir:
	${MKDIR_P} ${BIN_PATH}

build_dir:
	${MKDIR_P} ${BUILD_PATH}

clean:
	rm -rf ${BIN_PATH} ${BUILD_PATH}

# The following is to test your system and ensure a smoother experience.
# They are by no means necessary to actually compile a grpc-enabled software.

PROTOC_CMD = which $(PROTOC)
PROTOC_CHECK_CMD = $(PROTOC) --version | grep -q libprotoc.3
PLUGIN_CHECK_CMD = which $(GRPC_CPP_PLUGIN)
HAS_PROTOC = $(shell $(PROTOC_CMD) > /dev/null && echo true || echo false)
ifeq ($(HAS_PROTOC),true)
HAS_VALID_PROTOC = $(shell $(PROTOC_CHECK_CMD) 2> /dev/null && echo true || echo false)
endif
HAS_PLUGIN = $(shell $(PLUGIN_CHECK_CMD) > /dev/null && echo true || echo false)

SYSTEM_OK = false
ifeq ($(HAS_VALID_PROTOC),true)
ifeq ($(HAS_PLUGIN),true)
SYSTEM_OK = true
endif
endif

system-check:
ifneq ($(HAS_VALID_PROTOC),true)
	@echo " DEPENDENCY ERROR"
	@echo
	@echo "You don't have protoc 3.0.0 installed in your path."
	@echo "Please install Google protocol buffers 3.0.0 and its compiler."
	@echo "You can find it here:"
	@echo
	@echo "   https://github.com/google/protobuf/releases/tag/v3.0.0"
	@echo
	@echo "Here is what I get when trying to evaluate your version of protoc:"
	@echo
	-$(PROTOC) --version
	@echo
	@echo
endif
ifneq ($(HAS_PLUGIN),true)
	@echo " DEPENDENCY ERROR"
	@echo
	@echo "You don't have the grpc c++ protobuf plugin installed in your path."
	@echo "Please install grpc. You can find it here:"
	@echo
	@echo "   https://github.com/grpc/grpc"
	@echo
	@echo "Here is what I get when trying to detect if you have the plugin:"
	@echo
	-which $(GRPC_CPP_PLUGIN)
	@echo
	@echo
endif
ifneq ($(SYSTEM_OK),true)
	@false
endif
ifeq ($(GRPC_SOURCE),)
	@echo " DEPENDENCY ERROR"
	@echo 
	@echo "Please set the environment variable GRPC_SOURCE to built grpc source"
	@echo "directory."
	@echo
	@false
endif
