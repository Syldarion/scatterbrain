#ifndef TEXTEDITWITHFOCUSOUTEVENT_H
#define TEXTEDITWITHFOCUSOUTEVENT_H

#include <QTextEdit>

class TextEditWithFocusOutEvent : public QTextEdit
{
    Q_OBJECT

public:
    explicit TextEditWithFocusOutEvent(QWidget *parent = nullptr);
signals:
    void lostFocus();
protected:
    void focusOutEvent(QFocusEvent* e) override;
};

#endif // TEXTEDITWITHFOCUSOUTEVENT_H
