#include <iostream>
#include <vector>
#include <string>
#include <chrono>
#include <fstream>

#include "search.h"
#include "RegisterProblem.h"

void syntax_message()
{
    std::cout << "executable-name <method> <start> <goal> <output-file>\nwhere:\n";
    std::cout << "<method> = breadth|depth|best|astar\n";
    std::cout << "<start> is the starting number\n<goal> is the search goal\n";
    std::cout << "<output-file> is the file where the solution will be written.\n ";
}

std::string getActionString(const RegisterAction& action)
{
    switch (action)
    {
    case RegisterAction::INC:
        return "increase";
    case RegisterAction::DEC:
        return "decrease";
    case RegisterAction::DOUBLE:
        return "double";
    case RegisterAction::HALF:
        return "half";
    case RegisterAction::SQUARE:
        return "square";
    case RegisterAction::SQRT:
        return "root";
    default:
        return "";
    }
}

void writeSolutionToFile(const RegisterProblem& problem, Node<RegisterState, RegisterAction>* solution, const char* file_name)
{
    std::vector<Node<RegisterState, RegisterAction>*> path;
    Node<RegisterState, RegisterAction>* curr = solution;

    while (curr != nullptr) {
        path.push_back(curr);
        curr = curr->parent;
    }

    std::ofstream outfile(file_name);
    if (!outfile) {
        std::cout << "Cannot open text file\n";
        return;
    }

    outfile << depth(solution) << ", " << solution->g << '\n';

    for (int i = (int)path.size() - 2; i >= 0; i--) {
        Node<RegisterState, RegisterAction>* n = path[i];
        Node<RegisterState, RegisterAction>* parent = path[i + 1];
        outfile << getActionString(n->action) << " " << parent->state << " " << problem.getCost(parent->state, n->action) << '\n';
    }
}

int main(int argc, char* argv[])
{
    if (argc != 5)
    {
        std::cout << "Wrong number of arguments. Use correct syntax:\n";
        syntax_message();
        return -1;
    }

    long long start = std::stoll(argv[2]);
    long long goal = std::stoll(argv[3]);

    if (start < 0 || goal < 0) {
        std::cout << "This program only works with non negative integers.\n";
        return -1;
    }

    Node<RegisterState, RegisterAction>* solution{};
    RegisterProblem problem = RegisterProblem(start, goal);

    std::string method{ argv[1] };
    std::chrono::duration<double> time_elapsed{};
    if (method == "breadth") {
        std::cout << "Solving problem using breadth first search...\n";
        solution = breadthFirstSearch(problem, time_elapsed);
    }
    else if (method == "depth") {
        std::cout << "Solving problem using depth first search...\n";
        solution = iterativeDeepeningSearch(problem, time_elapsed);
    }
    else if (method == "best") {
        std::cout << "Solving problem using best first search...\n";
        solution = bestFirstSearch(problem, time_elapsed);
    }
    else if (method == "astar") {
        std::cout << "Solving problem using A*...\n";
        solution = aStar(problem, time_elapsed);
    }
    else {
        std::cout << method << " is not a valid method name\n. Use correct syntax:\n";
        syntax_message();
    }

    if (solution == nullptr)
        std::cout << "No solution found within 60 seconds.\n";
    else {
        std::cout << "Solution found in " << time_elapsed.count() << " seconds with " << depth(solution) << " steps.\n";
        writeSolutionToFile(problem, solution, argv[4]);
    }

    NodeCollector<RegisterState, RegisterAction>::freeNodes();

    return 0;
}