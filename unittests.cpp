#define CATCH_CONFIG_MAIN
#define CATCH_CONFIG_COLOUR_NONE
#include "catch.hpp"


#include "expression.hpp"
#include "tokenize.hpp"
#include "interpreter.hpp"
#include "interpreter_semantic_error.hpp"

#include <sstream>
using namespace std;

static Expression run(const std::string& program)
{
    std::istringstream iss(program);

    Interpreter interp;

    bool ok = interp.parse(iss);
    if (!ok)
    {
        std::cerr << "Failed to parse: " << program << std::endl;
        throw std::runtime_error("Parse error");
    }

    Expression result;
    try
    {
        result = interp.eval();
    }
    catch (const InterpreterSemanticError& err)
    {
        std::cerr << "Error: " << err.what() << std::endl;
        throw;  // rethrow the semantic error
    }

    return result;
}

//TESTS FOR EXPRESSION MODULE
TEST_CASE("Constructors and equality operators", "[expression]")
{
    SECTION("Default constructor")
    {
        Expression exp;
        REQUIRE(exp == Expression());
    }

    SECTION("Boolean constructor")
    {
        Expression trueExp(true);
        Expression falseExp(false);

        REQUIRE(trueExp == Expression(true));
        REQUIRE(falseExp == Expression(false));
        REQUIRE_FALSE(trueExp == falseExp);
    }

    SECTION("Number constructor")
    {
        Expression numExp(5.5);
        REQUIRE(numExp == Expression(5.5));
        REQUIRE_FALSE(numExp == Expression(6.0));
    }

    SECTION("Symbol constructor")
    {
        Expression symbolExp(std::string("symbol"));
        REQUIRE(symbolExp == Expression(std::string("symbol")));
        REQUIRE_FALSE(symbolExp == Expression(std::string("other")));
    }
}

TEST_CASE("Stream operator function", "[expression]")
{
    std::ostringstream os;

    SECTION("Boolean output")
    {
        Expression exp(true);
        os << exp;
        REQUIRE(os.str() == "True");
    }

    SECTION("Number output")
    {
        Expression exp(5.5);
        os << exp;
        REQUIRE(os.str() == "5.5");
    }

    SECTION("Symbol output")
    {
        Expression exp(std::string("symbol"));
        os << exp;
        REQUIRE(os.str() == "symbol");
    }
}

TEST_CASE("Token to atom conversion with edge cases", "[expression]")
{
    Atom atom;

    SECTION("Empty token")
    {
        REQUIRE_FALSE(token_to_atom("", atom));
    }

    SECTION("Whitespace token")
    {
        REQUIRE_FALSE(token_to_atom(" ", atom));
    }

    SECTION("Number tokens with different formats")
    {
        REQUIRE(token_to_atom("5", atom));
        REQUIRE(atom.type == NumberType);
        REQUIRE(atom.value.num_value == Approx(5));

        REQUIRE(token_to_atom("-5.5", atom));
        REQUIRE(atom.type == NumberType);
        REQUIRE(atom.value.num_value == Approx(-5.5));

        REQUIRE(token_to_atom("1e4", atom));
        REQUIRE(atom.type == NumberType);
        REQUIRE(atom.value.num_value == Approx(10000));

        REQUIRE(token_to_atom("-1.5e4", atom));
        REQUIRE(atom.type == NumberType);
        REQUIRE(atom.value.num_value == Approx(-15000));

        REQUIRE_FALSE(token_to_atom("1..5", atom));
        REQUIRE_FALSE(token_to_atom("1.5.1", atom));
    }

    SECTION("Symbol tokens with edge cases")
    {
        REQUIRE(token_to_atom("symbol", atom));
        REQUIRE(atom.type == SymbolType);
        REQUIRE(atom.value.sym_value == "symbol");

        REQUIRE(token_to_atom("symbol_with_underscores", atom));
        REQUIRE(atom.type == SymbolType);
        REQUIRE(atom.value.sym_value == "symbol_with_underscores");

        REQUIRE_FALSE(token_to_atom("123InvalidSymbol", atom));
        REQUIRE_FALSE(token_to_atom("123 InvalidSymbol", atom));
    }
}

