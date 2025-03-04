#ifndef BATTERY_MODEL_HPP
#define BATTERY_MODEL_HPP

#include <iostream>
#include "cadmium/modeling/devs/atomic.hpp"

using namespace cadmium;

struct BatteryState {
    double sigma;           // 时间推进值
    double state_of_charge; // 电池的当前电量（百分比）
    double capacity;        // 电池容量（kWh）
    double max_charge_rate; // 最大充电速率（kW）
    double max_discharge_rate; // 最大放电速率（kW）

    explicit BatteryState(): sigma(1.0), state_of_charge(50.0), capacity(100.0), 
                             max_charge_rate(10.0), max_discharge_rate(10.0) {}
};

#ifndef NO_LOGGING
std::ostream& operator<<(std::ostream &out, const BatteryState& state) {
    out << "{SOC: " << state.state_of_charge << "%, sigma: " << state.sigma << "}";
    return out;
}
#endif

class BatteryModel : public Atomic<BatteryState> {
public:
    // 输入输出端口声明
    cadmium::Port<double> power_in;   // 输入端口，接收功率（正值为充电，负值为放电）
    cadmium::Port<double> soc_out;    // 输出端口，输出当前SOC

    // 构造函数，初始化端口
    BatteryModel(const std::string id) : Atomic<BatteryState>(id, BatteryState()) {
        power_in = addInPort<double>("power_in");  // 初始化输入端口
        soc_out = addOutPort<double>("soc_out");   // 初始化输出端口
    }

    // 内部转移函数
    void internalTransition(BatteryState& state) const override {
        state.sigma = 1.0;  // 重置时间推进值
    }

    // 外部转移函数
    void externalTransition(BatteryState& state, double e) const override {
        if (!power_in->empty()) {
            double power = power_in->getBag().back();  // 获取输入功率

            // 计算SOC变化
            double delta_soc = (power * e) / state.capacity * 100;  // SOC变化量（百分比）
            state.state_of_charge += delta_soc;

            // 限制SOC在0%到100%之间
            if (state.state_of_charge > 100.0) {
                state.state_of_charge = 100.0;
            } else if (state.state_of_charge < 0.0) {
                state.state_of_charge = 0.0;
            }
        }
        state.sigma -= e;  // 更新剩余时间
    }

    // 输出函数
    void output(const BatteryState& state) const override {
        soc_out->addMessage(state.state_of_charge);  // 输出当前SOC
    }

    // 时间推进函数
    [[nodiscard]] double timeAdvance(const BatteryState& state) const override {
        return state.sigma;  // 返回剩余时间
    }
};

#endif