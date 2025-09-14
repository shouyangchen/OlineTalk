#pragma once

#include <QLineEdit>
#include <QObject>
#include "mistakekmgr.h"
class InputLine  : public QLineEdit
{
	Q_OBJECT
		friend class MistakeMgr;
public slots:

public:
	InputLine(QObject *parent);
	virtual  ~InputLine();
};

