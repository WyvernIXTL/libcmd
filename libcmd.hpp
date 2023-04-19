
/*
 * Copyright (C) 2021 Adam McKellar - All Rights Reserved
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */


#ifndef LIB_CMD
#define LIB_CMD

#include <vector>
#include <tuple>
#include <string>
#include <algorithm>
#include <iostream>
#include <sstream>

/* ============================================================================================================================== */

int SPACES = 12;

/* ============================================================================================================================== */

struct Flag
{
    bool* pointer;
    std::vector<std::string> hands;
    std::string description;
};

struct Option
{
    std::string* pointer;
    int* pointerInt;
    double* pointerDouble;
    std::vector<std::string> hands;
    std::string description;
    int type;
};

Flag make_flag(bool* pointer, std::vector<std::string> hands, std::string description){
    Flag flaggy;
    flaggy.pointer = pointer;
    flaggy.hands = hands;
    flaggy.description = description;
    return flaggy;
}

Option make_flag(std::string* pointer, std::vector<std::string> hands, std::string description){
    Option flaggy;
    flaggy.pointer = pointer;
    flaggy.hands = hands;
    flaggy.description = description;
    flaggy.type = 1;
    return flaggy;
}

Flag make_flag(bool* pointer, std::vector<std::string> hands){
    return make_flag(pointer, hands, "");
}

Option make_flag(std::string* pointer, std::vector<std::string> hands){
    return make_flag(pointer, hands, "");
}

Flag make_option(bool* pointer, std::vector<std::string> hands, std::string description){
    return make_flag(pointer, hands, description);
}

Option make_option(std::string* pointer, std::vector<std::string> hands, std::string description){
    return make_flag(pointer, hands, description);
}

Flag make_option(bool* pointer, std::vector<std::string> hands){
    return make_flag(pointer, hands, "");
}

Option make_option(std::string* pointer, std::vector<std::string> hands){
    return make_flag(pointer, hands, "");
}

Option make_option(int* pointer, std::vector<std::string> hands, std::string description){
    Option flaggy;
    flaggy.pointerInt = pointer;
    flaggy.hands = hands;
    flaggy.description = description;
    flaggy.type = 2;
    return flaggy;
}

Option make_option(int* pointer, std::vector<std::string> hands){
    return make_option(pointer, hands, "");
}

Option make_option(double* pointer, std::vector<std::string> hands, std::string description){
    Option flaggy;
    flaggy.pointerDouble = pointer;
    flaggy.hands = hands;
    flaggy.description = description;
    flaggy.type = 3;
    return flaggy;
}

Option make_option(double* pointer, std::vector<std::string> hands){
    return make_option(pointer, hands, "");
}


/* ============================================================================================================================== */

class cmdParser
{
private:
    int _argc;
    char** _argv;
    std::vector<Flag> _flags; //&bool, Shorthand, Long, description 
    std::vector<Option> _options; //&options, Shorthand, Long, description 

    bool _flagExists(char** begin, char** end, const std::string flag);
    std::string _retrieveOption(char** begin, char** end, const std::string option);

public:
    cmdParser(int argc, char* argv[],
        std::vector<Flag> flags,
        std::vector<Option> options);

    void digest();
    bool isEmpty();
    void printFlags();
    void printFlags(int spaces);
    void printOptions();
    void printOptions(int spaces);
    void printAll();
    void printAll(int spaces);
};


/* ============================================================================================================================== */

