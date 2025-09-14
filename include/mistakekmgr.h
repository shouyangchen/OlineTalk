#ifndef MISTAKEKMGR_H
#define MISTAKEKMGR_H
#include <QMap>
#include <QString>
#include "global.h"
class MistakeMgr:QObject
{
Q_OBJECT
private:
    QMap<ErrorCodes,QString>mistakes;
public:
    MistakeMgr(QObject*parent=nullptr);
    virtual~MistakeMgr()=default;
    void add_mistake(std::pair<ErrorCodes,QString>mistake);
    size_t get_mistake_nums();
    void delete_mistake(const ErrorCodes& key);
signals:
    void the_mistake_is_null();
};

#endif // MISTAKEKMGR_H
