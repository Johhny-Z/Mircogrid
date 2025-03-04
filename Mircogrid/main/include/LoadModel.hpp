#ifndef LOAD_MODEL_HPP
#define LOAD_MODEL_HPP

#include <iostream>
#include "cadmium/modeling/devs/atomic.hpp"

using namespace cadmium;

struct LoadState {
    double sigma;           // 时间推进值
    double power_demand;    // 当前负载需求（kW）

    explicit LoadState(): sigma(1.0), power_demand(0.0) {}
};

#ifndef NO_LOGGING
std::ostream& operator<<(std::ostream &out, const LoadState& state) {
    out << "{power_demand: " << state.power_demand << " kW, sigma: " << state.sigma << "}";
    return out;
}
#endif

class LoadModel : public Atomic<LoadState> {
public:
    // 输入输出端口声明
    cadmium::Port<double> demand_in;  // 输入端口，接收负载需求（kW）
    cadmium::Port<double> demand_out; // 输出端口，输出当前负载需求

    // 构造函数，初始化端口
    LoadModel(const std::string id) : Atomic<LoadState>(id, LoadState()) {
        demand_in = addInPort<double>("demand_in");  // 初始化输入端口
        demand_out = addOutPort<double>("demand_out");  // 初始化输出端口
    }

    // 内部转移函数
    void internalTransition(LoadState& state) const override {
        state.sigma = 1.0;  // 重置时间推进值
    }

    // 外部转移函数
    void externalTransition(LoadState& state, double e) const override {
        if (!demand_in->empty()) {
            state.power_demand = demand_in->getBag().back();  // 获取负载需求
        }
        state.sigma -= e;  // 更新剩余时间
    }

    // 输出函数
    void output(const LoadState& state) const override {
        demand_out->addMessage(state.power_demand);  // 输出当前负载需求
    }

    // 时间推进函数
    [[nodiscard]] double timeAdvance(const LoadState& state) const override {
        return state.sigma;  // 返回剩余时间
    }
};

#endif