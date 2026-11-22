#include <vector>
#include <string>
#include <iostream>
#include <utility>

#include "lexer.hpp"
#include "parser.hpp"

std::vector<InstructionValue> parseValues(std::vector<Token> &tokens, size_t &i)
{
    std::vector<InstructionValue> values = {};
    while (i < tokens.size() && tokens[i].type != TokenType::Newline && tokens[i].type != TokenType::EndOfFile)
    {
        InstructionValue value{};
        value.isSuccesfull = true;
        if (tokens[i].type == TokenType::String)
        {
            value.isLiteral = true;
            value.literal.string = tokens[i].value.string;
            value.literal.activeValue = ActiveValue::String;
            values.push_back(value);
        }
        else if (tokens[i].type == TokenType::Number)
        {
            value.isLiteral = true;
            value.literal.number = tokens[i].value.number;
            value.literal.activeValue = ActiveValue::Number;

            values.push_back(value);
        }
        else if (tokens[i].type == TokenType::Bool)
        {
            value.isLiteral = true;
            value.literal.boolean = tokens[i].value.boolean;
            value.literal.activeValue = ActiveValue::Boolean;

            values.push_back(value);
        }
        else if (tokens[i].type == TokenType::Identifier)
        {
            if (tokens[i].value.string != "arg")
            {
                value.isSuccesfull = false;
                const Token &token = tokens[i];
                std::cout
                    << "Error: Expected identifier 'arg'.\n"
                    << "  Received: " << getTokenName(token) << '\n'
                    << "  At line: " << token.location.line << '\n'
                    << "  From: " << token.location.start
                    << " to: " << token.location.start + token.location.length
                    << '\n';
            }
            else
            {
                if (tokens[i + 1].type != TokenType::Colon)
                {
                    value.isSuccesfull = false;
                    const Token &token = tokens[i + 1];
                    std::cout
                        << "Error: Expected ':' after identifier 'arg'.\n"
                        << "  Received: " << getTokenName(token) << '\n'
                        << "  At line: " << token.location.line << '\n'
                        << "  From: " << token.location.start
                        << " to: " << token.location.start + token.location.length
                        << '\n';
                }
                else
                {
                    ++i;
                    if (tokens[i + 1].type != TokenType::Number)
                    {
                        value.isSuccesfull = false;
                        const Token &token = tokens[i + 1];
                        std::cout
                            << "Error: Expected an index number after 'arg:'.\n"
                            << "  Received: " << getTokenName(token) << '\n'
                            << "  At line: " << token.location.line << '\n'
                            << "  From: " << token.location.start
                            << " to: " << token.location.start + token.location.length
                            << '\n';
                    }
                    else
                    {
                        ++i;
                        value.isLiteral = false;
                        value.argIndex = (int)tokens[i].value.number;
                    }
                }
            }

            values.push_back(value);
        }
        else
        {
            value.isSuccesfull = false;
            const Token &token = tokens[i];
            std::cout
                << "Error: Unknown token inside a command.\n"
                << "  Received: " << getTokenName(token) << '\n'
                << "  At line: " << token.location.line << '\n'
                << "  From: " << token.location.start
                << " to: " << token.location.start + token.location.length
                << '\n';
            values.push_back(value);
        }
        ++i;
    }

    return values;
}

