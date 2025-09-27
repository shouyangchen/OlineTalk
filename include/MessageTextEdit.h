#ifndef MESSAGETEXTEDIT_H
#define MESSAGETEXTEDIT_H

#include <QObject>
#include <QTextEdit>
#include <QMouseEvent>
#include <QApplication>
#include <QDrag>
#include <QMimeData>
#include <QMimeType>
#include <QFileInfo>
#include <QFileIconProvider>
#include <QPainter>
#include <QVector>
#include "global.h"

class MessageTextEdit : public QTextEdit
{
    Q_OBJECT
public:
    explicit MessageTextEdit(QWidget* parent = nullptr);
    ~MessageTextEdit();

    QList<ChatMessage> getMsgList();

    void insertFileFromUrl(const QStringList& urls);

signals:
    void send();

protected:
    void dragEnterEvent(QDragEnterEvent* event) override;
    void dropEvent(QDropEvent* event) override;
    void keyPressEvent(QKeyEvent* e) override;

private:
    void insertImages(const QString& url);
    void insertTextFile(const QString& url);
    bool canInsertFromMimeData(const QMimeData* source) const;
    void insertFromMimeData(const QMimeData* source);

    bool isImage(const QString& url) const;
    void insertChatMessage(QVector<ChatMessage>& list, const QString& type, const QVariant& context, const QPixmap& pix);

    QStringList extractUrlsFromText(const QString& text) const;
    QPixmap getFileIconPixmap(const QString& url) const;
    QString getFileSize(qint64 size) const;

private slots:
    void textEditChanged();

private:
    QVector<ChatMessage> mMsgList;    // 编辑区中的消息对象
    QVector<ChatMessage> mGetMsgList; // getMsgList()中临时收集
};

#endif // MESSAGETEXTEDIT_H