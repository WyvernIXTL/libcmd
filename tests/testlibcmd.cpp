
/*
 * Copyright (C) 2023 Adam McKellar
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */


#include <catch2/catch_test_macros.hpp>
#include "../libcmd.hpp"


TEST_CASE( "parseEmptyNoFlagsAndOptions", "[empty]" ) {
    auto executor = [](int argc, char** argv) {
        CmdParserFrame pars {
            argc,
            argv,
            {}
        };
        pars.digest();
    };

    REQUIRE_THROWS( executor(0, nullptr) );
    REQUIRE_THROWS( executor(2, nullptr) );
    REQUIRE_NOTHROW( executor(1, nullptr) );
}

TEST_CASE( "parseGoodFlags", "[flags]" ) {
    bool help = false;
    bool verbose = false;
    bool flaggy = false;

    const char* argv[] = {"programm", "--help2", "wasd", nullptr};

    CmdParserFrame pars {
        3,
        const_cast<char**>(argv),
        {
            Option(&help, {"-h2", "--help2", "--Usage2"}, "Shows this message"),
            Option(&verbose, {"--verbose"}),
            Option(&flaggy, {"wasd"})
        }
    };
    pars.digest();

    REQUIRE(help);
    REQUIRE(!verbose);
    REQUIRE(flaggy);
}

TEST_CASE( "parseGoodOptions", "[options]" ) {
    std::string inputStr;
    int inputInt = 0;
    double inputDouble = 0.0;


    SECTION( "string and postive numbers" ) {

        const char* argv[] = {"programm", "-s", "wasd", "-i", "345", "-d", "345.678", nullptr};

        CmdParserFrame pars {
            7,
            const_cast<char**>(argv),
            {
                Option(&inputStr, {"-s", "--string"}, "input string"),
                Option(&inputInt, {"-i", "--int"}, "input int"),
                Option(&inputDouble, {"-d", "--double"}, "input double"),
            }
        };
        pars.digest();

        REQUIRE(inputStr == "wasd");
        REQUIRE(inputInt == 345);
        REQUIRE(inputDouble == 345.678);

    }

    SECTION( "negative numbers" ){

        const char* argv[] = {"programm", "-i", "-345", "-d", "-345.678", nullptr};

        CmdParserFrame pars {
            5,
            const_cast<char**>(argv),
            {
                Option(&inputInt, {"-i", "--int"}, "input int"),
                Option(&inputDouble, {"-d", "--double"}, "input double"),
            }
        };
        pars.digest();

        REQUIRE(inputInt == -345);
        REQUIRE(inputDouble == -345.678);

    }



    SECTION( "overwriting options" ) {

        const char* argv[] = {"programm", "-s", "wasd", "-s", "qwertz", nullptr};

        CmdParserFrame pars {
            5,
            const_cast<char**>(argv),
            {
                Option(&inputStr, {"-s", "--string"}, "input string"),
                Option(&inputInt, {"-i", "--int"}, "input int"),
                Option(&inputDouble, {"-d", "--double"}, "input double"),
            }
        };
        pars.digest();

        REQUIRE(inputStr == "qwertz");

    }
    

}


TEST_CASE( "parseAnonymousOptions", "[anonoptions]") {
    SECTION( "anon option of all kinds" ) {
        bool vlog = false;
        std::string inputStr;
        int inputInt = 0;
        double inputDouble = 0.0;

        const char* argv[] = {"programm", "-s", "wasd", "-i", "345", "-d", "345.678", "-vlog", nullptr};

        CmdParserFrame pars {
            8,
            const_cast<char**>(argv),
            {   
                Option(&vlog, {}, "", {"-vlog"}),
                Option(&inputStr, {}, "input string", {"-s", "--string"}),
                Option(&inputInt, {}, "input int", {"-i", "--int"}),
                Option(&inputDouble, {}, "input double", {"-d", "--double"}),
            }
        };
        pars.digest();

        REQUIRE(vlog);
        REQUIRE(inputStr == "wasd");
        REQUIRE(inputInt == 345);
        REQUIRE(inputDouble == 345.678);
    }
}


