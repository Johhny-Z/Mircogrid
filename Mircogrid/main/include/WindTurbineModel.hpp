#ifndef WIND_TURBINE_MODEL_HPP
#define WIND_TURBINE_MODEL_HPP

#include <iostream>
#include "cadmium/modeling/devs/atomic.hpp"

using namespace cadmium;

struct WindTurbineState {
    double sigma;           // 时间推进值
    double power_output;    // 当前输出功率（kW）

    explicit WindTurbineState(): sigma(1.0), power_output(0.0) {}
};

#ifndef NO_LOGGING
std::ostream& operator<<(std::ostream &out, const WindTurbineState& state) {
    out << "{power_output: " << state.power_output << " kW, sigma: " << state.sigma << "}";
    return out;
}
#endif

class WindTurbineModel : public Atomic<WindTurbineState> {
public:
    // 输入输出端口声明
    cadmium::Port<double> wind_speed_in;  // 输入端口，接收风速（m/s）
    cadmium::Port<double> power_out;      // 输出端口，输出当前发电功率

    // 构造函数，初始化端口
    WindTurbineModel(const std::string id) : Atomic<WindTurbineState>(id, WindTurbineState()) {
        wind_speed_in = addInPort<double>("wind_speed_in");  // 初始化输入端口
        power_out = addOutPort<double>("power_out");         // 初始化输出端口
    }

    // 内部转移函数
    void internalTransition(WindTurbineState& state) const override {
        state.sigma = 1.0;  // 重置时间推进值
    }

    // 外部转移函数
    void externalTransition(WindTurbineState& state, double e) const override {
        if (!wind_speed_in->empty()) {
            double wind_speed = wind_speed_in->getBag().back();  // 获取风速
            state.power_output = wind_speed * 5.0;  // 假设风力发电效率为5 kW/(m/s)
        }
        state.sigma -= e;  // 更新剩余时间
    }

    // 输出函数
    void output(const WindTurbineState& state) const override {
        power_out->addMessage(state.power_output);  // 输出当前发电功率
    }

    // 时间推进函数
    [[nodiscard]] double timeAdvance(const WindTurbineState& state) const override {
        return state.sigma;  // 返回剩余时间
    }
};

#endif