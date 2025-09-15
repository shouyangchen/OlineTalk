#ifndef STYLEMANAGER_H
#define STYLEMANAGER_H

#include <QString>
#include <QWidget>

class StyleManager
{
public:
    static StyleManager* getInstance();
    
    // 加载QSS文件并应用到指定的widget
    bool loadStyleSheet(QWidget* widget, const QString& qssFilePath);
    
    // 从资源文件加载QSS内容
    QString loadStyleSheetContent(const QString& qssFilePath);
    
    // 应用样式表到widget
    void applyStyleSheet(QWidget* widget, const QString& styleContent);

private:
    StyleManager() = default;
    static StyleManager* instance;
};

#endif // STYLEMANAGER_H