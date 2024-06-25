#include <QDebug>
#include <QtTest/QtTest>
#include <QtWidgets>

#include "canvas_widget.hpp"
#include "main_window.hpp"
#include "message_widget.hpp"
#include "repl_widget.hpp"

#include <iostream>

// ADD YOUR TESTS TO THIS CLASS !!!!!!!
class TestGUI : public QObject {
  Q_OBJECT

public:
private slots:
  
  void initTestCase();
  void testREPLGood();
  void testREPLBad();
  void testREPLBad2Good();
  void testPoint();
  void testLine();
  void testArc();
  void testEnvRestore();
  void testMessage();
  void testEmptyREPLInput();
  void cleanupTestCase();

  void testLongMessage();
  void testMultipleCommandsInSingleInput();
  void testDrawWithNegativeCoordinates();
  void testREPLInputWithExcessiveWhitespace();
  void testArcWithZeroRadius();
  void testArcWithNegativeRadius();
  void testFullCircleArc();
  void testArcWithTinySpanAngle();
  void testDrawAndRemoveMultipleItems();
  void testDrawItemsWithLargeCoordinates();
  void testContinuousDrawing();
  void testLargeExpressionInREPL();
  void testMainWindowResize();
  void testDrawingOverlappingItems();
  void testREPLHistoryNavigation();
  void testBooleanExpression();
  void testMemoryLeakPrevention();
  void testFilePreloading();
  void testWidgetInitialization();
  void testMainWindowClosing();
  void testConsistentStateAfterError();
  void testSyntaxErrors();
  void testAlternatingMessages();
  void testClearCanvas();
  void testListExpression();

  
private:
  MainWindow w;

  REPLWidget *repl;
  QLineEdit *replEdit;
  MessageWidget *message;
  QLineEdit *messageEdit;
  CanvasWidget *canvas;
  QGraphicsScene *scene;
};

void TestGUI::initTestCase() 
{

  repl = w.findChild<REPLWidget *>();
  QVERIFY2(repl, "Could not find REPLWidget instance in MainWindow instance.");

  replEdit = repl->findChild<QLineEdit *>();
  QVERIFY2(replEdit,
           "Could not find QLineEdit instance in REPLWidget instance.");

  message = w.findChild<MessageWidget *>();
  QVERIFY2(message,
           "Could not find MessageWidget instance in MainWindow instance.");

  messageEdit = message->findChild<QLineEdit *>();
  QVERIFY2(messageEdit,
           "Could not find QLineEdit instance in MessageWidget instance.");

  canvas = w.findChild<CanvasWidget *>();
  QVERIFY2(canvas,
           "Could not find CanvasWidget instance in MainWindow instance.");

  scene = canvas->findChild<QGraphicsScene *>();
  QVERIFY2(scene,
           "Could not find QGraphicsScene instance in CanvasWidget instance.");

    w.setMinimumSize(800,600);
    w.show();
}

void TestGUI::testREPLGood() 
{
  QVERIFY(repl && replEdit);
  QVERIFY(message && messageEdit);

  // send a string to the repl widget
  QTest::keyClicks(replEdit, "(define a 1)");
  QTest::keyClick(replEdit, Qt::Key_Return, Qt::NoModifier);

  // check message
  QVERIFY2(messageEdit->isReadOnly(), "Expected QLineEdit inside MessageWidget to be read-only.");
  QCOMPARE(messageEdit->text(), QString("(1)"));
}

void TestGUI::testREPLBad() 
{
  QVERIFY(repl && replEdit);
  QVERIFY(message && messageEdit);

  // send a string to the repl widget
  QTest::keyClicks(replEdit, "(foo)");
  QTest::keyClick(replEdit, Qt::Key_Return, Qt::NoModifier);

  // check message
  QVERIFY2(messageEdit->isReadOnly(),
           "Expected QLineEdit inside MessageWidget to be read-only.");
  QVERIFY2(messageEdit->text().startsWith("Error"), "Expected error message.");

  // check background color and selection
  QPalette p = messageEdit->palette();
  QCOMPARE(p.highlight().color(), QColor(Qt::red));
  QVERIFY2(messageEdit->selectedText().startsWith("Error"),
           "Expected error to be selected.");
}