TEST_CASE("Expression equality with edge cases", "[expression]")
{
    SECTION("Comparing different types")
    {
        Expression boolExp(true);
        Expression numExp(5.5);
        Expression symbolExp(string("symbol"));

        REQUIRE_FALSE(boolExp == numExp);
        REQUIRE_FALSE(boolExp == symbolExp);
        REQUIRE_FALSE(numExp == symbolExp);
    }

    SECTION("Comparing same type with different values")
    {
        Expression numExp1(5.5);
        Expression numExp2(6.0);
        REQUIRE_FALSE(numExp1 == numExp2);

        Expression symbolExp1(string("symbol"));
        Expression symbolExp2(string("symbol2"));
        REQUIRE_FALSE(symbolExp1 == symbolExp2);
    }
}

TEST_CASE("Tokenization with comments", "[tokenize]") 
{
    std::istringstream input("(define x 10) ; this is a comment(+ x 5)");
    TokenSequenceType tokens = tokenize(input);

    REQUIRE(tokens.size() == 5);
    REQUIRE(tokens[0] == "(");
    REQUIRE(tokens[1] == "define");
    REQUIRE(tokens[2] == "x");
    REQUIRE(tokens[3] == "10");
    REQUIRE(tokens[4] == ")");
}

// Error Cases for Special Forms and Procedures
TEST_CASE("Define with non-symbol identifier", "[interpreter]")
{
    std::string program = "(define 123 456)";
    Interpreter interp;
    std::istringstream iss(program);
    REQUIRE(interp.parse(iss));
    REQUIRE_THROWS_AS(interp.eval(), InterpreterSemanticError);
}

TEST_CASE("If with non-boolean condition", "[interpreter]")
{
    std::string program = "(if 1 (+ 1 1) (- 1 1))";
    Interpreter interp;
    std::istringstream iss(program);
    REQUIRE(interp.parse(iss));
    REQUIRE_THROWS_AS(interp.eval(), InterpreterSemanticError);
}

TEST_CASE("Procedure with incorrect argument types", "[interpreter]")
{
    std::string program = "(+ True False)";
    Interpreter interp;
    std::istringstream iss(program);
    REQUIRE(interp.parse(iss));
    REQUIRE_THROWS_AS(interp.eval(), InterpreterSemanticError);
}

// Nested and Complex Expressions
TEST_CASE("Deeply nested expressions", "[interpreter]")
{
    std::string program = "(define a (if (< (* 2 3) 8) (+ 1 (if True 2 3)) 4))";
    Expression result = run(program);
    REQUIRE(result == Expression(3.));
}


// Division by Zero
TEST_CASE("Division by zero", "[interpreter]")
{
    std::string program = "(/ 1 0)";
    Interpreter interp;
    std::istringstream iss(program);
    REQUIRE(interp.parse(iss));
    REQUIRE_THROWS_AS(interp.eval(), InterpreterSemanticError);
}

// Invalid Syntax
TEST_CASE("Unmatched parentheses", "[interpreter]")
{
    std::string program = "(+ 1 (- 2 3)";
    Interpreter interp;
    std::istringstream iss(program);
    REQUIRE_FALSE(interp.parse(iss));
}

// Edge Cases for Numbers
TEST_CASE("Extremely large number", "[interpreter]")
{
    std::string program = "(+ 1e308 1e308)"; // This will result in infinity
    Expression result = run(program);
    REQUIRE(std::isinf(result.head.value.num_value));
}

// Comments Handling
TEST_CASE("Program with comments", "[interpreter]")
{
    std::string program = ";; This is a comment\n(+ 1 2)";
    Expression result = run(program);
    REQUIRE(result == Expression(3.));
}

// File Handling 
TEST_CASE("Execute from invalid file", "[interpreter]")
{
    std::string fname = "non_existent_file.slp";
    std::ifstream ifs(fname);
    Interpreter interp;
    REQUIRE_FALSE(interp.parse(ifs));
}


