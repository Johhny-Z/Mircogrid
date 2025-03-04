#ifndef PV_MODEL_HPP
#define PV_MODEL_HPP

#include <iostream>
#include "cadmium/modeling/devs/atomic.hpp"

using namespace cadmium;

struct PVState {
    double sigma;           // 时间推进值
    double power_output;    // 当前输出功率（kW）

    explicit PVState(): sigma(1.0), power_output(0.0) {}
};

#ifndef NO_LOGGING
std::ostream& operator<<(std::ostream &out, const PVState& state) {
    out << "{power_output: " << state.power_output << " kW, sigma: " << state.sigma << "}";
    return out;
}
#endif

class PVModel : public Atomic<PVState> {
public:
    // 输入输出端口声明
    cadmium::Port<double> irradiance_in;  // 输入端口，接收光照强度（kW/m²）
    cadmium::Port<double> power_out;      // 输出端口，输出当前发电功率

    // 构造函数，初始化端口
    PVModel(const std::string id) : Atomic<PVState>(id, PVState()) {
        irradiance_in = addInPort<double>("irradiance_in");  // 初始化输入端口
        power_out = addOutPort<double>("power_out");         // 初始化输出端口
    }

    // 内部转移函数
    void internalTransition(PVState& state) const override {
        state.sigma = 1.0;  // 重置时间推进值
    }

    // 外部转移函数
    void externalTransition(PVState& state, double e) const override {
        if (!irradiance_in->empty()) {
            double irradiance = irradiance_in->getBag().back();  // 获取光照强度
            state.power_output = irradiance * 10.0;  // 假设光伏效率为10 kW/m²
        }
        state.sigma -= e;  // 更新剩余时间
    }

    // 输出函数
    void output(const PVState& state) const override {
        power_out->addMessage(state.power_output);  // 输出当前发电功率
    }

    // 时间推进函数
    [[nodiscard]] double timeAdvance(const PVState& state) const override {
        return state.sigma;  // 返回剩余时间
    }
};

#endif