void TestGUI::testREPLBad2Good() {

  QVERIFY(repl && replEdit);
  QVERIFY(message && messageEdit);

  // send a string to the repl widget
  QTest::keyClicks(replEdit, "(foo)");
  QTest::keyClick(replEdit, Qt::Key_Return, Qt::NoModifier);

  // check message
  QVERIFY2(messageEdit->isReadOnly(),
           "Expected QLineEdit inside MessageWidget to be read-only.");
  QVERIFY2(messageEdit->text().startsWith("Error"), "Expected error message.");

  // check background color and selection
  QPalette p = messageEdit->palette();
  QCOMPARE(p.highlight().color(), QColor(Qt::red));
  QVERIFY2(messageEdit->selectedText().startsWith("Error"),
           "Expected error to be selected.");

  // send a string to the repl widget
  QTest::keyClicks(replEdit, "(define value 100)");
  QTest::keyClick(replEdit, Qt::Key_Return, Qt::NoModifier);

  // check message
  QVERIFY2(messageEdit->isReadOnly(),
           "Expected QLineEdit inside MessageWidget to be read-only.");
  QCOMPARE(messageEdit->text(), QString("(100)"));

  // check background color and selection
  p = messageEdit->palette();
  QVERIFY2(p.highlight().color() != QColor(Qt::red),
           "Did not expect red highlight on successful eval.");
  QVERIFY2(messageEdit->selectedText() == "",
           "Expected no selcted text on successful eval.");
}

void TestGUI::testPoint() {

  QVERIFY(repl && replEdit);
  QVERIFY(canvas && scene);

  // send a string to the repl widget
  QTest::keyClicks(replEdit, "(draw (point 0 0))");
  QTest::keyClick(replEdit, Qt::Key_Return, Qt::NoModifier);

  // check canvas
  QVERIFY2(scene->itemAt(QPointF(0, 0), QTransform()) != 0,
           "Expected a point in the scene. Not found.");
}

void TestGUI::testLine() 
{
  QVERIFY(repl && replEdit);
  QVERIFY(canvas && scene);

  // send a string to the repl widget
  QTest::keyClicks(replEdit, "(draw (line (point 10 0) (point 0 10)))");
  QTest::keyClick(replEdit, Qt::Key_Return, Qt::NoModifier);
  
  // check canvas
  QVERIFY2(scene->itemAt(QPointF(10, 0), QTransform()) != 0,
           "Expected a line in the scene. Not found.");
  QVERIFY2(scene->itemAt(QPointF(0, 10), QTransform()) != 0,
           "Expected a line in the scene. Not found.");
}

void TestGUI::testArc() 
{
  QVERIFY(repl && replEdit);
  QVERIFY(canvas && scene);

  // send a string to the repl widget
  QTest::keyClicks(replEdit, "(draw (arc (point 0 0) (point 100 0) pi))");
  QTest::keyClick(replEdit, Qt::Key_Return, Qt::NoModifier);

  // check canvas
  QVERIFY2(scene->itemAt(QPointF(100, 0), QTransform()) != 0,
           "Expected a point on the arc in the scene. Not found.");
  QVERIFY2(scene->itemAt(QPointF(-100, 0), QTransform()) != 0,
           "Expected a point on the arc in the scene. Not found.");

  QGraphicsItem * temp = scene->itemAt(QPointF(-20, -1), QTransform());
  qDebug() << temp;
  qDebug() << temp->boundingRect();
}

void TestGUI::testEnvRestore() {
  
  QVERIFY(repl && replEdit);
  QVERIFY(canvas && scene);

  QGraphicsItem * temp2 = scene->itemAt(QPointF(-20, 0), QTransform());
  qDebug() << temp2;
  
  // send a string to the repl widget
  QTest::keyClicks(replEdit, "(begin (draw (point -20 0)) (define pi 3))");
  QTest::keyClick(replEdit, Qt::Key_Return, Qt::NoModifier);

  // check canvas
  QGraphicsItem * temp = scene->itemAt(QPointF(-20, 0), QTransform());
  qDebug() << temp;
  
  QVERIFY2(scene->itemAt(QPointF(-20, 0), QTransform()) == 0,
           "Did not expected a point in the scene. One found.");
}

void TestGUI::testMessage(){

  MessageWidget message;

  QLineEdit * messageEdit = message.findChild<QLineEdit *>();
  QVERIFY2(messageEdit,
           "Could not find QLineEdit instance in MessageWidget instance.");
  
  // check message
  QVERIFY2(messageEdit->isReadOnly(),
           "Expected QLineEdit inside MessageWidget to be read-only.");
  QCOMPARE(messageEdit->text(), QString(""));
}

