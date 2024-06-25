#include "qt_interpreter.hpp"

#include <string>
#include <sstream>
#include <iostream>
#include <cmath>
#include <fstream>
#include <iomanip>

#include <QBrush>
#include <QDebug>

#include "qgraphics_arc_item.hpp"


#include "interpreter_semantic_error.hpp"
#include <QtWidgets>

QtInterpreter::QtInterpreter(QObject * parent): QObject(parent)
{
  // Nothing to be added here.
}

// Function that parses and evaluates a string stream.
// Function calls parse and evaluateExpression for interprer
// side to interpret the string stream and return expressions 
// to be drawn. drawExpression() function draws the expression returned. 
void QtInterpreter::parseAndEvaluate(QString entry) 
{
    try 
    {
        std::istringstream expressionStream(entry.toStdString());
        bool success = parse(expressionStream);

        if (success) 
        {
            // Special handling for 'begin' form
            if (ast.head.type == SymbolType && ast.head.value.sym_value == "begin") {
                Expression lastExpr;
                for (const auto& e : ast.tail) {
                    lastExpr = evaluateExpression(e);
                    drawExpression(lastExpr);
                }
                // Draw only the last expression
                drawExpression(lastExpr);
            } else {
                // Handle other forms normally
                Expression result = evaluateExpression(ast);
                drawExpression(result);
            }
        }
        else 
        {
            emit error("Error: failed to parse the expression.");
        }
    }
    catch (const InterpreterSemanticError& e) 
    {
        emit error(QString::fromStdString(e.what()));
    }
    catch (const std::exception& e) 
    {
        emit error(QString::fromStdString(e.what()));
    }
}


/*
  The function takes an Expression object as its input and determines the type of the expression 
  Boolean, Number, Symbol, Point, Line, Arc, or List. Based on the expression type the function
  emits the string or graphical representation of the expression.
*/
void QtInterpreter::drawExpression(const Expression& result) 
{
    std::string resultStr;
    QGraphicsEllipseItem* pointItem = nullptr;
    QGraphicsLineItem* lineItem = nullptr;
    QGraphicsArcItem* arcItem = nullptr;

    switch (result.head.type) 
    {
    case BooleanType:
        drawBoolean(result, resultStr);
        break;

    case NumberType:
        drawNumber(result, resultStr);
        break;
    
    case SymbolType:
        drawSymbol(result, resultStr);
        break;
    
    case PointType:
        drawPoint(result, resultStr, pointItem);
        break;
    
    case LineType:
        drawLine(result, resultStr, lineItem);
        break;
    
    case ArcType:
        drawArc(result, resultStr, arcItem);
        break;
    
    case ListType:
        drawList(result);
        return; // Early return for lists
    default:
        resultStr = "Unknown Type";
        break;
    }

    // Emit signals for drawing if applicable
    if (!resultStr.empty()) 
    {
        emit info(QString::fromStdString(resultStr));
    }
    if (lineItem != nullptr)
    {
        emit drawGraphic(lineItem);
    }
    if (arcItem != nullptr)
    {
        emit drawGraphic(arcItem);
    }
    if (pointItem != nullptr)
    {
        emit drawGraphic(pointItem);
    }
}

// Handles Boolean expressions. Converts the boolean value to a string representation.
void QtInterpreter::drawBoolean(const Expression& result, std::string& resultStr) 
{
    resultStr = result.head.value.bool_value ? "True" : "False";
}

// Handles Number expressions. Converts the numeric value to a string representation.
void QtInterpreter::drawNumber(const Expression& result, std::string& resultStr) 
{
    std::ostringstream stream;
    stream << std::fixed << std::setprecision(0) << result.head.value.num_value;
    resultStr = "(" + stream.str() + ")";
}

// Handles Symbol expressions. Directly uses the symbol's string value.
void QtInterpreter::drawSymbol(const Expression& result, std::string& resultStr) 
{
    resultStr = result.head.value.sym_value;
}

// Handles the drawing of Point expressions. Converts point coordinates to a string and creates a graphical point item.
void QtInterpreter::drawPoint(const Expression& result, std::string& resultStr, QGraphicsEllipseItem*& pointItem) 
{
    resultStr = "(" + std::to_string(result.head.value.point_value.x) + ", " + std::to_string(result.head.value.point_value.y) + ")";
    pointItem = new QGraphicsEllipseItem(result.head.value.point_value.x, result.head.value.point_value.y, 1, 1);
}

// Handles the drawing of Line expressions. Converts line coordinates to a string and creates a graphical line item.
void QtInterpreter::drawLine(const Expression& result, std::string& resultStr, QGraphicsLineItem*& lineItem) 
{
    resultStr = "((" + std::to_string(result.head.value.line_value.first.x) + ", " + std::to_string(result.head.value.line_value.first.y) + "), (" + std::to_string(result.head.value.line_value.second.x) + ", " + std::to_string(result.head.value.line_value.second.y) + "))";
    lineItem = new QGraphicsLineItem(result.head.value.line_value.first.x, result.head.value.line_value.first.y, result.head.value.line_value.second.x, result.head.value.line_value.second.y);
}

// Handles the drawing of Arc expressions. Converts arc parameters to a string and creates a graphical arc item.
void QtInterpreter::drawArc(const Expression& result, std::string& resultStr, QGraphicsArcItem*& arcItem) 
{
    qreal x = result.head.value.arc_value.center.x;
    qreal y = result.head.value.arc_value.center.y;
    qreal width = 2 * (result.head.value.arc_value.start.x - x);
    qreal height = result.head.value.arc_value.span;
    arcItem = new QGraphicsArcItem(x - width / 2, y - height / 2, width, height); // Adjusted to center the arc at (x,y)
    resultStr = "((" + std::to_string(x) + ", " + std::to_string(y) + "), (" + std::to_string(result.head.value.arc_value.start.x) + ", " + std::to_string(result.head.value.arc_value.start.y) + "), " + std::to_string(result.head.value.arc_value.span) + ")";
}

// Handles the drawing of List expressions. Iterates through each sub-expression in the list and draws them individually.
void QtInterpreter::drawList(const Expression& result) 
{
    // Iterate and draw each sub-expression
    for (const auto& subExpr : result.tail) 
    {
        drawExpression(subExpr);
    }
}
