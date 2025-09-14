#include "gif_play_view.h"
#include <QMovie>
#include <QVBoxLayout>
#include <QApplication>
#include <QScreen>

gif_play_view::gif_play_view(QWidget* parent)
    : QDialog(parent)
    , ui(new Ui::gif_play_view)
{
    ui->setupUi(this);
    this->ui->label = new QLabel(this);

    // 创建布局并设置为0边距
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(this->ui->label);
    layout->setContentsMargins(0, 0, 0, 0);  // 移除所有边距
    layout->setSpacing(0);  // 移除控件间距
    this->setLayout(layout);

    // 设置窗口属性
    this->setAttribute(Qt::WA_DeleteOnClose);
    this->setAttribute(Qt::WA_TranslucentBackground); // 背景透明
    this->setFixedSize(100, 100);
    this->setWindowFlag(Qt::SplashScreen);
    this->setWindowFlag(Qt::FramelessWindowHint); // 无边框
    
    // 设置 QLabel 属性
    this->ui->label->setScaledContents(true);
    this->ui->label->setAlignment(Qt::AlignCenter);
    this->ui->label->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    // 替换以下代码
    // this->move(QApplication::desktop()->screen()->rect().center() - this->rect().center());

    // 使用 QGuiApplication 和 QScreen 替代已弃用的 QApplication::desktop()
    QScreen* screen = QGuiApplication::primaryScreen();
    if (screen)
    {
        QRect screenGeometry = screen->geometry();
        this->move(screenGeometry.center() - this->rect().center());
    }
}

void gif_play_view::set_gif(std::shared_ptr<QMovie> gif)
{
    if (!gif->isValid())
    {
        qDebug() << "gif 无效！";
        return;
    }
    
    // 设置 GIF 缩放到与窗口相同大小
    gif->setScaledSize(QSize(100, 100)); // 与窗口大小一致
    
    this->ui->label->setMovie(gif.get());
    this->ui->label->setAlignment(Qt::AlignCenter);
}

gif_play_view::~gif_play_view()
{
    delete ui;
}
