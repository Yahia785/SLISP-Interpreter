#ifndef QT_INTERPRETER_HPP
#define QT_INTERPRETER_HPP

#include <string>

#include <QObject>
#include <QString>
#include <QGraphicsItem>

#include "interpreter.hpp"
#include "canvas_widget.hpp"
#include <qgraphics_arc_item.hpp>

class QtInterpreter: public QObject, private Interpreter{
Q_OBJECT

public:

  QtInterpreter(QObject * parent = nullptr);

  void drawBoolean(const Expression& result, std::string& resultStr);
  void drawNumber(const Expression& result, std::string& resultStr);
  void drawSymbol(const Expression& result, std::string& resultStr);
  void drawPoint(const Expression& result, std::string& resultStr, QGraphicsEllipseItem*& pointItem);
  void drawLine(const Expression& result, std::string& resultStr, QGraphicsLineItem*& lineItem);
  void drawArc(const Expression& result, std::string& resultStr, QGraphicsArcItem*& arcItem);
  void drawList(const Expression& result);

signals:

  void drawGraphic(QGraphicsItem * item);

  void info(QString message);

  void error(QString message);

public slots:

  void parseAndEvaluate(QString entry);
  void drawExpression(const Expression& result);
};

#endif
