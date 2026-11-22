#pragma once
#include <vector>
#include <string>
#include "lexer.hpp"

enum class Context{
    Flag,
    Option,
    MainBody,
    OptionBody,
    FlagBody,
};

struct InstructionValue
{
    Value literal;
    size_t argIndex;
    bool isLiteral;
    bool isSuccesfull;
};

enum class InstructionType
{
    Option,
    Flag,
    Print,
    AddToPayload,
    Target
};

struct CommonInstruction
{
    InstructionType type;                    // for instruction type
    std::string name;                        // for the names of flags and options
    std::vector<InstructionValue> arguments; // for arguments that commands receive. ex: print "Hello","From","Synapse".
    std::vector<CommonInstruction> body;     // the body of options or blocks.

    std::string delimiter; //for flag
    int argCount= 0;
};

struct parseResult
{
    bool didSucced;
    std::vector<CommonInstruction> body;
};

parseResult parse(std::vector<Token> &tokens, size_t &i,Context  context);