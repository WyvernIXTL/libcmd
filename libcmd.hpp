
/*
 * Copyright (c) 2021, 2023 Adam McKellar
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */


#ifndef LIB_CMD
#define LIB_CMD

#include <list>
#include <vector>
#include <string>
#include <algorithm>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <unordered_map>
#include <functional>

/* ============================================================================================================================== */

int SPACES = 12;

std::string LICENSENOTICE = R"(
This program uses the libcmd library with following copyright notice and license text:

Copyright (c) 2021, 2023 Adam McKellar

Permission to use, copy, modify, and distribute this software for any
purpose with or without fee is hereby granted, provided that the above
copyright notice and this permission notice appear in all copies.

THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
)";


/* ============================================================================================================================== */

enum Type {BOOL, STRING, INT, DOUBLE, LAMBDA};

/**
 * Class for handling Options.
 * 
 * There are 3 inputs: 
 * 1. The variable you want to be overwritten by the parsed argument.
 * 2. All strings to be identified as option/flag.
 * 3. The description for the help message.
 * 4. All strings to be identified as option/flag you do not wish to print.
 */
class Option {
private:
    Type _type;
    std::vector<std::string> _hands;
    std::vector<std::string> _anonymousHands;
    std::string _description;

public:
    union {
        bool* pointerBool;
        std::string* pointerString;
        int* pointerInt;
        double* pointerDouble;
    };  
    std::function<void(void*)> flagLambda;

    Option (bool* pointer, std::vector<std::string> hands, std::string description = "", std::vector<std::string> anonymousHands = {});
    Option (std::string* pointer, std::vector<std::string> hands, std::string description = "", std::vector<std::string> anonymousHands = {});
    Option (int* pointer, std::vector<std::string> hands, std::string description = "", std::vector<std::string> anonymousHands = {});
    Option (double* pointer, std::vector<std::string> hands, std::string description = "", std::vector<std::string> anonymousHands = {});
    Option (std::function<void(void*)> lambda, std::vector<std::string> hands, std::string description = "", std::vector<std::string> anonymousHands = {});

    Type getType();
    std::vector<std::string> getHands();
    std::string getDescription();
    std::vector<std::string> getAnonymousHands();
};


Option::Option (bool* pointer, std::vector<std::string> hands, std::string description, std::vector<std::string> anonymousHands)
        : _hands(hands), _description(description), _type(Type::BOOL), pointerBool(pointer), _anonymousHands(anonymousHands) {}

Option::Option (std::string* pointer, std::vector<std::string> hands, std::string description, std::vector<std::string> anonymousHands)
        : _hands(hands), _description(description), _type(Type::STRING), pointerString(pointer), _anonymousHands(anonymousHands) {} 

Option::Option (int* pointer, std::vector<std::string> hands, std::string description, std::vector<std::string> anonymousHands)
        : _hands(hands), _description(description), _type(Type::INT), pointerInt(pointer), _anonymousHands(anonymousHands) {}

Option::Option (double* pointer, std::vector<std::string> hands, std::string description, std::vector<std::string> anonymousHands)
        : _hands(hands), _description(description), _type(Type::DOUBLE), pointerDouble(pointer), _anonymousHands(anonymousHands) {}

Option::Option (std::function<void(void*)> lambda, std::vector<std::string> hands, std::string description, std::vector<std::string> anonymousHands)
        : _hands(hands), _description(description), _type(Type::LAMBDA), flagLambda(lambda), _anonymousHands(anonymousHands) {}

Type Option::getType() {
    return _type;
}

std::vector<std::string> Option::getHands() {
    return _hands;
}

std::string Option::getDescription() {
    return _description;
}

std::vector<std::string> Option::getAnonymousHands() {
    return _anonymousHands;
}


/* ============================================================================================================================== */

/**
 * Class for parsing command line arguments.
 * 
 * Two lists are given as inputs: Flags and Options.
 * These lists are then on creation of this class used to parse argc and argv arguments of the main class.
 * This class also includes pretty print capabilities of said Flags and Options.
 * SubCommands should not print license.
 */
class CmdParserFrame {
private:
    int _argc;
    char** _argv;

    std::string _commandName;
    std::string _subCommandUsageHeader;
    std::string _programDescription;
    std::string _subCommandDescription;
    std::string _subCommandCascadeString;
    bool* _wasCommandCalled;
    std::vector<CmdParserFrame> _subCommands;
    std::list<Option> _options;

public:
    CmdParserFrame(int argc, char* argv[],
        std::list<Option> options,
        std::vector<CmdParserFrame> subCommands = {}
        );