void cmdParser::digest(){
    if(this->isEmpty()) return;
    std::vector<std::string> inputStringsNotAlien;
    for(auto& elem : _flags){
        *(elem.pointer) = false;
        for(auto& hand : elem.hands) {
            if(_flagExists(_argv, _argv+_argc, hand)){
                *(elem.pointer) = *(elem.pointer) || true;
                inputStringsNotAlien.push_back(hand);
            }      
        }
    }
    for(auto& elem : _options){
        if(elem.type == 1) {
            *(elem.pointer) = "";
            for(auto& hand : elem.hands) {
                if(_flagExists(_argv, _argv+_argc, hand)){
                    *(elem.pointer) = *(elem.pointer) + _retrieveOption(_argv, _argv+_argc, hand);
                    inputStringsNotAlien.push_back(hand);
                    inputStringsNotAlien.push_back(*(elem.pointer));
                }     
            }
        } else if(elem.type == 2){
            std::string CacheIntString = "";
            for(auto& hand : elem.hands) {
                if(_flagExists(_argv, _argv+_argc, hand)){
                    CacheIntString = CacheIntString + _retrieveOption(_argv, _argv+_argc, hand);
                    inputStringsNotAlien.push_back(hand);
                    inputStringsNotAlien.push_back(CacheIntString);
                }
            }
            if(CacheIntString != ""){
                try{
                    *(elem.pointerInt) = std::stoi(CacheIntString);
                } catch (...){
                    std::cout << "ERROR: Expected type >>int<<, but got: " << CacheIntString << std::endl;
                    abort();
                }
            }
        } else if(elem.type == 3){
            std::string CacheIntString = "";
            for(auto& hand : elem.hands) {
                if(_flagExists(_argv, _argv+_argc, hand)){
                    CacheIntString = CacheIntString + _retrieveOption(_argv, _argv+_argc, hand);
                    inputStringsNotAlien.push_back(hand);
                    inputStringsNotAlien.push_back(CacheIntString);
                }          
            }
            if(CacheIntString != ""){
                try{
                    *(elem.pointerDouble) = std::stod(CacheIntString);
                } catch (...){
                    std::cout << "ERROR: Expected type >>double<<, but got: " << CacheIntString << std::endl;
                    abort();
                }
            }
        } else {
            std::cout << "something went wrong" << std::endl;
            abort();
        }        
    }
    for(char** itr = _argv + 1; itr != _argv + _argc; ++itr){
        if(std::find(inputStringsNotAlien.begin(), inputStringsNotAlien.end(), *itr) == inputStringsNotAlien.end()){
            std::cout << "ERROR: Unknown Argument " << *itr << std::endl;
            abort();
        }
    }
    
}

bool cmdParser::isEmpty(){
    if(_argc == 1)
        return true;
    return false;
}


cmdParser::cmdParser(int argc, char* argv[],
        std::vector<Flag> flags,
        std::vector<Option> options)
    : _argc(argc), _argv(argv), _flags(flags), _options(options)
{
    digest();
}

std::string cmdParser::_retrieveOption(char** begin, char** end, const std::string option)
{
    char** itr = std::find(begin, end, option);
    if(itr != end && itr + 1 != end){
        return *(itr+1);
    }
    return "";
}

bool cmdParser::_flagExists(char** begin, char** end, const std::string flag)
{
    return std::find(begin, end, flag) != end;
}

std::string space(int n){
    if(n <= 0){
        return "";
    }
    std::ostringstream os;
    for(int i = 0; i < n; i++)
        os << " ";
    return os.str();
}

std::string makeHandToString(std::vector<std::string> hand, int handsAmount, int spaces){
    std::ostringstream os;
    auto makeRoom = [spaces](std::string& hand){
        return space(spaces - hand.length());
    };
    for(auto& elem : hand){
        os << elem << makeRoom(elem);
    }
    for(int i = 0; i < handsAmount - int(hand.size()); ++i){
        os << space(spaces);
    }
    return os.str();
}

template<typename T>
int getHandCount(std::vector<T> hands){
    auto max = [](auto a, auto b){
        if(a > b)
            return a;
        return b;
    };
    int highest = 0;
    for(auto elem : hands){
        int count = 0;
        for(auto hand : elem.hands){
            count++;
        }
        highest = max(highest, count);
    }
    return highest;
}

void cmdParser::printFlags(int spaces){
    int amountOfFlags = getHandCount(_flags);
    for(std::size_t i = 0; i < _flags.size(); ++i){
        if(i == 0)
            std::cout << "FLAGS:" << space(spaces-6) << makeHandToString(_flags[i].hands, amountOfFlags, spaces) << _flags[i].description << std::endl;
        else
            std::cout << space(spaces) << makeHandToString(_flags[i].hands, amountOfFlags, spaces) << _flags[i].description << std::endl;
    }
}

void cmdParser::printFlags(){
    printFlags(SPACES);
}

void cmdParser::printOptions(int spaces){
    int amountOfFlags = getHandCount(_options);
    for(std::size_t i = 0; i < _options.size(); ++i){
        if(i == 0)
            std::cout << "OPTIONS:" << space(spaces-8) << makeHandToString(_options[i].hands, amountOfFlags, spaces) << _options[i].description << std::endl;
        else
            std::cout << space(spaces) << makeHandToString(_options[i].hands, amountOfFlags, spaces) << _options[i].description << std::endl;
    }
}

void cmdParser::printOptions(){
    printOptions(SPACES);
}

void cmdParser::printAll(int spaces){
    printFlags(spaces);
    std::cout << " " << std::endl;
    printOptions(spaces);
}

void cmdParser::printAll(){
    printAll(SPACES);
}

#endif