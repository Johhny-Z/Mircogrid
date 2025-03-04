#ifndef TOP_COUPLED_MODEL_HPP
#define TOP_COUPLED_MODEL_HPP

#include <cadmium/modeling/devs/coupled.hpp>
#include "RenewableEnergyCoupledModel.hpp"
#include "LoadModel.hpp"
#include "BatteryModel.hpp"

using namespace cadmium;

struct TopCoupledModel : public Coupled {
    // 输入输出端口声明
    cadmium::Port<double> irradiance_in;  // 输入端口，接收光照强度
    cadmium::Port<double> wind_speed_in;  // 输入端口，接收风速
    cadmium::Port<double> demand_in;      // 输入端口，接收负载需求
    cadmium::Port<double> battery_soc_out; // 输出端口，输出电池的SOC

    // 构造函数，初始化端口和模型
    TopCoupledModel(const std::string id) : Coupled(id) {
        // 初始化输入输出端口
        irradiance_in = addInPort<double>("irradiance_in");
        wind_speed_in = addInPort<double>("wind_speed_in");
        demand_in = addInPort<double>("demand_in");
        battery_soc_out = addOutPort<double>("battery_soc_out");

        // 添加耦合模型和原子模型
        auto renewable_energy_model = addComponent<RenewableEnergyCoupledModel>("renewable_energy_model");
        auto battery_model = addComponent<BatteryModel>("battery_model");
        auto load_model = addComponent<LoadModel>("load_model");

        // 定义内部耦合
        addCoupling(irradiance_in, renewable_energy_model->irradiance_in);  // 光照强度输入到可再生能源模型
        addCoupling(wind_speed_in, renewable_energy_model->wind_speed_in);  // 风速输入到可再生能源模型
        addCoupling(demand_in, load_model->demand_in);                      // 负载需求输入到负载模型

        // 可再生能源模型输出到电池和负载
        addCoupling(renewable_energy_model->power_out, battery_model->power_in);
        addCoupling(renewable_energy_model->power_out, load_model->demand_in);

        // 电池输出到负载
        addCoupling(battery_model->soc_out, load_model->demand_in);

        // 电池SOC输出到外部端口
        addCoupling(battery_model->soc_out, battery_soc_out);
    }
};

#endif