    CmdParserFrame(int argc, char* argv[],
        std::list<Option> options,
        std::string programName,
        std::string subCommandUsageHeader = "",
        std::string programDescription = "",
        std::string licenseText = "",
        std::vector<CmdParserFrame> subCommands = {}
        );

    CmdParserFrame(std::list<Option> options,
            std::string commandName,
            bool* wasCommandCalled = nullptr,
            std::vector<CmdParserFrame> subCommands = {},
            std::string commandDescription = ""
            );

    void digest();
    void comfortDigest();
    bool isEmpty();
    void printOptions(int spaces = SPACES, std::string prefix = "", std::function<bool(Type)> include = [](Type a){return true;});
    void printAll(int spaces = SPACES, bool andExit = false);

    friend class CmdParserFrame;
};

/// @brief Class to call in CmdParser for subcommands.
class SubCommand : public CmdParserFrame {
    public:
    SubCommand(std::list<Option> options,
        std::string commandName,
        bool* wasCommandCalled = nullptr,
        std::vector<CmdParserFrame> subCommands = {},
        std::string commandDescription = ""
        ) : CmdParserFrame(options, commandName, wasCommandCalled, subCommands, commandDescription) {}
};

/// @brief Class to call in your main function.
class CmdParser : public CmdParserFrame {
    public:
    CmdParser(int argc, char* argv[],
        std::list<Option> options,
        std::string programName = "program",
        std::string subCommandUsageHeader = "",
        std::string programDescription = "",
        std::string licenseText = "",
        std::vector<CmdParserFrame> subCommands = {}
        ) : CmdParserFrame(argc, argv, options, programName, subCommandUsageHeader, programDescription, licenseText, subCommands) {}
};


/* ============================================================================================================================== */


/**
 * @brief Construct a new cmd Parser::cmdParser object for testing purposes without any help or license text and flags. This barebone constructer will result in undefined behaviour!
 * 
 * @param argc Argument count of your main function (probably: "argc").
 * @param argv Array of arguments given to your main function (probably: "argv").
 * @param options Array or Vector of Option class given to parse argv.
 */
CmdParserFrame::CmdParserFrame(int argc, char* argv[],
                    std::list<Option> options,
                    std::vector<CmdParserFrame> subCommands
                    )
    : _argc(argc), _argv(argv), _options(options), _subCommands(subCommands), _commandName("program"), _wasCommandCalled(nullptr), _subCommandCascadeString("program"), _subCommandUsageHeader("")
{}


/**
 * @brief Construct a new Cmd Parser:: Cmd Parser object  This should be the starting point in your main function.
 * 
 * @param argc Argument count of your main function (probably: "argc").
 * @param argv Array of arguments given to your main function (probably: "argv").
 * @param options List/Array of Options given to be parsed. (not for subcommands).
 * @param programName The name of your program.
 * @param subCommandUsageHeader Usage or notice used for all subcommands. Make it short!
 * @param programDescription Description and usage displayed by your programs --help flag.
 * @param licenseText Text displayed for flag --license.
 * @param subCommands Array of CmdParser which will act as subcommands.
 */
CmdParserFrame::CmdParserFrame(int argc, char* argv[],
                    std::list<Option> options,
                    std::string programName,
                    std::string subCommandUsageHeader,
                    std::string programDescription,
                    std::string licenseText,
                    std::vector<CmdParserFrame> subCommands
                    )
    : _argc(argc), _argv(argv), _options(options), _commandName(programName), _subCommandUsageHeader(subCommandUsageHeader), _programDescription(programDescription), _subCommands(subCommands), _wasCommandCalled(nullptr), _subCommandCascadeString(programName)
{
    auto printHelp = [=](void* self){
        std::cout << ((CmdParserFrame*) self)->_programDescription << std::endl;
        ((CmdParserFrame*) self)->printAll();
        exit(0);
    };
    auto printLicense = [=](void* self){
        std::cout << licenseText << "\n" << LICENSENOTICE << std::endl;
        exit(0);
    };
    _options.push_front(Option(printLicense, {"--license"}, "Print licenses.", {"--License", "/License", "/license"}));
    _options.push_front(Option(printHelp, {"-h", "--help"}, "Show this message.", {"/h"}));
    if (this->isEmpty()) printHelp(this);
}