TEST_CASE( "parseBadOptions", "[badoptions]" ) {

    SECTION( "options without value" ){
        const char* argv[] = {"programm", "-s", "-s2", "qwertz", nullptr};

        std::string inputStr = "NONE";
        std::string inputStr2 = "NONE";

        CmdParserFrame pars {
            4,
            const_cast<char**>(argv),
            {
                Option(&inputStr, {"-s", "--string"}, "input string"),
                Option(&inputStr2, {"-s2", "--string2"}, "input string"),
            }
        };
        pars.digest();

        REQUIRE(inputStr == "NONE");
        REQUIRE(inputStr2 == "qwertz");
    }

    SECTION( "bad numbers int" ) {
        const char* argv[] = {"programm", "-i", "abc", nullptr};

        int inputInt = 0;

        CmdParserFrame pars {
            3,
            const_cast<char**>(argv),
            {
                Option(&inputInt, {"-i", "--int"}, "input int"),
            }
        };

        REQUIRE_THROWS(pars.digest());        
    }

    SECTION( "bad numbers double" ) {
        const char* argv[] = {"programm", "-d", "abc", nullptr};

        double inputDouble = 0.0;

        CmdParserFrame pars {
            3,
            const_cast<char**>(argv),
            {
                Option(&inputDouble, {"-d", "--double"}, "input double"),
            }
        };

        REQUIRE_THROWS(pars.digest());        
    }

    SECTION( "unkown argument" ) {
        std::string inputStr;

        const char* argv[] = {"programm", "-s", "wasd", "unkown", "-s", "qwertz", nullptr};

        CmdParserFrame pars {
            6,
            const_cast<char**>(argv),
            {
                Option(&inputStr, {"-s", "--string"}, "input string"),
            }
        };

        REQUIRE_THROWS(pars.digest());

    }

}


TEST_CASE( "parseSubcommands", "parsesubcommands" ) {
    SECTION( "one subcommand" ) {
        std::string inputStr;
        bool subGotCalled = false;

        const char* argv[] = {"programm", "sub", "-s", "wasd", nullptr};

        CmdParserFrame pars {
            4,
            const_cast<char**>(argv),
            {
                Option(&inputStr, {"-s", "--string"}, "input string")
            },
            {
                CmdParserFrame(
                    {
                        Option(&inputStr, {"-s", "--string"}, "input string")
                    },
                    "sub",
                    &subGotCalled
                )
            }
        };

        pars.digest();

        REQUIRE(subGotCalled);
        REQUIRE(inputStr == "wasd");
    }

    SECTION( "more subcommands" ) {
        std::string inputStr;
        int num = 0;
        double dub = 0.0;
        bool flag = false;

        bool subGotCalled = false;
        bool bahwaGotCalled = false;
        bool buhuGotCalled = false;

        const char* argv[] = {"programm", "sub", "BAHWA", "-s2", "wasd", "--num", "6", "--dub", "345.567", "--flag",  nullptr};

        CmdParserFrame pars {
            10,
            const_cast<char**>(argv),
            {
                Option(&inputStr, {"-s", "--string"}, "input string")
            },
            {
                CmdParserFrame(
                    {
                        Option(&inputStr, {"-s", "--string"}, "input string")
                    },
                    "sub",
                    &subGotCalled,
                    {
                        CmdParserFrame(
                            {
                                Option(&inputStr, {"-s2", "--string2"}, "input string")
                            },
                            "buhu",
                            &buhuGotCalled
                        ),
                        CmdParserFrame(
                            {
                                Option(&inputStr, {"-s2", "--string2"}, "input string"),
                                Option(&num, {"--num"}),
                                Option(&dub, {"--dub"}),
                                Option(&flag, {"--flag"})
                            },
                            "BAHWA",
                            &bahwaGotCalled
                        )
                    }
                )
            }
        };

        pars.digest();

        REQUIRE(!subGotCalled);
        REQUIRE(bahwaGotCalled);
        REQUIRE(!buhuGotCalled);
        REQUIRE(num == 6);
        REQUIRE(dub == 345.567);
        REQUIRE(flag);
        REQUIRE(inputStr == "wasd");
    }

    SECTION( "deep recursion" ) {
        std::string inputStr;
        int num = 0;
        double dub = 0.0;
        bool flag = false;

        bool subGotCalled = false;
        bool bahwaGotCalled = false;

        const char* argv[] = {"programm", "sub", "subsub", "BAHWA", "-s2", "wasd", "--num", "6", "--dub", "345.567", "--flag",  nullptr};

        CmdParserFrame pars {
            11,
            const_cast<char**>(argv),
            {
                Option(&inputStr, {"-s", "--string"}, "input string")
            },
            {
                CmdParserFrame(
                    {
                        Option(&inputStr, {"-s", "--string"}, "input string")
                    },
                    "sub",
                    &subGotCalled,
                    {
                        CmdParserFrame(
                            {},
                            "subsub",
                            nullptr,
                            {
                                CmdParserFrame(
                                    {
                                        Option(&inputStr, {"-s2", "--string2"}, "input string"),
                                        Option(&num, {"--num"}),
                                        Option(&dub, {"--dub"}),
                                        Option(&flag, {"--flag"})
                                    },
                                    "BAHWA",
                                    &bahwaGotCalled
                                )
                            }
                        )
                    }
                )
            }
        };

        pars.digest();

        REQUIRE(!subGotCalled);
        REQUIRE(bahwaGotCalled);
        REQUIRE(num == 6);
        REQUIRE(dub == 345.567);
        REQUIRE(flag);
        REQUIRE(inputStr == "wasd");
    }
}
