#pragma once

#include <iostream>
#include <thread>
#include <grpcpp/grpcpp.h>
#include "service.grpc.pb.h"  // 自动生成的 gRPC 头文件

class AServiceStub {
public:
    AServiceStub(std::shared_ptr<grpc::Channel> channel) 
        : stub_(AService::NewStub(channel)) {
        std::cout << "AServiceStub created" << std::endl;
        worker_thread_ = std::thread(&AServiceStub::RunService, this);
    }

    ~AServiceStub() {
        std::cout << "AServiceStub Destructor" << std::endl;
        if (worker_thread_.joinable()) {
            worker_thread_.join();  // 等待线程结束
        }
    }

    void RunService() {
        // 模拟发送 gRPC 请求
        grpc::ClientContext context;
        Request request;
        Response response;

        request.set_message("Hello, AService!");

        grpc::Status status = stub_->CallService(&context, request, &response);
        if (status.ok()) {
            std::cout << "Response from AService: " << response.message() << std::endl;
        } else {
            std::cerr << "gRPC call failed!" << std::endl;
        }
    }

private:
    std::unique_ptr<AService::Stub> stub_;
    std::thread worker_thread_;
};