/////////////////////////////////////////////////////////////////////////////////////////////////Added tests////////////////////////////////////////////////////////////////////////////////////////////

void TestGUI::testEmptyREPLInput() 
{
    QVERIFY(repl && replEdit);
    QVERIFY(message && messageEdit);

    // Send an empty string to the repl widget
    QTest::keyClicks(replEdit, "   ");
    QTest::keyClick(replEdit, Qt::Key_Return, Qt::NoModifier);

    // Check message - expecting no change or specific handling of empty input
    QCOMPARE(messageEdit->text(), QString("Error: failed to parse the expression."));
}

void TestGUI::testLongMessage() 
{
    MessageWidget message;

    QLineEdit* messageEdit = message.findChild<QLineEdit*>();
    QVERIFY2(messageEdit, "Could not find QLineEdit instance in MessageWidget instance.");

    QString longMessage = QString(1000, 'a'); // A very long message
    message.info(longMessage);

    // Check if the message is set correctly
    QCOMPARE(messageEdit->text(), longMessage);
}

void TestGUI::testMultipleCommandsInSingleInput() 
{
    QTest::keyClicks(replEdit, "(define x 10) (define y 20)");
    QTest::keyClick(replEdit, Qt::Key_Return, Qt::NoModifier);

    QCOMPARE(messageEdit->text(), QString("Error: failed to parse the expression."));
}

void TestGUI::testDrawWithNegativeCoordinates() 
{
    QTest::keyClicks(replEdit, "(draw (point -50 -50))");
    QTest::keyClick(replEdit, Qt::Key_Return, Qt::NoModifier);

    QVERIFY2(scene->itemAt(QPointF(-50, -50), QTransform()) != nullptr,
        "Expected a point at negative coordinates in the scene. Not found.");
}

void TestGUI::testClearCanvas() 
{
    // Draw something first
    QTest::keyClicks(replEdit, "(draw (point 10 10))");
    QTest::keyClick(replEdit, Qt::Key_Return, Qt::NoModifier);

    // Clear the canvas
    QTest::keyClicks(replEdit, "(clear)");
    QTest::keyClick(replEdit, Qt::Key_Return, Qt::NoModifier);

    QVERIFY2(scene->items().isEmpty(), "Expected the canvas to be cleared.");
}

void TestGUI::testREPLInputWithExcessiveWhitespace()
{
    QTest::keyClicks(replEdit, "     (define    z    30)     ");
    QTest::keyClick(replEdit, Qt::Key_Return, Qt::NoModifier);

    QCOMPARE(messageEdit->text(), QString("Error: failed to parse the expression."));
}

void TestGUI::testArcWithZeroRadius() 
{
    QVERIFY(repl && replEdit);
    QVERIFY(canvas && scene);

    QTest::keyClicks(replEdit, "(draw (arc (point 0 0) (point 0 0) 0))");
    QTest::keyClick(replEdit, Qt::Key_Return, Qt::NoModifier);

    // Check if nothing is drawn on the canvas
    QVERIFY2(scene->itemAt(QPointF(0, 0), QTransform()) == nullptr,
        "Did not expect any item at (0,0) for zero radius arc.");
}

void TestGUI::testArcWithNegativeRadius() 
{
    QVERIFY(repl && replEdit);
    QVERIFY(canvas && scene);

    QTest::keyClicks(replEdit, "(draw (arc (point 0 0) (point -10 0) -pi))");
    QTest::keyClick(replEdit, Qt::Key_Return, Qt::NoModifier);

    // Check if the arc is handled gracefully
    QVERIFY2(scene->itemAt(QPointF(-10, 0), QTransform()) == nullptr,
        "Did not expect any item for negative radius arc.");
}

void TestGUI::testFullCircleArc() 
{
    QVERIFY(repl && replEdit);
    QVERIFY(canvas && scene);

    QTest::keyClicks(replEdit, "(draw (arc (point 0 0) (point 10 0) 6.283))"); // 2*PI radians
    QTest::keyClick(replEdit, Qt::Key_Return, Qt::NoModifier);

    // Check if a full circle is drawn
    QVERIFY2(scene->itemAt(QPointF(10, 0), QTransform()) != nullptr,
        "Expected a point on the full circle arc in the scene.");
}

