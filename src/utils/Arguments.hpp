#pragma once

#include <stdexcept>
#include <string>
#include <unordered_map>

struct ArgumentError : std::runtime_error
{
    ArgumentError(std::string const& message)
    : std::runtime_error(message)
    {
    }
};

// TODO: make the argument customizable and use it
class Arguments
{
public:
    Arguments(int argc, char * argv[])
    {
        command = argv[0];

        for (int index = 1; index < argc; index++)
        {
            std::string const arg = argv[index];

            if (arg ==  "--from")
            {
                if (++index < argc)
                {
                    flags[arg] = argv[index];
                }
                else
                {
                    throw ArgumentError("missing value for flag --from");
                }
            }
            else if (arg ==  "--to")
            {
                if (++index < argc)
                {
                    flags[arg] = argv[index];
                }
                else
                {
                    throw ArgumentError("missing value for flag --to");
                }
            }
            else if (arg ==  "--help")
            {
                flags[arg] = "";
            }
        }
    }

    std::string getCommand() const
    {
        return command;
    }

    std::string getFlag(std::string name) const
    {
        auto it = flags.find(name);

        if (it == flags.end())
        {
            throw ArgumentError("invalid flag");
        }

        return it->second;
    }

private:
    std::string command;
    std::unordered_map<std::string, std::string> flags;
};
