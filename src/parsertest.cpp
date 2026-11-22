#include <vector>
#include <string>
#include "lexer.hpp"
#include "parser.hpp"
#include <iostream>

void instructionToName(
    const std::vector<CommonInstruction>& instructions,
    size_t depth = 0
)
{
    const std::string indent(depth * 4, ' ');
    const std::string childIndent((depth + 1) * 4, ' ');
    const std::string argumentIndent((depth + 2) * 4, ' ');

    for (const auto& instruction : instructions)
    {
        std::cout << indent;

        switch (instruction.type)
        {
            case InstructionType::Option:
                std::cout << "Option";
                break;

            case InstructionType::Target:
                std::cout << "Target";
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
            std::cout << " (name: \"" << instruction.name << "\")";

        if (!instruction.delimiter.empty())
            std::cout << " (delimiter: \"" << instruction.delimiter << "\")";

        if (instruction.type == InstructionType::Flag)
            std::cout << " (argCount: " << instruction.argCount << ")";

        std::cout << '\n';

        // Print arguments belonging to this instruction.
        if (!instruction.arguments.empty())
        {
            std::cout << childIndent << "Arguments:\n";

            for (const auto& argument : instruction.arguments)
            {
                std::cout << argumentIndent;

                if (!argument.isSuccesfull)
                {
                    std::cout << "Invalid argument\n";
                    continue;
                }

                if (argument.isLiteral)
                {
                    switch (argument.literal.activeValue)
                    {
                        case ActiveValue::String:
                            std::cout << "Literal: \""
                                      << argument.literal.string << "\"";
                            break;

                        case ActiveValue::Number:
                            std::cout << "Literal: "
                                      << argument.literal.number;
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
                    std::cout << "Argument reference: "
                              << argument.argIndex;
                }

                std::cout << '\n';
            }
        }

        // Recursively print nested instructions.
        if (!instruction.body.empty())
        {
            std::cout << childIndent << "Body:\n";
            instructionToName(instruction.body, depth + 2);
        }
        else if (
            instruction.type == InstructionType::Flag ||
            instruction.type == InstructionType::Option
        )
        {
            std::cout << childIndent << "Body: <empty>\n";
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
    Context context = Context::MainBody;
    parseResult result = parse(tokens, index,context);

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