void TestGUI::testArcWithTinySpanAngle() 
{
    QVERIFY(repl && replEdit);
    QVERIFY(canvas && scene);

    QTest::keyClicks(replEdit, "(draw (arc (point 0 0) (point 10 0) 0.0001))");
    QTest::keyClick(replEdit, Qt::Key_Return, Qt::NoModifier);

    // Check if the arc is drawn correctly
    QVERIFY2(scene->itemAt(QPointF(10, 0), QTransform()) != nullptr,
        "Expected a point on the tiny span angle arc in the scene.");
}

void TestGUI::testDrawAndRemoveMultipleItems() 
{
    // Draw multiple items
    QTest::keyClicks(replEdit, "(begin (draw (point 10 10)) (draw (line (point 20 20) (point 30 30))) (draw (arc (point 40 40) (point 50 40) pi)))");
    QTest::keyClick(replEdit, Qt::Key_Return, Qt::NoModifier);

    // Check if items are drawn
    QVERIFY2(!scene->items().isEmpty(), "Expected multiple items in the scene.");

    // Clear the canvas
    QTest::keyClicks(replEdit, "(clear)");
    QTest::keyClick(replEdit, Qt::Key_Return, Qt::NoModifier);

    // Check if canvas is cleared
    QVERIFY2(scene->items().isEmpty(), "Expected the canvas to be cleared after removal.");
}

void TestGUI::testDrawItemsWithLargeCoordinates() 
{
    QTest::keyClicks(replEdit, "(draw (point 10000 10000))");
    QTest::keyClick(replEdit, Qt::Key_Return, Qt::NoModifier);

    // Check if the item with large coordinates is drawn
    QVERIFY2(scene->itemAt(QPointF(10000, 10000), QTransform()) != nullptr,
        "Expected a point at large coordinates in the scene.");
}

void TestGUI::testContinuousDrawing() 
{
    for (int i = 0; i < 9; ++i) 
    {
        QTest::keyClicks(replEdit, QString("(draw (point %1 %1))").arg(i * 10));
        QTest::keyClick(replEdit, Qt::Key_Return, Qt::NoModifier);
    }
    // Check if multiple items are drawn
    QVERIFY2(scene->items().count() == 10, "Expected 10 points in the scene.");
    
}

void TestGUI::testLargeExpressionInREPL() 
{
    QVERIFY(repl && replEdit);
    QVERIFY(message && messageEdit);

    QString largeExpression = "(begin ";
    for (int i = 0; i < 100; ++i) {
        largeExpression += QString("(define var%1 %2) ").arg(i).arg(i);
    }
    largeExpression += ")";

    QTest::keyClicks(replEdit, largeExpression);
    QTest::keyClick(replEdit, Qt::Key_Return, Qt::NoModifier);

    // Check for successful processing or error handling
    QVERIFY2(!messageEdit->text().contains("Error"), "Large expression caused an error.");
}


void TestGUI::testMainWindowResize() {
    w.resize(1024, 768); // Resize to a different size

    // Check if components are still accessible
    QVERIFY2(repl && replEdit, "REPLWidget not accessible after resize.");
    QVERIFY2(canvas && scene, "CanvasWidget not accessible after resize.");
    QVERIFY2(message && messageEdit, "MessageWidget not accessible after resize.");
}

void TestGUI::testDrawingOverlappingItems() 
{
    QTest::keyClicks(replEdit, "(begin (draw (point 50 50)) (draw (point 50 50)))");
    QTest::keyClick(replEdit, Qt::Key_Return, Qt::NoModifier);

    // Check if both points are drawn at the same location
    QList<QGraphicsItem*> itemsAtLocation = scene->items(QPointF(50, 50));
    QVERIFY2(itemsAtLocation.count() >= 2, "Expected at least two overlapping points in the scene.");
}

void TestGUI::testREPLHistoryNavigation()
{
    // Enter a few commands
    QTest::keyClicks(replEdit, "(define a 10)");
    QTest::keyClick(replEdit, Qt::Key_Return, Qt::NoModifier);
    QTest::keyClicks(replEdit, "(define b 20)");
    QTest::keyClick(replEdit, Qt::Key_Return, Qt::NoModifier);

    // Navigate back in history
    QTest::keyClick(replEdit, Qt::Key_Up, Qt::NoModifier);
    QTest::keyClick(replEdit, Qt::Key_Up, Qt::NoModifier);

    // Check if the first command is shown
    QCOMPARE(replEdit->text(), QString("(define a 10)"));

    // Navigate back in history
    QTest::keyClick(replEdit, Qt::Key_Down, Qt::NoModifier);
    QTest::keyClick(replEdit, Qt::Key_Down, Qt::NoModifier);
}


