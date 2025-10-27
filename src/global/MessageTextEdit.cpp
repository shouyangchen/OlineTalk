#include "MessageTextEdit.h"
#include <QDebug>
#include <QMessageBox>

MessageTextEdit::MessageTextEdit(QWidget* parent)
    : QTextEdit(parent)
{
    this->setMaximumHeight(60);
    // connect(this, SIGNAL(textChanged()), this, SLOT(textEditChanged()));
}

MessageTextEdit::~MessageTextEdit() {}

QList<ChatMessage> MessageTextEdit::getMsgList()
{
    mGetMsgList.clear();

    QString docText = this->document()->toPlainText();
    QString currText;
    int indexUrl = 0;
    int count = mMsgList.size();

    for (int i = 0; i < docText.size(); ++i)
    {
        if (docText[i] == QChar::ObjectReplacementCharacter)
        {
            if (!currText.isEmpty())
            {
                insertChatMessage(mGetMsgList, "text", currText, QPixmap());
                currText.clear();
            }
            while (indexUrl < count)
            {
                const ChatMessage& msg = mMsgList[indexUrl];
                // 对于图片或文件，message_context是url或路径
                if (this->document()->toHtml().contains(msg.message_context.toString(), Qt::CaseSensitive))
                {
                    ++indexUrl;
                    mGetMsgList.append(msg);
                    break;
                }
                ++indexUrl;
            }
        }
        else
        {
            currText.append(docText[i]);
        }
    }
    if (!currText.isEmpty())
    {
        insertChatMessage(mGetMsgList, "text", currText, QPixmap());
        currText.clear();
    }
    mMsgList.clear();
    this->clear();
    return mGetMsgList.toList();
}

void MessageTextEdit::dragEnterEvent(QDragEnterEvent* event)
{
    if (event->source() == this)
        event->ignore();
    else
        event->accept();
}

void MessageTextEdit::dropEvent(QDropEvent* event)
{
    insertFromMimeData(event->mimeData());
    event->accept();
}

void MessageTextEdit::keyPressEvent(QKeyEvent* e)
{
    if ((e->key() == Qt::Key_Enter || e->key() == Qt::Key_Return) && !(e->modifiers() & Qt::ShiftModifier))
    {
        emit send();
        return;
    }
    QTextEdit::keyPressEvent(e);
}

void MessageTextEdit::insertFileFromUrl(const QStringList& urls)
{
    if (urls.isEmpty())
        return;

    for (const QString& url : urls)
    {
        if (isImage(url))
            insertImages(url);
        else
            insertTextFile(url);
    }
}

void MessageTextEdit::insertImages(const QString& url)
{
    QImage image(url);
    if (image.width() > 120 || image.height() > 80)
    {
        if (image.width() > image.height())
            image = image.scaledToWidth(120, Qt::SmoothTransformation);
        else
            image = image.scaledToHeight(80, Qt::SmoothTransformation);
    }
    QTextCursor cursor = this->textCursor();
    cursor.insertImage(image, url);

    insertChatMessage(mMsgList, "image", url, QPixmap::fromImage(image));
}

void MessageTextEdit::insertTextFile(const QString& url)
{
    QFileInfo fileInfo(url);
    if (fileInfo.isDir())
    {
        QMessageBox::information(this, "提示", "只允许拖拽单个文件!");
        return;
    }

    if (fileInfo.size() > 100 * 1024 * 1024)
    {
        QMessageBox::information(this, "提示", "发送的文件大小不能大于100M");
        return;
    }

    QPixmap pix = getFileIconPixmap(url);
    QTextCursor cursor = this->textCursor();
    cursor.insertImage(pix.toImage(), url);
    insertChatMessage(mMsgList, "file", url, pix);
}

bool MessageTextEdit::canInsertFromMimeData(const QMimeData* source) const
{
    return QTextEdit::canInsertFromMimeData(source);
}

void MessageTextEdit::insertFromMimeData(const QMimeData* source)
{
    QStringList urls = extractUrlsFromText(source->text());
    if (urls.isEmpty())
        return;

    for (const QString& url : urls)
    {
        if (isImage(url))
            insertImages(url);
        else
            insertTextFile(url);
    }
}

bool MessageTextEdit::isImage(const QString& url) const
{
    static const QStringList imageFormatList = QString("bmp,jpg,png,tif,gif,pcx,tga,exif,fpx,svg,psd,cdr,pcd,dxf,ufo,eps,ai,raw,wmf,webp").split(",");
    QFileInfo fileInfo(url);
    QString suffix = fileInfo.suffix();
    return imageFormatList.contains(suffix, Qt::CaseInsensitive);
}

void MessageTextEdit::insertChatMessage(QVector<ChatMessage>& list, const QString& type, const QVariant& context, const QPixmap& pix)
{
    ChatMessage msg;
    msg.message_type = type;
    msg.message_context = context;
	msg.pixmap = pix;//如果为图片或文件则存储缩略图
    msg.timestamp = QDateTime::currentDateTime();
    //msg.sender = "SELF"; // 这里默认发消息的是自己
    list.append(msg);
}

QStringList MessageTextEdit::extractUrlsFromText(const QString& text) const
{
    QStringList urls;
    if (text.isEmpty()) return urls;

    QStringList list = text.split("\n");
    for (const QString& url : list)
    {
        if (!url.isEmpty())
        {
            QStringList str = url.split("///");
            if (str.size() >= 2)
                urls.append(str.at(1));
        }
    }
    return urls;
}

QPixmap MessageTextEdit::getFileIconPixmap(const QString& url) const
{
    QFileIconProvider provider;
    QFileInfo fileinfo(url);
    QIcon icon = provider.icon(fileinfo);

    QString strFileSize = getFileSize(fileinfo.size());
    QFont font(QString("宋体"), 10, QFont::Normal, false);
    QFontMetrics fontMetrics(font);
    QSize textSize = fontMetrics.size(Qt::TextSingleLine, fileinfo.fileName());
    QSize fileSize = fontMetrics.size(Qt::TextSingleLine, strFileSize);
    int maxWidth = qMax(textSize.width(), fileSize.width());
    QPixmap pix(50 + maxWidth + 10, 50);
    pix.fill();

    QPainter painter;
    painter.begin(&pix);
    QRect rect(0, 0, 50, 50);
    painter.drawPixmap(rect, icon.pixmap(40, 40));
    painter.setPen(Qt::black);
    QRect rectText(50 + 10, 3, textSize.width(), textSize.height());
    painter.drawText(rectText, fileinfo.fileName());
    QRect rectFile(50 + 10, textSize.height() + 5, fileSize.width(), fileSize.height());
    painter.drawText(rectFile, strFileSize);
    painter.end();
    return pix;
}

QString MessageTextEdit::getFileSize(qint64 size) const
{
    QString Unit;
    double num;
    if (size < 1024) {
        num = size;
        Unit = "B";
    }
    else if (size < 1024 * 1024) {
        num = size / 1024.0;
        Unit = "KB";
    }
    else if (size < 1024 * 1024 * 1024) {
        num = size / 1024.0 / 1024.0;
        Unit = "MB";
    }
    else {
        num = size / 1024.0 / 1024.0 / 1024.0;
        Unit = "GB";
    }
    return QString::number(num, 'f', 2) + " " + Unit;
}

void MessageTextEdit::textEditChanged()
{
    // 可扩展：文本变化时处理逻辑
}