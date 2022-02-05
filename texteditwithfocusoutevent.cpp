#include "texteditwithfocusoutevent.h"

TextEditWithFocusOutEvent::TextEditWithFocusOutEvent(QWidget *parent)
    : QTextEdit{parent}
{

}

void TextEditWithFocusOutEvent::focusOutEvent(QFocusEvent *e)
{
    emit lostFocus();

    QTextEdit::focusOutEvent(e);
}
