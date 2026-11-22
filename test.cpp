#include <iostream>
#include <string>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

int main() {
    std::string input;
    std::getline(std::cin, input);

    json request = json::parse(input);

    std::string command = request["command"];
    std::string source = request["source"];

    std::cout << "Command: " << command << '\n';
    std::cout << "Source: " << source << '\n';
}