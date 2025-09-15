#include <QApplication>
#include "mainwindow.h"
#include <QFile>
#include <QSettings>
#include <QDir>
#include "loginui.h"
#include "StyleManager.h"

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);

    // 配置文件加载
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

    // 加载全局样式表
    QString globalStyleContent = StyleManager::getInstance()->loadStyleSheetContent(":/style/global.qss");
    if (!globalStyleContent.isEmpty()) {
        a.setStyleSheet(globalStyleContent);
        qDebug() << "Global stylesheet loaded successfully";
    } else {
        // 备用方案：尝试加载原有的样式文件
        QFile file(":/style/stylesheet.qss");
        if (file.open(QFile::ReadOnly)) {
            QString styleSheet = QLatin1String(file.readAll());
            a.setStyleSheet(styleSheet);
            file.close();
            qDebug() << "Legacy stylesheet loaded";
        } else {
            qWarning("Could not load any stylesheet");
        }
    }

    mainwindow w;
    w.show();
    return QApplication::exec();
}
