#include "RegisterProblem.h"

#include <cmath>
#include <vector>

RegisterProblem::RegisterProblem(long long start, long long goal)
{
    this->start = start;
    this->goal = goal;
}

RegisterState RegisterProblem::getStart() const
{
    return start;
}

bool RegisterProblem::isGoal(const RegisterState& state) const
{
    return state == goal;
}

bool RegisterProblem::areEqual(const RegisterState& state1, const RegisterState& state2) const
{
    return state1 == state2;
}

std::vector<std::pair<RegisterState, RegisterAction>> RegisterProblem::getChildren(const RegisterState& state) const
{
    std::vector<std::pair<RegisterState, RegisterAction>> children;

    if (state < 1e9)
        children.push_back({ state + 1, RegisterAction::INC });

    if (state > 0) {
        children.push_back({ state - 1, RegisterAction::DEC });
        children.push_back({ state / 2, RegisterAction::HALF });
    }

    if (state > 0 && 2 * state <= 1e9)
        children.push_back({ 2 * state, RegisterAction::DOUBLE });

    if (state * state <= 1e9)
        children.push_back({ state * state, RegisterAction::SQUARE });

    RegisterState root = std::round(std::sqrt(state));
    if (state > 1 && root * root == state)
        children.push_back({ root, RegisterAction::SQRT });

    return children;
}

long long RegisterProblem::getCost(const RegisterState& state, const RegisterAction& action) const
{
    switch (action)
    {
    case RegisterAction::INC:
    case RegisterAction::DEC:
        return 2;
    case RegisterAction::DOUBLE:
        return std::ceil(state / 2.0) + 1;
    case RegisterAction::HALF:
        return std::ceil(state / 4.0) + 1;
    case RegisterAction::SQUARE:
        return std::ceil((state * state - state) / 4.0) + 1;
    case RegisterAction::SQRT:
        return std::ceil((state - std::sqrt(state)) / 4.0) + 1;
    default:
        return 0;
    }
}

long long RegisterProblem::heuristic(const RegisterState& state) const
{
    return std::ceil((std::abs(state - goal)) / 4.0);
}