#pragma once

#include <stack>
#include <vector>
#include <queue>
#include <utility>
#include <unordered_set>
#include <unordered_map>
#include <string>
#include <functional>
#include <chrono>
#include <random>
#include <algorithm>

#include "Problem.h"

const float TIME_LIMIT{ 60.0 };

template<typename State, typename Action>
struct Node
{
    State state{};
    long long g{};
    long long h{};
    long long f{};
    Node* parent{};
    Action action{};
};

template<typename State, typename Action>
class NodeCollector
{
private:
    static inline std::vector<Node<State, Action>*> allocated_nodes;

public:
    static Node<State, Action>* createNode(State state, long long g, long long h, long long f, Node<State, Action>* parent, Action action)
    {
        auto* n = new Node<State, Action>{ state, g, h, f, parent, action };
        allocated_nodes.push_back(n);
        return n;
    }

    static void freeNodes()
    {
        for (auto n : allocated_nodes)
            delete n;

        allocated_nodes.clear();
    }
};

template<typename State, typename Action>
long long depth(Node<State, Action>* aNodePtr)
{
    long long depth{};
    while (aNodePtr != nullptr) {
        depth++;
        aNodePtr = aNodePtr->parent;
    }

    return depth;
}

template<typename State, typename Action>
bool isCycle(const Problem<State, Action>& problem, Node<State, Action>* aNode)
{
    Node<State, Action>* currNode = aNode->parent;

    while (currNode != nullptr) {
        if (problem.areEqual(currNode->state, aNode->state))
            return true;
        currNode = currNode->parent;
    }

    return false;
}

template<typename State, typename Action>
std::vector<Node<State, Action>*> expand(const Problem<State, Action>& problem, Node<State, Action>* aNode)
{
    std::vector<Node<State, Action>*> childNodes{};
    auto children{ problem.getChildren(aNode->state) };

    for (const auto& [nextState, action] : children) {
        long long stepCost = problem.getCost(aNode->state, action);
        long long g = aNode->g + stepCost;
        long long h = problem.heuristic(nextState);
        long long f = g + h;

        childNodes.push_back(NodeCollector<State, Action>::createNode(nextState, g, h, f, aNode, action));
    }


    return childNodes;
}

template <typename State, typename Action>
Node<State, Action>* breadthFirstSearch(const Problem<State, Action>& problem, std::chrono::duration<double>& time_elapsed)
{
    auto start = problem.getStart();
    std::queue<Node<State, Action>*> frontier;
    std::unordered_set<State> visited;

    auto* curr = NodeCollector<State, Action>::createNode(start, 0, 0, 0, nullptr, Action{});
    frontier.push(curr);
    visited.insert(curr->state);

    auto time_start = std::chrono::steady_clock::now();
    while (!frontier.empty())
    {
        auto time_now = std::chrono::steady_clock::now();
        time_elapsed = time_now - time_start;

        if (time_elapsed.count() > TIME_LIMIT)
            return nullptr;

        curr = frontier.front();
        frontier.pop();

        State s;
        for (Node<State, Action>* child : expand(problem, curr))
        {
            s = child->state;
            if (problem.isGoal(s))
                return child;

            if (visited.insert(s).second) {
                frontier.push(child);
            }
        }

    }
    return nullptr;
}

template <typename State, typename Action>
Node<State, Action>* depthLimitedSearch(const Problem<State, Action>& problem, const long long depth_limit, const std::chrono::time_point<std::chrono::steady_clock> time_start)
{
    auto start = problem.getStart();
    std::stack<Node<State, Action>*> frontier{};

    auto* curr = NodeCollector<State, Action>::createNode(start, 0, 0, 0, nullptr, Action{});
    frontier.push(curr);

    while (!frontier.empty())
    {
        auto time_now = std::chrono::steady_clock::now();
        std::chrono::duration<double> time_elapsed = time_now - time_start;
        if (time_elapsed.count() > TIME_LIMIT)
            return nullptr;

        curr = frontier.top();
        frontier.pop();

        if (problem.isGoal(curr->state))
            return curr;
        else if (depth(curr) > depth_limit)
            continue;

        if (!isCycle(problem, curr)) {
            for (Node<State, Action>* child : expand(problem, curr)) {
                frontier.push(child);
            }
        }
    }
    return nullptr;
}

template <typename State, typename Action>
Node<State, Action>* iterativeDeepeningSearch(const Problem<State, Action>& problem, std::chrono::duration<double>& time_elapsed)
{
    long long depth_limit{};
    Node<State, Action>* resultNode{};
    auto time_start = std::chrono::steady_clock::now();

    while (true) {
        auto time_now = std::chrono::steady_clock::now();
        time_elapsed = time_now - time_start;
        if (time_elapsed.count() > TIME_LIMIT) {
            return nullptr;
        }

        resultNode = depthLimitedSearch(problem, depth_limit, time_start);
        if (resultNode != nullptr) {
            return resultNode;
        }

        depth_limit++;
    }
}

template <typename State, typename Action>
Node<State, Action>* informedSearch(const Problem<State, Action>& problem, std::function<bool(Node<State, Action>*, Node<State, Action>*)> cmp, std::chrono::duration<double>& time_elapsed)
{
    std::priority_queue<Node<State, Action>*, std::vector<Node<State, Action>*>, decltype(cmp)>frontier(cmp);

    std::unordered_map<State, Node<State, Action>*> visited;
    auto start = problem.getStart();
    auto* curr = NodeCollector<State, Action>::createNode(start, 0, problem.heuristic(start), problem.heuristic(start), nullptr, Action{});
    frontier.push(curr);
    visited.insert({ curr->state, curr });

    auto time_start = std::chrono::steady_clock::now();
    while (!frontier.empty())
    {
        auto time_now = std::chrono::steady_clock::now();
        time_elapsed = time_now - time_start;
        if (time_elapsed.count() > TIME_LIMIT)
            return nullptr;

        curr = frontier.top();
        frontier.pop();

        if (problem.isGoal(curr->state))
            return curr;

        State s;
        bool hasBeenVisited;
        for (Node<State, Action>* child : expand(problem, curr))
        {
            s = child->state;
            hasBeenVisited = visited.find(s) != visited.end();
            if (!hasBeenVisited || child->g < visited[s]->g) {
                visited[s] = child;
                frontier.push(child);
            }
        }
    }
    return nullptr;
}

template <typename State, typename Action>
Node<State, Action>* bestFirstSearch(const Problem<State, Action>& problem, std::chrono::duration<double>& time_elapsed)
{
    std::function<bool(Node<State, Action>*, Node<State, Action>*)>  cmp = [](Node<State, Action>* a, Node<State, Action>* b) {return a->h > b->h; };
    return informedSearch(problem, cmp, time_elapsed);
}

template <typename State, typename Action>
Node<State, Action>* aStar(const Problem<State, Action>& problem, std::chrono::duration<double>& time_elapsed)
{

    std::function<bool(Node<State, Action>*, Node<State, Action>*)> cmp = [](Node<State, Action>* a, Node<State, Action>* b) {return a->f > b->f; };
    return informedSearch(problem, cmp, time_elapsed);
}
