#include <QApplication>
#include "mainwindow.h"
#include <QFile>
#include <QSettings>
#include <QDir>
#include "loginui.h"

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
	QString appPath = QCoreApplication::applicationDirPath();
	QString configFile ="config.ini";
    QString configFilePath = QDir::toNativeSeparators(appPath + QDir::separator()+ configFile);
	QSettings settings(configFilePath, QSettings::IniFormat);
	QString gate_host = settings.value("GateServer/host").toString();
	QString gate_port = settings.value("GateServer/port").toString();
    if (gate_host.isEmpty() || gate_port.isEmpty()) {
        qWarning("Gate server configuration is missing in config.ini");
        return -1; // Exit if configuration is not set
	}
	gate_url_prefix = QString("http://%1:%2").arg(gate_host, gate_port);

    QFile file(":/style/stylesheet.qss");
    file.open(QFile::ReadOnly);
    if (file.isOpen()) {
        QString styleSheet = QLatin1String(file.readAll());
        a.setStyleSheet(styleSheet);
    } else {
        qWarning("Could not open stylesheet.qss");
    }
    mainwindow w;
    w.show();
    return QApplication::exec();
}
