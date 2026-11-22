#include <cstdint>
#include <string>
#include "lexer.hpp"
#include <iostream>
#include <cmath>
inline bool isCharacter(const char &ch)
{
    unsigned char lower = static_cast<unsigned char>(ch) | 0x20;
    return (static_cast<unsigned char>(lower - 'a') < 26) || (ch == '_');
}

std::vector<Token> lex(std::string_view source)
{
    size_t line = 0;
    std::vector<Token> tokens = {};
    for (size_t i = 0; i < source.size(); i++)
    {
        Token token;
        char chr = source[i];

        if (chr == '{')
        {
            token.value = {ActiveValue::NoValue, 0};
            token.type = TokenType::OpenBrace;
            token.location = {i, 1, line, i};
            tokens.push_back(token);
        }

        else if (chr == '}')
        {
            token.value = {ActiveValue::NoValue, 0};
            token.type = TokenType::CloseBrace;
            token.location = {i, 1, line, i};
            tokens.push_back(token);
        }

        else if (chr == ':')
        {
            token.value = {ActiveValue::NoValue, 0};
            token.type = TokenType::Colon;
            token.location = {i, 1, line, i};
            tokens.push_back(token);
        }

        else if (chr == ',')
        {
            token.value = {ActiveValue::NoValue, 0};
            token.type = TokenType::Comma;
            token.location = {i, 1, line, i};
            tokens.push_back(token);
        }

        else if (chr == '#')
        {
            while (i < source.size() && source[i] != '\n')
            {
                ++i;
            }
        }

        else if (chr == '=')
        {
            token.value = {ActiveValue::NoValue, 0};
            token.type = TokenType::Equals;
            token.location = {i, 1, line, i};
            tokens.push_back(token);
        }
        else if (chr == '\n')
        {

            token.value = {ActiveValue::NoValue, 0};
            token.type = TokenType::Newline;
            token.location = {i, 1, line, i};
            line++;
            tokens.push_back(token);
        }
        else if (isCharacter(chr))
        {
            std::string buffer;

            while (i < source.size() && (std::isdigit(static_cast<unsigned char>(source[i])) || isCharacter(source[i])))
            {
                buffer.push_back(source[i]);
                ++i;
            }
            --i;
            if (buffer == "true")
            {
                token.location = {i, buffer.size()};
                token.value.activeValue = ActiveValue::Boolean;
                token.value.boolean = true;
                token.type = TokenType::Bool;
                tokens.push_back(token);
            }
            else if (buffer == "false")
            {
                token.location = {i, buffer.size()};
                token.value.activeValue = ActiveValue::Boolean;
                token.value.boolean = false;
                token.type = TokenType::Bool;
                tokens.push_back(token);
            }
            else
            {
                token.location = {i, buffer.size()};
                token.value = {ActiveValue::String, 0, buffer};
                token.type = TokenType::Identifier;
                tokens.push_back(token);
            }
        }
        else if (std::isdigit(static_cast<unsigned char>(source[i])))
        {
            token.type = TokenType::Number;

            double value = 0;

            size_t k = 0;

            while (i + k < source.size() && std::isdigit(static_cast<unsigned char>(source[i + k])))
            {
                value = value * 10 + (source[i + k] - '0');
                ++k;
            }

            if (i + k + 1 < source.size() && source[i + k] == '.' && std::isdigit(static_cast<unsigned char>(source[i + k + 1])))
            {
                k++;

                double place = 0.1;

                while (i + k < source.size() && std::isdigit(static_cast<unsigned char>(source[i + k])))
                {
                    value += (source[i + k] - '0') * place;
                    place *= 0.1;
                    ++k;
                }
            }
            token.location = {i,k,line,i};
            token.value = {ActiveValue::Number, value};
            tokens.push_back(token);

            i += k - 1;
        }
        else if (chr == '-' && std::isdigit(static_cast<unsigned char>(source[i + 1])))
        {
            ++i;
            token.type = TokenType::Number;

            double value = 0;

            size_t k = 0;

            while (i + k < source.size() && std::isdigit(static_cast<unsigned char>(source[i + k])))
            {
                value = value * 10 + (source[i + k] - '0');
                ++k;
            }

            if (i + k + 1 < source.size() && source[i + k] == '.' && std::isdigit(static_cast<unsigned char>(source[i + k + 1])))
            {
                k++;

                double place = 0.1;

                while (i + k < source.size() && std::isdigit(static_cast<unsigned char>(source[i + k])))
                {
                    value += (source[i + k] - '0') * place;
                    place *= 0.1;
                    ++k;
                }
            }
            token.location = {i,1+k,line,i};
            token.value = {ActiveValue::Number, -value};
            tokens.push_back(token);

            i += k - 1;
        }

        else if (chr == '"')
        {
            std::string buffer;

            size_t k = 1;

            while (i + k < source.size() && source[i + k] != '"')
            {
                char currentChar = source[i + k];

                if (currentChar == '\\')
                {
                    if (i + k + 1 >= source.size())
                    {
                        throw std::runtime_error("Error: Incomplete escape sequence.");
                    }

                    char escape = source[i + k + 1];

                    switch (escape)
                    {
                    case '0':
                        if (i + k + 3 < source.size() && source[i + k + 2] == '3' && source[i + k + 3] == '3')
                        {
                            buffer.push_back('\033');
                            k += 4;
                        }
                        else
                        {
                            buffer.push_back('\0');
                            k += 2;
                        }
                        continue;

                    case 'x':

                        if (i + k + 3 < source.size() && source[i + k + 2] == '1' && (source[i + k + 3] == 'b' || source[i + k + 3] == 'B'))
                        {
                            buffer.push_back('\x1b');
                            k += 4;
                        }
                        else
                        {
                            throw std::runtime_error("Error: Invalid hex escape sequence.");
                        }
                        continue;
                    case 'n':
                        buffer.push_back('\n');
                        break;
                    case 't':
                        buffer.push_back('\t');
                        break;

                    case 'r':
                        buffer.push_back('\r');
                        break;

                    case '\\':
                        buffer.push_back('\\');
                        break;

                    case '"':
                        buffer.push_back('"');
                        break;
                    case '\'':
                        buffer.push_back('\'');
                        break;
                    default:
                        throw std::runtime_error("Error: Unknown escape sequence.");
                    }

                    k += 2;
                    continue;
                }

                buffer.push_back(currentChar);
                ++k;
            }

            if (i + k >= source.size())
            {
                throw std::runtime_error("Error: String never finished.");
            }
            token.location = {i, buffer.size()};
            token.type = TokenType::String;
            token.value = {ActiveValue::String, 0, buffer};

            tokens.emplace_back(std::move(token));

            i += k;
        }
    }

    tokens.push_back({{ActiveValue::NoValue, 0}, TokenType::EndOfFile});

    return tokens;
}

std::string getTokenName(Token token)
{
    if (token.type == TokenType::CloseBrace)
    {
        return "CloseBrace";
    }
    if (token.type == TokenType::OpenBrace)
    {
        return "OpenBrace";
    }
    if (token.type == TokenType::Colon)
    {
        return "Colon";
    }
    if (token.type == TokenType::Equals)
    {
        return "Equals";
    }
    if (token.type == TokenType::Identifier)
    {
        return "Identifier(" + token.value.string + "";
    }
    if (token.type == TokenType::EndOfFile)
    {
        return "EndOfFile";
    }
    if (token.type == TokenType::Number)
    {
        return "Number";
    }
    if (token.type == TokenType::String)
    {
        return "String";
    }
    if (token.type == TokenType::Bool)
    {
        return "Bool";
    }
    if (token.type == TokenType::Newline)
    {
        return "Newline";
    }
}