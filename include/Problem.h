#pragma once

#include <vector>
#include <utility>

template <typename State, typename Action>
class Problem
{
public:
    virtual bool isGoal(const State& state) const = 0;
    virtual bool areEqual(const State& state1, const State& state2) const = 0;
    virtual std::vector<std::pair<State, Action>> getChildren(const State& state) const = 0;
    virtual long long getCost(const State& state, const Action& action) const = 0;
    virtual long long heuristic(const State& state) const = 0;
    virtual State getStart() const = 0;

    virtual ~Problem() {};
};