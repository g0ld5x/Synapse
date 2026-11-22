#include <cstdint>
#include <string>
#include "lexer.hpp"
#include <iostream>

int main()
{
    std::string source;
    std::string line;

    while (std::getline(std::cin, line))
    {
        source += line + '\n';
    }

    std::vector<Token> tokens = lex(source);

    for (size_t i = 0; i < tokens.size(); i++)
    {
        if (tokens[i].type == TokenType::CloseBrace)
        {
            std::cout << "CloseBrace, ";
        }
        if (tokens[i].type == TokenType::OpenBrace)
        {
            std::cout << "OpenBrace, ";
        }
        if (tokens[i].type == TokenType::Colon)
        {
            std::cout << "Colon, ";
        }
        if (tokens[i].type == TokenType::Equals)
        {
            std::cout << "Equals, ";
        }
        if (tokens[i].type == TokenType::Identifier)
        {
            std::cout << "Identifier(" << tokens[i].value.string << "), ";
        }
        if (tokens[i].type == TokenType::EndOfFile)
        {
            std::cout << "\n EndOfFile.";
        }
        if (tokens[i].type == TokenType::Number)
        {
            std::cout << "Number(" << tokens[i].value.number << "), ";
        }
        if (tokens[i].type == TokenType::String)
        {
            std::cout << "String(" << tokens[i].value.string << "), ";
        }
        if (tokens[i].type == TokenType::Bool)
        {
            std::cout << "Bool(" << std::boolalpha << tokens[i].value.boolean << "), ";
        }
        if (tokens[i].type == TokenType::Newline)
        {
            std::cout << "Newline, ";
        }
    }
    return 0;
}