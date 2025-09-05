
#include <memory>
#include <iostream>
#include "AServiceStub.hpp"  // 包含 AServiceStub 的声明

class ClientAService {
public:
    static ClientAService& Instance() {
        static ClientAService instance;
        return instance;
    }

    void StartService() {
        service_stub_ = std::make_unique<AServiceStub>(
            grpc::CreateChannel("localhost:50051", grpc::InsecureChannelCredentials())
        );
    }

    void StopService() {
        service_stub_.reset();  // 销毁 AServiceStub
    }

private:
    std::unique_ptr<AServiceStub> service_stub_;
};

int main() {
    ClientAService& client = ClientAService::Instance();
    client.StartService();

    // 模拟业务逻辑
    std::this_thread::sleep_for(std::chrono::seconds(3));

    client.StopService();  // 停止服务并销毁 AServiceStub
    return 0;
}
