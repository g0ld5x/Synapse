#pragma once

#include <cstdint>
#include <string>
#include <vector>

struct SourceLocation
{

    size_t start;
    size_t length;

    size_t line;
    size_t column;
};

enum class TokenType : std::uint8_t
{
    Identifier, //--done
    Equals,     //--done
    Comma,      //--done
    Colon,      //--done
    String,     //--done
    Number,     //--done
    Newline,    //--done
    OpenBrace,  //--done
    CloseBrace, //--done
    Bool,       //--done
    EndOfFile,  //--done
};

enum class ActiveValue : std::uint8_t
{
    Number,
    String,
    Boolean,
    NoValue
};

struct Value
{
    ActiveValue activeValue;

    union
    {
        double number;
        bool boolean;
    };

    std::string string;
};

struct Token
{
    Value value;
    TokenType type;
    SourceLocation location;
};

std::vector<Token> lex(std::string_view source);

std::string getTokenName(Token token);