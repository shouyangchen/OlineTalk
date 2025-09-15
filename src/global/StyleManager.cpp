#include "StyleManager.h"
#include <QFile>
#include <QDebug>

StyleManager* StyleManager::instance = nullptr;

StyleManager* StyleManager::getInstance()
{
    if (instance == nullptr) {
        instance = new StyleManager();
    }
    return instance;
}

bool StyleManager::loadStyleSheet(QWidget* widget, const QString& qssFilePath)
{
    if (!widget) {
        qDebug() << "StyleManager: Widget is null";
        return false;
    }
    
    QString styleContent = loadStyleSheetContent(qssFilePath);
    if (styleContent.isEmpty()) {
        qDebug() << "StyleManager: Failed to load stylesheet:" << qssFilePath;
        return false;
    }
    
    applyStyleSheet(widget, styleContent);
    qDebug() << "StyleManager: Successfully loaded stylesheet:" << qssFilePath;
    return true;
}

QString StyleManager::loadStyleSheetContent(const QString& qssFilePath)
{
    QFile qssFile(qssFilePath);
    if (!qssFile.open(QFile::ReadOnly)) {
        qDebug() << "StyleManager: Cannot open file:" << qssFilePath;
        return QString();
    }
    
    QString content = QString::fromUtf8(qssFile.readAll());
    qssFile.close();
    return content;
}

void StyleManager::applyStyleSheet(QWidget* widget, const QString& styleContent)
{
    if (widget && !styleContent.isEmpty()) {
        widget->setStyleSheet(styleContent);
    }
}