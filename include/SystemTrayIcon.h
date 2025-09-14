#ifndef SYSTEMTRAYICON_H
#define SYSTEMTRAYICON_H
#include <qaction.h>
#include <QSystemTrayIcon>
#include <QMenu>
#include "singleton.h"
#include "chat_ui.h"

enum TrayIcon :qint16
{
	OPEN_CHAT_UI = 4001,
	EXIT_APP = 4002,
	SETTING = 4003
};



class SystemTrayIcon:public QSystemTrayIcon,public singleton<SystemTrayIcon>
{
private:
	friend  class singleton<SystemTrayIcon>;
	friend  class chat_ui;
	QMap<TrayIcon, QAction*>TrayIcon_Actions;
	QMenu* context_menu;
	void init_Actions();
	void set_Actions();
	SystemTrayIcon(QObject* parent = nullptr);
	SystemTrayIcon(const QIcon& icon, QObject* parent = nullptr);
protected:
	bool event(QEvent* event) override;
public:
	SystemTrayIcon(SystemTrayIcon&) = delete;
	SystemTrayIcon(SystemTrayIcon&&) = delete;
	SystemTrayIcon& operator=(SystemTrayIcon&) = delete;
	~SystemTrayIcon()override;
};

#endif

