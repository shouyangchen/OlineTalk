#ifndef SERACHEDIT_H
#define SERACHEDIT_H
#include <QLineEdit>

class SerachEdit:public QLineEdit
{
	Q_OBJECT
private:
	void limit_text_length(QString text);
	int max_length;
public:
	SerachEdit(QWidget* parent=nullptr);
	void set_max_length(int max_length);
	signals:
		void sig_focus_out();
protected:
	void focusOutEvent(QFocusEvent* event) override;
};
#endif
