#define CATCH_CONFIG_MAIN
#define CATCH_CONFIG_COLOUR_NONE

#include "catch.hpp"
#include "expression.hpp"
#include "tokenize.hpp"
#include "interpreter.hpp"
#include "interpreter_semantic_error.hpp"

#include <sstream>
using namespace std;


// TODO: add your unit tests here
TEST_CASE("Default ", "[Expression]")
{
    Expression exp;
    REQUIRE(exp.head.type == NoneType);
    REQUIRE(exp.tail.empty());
}

TEST_CASE("Boolean ", "[Expression]")
{
    {
        Expression exp(true);
        REQUIRE(exp.head.type == BooleanType);
        REQUIRE(exp.head.value.bool_value == true);
        REQUIRE(exp.tail.empty());
    }

    {
        Expression exp(false);
        REQUIRE(exp.head.type == BooleanType);
        REQUIRE(exp.head.value.bool_value == false);
        REQUIRE(exp.tail.empty());
    }
}

TEST_CASE("Number", "[Expression]")
{
    Expression exp(3.);
    REQUIRE(exp.head.type == NumberType);
    REQUIRE(exp.head.value.num_value == 3);
    REQUIRE(exp.tail.empty());
}


TEST_CASE("Tokenize Function Tests", "[tokenize]")
{
    {
        std::stringstream input("(+ 1 2 3)");
        TokenSequenceType tokens = tokenize(input);
        REQUIRE(tokens.size() == 6);
        REQUIRE(tokens[0] == "(");
        REQUIRE(tokens[1] == "+");
        REQUIRE(tokens[2] == "1");
        REQUIRE(tokens[3] == "2");
        REQUIRE(tokens[4] == "3");
    }

    {
        std::stringstream input("1 2 ; this is a comment \n 3");
        TokenSequenceType tokens = tokenize(input);
        REQUIRE(tokens.size() == 3);
        REQUIRE(tokens[0] == "1");
        REQUIRE(tokens[1] == "2");
        REQUIRE(tokens[2] == "3");
    }

    {
        std::stringstream input("((a))");
        TokenSequenceType tokens = tokenize(input);
        REQUIRE(tokens.size() == 5);
        REQUIRE(tokens[0] == "(");
        REQUIRE(tokens[1] == "(");
        REQUIRE(tokens[2] == "a");
        REQUIRE(tokens[3] == ")");
        REQUIRE(tokens[4] == ")");
    }

    {
        std::stringstream input("  a   b  c ");
        TokenSequenceType tokens = tokenize(input);
        REQUIRE(tokens.size() == 3);
        REQUIRE(tokens[0] == "a");
        REQUIRE(tokens[1] == "b");
        REQUIRE(tokens[2] == "c");
    }

    {
        std::stringstream input("");
        TokenSequenceType tokens = tokenize(input);
        REQUIRE(tokens.size() == 0);
    }

}