/**
 * @brief Construct a new Cmd Parser:: Cmd Parser object  This should only be used for subcommands. 
 * 
 * @param options List/Array of Options given to be parsed for this SubCommand.
 * @param commandName SubCommand name.
 * @param wasCommandCalled Pointer to bool, which will be set to true if the subcommand is the subcommand called.
 * @param subCommands SubCommands of this SubCommand. If one of them is called => !wasCommandCalled
 * @param commandDescription Description of SubCommand printed by printAll().
 */
CmdParserFrame::CmdParserFrame(std::list<Option> options,
                    std::string commandName,
                    bool* wasCommandCalled,
                    std::vector<CmdParserFrame> subCommands,
                    std::string commandDescription
                    )
                    : _options(options), _commandName(commandName), _wasCommandCalled(wasCommandCalled), _subCommands(subCommands), _subCommandDescription(commandDescription) 
{
    auto printHelp = [&](void* self){
        std::cout << ((CmdParserFrame*) self)->_subCommandUsageHeader << std::endl;
        ((CmdParserFrame*) self)->printAll();
        exit(0);
    };
    _options.push_front(Option(printHelp, {"-h", "--help"}, "Show this message.", {"/h"}));
}


/**
 * @brief Parse command line arguments.
 * 
 * @throws std::invalid_argument if invalid integer is parsed for option of integer type.
 * @throws std::invalid_argument if invalid double is parsed for option of double type.
 * @throws std::invalid_argument if invalid type n > 3 OR n < 1 is given in form of an option.
 */
void CmdParserFrame::digest() {
    if (this->isEmpty()) return;

    if (_argc >= 2){
        for (auto& subCommand : _subCommands) {       
            if (std::string(*(_argv + 1)) == subCommand._commandName) {
                if (subCommand._wasCommandCalled) { *(subCommand._wasCommandCalled) = true; }
                if (_wasCommandCalled) { *_wasCommandCalled = false; }
                subCommand._argc = _argc - 1;
                subCommand._argv = _argv + 1;
                subCommand._subCommandCascadeString = _subCommandCascadeString + " " + subCommand._commandName;
                subCommand._subCommandUsageHeader = _subCommandUsageHeader;
                subCommand.digest();
                return;
            }
        }
    }

    std::unordered_map<std::string, Option*> opt;
    for (auto& elem : _options) {
        for(auto& hand : elem.getHands()) {
            opt.insert({hand, &elem});
        }
        for(auto& hand : elem.getAnonymousHands()) {
            opt.insert({hand, &elem});
        }
    }
    

    for (char** itr = _argv + 1; itr != _argv + _argc; ++itr) {
                auto hand = opt.find(std::string(*itr));
        if (hand != opt.end()) {
            if (hand->second->getType() == BOOL) {
                *(hand->second->pointerBool) = true;
            } else if (hand->second->getType() == LAMBDA) {
                hand->second->flagLambda((void*) this);
            } else {
                if (itr + 1 != _argv + _argc) {
                    if (opt.find(*(itr + 1)) == opt.end()) {
                        switch (hand->second->getType()) 
                        {
                        case STRING:
                            *(hand->second->pointerString) = *(itr + 1);
                            break;

                        case INT:
                            try{
                                *(hand->second->pointerInt) = std::stoi(*(itr + 1));
                            } catch (...) {
                                std::ostringstream oserr;
                                oserr << "ERROR: Expected type >>int<<, but got: " << *(itr + 1);
                                throw std::invalid_argument( oserr.str() );
                            }
                            break;
                        
                        case DOUBLE:
                            try{
                                *(hand->second->pointerDouble) = std::stod(*(itr + 1));
                            } catch (...){
                                std::ostringstream oserr;
                                oserr << "ERROR: Expected type >>double<<, but got: " << *(itr + 1);
                                throw std::invalid_argument( oserr.str() );
                            }
                            break;
                        
                        default:
                            throw std::invalid_argument( "ERROR: unknown parsing from string to <type>" );
                            break;
                        }
                        ++itr;
                    }
                }
            }
        } else {
            std::ostringstream oserr;
            oserr << "ERROR: Unkown argument: " << *(itr);
            throw std::invalid_argument( oserr.str() ); 
        }
    }

}

/** 
 *  Check if arguments are empty.
 * 
 *  @return Return true if there is only one element and false if there are more elements.
 *  @throws std::invalid_argument if there are zero or less arguments.
 *  @throws std::invalid_argument if the input is a nullptr.
 */