TEST_CASE("Test Interpreter procedures with no arguments", "[interpreter]")
{
    std::string notNoArgs = "(foo)";
    REQUIRE_THROWS_AS(run(notNoArgs), InterpreterSemanticError);

    std::string andNoArgs = "(and)";
    REQUIRE_THROWS_AS(run(andNoArgs), InterpreterSemanticError);

    std::string orNoArgs = "(or)";
    REQUIRE_THROWS_AS(run(orNoArgs), InterpreterSemanticError);

    std::string addNoArgs = "(+)";
    REQUIRE_THROWS_AS(run(addNoArgs), InterpreterSemanticError);

    std::string logNoArgs = "(log10)";
    REQUIRE_THROWS_AS(run(logNoArgs), InterpreterSemanticError);

    std::string powNoArgs = "(pow)";
    REQUIRE_THROWS_AS(run(powNoArgs), InterpreterSemanticError);
}

TEST_CASE("Test addSymbol and get", "[environment]")
{
    Environment env;
    Symbol testSymbol = "testSymbol";
    Expression testExpression(42.0);

    // Test adding a symbol
    env.addSymbol(testSymbol, testExpression);

    // Test retrieving the added symbol
    REQUIRE(env.get(testSymbol) == testExpression);

    // Test retrieving a non-existent symbol
    REQUIRE_THROWS_AS(env.get("nonExistentSymbol"), InterpreterSemanticError);

    // Now the symbol should be defined
    REQUIRE(env.isSymbolDefined(testSymbol));
}


TEST_CASE("Not procedure", "[interpreter]")
{
    Interpreter interp;

    // Test notProcedure
    {
        istringstream iss("(not True)");
        REQUIRE(interp.parse(iss) == true);
        REQUIRE(interp.eval() == Expression(false));

        istringstream iss2("(not False)");
        REQUIRE(interp.parse(iss2) == true);
        REQUIRE(interp.eval() == Expression(true));

        istringstream iss3("(not 5)");
        REQUIRE(interp.parse(iss3) == true);
        REQUIRE_THROWS_AS(interp.eval(), InterpreterSemanticError);
    }
}

TEST_CASE("and procedure", "[interpreter]")
{
    Interpreter interp;
    // Test andProcedure
    {
        istringstream iss("(and True True)");
        REQUIRE(interp.parse(iss) == true);
        REQUIRE(interp.eval() == Expression(true));

        istringstream iss2("(and True False)");
        REQUIRE(interp.parse(iss2) == true);
        REQUIRE(interp.eval() == Expression(false));

        istringstream iss3("(and True 5)");
        REQUIRE(interp.parse(iss3) == true);
        REQUIRE_THROWS_AS(interp.eval(), InterpreterSemanticError);
    }
}

TEST_CASE("or procedure", "[interpreter]")
{
    Interpreter interp;
    // Test orProcedure
    {
        istringstream iss("(or True False)");
        REQUIRE(interp.parse(iss) == true);
        REQUIRE(interp.eval() == Expression(true));

        istringstream iss2("(or False False)");
        REQUIRE(interp.parse(iss2) == true);
        REQUIRE(interp.eval() == Expression(false));

        istringstream iss3("(or True 5)");
        REQUIRE(interp.parse(iss3) == true);
        REQUIRE_THROWS_AS(interp.eval(), InterpreterSemanticError);
    }
}

TEST_CASE("ADD procedure", "[interpreter]")
{
    Interpreter interp;
    // Test ADDProcedure
    {
        istringstream iss("(+ 5 3)");
        REQUIRE(interp.parse(iss) == true);
        REQUIRE(interp.eval() == Expression(8.0));

        istringstream iss2("(+ 5 True)");
        REQUIRE(interp.parse(iss2) == true);
        REQUIRE_THROWS_AS(interp.eval(), InterpreterSemanticError);
    }
}

TEST_CASE("Subtract procedure", "[interpreter]")
{
    Interpreter interp;
    // Test subtractProcedure
    {
        istringstream iss("(- 5)");
        REQUIRE(interp.parse(iss) == true);
        REQUIRE(interp.eval() == Expression(-5.0));

        istringstream iss2("(- 5 3)");
        REQUIRE(interp.parse(iss2) == true);
        REQUIRE(interp.eval() == Expression(2.0));

        istringstream iss3("(- 5 True)");
        REQUIRE(interp.parse(iss3) == true);
        REQUIRE_THROWS_AS(interp.eval(), InterpreterSemanticError);
    }
}

