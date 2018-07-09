#ifndef CONSOLE_H
#define CONSOLE_H

#include <QPlainTextEdit>
#include <QAction>
#include <QMenu>

class Console : public QPlainTextEdit
{
    Q_OBJECT

signals:
    void closed();
    void eSendCommand(const QString &cmd);

public:
    explicit Console(QWidget *parent = 0);
    ~Console();

    void setLocalEchoEnabled(bool set);

public slots:
    void putData(const QByteArray &data);
    void putString(const QString &string);
    void hCommandError(const QString &err_str);

protected:
    virtual void keyPressEvent(QKeyEvent *e);
    virtual void mousePressEvent(QMouseEvent *e);
    virtual void mouseDoubleClickEvent(QMouseEvent *e);
    virtual void contextMenuEvent(QContextMenuEvent *e);
    virtual void hideEvent(QHideEvent *e);

private:
    void addStringToHistory(const QString &str);
    QString getStringFromHistory(int index);
    void updateHistoryIndex(bool fDirection);


private:
    QMenu* context;
    QAction* clearAct;

    bool localEchoEnabled;
    QString currentTypedString;
    int typePosition;
    QStringList history;
    int historyIndex;

};

#endif // CONSOLE_H
