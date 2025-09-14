#include "SystemTrayIcon.h"


SystemTrayIcon::SystemTrayIcon(QObject* parent):QSystemTrayIcon(parent)
{
	this->context_menu = new QMenu();
	this->init_Actions();
	this->set_Actions();
}

SystemTrayIcon::SystemTrayIcon(const QIcon& icon, QObject* parent):QSystemTrayIcon(icon,parent)
{
	this->context_menu = new QMenu();
	this->init_Actions();
	this->set_Actions();
}


bool SystemTrayIcon::event(QEvent* event)
{
	return QSystemTrayIcon::event(event);
}

void SystemTrayIcon::init_Actions()
{
	this->TrayIcon_Actions.insert(TrayIcon::OPEN_CHAT_UI, new QAction(u8"打开界面",this));
	this->TrayIcon_Actions.insert(TrayIcon::EXIT_APP, new QAction(u8"退出",this));
	this->TrayIcon_Actions.insert(TrayIcon::SETTING, new QAction(u8"设置", this));
}


void SystemTrayIcon::set_Actions()
{
	this->context_menu->addAction(this->TrayIcon_Actions[SETTING]);
	this->context_menu->addAction(this->TrayIcon_Actions[EXIT_APP]);
	this->context_menu->addAction(this->TrayIcon_Actions[OPEN_CHAT_UI]);	
	this->setContextMenu(this->context_menu);
}

SystemTrayIcon::~SystemTrayIcon()
{
	delete this->context_menu;
}