TEST_CASE("Multiply procedure", "[interpreter]")
{
    Interpreter interp;
    // Test multiplyProcedure
    {
        istringstream iss("(* 5 3)");
        REQUIRE(interp.parse(iss) == true);
        REQUIRE(interp.eval() == Expression(15.0));

        istringstream iss2("(* 5 True)");
        REQUIRE(interp.parse(iss2) == true);
        REQUIRE_THROWS_AS(interp.eval(), InterpreterSemanticError);
    }
}

TEST_CASE("divide procedure", "[interpreter]")
{
    Interpreter interp;
    // Test divideProcedure
    {
        istringstream iss("(/ 6 3)");
        REQUIRE(interp.parse(iss) == true);
        REQUIRE(interp.eval() == Expression(2.0));

        istringstream iss2("(/ 6 0)");
        REQUIRE(interp.parse(iss2) == true);
        REQUIRE_THROWS_AS(interp.eval(), InterpreterSemanticError);

        istringstream iss3("(/ 6 True)");
        REQUIRE(interp.parse(iss3) == true);
        REQUIRE_THROWS_AS(interp.eval(), InterpreterSemanticError);
    }
}

TEST_CASE("less than procedure", "[interpreter]")
{
    Interpreter interp;
    // Test lessThanProcedure
    {
        istringstream iss("(< 3 5)");
        REQUIRE(interp.parse(iss) == true);
        REQUIRE(interp.eval() == Expression(true));

        istringstream iss2("(< 5 3)");
        REQUIRE(interp.parse(iss2) == true);
        REQUIRE(interp.eval() == Expression(false));

        istringstream iss3("(< 5 True)");
        REQUIRE(interp.parse(iss3) == true);
        REQUIRE_THROWS_AS(interp.eval(), InterpreterSemanticError);
    }
}

TEST_CASE("Test for Built-in Procedure: sin", "[interpreter]")
{
    Interpreter interp;

    {
        std::istringstream input("(sin pi)");
        REQUIRE(interp.parse(input));
        REQUIRE(interp.eval() == Expression(0.));
    }

    {
        std::istringstream input("(sin 'a')");
        REQUIRE(interp.parse(input));
        REQUIRE_THROWS_AS(interp.eval(), InterpreterSemanticError);
    }
}


TEST_CASE("Test Interpreter resetEnvironment", "[interpreter]")
{
    Interpreter interp;
    Environment env;

    {
        std::istringstream input("(define a 10)");
        interp.parse(input);
        interp.eval();

        // Reset environment
        interp.resetEnvironment();

        // a should no longer be defined
        REQUIRE(env.isSymbolDefined("a") == false);
    }

	{
		Interpreter interpreter;
		std::istringstream input("(define a 10)");
		interpreter.parse(input);
		interpreter.eval();

		REQUIRE(interpreter.isSymbolStringDefined("a") == true);
		REQUIRE(interpreter.isSymbolStringDefined("nonExistentSymbol") == false);
	}

    {
        Interpreter interpreter;
        std::istringstream invalidInput("(begin (define r 10) (* pi (* r))"); 

        REQUIRE(interpreter.parse(invalidInput) == false);

        // Testing eval without a valid parse
        REQUIRE_THROWS_AS(interpreter.eval(), InterpreterSemanticError);
    }

}


TEST_CASE("Test for unsupported operation", "[interpreter]")
{
    std::string program = "(unsupportedOp 1 2)";
    std::istringstream iss(program);
    Interpreter interpreter;
    REQUIRE(interpreter.parse(iss));
    REQUIRE_THROWS_AS(interpreter.eval(), InterpreterSemanticError);
}

TEST_CASE("Test for 'if' with incorrect number of arguments", "[interpreter]")
{
    std::string program = "(if (< 1 2) (+ 1 1))";
    std::istringstream iss(program);
    Interpreter interpreter;
    REQUIRE(interpreter.parse(iss));
    REQUIRE_THROWS_AS(interpreter.eval(), InterpreterSemanticError);
}

