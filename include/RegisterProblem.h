#pragma once

#include <vector>
#include <utility>

#include "Problem.h"

using RegisterState = long long;

enum RegisterAction
{
    INC,
    DEC,
    DOUBLE,
    HALF,
    SQUARE,
    SQRT
};

class RegisterProblem : public Problem<RegisterState, RegisterAction>
{
private:
    RegisterState start;
    RegisterState goal;

public:
    RegisterProblem(long long start, long long goal);
    RegisterState getStart() const override;
    bool isGoal(const RegisterState& state) const override;
    bool areEqual(const RegisterState& state1, const RegisterState& state2) const override;
    std::vector<std::pair<RegisterState, RegisterAction>> getChildren(const RegisterState& state) const override;
    long long getCost(const RegisterState& state, const RegisterAction& action) const override;
    long long heuristic(const RegisterState& state) const override;
};