void TestGUI::testBooleanExpression() 
{
    QTest::keyClicks(replEdit, "(not False)");
    QTest::keyClick(replEdit, Qt::Key_Return, Qt::NoModifier);
    QCOMPARE(messageEdit->text(), QString("True"));

    QTest::keyClicks(replEdit, "(not True)");
    QTest::keyClick(replEdit, Qt::Key_Return, Qt::NoModifier);
    QCOMPARE(messageEdit->text(), QString("False"));

    QTest::keyClicks(replEdit, "(and False True)");
    QTest::keyClick(replEdit, Qt::Key_Return, Qt::NoModifier);
    QCOMPARE(messageEdit->text(), QString("False"));

    QTest::keyClicks(replEdit, "(or False True)");
    QTest::keyClick(replEdit, Qt::Key_Return, Qt::NoModifier);
    QCOMPARE(messageEdit->text(), QString("True"));
}

void TestGUI::testMemoryLeakPrevention() 
{
    QTest::keyClicks(replEdit, "(draw (point 10 10))");
    QTest::keyClick(replEdit, Qt::Key_Return, Qt::NoModifier);
    QTest::keyClicks(replEdit, "(clear)");
    QTest::keyClick(replEdit, Qt::Key_Return, Qt::NoModifier);
    QCOMPARE(scene->items().count(), 0);
}

void TestGUI::testFilePreloading() 
{
    MainWindow testWindow("testfile.vts");
    testWindow.show();

    REPLWidget* replWidget = testWindow.findChild<REPLWidget*>();
    QVERIFY2(replWidget, "REPLWidget not found in MainWindow with file preload.");

    QLineEdit* replEdit = replWidget->findChild<QLineEdit*>();
    QVERIFY2(replEdit, "QLineEdit not found in REPLWidget with file preload.");
}

void TestGUI::testWidgetInitialization() 
{
    QVERIFY2(repl, "REPLWidget should be initialized.");
    QVERIFY2(message, "MessageWidget should be initialized.");
    QVERIFY2(canvas, "CanvasWidget should be initialized.");
}

void TestGUI::testMainWindowClosing() 
{
    w.close();
    QTest::qWait(100);
    QVERIFY2(!w.isVisible(), "MainWindow should be closed.");
}

void TestGUI::testConsistentStateAfterError() 
{
    QTest::keyClicks(replEdit, "(foo)");
    QTest::keyClick(replEdit, Qt::Key_Return, Qt::NoModifier);
    QVERIFY2(messageEdit->text().startsWith("Error"), "Expected error message.");

    QTest::keyClicks(replEdit, "(define z 10)");
    QTest::keyClick(replEdit, Qt::Key_Return, Qt::NoModifier);
    QCOMPARE(messageEdit->text(), QString("(10)"));
}

void TestGUI::testSyntaxErrors() 
{
    QTest::keyClicks(replEdit, "(define r )"); // Incomplete expression
    QTest::keyClick(replEdit, Qt::Key_Return, Qt::NoModifier);
    QVERIFY2(messageEdit->text().contains("Error"), "Expected syntax error message.");
}

void TestGUI::testAlternatingMessages() 
{
    message->info("Info message");
    QCOMPARE(messageEdit->text(), QString("Info message"));

    message->error("Error message");
    QCOMPARE(messageEdit->text(), QString("Error message"));

    message->info("Another info message");
    QCOMPARE(messageEdit->text(), QString("Another info message"));
}


void TestGUI::testListExpression()
{
    QTest::keyClicks(replEdit, "(clear)");
    QTest::keyClick(replEdit, Qt::Key_Return, Qt::NoModifier);

    QTest::keyClicks(replEdit, "(begin (draw (point 1 1)) (draw (point 2 2)))");
    QTest::keyClick(replEdit, Qt::Key_Return, Qt::NoModifier);
   
    qDebug() << "ITEMS: " << scene->items().count();

    QVERIFY2(scene->items().count() >= 2, "Expected at least two points in the scene.");
}



void TestGUI::cleanupTestCase() 
{

  QTest::qWait(5000);
  
}


QTEST_MAIN(TestGUI)
#include "test_gui.moc"