TEST_CASE("Test for 'define' with incorrect usage", "[interpreter]") 
{
    std::string program1 = "(define 1 2)";
    std::string program2 = "(define x)";
    std::string program3 = "(define + 10)";
    std::istringstream iss1(program1);
    std::istringstream iss2(program2);
    std::istringstream iss3(program3);
    Interpreter interpreter1, interpreter2, interpreter3;
    REQUIRE(interpreter1.parse(iss1));
    REQUIRE(interpreter2.parse(iss2));
    REQUIRE(interpreter3.parse(iss3));
    REQUIRE_THROWS_AS(interpreter1.eval(), InterpreterSemanticError);
    REQUIRE_THROWS_AS(interpreter2.eval(), InterpreterSemanticError);
    REQUIRE_THROWS_AS(interpreter3.eval(), InterpreterSemanticError);
}

TEST_CASE("Test for valid use of begin", "[environment]")
{
    // Valid usage of begin
    {
        std::string program = "(begin (define a 1) (define b (+ a 1)) (* b 10))";
        Expression result = run(program);
        REQUIRE(result == Expression(20.));
    }

    // begin with no expressions should throw an error
    {
        std::string program = "(begin)";
        REQUIRE_THROWS_AS(run(program), InterpreterSemanticError);
    }

    // begin with only definitions should return last definition
    {
        std::string program = "(begin (define a 1) (define b 2))";
        Expression result = run(program);
        REQUIRE(result == Expression(2.));
   }

    // begin should not return from the middle of its body
    {
        std::string program = "(begin (define a 1) (define b 2) (define c 3))";
        Expression result = run(program);
        REQUIRE(result == Expression(3.));
    }
}

TEST_CASE("Test pointProcedure", "[Interpreter]") 
{
    Interpreter interp;

    SECTION("Invalid number of arguments") 
    {
        std::string program = "(point 0)";
        std::istringstream iss(program);

        REQUIRE(interp.parse(iss));
        REQUIRE_THROWS_AS(interp.eval(), InterpreterSemanticError);
    }

    SECTION("Invalid argument types") 
    {
        std::string program = "(point \"not-a-number\" 0)";
        std::istringstream iss(program);
        
        REQUIRE(interp.parse(iss));
        REQUIRE_THROWS_AS(interp.eval(), InterpreterSemanticError);
    }
}

TEST_CASE("Test lineProcedure", "[interpreter]") 
{
    Interpreter interp;

    SECTION("Invalid number of arguments") 
    {
        std::string program = "(line (point 0 0))";
        std::istringstream iss(program);

        REQUIRE(interp.parse(iss));
        REQUIRE_THROWS_AS(interp.eval(), InterpreterSemanticError);
    }

    SECTION("Invalid argument types") 
    {
        std::string program = "(line 0 (point 10 10))";
        std::istringstream iss(program);

        REQUIRE(interp.parse(iss));
        REQUIRE_THROWS_AS(interp.eval(), InterpreterSemanticError);
    }
}

TEST_CASE("Interpreter Parse 1", "[interpreter]") 
{
    std::istringstream input("(+ 1 2) extra");
    Interpreter interpreter;
    REQUIRE_FALSE(interpreter.parse(input));
}

TEST_CASE("Interpreter Parse 2", "[IncorrectFirstCharacter]")
{
    std::istringstream input("1 + 2");
    Interpreter interpreter;
    REQUIRE_FALSE(interpreter.parse(input));
}


TEST_CASE("Test arcProcedure", "[interpreter]") 
{
    Interpreter interp;

    SECTION("Invalid number of arguments") 
    {
        std::string program = "(arc (point 0 0) (point 10 10))";
        std::istringstream iss(program);

        REQUIRE(interp.parse(iss));
        REQUIRE_THROWS_AS(interp.eval(), InterpreterSemanticError);
    }

    SECTION("Invalid argument types") 
    {
        std::string program = "(arc (point 0 0) 10 1.57)";
        std::istringstream iss(program);

        REQUIRE(interp.parse(iss));
        REQUIRE_THROWS_AS(interp.eval(), InterpreterSemanticError);
    }
}