parseResult parse(std::vector<Token> &tokens, size_t &i, Context context)
{
    parseResult result;
    result.didSucced = true;
    if (context == Context::MainBody)
    {
        while (i < tokens.size())
        {
            CommonInstruction instr;
            if (tokens[i].type == TokenType::Identifier)
            {
                if (tokens[i].value.string == "target")
                {
                    instr.type = InstructionType::Target;
                    if (tokens[i + 1].type != TokenType::String)
                    {
                        result.didSucced = false;
                        const Token &token = tokens[i + 1];

                        std::cout
                            << "Error: Expected a string value for target.\n"
                            << "  Received: " << getTokenName(token) << '\n'
                            << "  At line: " << token.location.line << '\n'
                            << "  From: " << token.location.start
                            << " to: " << token.location.start + token.location.length
                            << '\n';
                    }
                    else
                    {
                        ++i;
                        instr.name = tokens[i].value.string;
                    }

                    result.body.push_back(instr);
                }
                else if (tokens[i].value.string == "flag")
                {
                    parseResult flagResult = parse(tokens, i, Context::Flag);
                    if (!flagResult.didSucced)
                    {
                        result.didSucced = false;
                        return result;
                    }
                    else
                    {
                        result.body.push_back(flagResult.body[0]);
                    }
                }
                else if (tokens[i].value.string == "option")
                {
                    parseResult optionResult = parse(tokens, i, Context::Option);
                    if (!optionResult.didSucced)
                    {
                        result.didSucced = false;
                        return result;
                    }
                    else
                    {
                        result.body.push_back(optionResult.body[0]);
                    }
                }
                else if (tokens[i].value.string == "addToPayload")
                {
                    ++i;
                    instr.arguments = parseValues(tokens, i);
                    instr.type = InstructionType::AddToPayload;

                    for (const auto &argument : instr.arguments)
                    {
                        if (!argument.isSuccesfull)
                            result.didSucced = false;
                    }

                    result.body.push_back(instr);
                }
                else if (tokens[i].value.string == "print")
                {
                    ++i;
                    instr.arguments = parseValues(tokens, i);
                    instr.type = InstructionType::Print;

                    for (const auto &argument : instr.arguments)
                    {
                        if (!argument.isSuccesfull)
                            result.didSucced = false;
                    }

                    result.body.push_back(instr);
                }
            }
            ++i;
        }
    }
    else if (context == Context::Flag)
    {
        CommonInstruction instr;
        instr.type = InstructionType::Flag;
        if (tokens[i + 1].type != TokenType::String)
        {
            result.didSucced = false;
            const Token &token = tokens[i + 1];

            std::cout
                << "Error: Expected a string value for flag name.\n"
                << "  Received: " << getTokenName(token) << '\n'
                << "  At line: " << token.location.line << '\n'
                << "  From: " << token.location.start
                << " to: " << token.location.start + token.location.length
                << '\n';
        }
        else
        {
            ++i;
            instr.name = tokens[i].value.string;
        }

        ++i;

        while (tokens[i].type == TokenType::Comma)
        {
            ++i;

            if (tokens[i].type != TokenType::Identifier)
            {
                result.didSucced = false;
                const Token &token = tokens[i];

                std::cout
                    << "Error: Expected a 'argCount' or 'delimiter' for flag metadata.\n"
                    << "  Received: " << getTokenName(token) << '\n'
                    << "  At line: " << token.location.line << '\n'
                    << "  From: " << token.location.start
                    << " to: " << token.location.start + token.location.length
                    << '\n';

                break;
            }

            if (tokens[i].value.string == "argCount")
            {
                ++i;

                if (tokens[i].type != TokenType::Equals)
                {
                    result.didSucced = false;
                    const Token &token = tokens[i];

                    std::cout
                        << "Error: Expected '=' after flag metadata 'argCount'\n"
                        << "  Received: " << getTokenName(token) << '\n'
                        << "  At line: " << token.location.line << '\n'
                        << "  From: " << token.location.start
                        << " to: " << token.location.start + token.location.length
                        << '\n';

                    break;
                }

                ++i;

                if (tokens[i].type != TokenType::Number)
                {
                    result.didSucced = false;
                    const Token &token = tokens[i];

                    std::cout
                        << "Error: Expected a number value as the value for 'argCount'\n"
                        << "  Received: " << getTokenName(token) << '\n'
                        << "  At line: " << token.location.line << '\n'
                        << "  From: " << token.location.start
                        << " to: " << token.location.start + token.location.length
                        << '\n';
                }
                ++i;
                instr.argCount = tokens[i].value.number;
            }
            else if (tokens[i].value.string == "delimiter")
            {
                ++i;

                if (tokens[i].type != TokenType::Equals)
                {
                    result.didSucced = false;
                    const Token &token = tokens[i];

                    std::cout
                        << "Error: Expected '=' after flag metadata 'delimiter'\n"
                        << "  Received: " << getTokenName(token) << '\n'
                        << "  At line: " << token.location.line << '\n'
                        << "  From: " << token.location.start
                        << " to: " << token.location.start + token.location.length
                        << '\n';

                    break;
                }

                ++i;

                if (tokens[i].type != TokenType::String)
                {
                    result.didSucced = false;
                    const Token &token = tokens[i];

                    std::cout
                        << "Error: Expected a string value as the value for 'delimiter'\n"
                        << "  Received: " << getTokenName(token) << '\n'
                        << "  At line: " << token.location.line << '\n'
                        << "  From: " << token.location.start
                        << " to: " << token.location.start + token.location.length
                        << '\n';
                }
                ++i;
                instr.delimiter = tokens[i].value.string;
            }
            else
            {
                result.didSucced = false;
                const Token &token = tokens[i];

                std::cout
                    << "Error: Expected a 'argCount' or 'delimiter' for flag metadata.\n"
                    << "  Received: " << getTokenName(token) << '\n'
                    << "  At line: " << token.location.line << '\n'
                    << "  From: " << token.location.start
                    << " to: " << token.location.start + token.location.length
                    << '\n';

                break;
            }
        }

        if (tokens[i].type == TokenType::OpenBrace)
        {
            ++i; // Consume {

            parseResult bodyResult = parse(tokens, i, Context::FlagBody);

            if (!bodyResult.didSucced)
            {
                result.didSucced = false;
                // Propagate errors
            }
            else
            {
                instr.body = std::move(bodyResult.body);
            }
        }

        result.body.push_back(instr);
    }
    else if (context == Context::Option)
    {
        CommonInstruction instr;
        instr.type = InstructionType::Option;
        if (tokens[i + 1].type != TokenType::String)
        {
            result.didSucced = false;
            const Token &token = tokens[i + 1];

            std::cout
                << "Error: Expected a string value for option name.\n"
                << "  Received: " << getTokenName(token) << '\n'
                << "  At line: " << token.location.line << '\n'
                << "  From: " << token.location.start
                << " to: " << token.location.start + token.location.length
                << '\n';
        }
        else
        {
            ++i;
            instr.name = tokens[i].value.string;
        }

        if (tokens[i + 1].type != TokenType::OpenBrace)
        {
            result.didSucced = false;
            const Token &token = tokens[i + 1];

            std::cout
                << "Error: Expected '{' for option body..\n"
                << "  Received: " << getTokenName(token) << '\n'
                << "  At line: " << token.location.line << '\n'
                << "  From: " << token.location.start
                << " to: " << token.location.start + token.location.length
                << '\n';
        }
        else
        {
            ++i;
            parseResult bodyResult = parse(tokens, i, Context::OptionBody);

            if (!bodyResult.didSucced)
            {
                result.didSucced = false;
            }
            else
            {
                instr.body = std::move(bodyResult.body);
            }
        }

        result.body.push_back(std::move(instr));
    }
    else if (context == Context::FlagBody)
    {
        while (i < tokens.size())
        {
            if (tokens[i].type == TokenType::CloseBrace)
            {
                ++i;
                break;
            }

            if (tokens[i].type == TokenType::Newline)
            {
                ++i;
                continue;
            }
            CommonInstruction instr;
            if (tokens[i].type == TokenType::Identifier)
            {
                if (tokens[i].value.string == "target")
                {
                    result.didSucced = false;
                    const Token &token = tokens[i];

                    std::cout
                        << "Error: The target command cannot be used inside a flag. It can only be used inside the main body..\n"
                        << "  Received: " << getTokenName(token) << '\n'
                        << "  At line: " << token.location.line << '\n'
                        << "  From: " << token.location.start
                        << " to: " << token.location.start + token.location.length
                        << '\n';
                }
                else if (tokens[i].value.string == "flag")
                {
                    result.didSucced = false;
                    const Token &token = tokens[i];

                    std::cout
                        << "Error: A flag cannot be inside a flag.\n"
                        << "  Received: " << getTokenName(token) << '\n'
                        << "  At line: " << token.location.line << '\n'
                        << "  From: " << token.location.start
                        << " to: " << token.location.start + token.location.length
                        << '\n';
                }
                else if (tokens[i].value.string == "addToPayload")
                {
                    ++i;
                    instr.arguments = parseValues(tokens, i);
                    instr.type = InstructionType::AddToPayload;

                    for (const auto &argument : instr.arguments)
                    {
                        if (!argument.isSuccesfull)
                            result.didSucced = false;
                    }

                    result.body.push_back(instr);
                }
                else if (tokens[i].value.string == "print")
                {
                    ++i;
                    instr.arguments = parseValues(tokens, i);
                    instr.type = InstructionType::Print;

                    for (const auto &argument : instr.arguments)
                    {
                        if (!argument.isSuccesfull)
                            result.didSucced = false;
                    }

                    result.body.push_back(instr);
                }
            }
            ++i;
        }
    }

    else if (context == Context::OptionBody)
    {
        while (i < tokens.size())
        {
            if (tokens[i].type == TokenType::CloseBrace)
            {
                ++i;
                break;
            }

            if (tokens[i].type == TokenType::Newline)
            {
                ++i;
                continue;
            }
            CommonInstruction instr;
            if (tokens[i].type == TokenType::Identifier)
            {
                if (tokens[i].value.string == "target")
                {
                    result.didSucced = false;
                    const Token &token = tokens[i];

                    std::cout
                        << "Error: The target command cannot be used inside a flag. It can only be used inside the main body..\n"
                        << "  Received: " << getTokenName(token) << '\n'
                        << "  At line: " << token.location.line << '\n'
                        << "  From: " << token.location.start
                        << " to: " << token.location.start + token.location.length
                        << '\n';
                }

                else if (tokens[i].value.string == "option")
                {
                    result.didSucced = false;
                    const Token &token = tokens[i];

                    std::cout
                        << "Error: The option block command cannot be used inside an option. It can only be used inside the main body..\n"
                        << "  Received: " << getTokenName(token) << '\n'
                        << "  At line: " << token.location.line << '\n'
                        << "  From: " << token.location.start
                        << " to: " << token.location.start + token.location.length
                        << '\n';
                }
                else if (tokens[i].value.string == "flag")
                {
                    parseResult flagResult = parse(tokens, i, Context::Flag);
                    if (!flagResult.didSucced)
                    {
                        result.didSucced = false;
                        return result;
                    }
                    else
                    {
                        result.body.push_back(flagResult.body[0]);
                    }
                }
                else if (tokens[i].value.string == "addToPayload")
                {
                    ++i;
                    instr.arguments = parseValues(tokens, i);
                    instr.type = InstructionType::AddToPayload;

                    for (const auto &argument : instr.arguments)
                    {
                        if (!argument.isSuccesfull)
                            result.didSucced = false;
                    }

                    result.body.push_back(instr);
                }
                else if (tokens[i].value.string == "print")
                {
                    ++i;
                    instr.arguments = parseValues(tokens, i);
                    instr.type = InstructionType::Print;

                    for (const auto &argument : instr.arguments)
                    {
                        if (!argument.isSuccesfull)
                            result.didSucced = false;
                    }

                    result.body.push_back(instr);
                }
            }
            ++i;
        }
    }

    return result;
}