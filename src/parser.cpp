#include <vector>
#include <string>
#include <iostream>
#include <utility>

#include "lexer.hpp"
#include "parser.hpp"


parseResult parseSequence(
    std::vector<Token> &tokens,
    size_t &i,
    bool expectClosingBrace)
{
    parseResult instructions = {true, {}};

    while (i < tokens.size())
    {

        Token &token = tokens[i];

        if (token.type == TokenType::Newline)
        {
            ++i;
            continue;
        }


        if (token.type == TokenType::EndOfFile)
        {
            break;
        }

        if (token.type == TokenType::CloseBrace)
        {
            break;
        }

        if (token.type != TokenType::Identifier)
        {
            std::cout << "Unexpected token: "
                      << getTokenName(token) << '\n';

            return {false, {}};
        }


        if (token.value.string == "option")
        {

            CommonInstruction instruction{};
            instruction.type = InstructionType::Option;


            ++i;

            if (i >= tokens.size() ||
                tokens[i].type != TokenType::String)
            {

                std::cout << "A string was expected after the option identifier.\n";
                return {false, {}};
            }

            instruction.name = tokens[i].value.string;


            ++i;

            if (i >= tokens.size() ||
                tokens[i].type != TokenType::OpenBrace)
            {

                std::cout << "A '{' was expected after the option name.\n";
                return {false, {}};
            }


            ++i;

            auto bodyResult = parseSequence(tokens, i, true);

            if (!bodyResult.didSucced)
            {
                return bodyResult;
            }

            instruction.body = std::move(bodyResult.body);

            instructions.body.push_back(std::move(instruction));
        }
        else
        {
            std::cout << "Unknown instruction: "
                      << token.value.string << '\n';

            return {false, {}};
        }
    }

    if (expectClosingBrace)
    {

        if (i >= tokens.size() ||
            tokens[i].type != TokenType::CloseBrace)
        {

            std::cout << "Expected a closing '}'.\n";
            return {false, {}};
        }

        ++i;
    }

    else if (i < tokens.size() &&
             tokens[i].type == TokenType::CloseBrace)
    {

        std::cout << "Unexpected closing '}'.\n";
        return {false, {}};
    }

    return instructions;
}


parseResult parseBody(
    std::vector<Token> &tokens,
    size_t &i)
{
    return parseSequence(tokens, i, true);
}


parseResult parse(
    std::vector<Token> &tokens,
    size_t &i)
{
    return parseSequence(tokens, i, false);
}