TEST_CASE("Test drawProcedure", "[interpreter]") 
{
    Interpreter interp;


    SECTION("Empty draw command") 
    {
        std::string program = "(draw)";
        std::istringstream iss(program);

        REQUIRE(interp.parse(iss));
        REQUIRE_THROWS_AS(interp.eval(), InterpreterSemanticError);
    }

    SECTION("Invalid draw argument") 
    {
        std::string program = "(draw (point 0 0) (invalid 10 20))";
        std::istringstream iss(program);

        REQUIRE(interp.parse(iss));
        REQUIRE_THROWS_AS(interp.eval(), InterpreterSemanticError);
    }

    SECTION("Incomplete draw command") 
    {
        std::string program = "(draw (line (point 0 0)))";
        std::istringstream iss(program);

        REQUIRE(interp.parse(iss));
        REQUIRE_THROWS_AS(interp.eval(), InterpreterSemanticError);
    }
}

TEST_CASE("End-to-End Tests", "[Interpreter]") 
{
    Interpreter interp;

    {
        std::string program = "(+ 1 (* 2 3))";
        std::istringstream iss(program);
        interp.parse(iss);
        REQUIRE(interp.eval() == Expression(7.));
    }

    {
        std::string program = "(+ 1)";
        std::istringstream iss(program);

        REQUIRE(interp.parse(iss));
        REQUIRE_THROWS_AS(interp.eval(), InterpreterSemanticError);
    }

    {
        std::string program = "(* 1)";
        std::istringstream iss(program);

        REQUIRE(interp.parse(iss));
        REQUIRE_THROWS_AS(interp.eval(), InterpreterSemanticError);
    }
  
    {
        std::string program = "(/ 1)";
        std::istringstream iss(program);

        REQUIRE(interp.parse(iss));
        REQUIRE_THROWS_AS(interp.eval(), InterpreterSemanticError);
    }
}

TEST_CASE("Draw Procedure Tests", "[interpreter]") 
{
    Interpreter interp;

    {
        std::string program = "(draw (point 0 0))";
        std::istringstream iss(program);

        REQUIRE(interp.parse(iss));
        REQUIRE_NOTHROW(interp.eval());
    }
 
    {
        std::string program = "(draw (line (point 0 0) (point 10 10)))";
        std::istringstream iss(program);

        REQUIRE(interp.parse(iss));
        REQUIRE_NOTHROW(interp.eval());
    }

    {
        std::string program = "(draw (arc (point 0 0) (point 10 10) 3.14159))";
        std::istringstream iss(program);

        REQUIRE(interp.parse(iss));
        REQUIRE_NOTHROW(interp.eval());
    }

    {
        std::string program = "(draw (invalid 0 0))";
        std::istringstream iss(program);

        REQUIRE(interp.parse(iss));
        REQUIRE_THROWS_AS(interp.eval(), InterpreterSemanticError);
    }
}

TEST_CASE("test arc", "[interpreter]") 
{
    Interpreter interp;

    {
        std::string program = "(draw (arc (point 0 0) (point 100 0) pi))";
        std::istringstream iss(program);
        bool ok = interp.parse(iss);

        Expression result = run(program);

        Expression arc;
        arc.head.type = ArcType;
        arc.head.value.arc_value.center.x = 0;
        arc.head.value.arc_value.center.y = 0;
        arc.head.value.arc_value.start.x = 100;
        arc.head.value.arc_value.start.y = 0;
        arc.head.value.arc_value.span = atan2(0, -1);
        arc.head.value.sym_value = "arc";

        REQUIRE(result == arc);
    }

    {
        std::string program = "(draw (line (point 10 0) (point 0 10)))";
        std::istringstream iss(program);
        bool ok = interp.parse(iss);
       
        Expression result = run(program);
        
        Expression line;
        line.head.type = LineType;
        line.head.value.line_value.first.x = 10;
        line.head.value.line_value.first.y = 0;
        line.head.value.line_value.second.x = 0;
        line.head.value.line_value.second.y = 10;
        line.head.value.sym_value = "line";
        
        REQUIRE(result == line);
    }
}


