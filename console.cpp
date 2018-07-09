#include "console.h"

#include <QScrollBar>

Console::Console(QWidget *parent)
    : QPlainTextEdit(parent)
    , localEchoEnabled(false)
{
    currentTypedString = "";
    typePosition = 0;
    historyIndex = -1;
    document()->setMaximumBlockCount(0);
    QPalette p = palette();
    p.setColor(QPalette::Base, Qt::black);
    p.setColor(QPalette::Text, Qt::green);
    setPalette(p);
    setFont(QFont("Consolas", 14));
    setWindowTitle("ELOCharts - Консоль разработчика");
    setGeometry(350, 40, 600, 400);
    context = new QMenu(this);
    clearAct = new QAction(tr("Очистить.."), this);
    connect(clearAct, SIGNAL(triggered()), this, SLOT(clear()));
    context->addAction(clearAct);
    insertPlainText(">");
}

Console::~Console()
{
    delete clearAct;
}


void Console::setLocalEchoEnabled(bool set)
{
    localEchoEnabled = set;
}

void Console::putData(const QByteArray &data)
{
    insertPlainText("\r<");
    QByteArray temp = data.toHex();
    for(int i = 0; i < temp.size(); i++) {
       insertPlainText(QString(temp.at(i)));
       if(i != temp.size() - 1) {
           i++;
           insertPlainText(QString(temp.at(i)));
       }
       insertPlainText(" ");
    }
    insertPlainText("\r");

    QScrollBar *bar = verticalScrollBar();
    bar->setValue(bar->maximum());
}

void Console::putString(const QString &string)
{
    insertPlainText("\r<");
    insertPlainText(string);
    insertPlainText("\r");
    QScrollBar *bar = verticalScrollBar();
    bar->setValue(bar->maximum());
}

void Console::hCommandError(const QString &err_str)
{
    QString text = "Unknown command \"" + err_str + "\"\n";
    insertPlainText(text);
    insertPlainText("\r");

    QScrollBar *bar = verticalScrollBar();
    bar->setValue(bar->maximum());
}

void Console::keyPressEvent(QKeyEvent *e)
{
    QScrollBar *bar = verticalScrollBar();
    QTextCursor plainCursor = this->textCursor();;
    switch (e->key()) {
    case Qt::Key_Backspace:
        this->textCursor().deletePreviousChar();
        if(!currentTypedString.isEmpty()) {
            if(typePosition) {
                typePosition--;
            }
            currentTypedString.remove(typePosition, 1);
        }
        break;
    case Qt::Key_Left:
        plainCursor.movePosition(QTextCursor::Left);
        this->setTextCursor(plainCursor);
        if(typePosition) {
            typePosition--;
        }
        break;
    case Qt::Key_Right:
        plainCursor.movePosition(QTextCursor::Right);
        this->setTextCursor(plainCursor);
        typePosition++;
        break;
    //история команд
    case Qt::Key_Up:
    case Qt::Key_Down:
        for(int i = 0; i < currentTypedString.size(); i++) {
            this->textCursor().deletePreviousChar();
        }
        currentTypedString = getStringFromHistory(historyIndex);
        if(e->key() == Qt::Key_Up) {
            updateHistoryIndex(false);
        }
        else {
            updateHistoryIndex(true);
        }
        insertPlainText(currentTypedString);
        typePosition = currentTypedString.size();
        bar->setValue(bar->maximum());
        break;
    case Qt::Key_F1:
    case Qt::Key_Escape:
        emit eSendCommand("stop");
        this->hide();
        break;
    case Qt::Key_Return:
    case Qt::Key_Enter:
        insertPlainText("\r");
        emit eSendCommand(currentTypedString);
        addStringToHistory(currentTypedString);
        historyIndex = history.size() - 1;
        currentTypedString.clear();
        typePosition = 0;
        bar->setValue(bar->maximum());
        break;
    case Qt::Key_F2:
    case Qt::Key_F3:
    case Qt::Key_F4:
    case Qt::Key_F5:
    case Qt::Key_F6:
    case Qt::Key_F7:
    case Qt::Key_F8:
    case Qt::Key_F9:
    case Qt::Key_F10:
    case Qt::Key_F11:
    case Qt::Key_F12:
    case Qt::Key_Control:
    case Qt::Key_Shift:
    case Qt::Key_Alt:
        //ignore modifiers
        break;
    default:
        if (localEchoEnabled) {
            QPlainTextEdit::keyPressEvent(e);
        }
        //currentTypedString.append(e->text().toLocal8Bit());
        currentTypedString.insert(typePosition, e->text().toLocal8Bit());
        typePosition++;
        if(currentTypedString.size() > 99) {
            currentTypedString.clear();
            typePosition = 0;
        }
    }
}

void Console::mousePressEvent(QMouseEvent *e)
{
    Q_UNUSED(e)
    setFocus();
}

void Console::mouseDoubleClickEvent(QMouseEvent *e)
{
    Q_UNUSED(e)
}

void Console::contextMenuEvent(QContextMenuEvent *e)
{
    context->exec(e->globalPos());
}

void Console::hideEvent(QHideEvent *e)
{
    Q_UNUSED(e)
    emit closed();
}

void Console::addStringToHistory(const QString &str)
{
    if(history.size() > 50) {
        history.removeFirst();
    }
    history.append(str);
}

QString Console::getStringFromHistory(int index)
{
    QString historyString = "";
    if((index >= 0) && (index < history.size())) {
        historyString = history.at(index);
    }
    return historyString;
}

void Console::updateHistoryIndex(bool fDirection)
{
    if(fDirection) {
        if(historyIndex < (history.size() - 1)) {
            historyIndex++;
        }
    }
    else {
        if(historyIndex > 0) {
            historyIndex--;
        }
    }
}
