#include <vector>
#include <string>
#include "lexer.hpp"
#include "parser.hpp"
#include <iostream>

void instructionToName(const std::vector<CommonInstruction> &instructions, size_t depth = 0)
{
    std::string indent(depth * 4, ' ');

    for (const auto &instruction : instructions)
    {
        std::cout << indent;

        switch (instruction.type)
        {
        case InstructionType::Option:
            std::cout << "Option";
            break;

        case InstructionType::Flag:
            std::cout << "Flag";
            break;

        case InstructionType::Print:
            std::cout << "Print";
            break;

        case InstructionType::AddToPayload:
            std::cout << "AddToPayload";
            break;
        }

        if (!instruction.name.empty())
        {
            std::cout << " (name: \"" << instruction.name << "\")";
        }

        std::cout << '\n';

        if (!instruction.arguments.empty())
        {
            std::cout << indent << "Arguments:\n";

            for (const auto &argument : instruction.arguments)
            {
                std::cout << indent << "    ";

                if (argument.isLiteral)
                {
                    switch (argument.literal.activeValue)
                    {
                    case ActiveValue::String:
                        std::cout << "Literal: \"" << argument.literal.string << "\"";
                        break;

                    case ActiveValue::Number:
                        std::cout << "Literal: " << argument.literal.number;
                        break;

                    case ActiveValue::Boolean:
                        std::cout << "Literal: "
                                  << (argument.literal.boolean ? "true" : "false");
                        break;

                    default:
                        std::cout << "Literal: <no value>";
                        break;
                    }
                }
                else
                {
                    std::cout << "Argument reference: " << argument.argIndex;
                }

                std::cout << '\n';
            }
        }

        if (!instruction.body.empty())
        {
            std::cout << indent << "Body:\n";
            instructionToName(instruction.body, depth + 1);
        }
    }
}

int main()
{
    std::string source;
    std::string line;

    while (std::getline(std::cin, line))
    {
        source += line + '\n';
    }

    size_t index = 0;
    auto tokens = lex(source);
    parseResult result = parse(tokens, index);

    if (result.didSucced == true)
    {
        std::cout << "Success! \n";
    }
    else
    {
        std::cout << "Failed \n";
    }

    instructionToName(result.body);
}