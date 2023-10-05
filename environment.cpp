#include "environment.hpp"
#include "interpreter_semantic_error.hpp"

#include <cassert>
#include <cmath>

#include "environment.hpp"
#include "interpreter_semantic_error.hpp"

//Functon that handles a logical negation procedure
Expression notProcedure(const std::vector<Atom>& args)
{
    if (args.size() != 1 || args[0].type != BooleanType) 
    {
        throw InterpreterSemanticError("Error: Invalid argument for not");
    }
    return Expression(!args[0].value.bool_value);
}

//Functon that handles a logical AND procedure
Expression andProcedure(const std::vector<Atom>& args)
{
    for (const auto& arg : args)
    {
        if (arg.type != BooleanType || args.size() < 2)
        {
            throw InterpreterSemanticError("Error: Invalid argument for and");
        }
        if (!arg.value.bool_value) 
        {
            return Expression(false);
        }
    }
    return Expression(true);
}

//Functon that handles a logical OR procedure
Expression orProcedure(const std::vector<Atom>& args) 
{
    for (const auto& arg : args)
    {
        if (arg.type != BooleanType || args.size() < 2)
        {
            throw InterpreterSemanticError("Error: Invalid argument for or");
        }
        if (arg.value.bool_value) 
        {
            return Expression(true);
        }
    }
    return Expression(false);
}

//Functon that handles an arithmetic add procedure
Expression ADDProcedure(const std::vector<Atom>& args)
{
    double sum = 0.0;
    for (const auto& arg : args) 
    {
        if (arg.type != NumberType || args.size() < 2)
        {
            throw InterpreterSemanticError("Error: Invalid argument for addition");
        }
        sum += arg.value.num_value;
    }
    return Expression(sum);
}

//Functon that handles an arithmetic subtract procedure
Expression subtractProcedure(const std::vector<Atom>& args) 
{
    if (args.size() == 1) 
    {  // Unary minus
        if (args[0].type != NumberType) 
        {
            throw InterpreterSemanticError("Error: Invalid argument for unary subtraction");
        }
        return Expression(-args[0].value.num_value);
    }
    else if (args.size() == 2) 
    {  // Binary subtraction
        if (args[0].type != NumberType || args[1].type != NumberType)
        {
            throw InterpreterSemanticError("Error: Invalid arguments for binary subtraction");
        }
        return Expression(args[0].value.num_value - args[1].value.num_value);
    }
    else 
    {
        throw InterpreterSemanticError("Error: Invalid number of arguments for subtraction");
    }
}

//Functon that handles an arithmetic multiply procedure
Expression multiplyProcedure(const std::vector<Atom>& args)
{
    double product = 1.0;
    for (const auto& arg : args) 
    {
        if (arg.type != NumberType || args.size() < 2)
        {
            throw InterpreterSemanticError("Error: Invalid argument for multiplication");
        }
        product *= arg.value.num_value;
    }
    return Expression(product);
}

//Functon that handles an arithmetic divide procedure
Expression divideProcedure(const std::vector<Atom>& args)
{
    if (args.size() != 2 || args[0].type != NumberType || args[1].type != NumberType || args[1].value.num_value == 0) 
    {
        throw InterpreterSemanticError("Error: Invalid arguments for division");
    }
    return Expression(args[0].value.num_value / args[1].value.num_value);
}

//Functon that handles a less than comparison procedure
Expression lessThanProcedure(const std::vector<Atom>& args)
{
    if (args.size() != 2 || args[0].type != NumberType || args[1].type != NumberType) 
    {
        throw InterpreterSemanticError("Error: Invalid arguments for < operation");
    }
    return Expression(args[0].value.num_value < args[1].value.num_value);
}

//Functon that handles a less than or equal procedure
Expression lessThanOrEqualProcedure(const std::vector<Atom>& args) 
{
    if (args.size() != 2 || args[0].type != NumberType || args[1].type != NumberType)
    {
        throw InterpreterSemanticError("Error: Invalid arguments for <= operation");
    }
    return Expression(args[0].value.num_value <= args[1].value.num_value);
}

//Functon that handles a greater than comparison procedure
Expression greaterThanProcedure(const std::vector<Atom>& args)
{
    if (args.size() != 2 || args[0].type != NumberType || args[1].type != NumberType)
    {
        throw InterpreterSemanticError("Error: Invalid arguments for > operation");
    }
    return Expression(args[0].value.num_value > args[1].value.num_value);
}

