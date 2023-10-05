#include "expression.hpp"

#include <cmath>
#include <limits>
#include <cctype>

// system includes
#include <sstream>
#include <iostream>
using namespace std;

//Boolean expression constructor
Expression::Expression(bool tf)
{
	head.type = BooleanType;
	head.value.bool_value = tf;
}

//Number expression constructor
Expression::Expression(double num)
{
	head.type = NumberType;
	head.value.num_value = num;
}

//Symbol expression constructor
Expression::Expression(const std::string & sym)
{
	head.type = SymbolType;
	head.value.sym_value = sym;
}

//Equality operator function
//Returns a bool value for a comparison between two expression objects 
//that have the same type. 
bool Expression::operator==(const Expression& exp) const noexcept
{	
	// Compare types
	if (head.type != exp.head.type) 
	{
		return false;
	}

	// Check if both expressions have NoneType heads
	if (head.type == NoneType && exp.head.type == NoneType)
	{
		return true;
	}

	switch (head.type) 
	{
	case BooleanType:
		return head.value.bool_value == exp.head.value.bool_value;
	case NumberType:
		// Compare floating-point numbers with tolerance
		return std::abs(head.value.num_value - exp.head.value.num_value) <= std::numeric_limits<double>::epsilon();
	case SymbolType:
		return head.value.sym_value == exp.head.value.sym_value;
	default:
		std::cerr << "ERROR: Invalid type " << std::endl;
		return false; // Invalid type
	}
	return true;
}

//Operator for printing out expressions based on type
std::ostream& operator<<(std::ostream& out, const Expression& exp) 
{
	if (exp.tail.empty())
	{
		if (exp.head.type == BooleanType)
		{
			out << (exp.head.value.bool_value ? "True" : "False");
		}
		else if (exp.head.type == NumberType)
		{
			out << exp.head.value.num_value;
		}
		else if (exp.head.type == SymbolType)
		{
			out << exp.head.value.sym_value;
		}
	}
	else
	{
		out << "(" << exp.head;

		for (const Expression& tailExpr : exp.tail)
		{
			out << " " << tailExpr;
		}

		out << ")";
	}

	return out;
}


//  Function to convert a given token into an atom based on type,
//  return true if a token is valid. otherwise, return false.

/* 
*  -If the token is "True" or "False," it's converted to a Boolean Atom.
*  -If the token is numeric(integer or floating - point), it's converted to a Number Atom.
*  -If the token is a string, it's considered a Symbol and converted accordingly.
*/
bool token_to_atom(const std::string& token, Atom& atom)
{
	if (token == " ")
		return false;

	if (token == "True")
	{
		atom.type = BooleanType;
		atom.value.bool_value = true;
	}
	else if (token == "False") 
	{
		atom.type = BooleanType;
		atom.value.bool_value = false;
	}
	else if (std::isdigit(token[0]) || (token[0] == '-' && std::isdigit(token[1]))) 
	{
		if (token.size() > 1)
		{
			for (int i = 1; i < token.size(); i++)
			{
				if (isalpha(token[i]) && token[i] != 'e' && token[i] != '+' && token[i] != '-')
				{
					return false;
				}
				else
				{
					atom.type = NumberType;
					atom.value.num_value = std::stod(token);
					return true;
				}
			}
		}
		else
		{
			atom.type = NumberType;
			atom.value.num_value = std::stod(token);
			return true;
		}
	}
	else 
	{
		atom.type = SymbolType;
		atom.value.sym_value = token;
	}
	return true;
}

