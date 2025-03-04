#ifndef RENEWABLE_ENERGY_COUPLED_MODEL_HPP
#define RENEWABLE_ENERGY_COUPLED_MODEL_HPP

#include <cadmium/modeling/devs/coupled.hpp>
#include "PVModel.hpp"
#include "WindTurbineModel.hpp"

using namespace cadmium;

struct RenewableEnergyCoupledModel : public Coupled {
    // 输入输出端口声明
    cadmium::Port<double> irradiance_in;  // 输入端口，接收光照强度
    cadmium::Port<double> wind_speed_in;  // 输入端口，接收风速
    cadmium::Port<double> power_out;      // 输出端口，输出总发电功率

    // 构造函数，初始化端口和模型
    RenewableEnergyCoupledModel(const std::string id) : Coupled(id) {
        // 初始化输入输出端口
        irradiance_in = addInPort<double>("irradiance_in");
        wind_speed_in = addInPort<double>("wind_speed_in");
        power_out = addOutPort<double>("power_out");

        // 添加原子模型
        auto pv_model = addComponent<PVModel>("pv_model");
        auto wind_turbine_model = addComponent<WindTurbineModel>("wind_turbine_model");

        // 定义内部耦合
        addCoupling(irradiance_in, pv_model->irradiance_in);        // 光照强度输入到光伏系统
        addCoupling(wind_speed_in, wind_turbine_model->wind_speed_in);  // 风速输入到风力发电机

        // 光伏和风力发电机输出到总发电功率端口
        addCoupling(pv_model->power_out, power_out);
        addCoupling(wind_turbine_model->power_out, power_out);
    }
};

#endif