//Functon that handles a greater than or equal comparison procedure
Expression greaterThanOrEqualProcedure(const std::vector<Atom>& args) 
{
    if (args.size() != 2 || args[0].type != NumberType || args[1].type != NumberType) 
    {
        throw InterpreterSemanticError("Error: Invalid arguments for >= operation");
    }
    return Expression(args[0].value.num_value >= args[1].value.num_value);
}

//Functon that handles an equal comparison procedure
Expression equalProcedure(const std::vector<Atom>& args) 
{
    if (args.size() != 2 || args[0].type != NumberType || args[1].type != NumberType)
    {
        throw InterpreterSemanticError("Error: Invalid arguments for = operation");
    }
    return Expression(args[0].value.num_value == args[1].value.num_value);
}

//Functon that handles an arithmetic logarithmic procedure
Expression log10Procedure(const std::vector<Atom>& args)
{
    if (args.size() != 1 || args[0].type != NumberType)
    {
        throw InterpreterSemanticError("Error: Invalid arguments for log10 operation");
    }
    if (args[0].value.num_value <= 0)
    {
        throw InterpreterSemanticError("Error: Non-positive argument for log10");
    }
    return Expression(std::log10(args[0].value.num_value));
}

//Functon that handles an arithmetic power procedure
Expression powProcedure(const std::vector<Atom>& args)
{
    if (args.size() != 2 || args[0].type != NumberType || args[1].type != NumberType)
    {
        throw InterpreterSemanticError("Error: Invalid arguments for pow operation");
    }
    return Expression(std::pow(args[0].value.num_value, args[1].value.num_value));
}

//Class constructor
//Contains built in symbols and procedures
Environment::Environment() 
{
    //Built in symbols
    addSymbol("pi", Expression(atan2(0, -1)));

    //Built in procedures
    addProcedure("not", notProcedure);
    addProcedure("and", andProcedure);
    addProcedure("or", orProcedure);
    addProcedure("<", lessThanProcedure);
    addProcedure("<=", lessThanOrEqualProcedure);
    addProcedure(">", greaterThanProcedure);
    addProcedure(">=", greaterThanOrEqualProcedure);
    addProcedure("=", equalProcedure);
    addProcedure("+", ADDProcedure);
    addProcedure("-", subtractProcedure);
    addProcedure("*", multiplyProcedure);
    addProcedure("/", divideProcedure);
    addProcedure("log10", log10Procedure);
    addProcedure("pow", powProcedure);
}

//Adds a given symbol to the environment
void Environment::addSymbol(const Symbol& symbol, const Expression& value) 
{
    EnvResult result;
    result.type = ExpressionType;
    result.exp = value;
    envmap[symbol] = result;
}

//Adds a given procedure to the environment
void Environment::addProcedure(const Symbol& symbol, Procedure procedure)
{
    EnvResult result;
    result.type = ProcedureType;
    result.proc = procedure;
    envmap[symbol] = result;
}

//Gets the procedure / symbol based on the given symbol
Expression Environment::get(const Symbol& symbol)
{
    auto it = envmap.find(symbol);
    if (it != envmap.end() && it->second.type == ExpressionType)
    {
        return it->second.exp;
    }
    throw InterpreterSemanticError("Error: Symbol not found or not associated with an expression.");
}

//Checks if the symbol is defined in the environment
bool Environment::isSymbolDefined(const Symbol& symbol)
{
    return envmap.find(symbol) != envmap.end();
}

//Evaluates procedure based on type
/*
* This function takes a symbol and a vector of arguments and looks up the symbol
* in the environment. If the symbol is found and associated with a procedure, it
* evaluates the procedure with the provided arguments. The procedure is expected
* to be a callable function that can accept a vector of Atom arguments and return
* an Expression result.
*/
Expression Environment::evaluateProcedure(const Symbol& symbol, const std::vector<Expression>& args)
{
    auto it = envmap.find(symbol);
    if (it != envmap.end() && it->second.type == ProcedureType)
    {
        std::vector<Atom> atomArgs;

        for (const auto& exp : args)
        {
            Atom atom;

            // If the expression represents an atomic value (number or boolean)
            if (exp.head.type == NumberType || exp.head.type == BooleanType) 
            {
                atom = exp.head;
            }
            // Otherwise, if it's a symbol, attempt to convert it
            else if (exp.head.type == SymbolType && !token_to_atom(exp.head.value.sym_value, atom))
            {
                throw InterpreterSemanticError("Error: Failed to convert symbol to atom.");
            }
            else 
            {
                throw InterpreterSemanticError("Error: Unexpected expression type.");
            }
            atomArgs.push_back(atom);
        }
        return it->second.proc(atomArgs);
    }
    throw InterpreterSemanticError("Error: Symbol not found or not associated with a procedure.");
}



