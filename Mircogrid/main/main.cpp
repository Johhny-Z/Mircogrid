#include <cadmium/simulation/root_coordinator.hpp>  // 包含根协调器头文件
#include <cadmium/simulation/logger/csv.hpp>        // 包含CSV日志记录器头文件
#include "TopCoupledModel.hpp"                      // 包含顶层耦合模型头文件

using namespace cadmium;

int main() {
    // 创建顶层耦合模型指针
    auto top_model = std::make_shared<TopCoupledModel>("top_model");

    // 初始化根协调器
    auto rootCoordinator = cadmium::RootCoordinator(top_model);

    // 设置日志记录器
    rootCoordinator.setLogger<cadmium::CSVLogger>("microgrid_log.csv", ";");

    // 启动仿真
    rootCoordinator.start();
    rootCoordinator.simulate(24.0);  // 仿真24小时
    rootCoordinator.stop();

    return 0;}
