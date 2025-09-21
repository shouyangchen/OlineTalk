#include "SerachEdit.h"
SerachEdit::SerachEdit(QWidget* parent):QLineEdit(parent)
{
	connect(this, &QLineEdit::textChanged, this, &SerachEdit::limit_text_length);
}




void SerachEdit::limit_text_length(QString text)
{
	if (this->max_length<=0)
	{
		return;
	}

	QByteArray byte_array = text.toUtf8();
	if (byte_array.size()>this->max_length)
	{
		byte_array = byte_array.left(this->max_length);
		this->setText(QString::fromUtf8(byte_array));
	}
}


void SerachEdit::set_max_length(int max_length)
{
	this->max_length = max_length;
}

void SerachEdit::focusInEvent(QFocusEvent* event)
{
	QLineEdit::focusInEvent(event);
	emit sig_focus_in();
}

void SerachEdit::focusOutEvent(QFocusEvent* event)
{
	QLineEdit::focusOutEvent(event);
	emit sig_focus_out();
}