bool CmdParserFrame::isEmpty() {
    if(_argc == 1)
        return true;
    else if(_argc < 1)
        throw std::invalid_argument( "ERROR: invalid number of arguments (argv <= 0)" );
    else if(!_argv)
        throw std::invalid_argument( "ERROR: nullptr given instead of char**" );
    return false;
}

/**
 * @brief Digest which does not throw exceptions but rather prints them and exits the program.
 * 
 */
void CmdParserFrame::comfortDigest() {
    try {
        digest();
    } catch(const std::invalid_argument& e) {
        std::cout << e.what() << std::endl;
        exit(1);
    }
}


/**
 * @brief Return n white spaces.
 * 
 * @param n number of spaces.
 * @return std::string with n spaces " ".
 */
std::string space(int n) {
    if(n <= 0){
        return "";
    }
    std::ostringstream os;
    for(int i = 0; i < n; i++)
        os << " ";
    return os.str();
}

/**
 * @brief Return pretty printable string of multiple hands.
 * 
 * @param hand the hands to be printed.
 * @param handsAmount the number of hands to be printed.
 * @param spaces the amount of spaces between the hands.
 * @return std::string single line string of hands dinstanced by handsAmount spaces.
 */
std::string makeHandToString(std::vector<std::string> hand, int handsAmount, int spaces) {
    std::ostringstream os;
    auto makeRoom = [spaces](std::string& hand) {
        return space(spaces - int(hand.length()));
    };
    for(auto& elem : hand) {
        os << elem << makeRoom(elem);
    }
    for(int i = 0; i < handsAmount - int(hand.size()); ++i) {
        os << space(spaces);
    }
    return os.str();
}

/**
 * @brief Return the maximum of count of hands of a single Flag or Option of all Flag or Options.
 * 
 * @param options a list of Options.
 * @param include lambda or function deciding which Types to inlcude.
 * @return int Example: If the flag Help has the hands "-help", "--help", "-h", "-H" and has the most hands returns 4.
 */
int getHandCount(std::list<Option> options, std::function<bool(Type)> inlcude = [](Type a){return true;}) {
    auto max = [](auto a, auto b) {
        if(a > b)
            return a;
        return b;
    };
    int highest = 0;
    for(auto option : options) {
        if (!inlcude(option.getType())) continue;        
        int count = 0;
        for(auto& hand : option.getHands()) {
            count++;
        }
        highest = max(highest, count);
    }
    return highest;
}

/**
 * @brief Pretty print options.
 * 
 * @param spaces The size of the tabs. Make larger for long options.
 * @param prefix First line of print gets a prefix.
 * @param include The data Type of Option you want to include, like BOOL for flags.
 */
void CmdParserFrame::printOptions(int spaces, std::string prefix, std::function<bool(Type)> include) {
    int amountOfFlags = getHandCount(_options, include );
    bool firstLine = true;
    for(auto& opt : _options) {
        if (include(opt.getType())) {
            if (firstLine) {
                firstLine = false;
                std::cout << prefix << space(spaces - int(prefix.length()));
            } else {
                std::cout << space(spaces);
            }
            std::cout << makeHandToString(opt.getHands(), amountOfFlags, spaces) << opt.getDescription() << std::endl;
        }
    }
}


/**
 * @brief Pretty print all hands of all flags and options and description of said structs.
 * 
 * @param spaces the amount of spaces between hands.
 */
void CmdParserFrame::printAll(int spaces, bool andExit) {
    std::cout << "\nUsage for: " << _subCommandCascadeString << "\n" << std::endl;

    printOptions(spaces, "Flags:", [](Type a){return a == BOOL || a == LAMBDA;});
    std::cout << " " << std::endl;
    printOptions(spaces, "Options:", [](Type a){return a != BOOL && a != LAMBDA;});
           
    for (int i = 0; i < _subCommands.size(); ++i) {
        if (i == 0) std::cout << "\nSubcmd:" << space(spaces - 7);
        else std::cout << space(spaces);
        std::cout << _subCommands[i]._commandName << space(spaces*2 - int(_subCommands[i]._commandName.length())) << _subCommands[i]._subCommandDescription << std::endl;
    }
    if (_subCommands.size() == 1)
        std::cout << "\nFor more help: " << _subCommandCascadeString << " " << _subCommands[0]._commandName << " --help\n" << std::endl;
    if (_subCommands.size() > 1)
        std::cout << "\nFor more help: " << _subCommandCascadeString << " [subcmd] --help\n" << std::endl;
    
    if (andExit) exit(0);
}


#endif