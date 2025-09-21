#include "chatitembase.h"

ChatItemBase::ChatItemBase(ChatRole role, QWidget* parent):QWidget(parent),
role_m(role)
{
	this->name_label_m = new QLabel();
	this->name_label_m->setObjectName("chat_user_name");//设置元对象名称
	QFont font("Microsoft YaHei");//创建字体用于设置气泡里的字体设置
	font.setPointSize(9);
	this->name_label_m->setFont(font);
	this->name_label_m->setFixedHeight(20);//设定固定的高度
	this->icon_label_m = new QLabel();
	this->icon_label_m->setScaledContents(true);//允许缩放
	this->icon_label_m->setFixedSize(42, 42);//设定固定的大小
	this->bubble_ml = new QWidget();
	//创建一个网格布局用户将气泡和头像昵称框起来
	QGridLayout* Glayout = new QGridLayout();
	Glayout->setVerticalSpacing(3);//设置控件的之的间垂直空隙
	Glayout->setHorizontalSpacing(3);//设置控件之间的水平间隙
	Glayout->setContentsMargins(3, 3, 3, 3);//设置控件与容器边界之间的间隙
	QSpacerItem* Spacer = new QSpacerItem(40, 20, QSizePolicy::Expanding,QSizePolicy::Minimum);//设置一个拉升控件
	if (this->role_m==ChatRole::SELF)
	{
		this->name_label_m->setContentsMargins(0, 0, 8, 0);//如果消息由哦自己发出
		this->name_label_m->setAlignment(Qt::AlignRight);//那么昵称靠右对齐
		//将昵称控件添加进入网格布局
		Glayout->addWidget(this->name_label_m, 0, 1, 1, 1);
		Glayout->addWidget(this->icon_label_m, 0, 2, 2, 1, Qt::AlignTop);
		Glayout->addItem(Spacer, 1, 0, 1, 1);
		Glayout->addWidget(bubble_ml, 1, 1, 1, 1);
		Glayout->setColumnStretch(0, 2);//设置伸缩因子
		Glayout->setColumnStretch(1, 3);
	}
	else
	{
		this->name_label_m->setContentsMargins(8, 0, 0, 0);//如果消息不由自己发出
		this->name_label_m->setAlignment(Qt::AlignLeft);//那么昵称靠左对齐
		//将昵称控件添加进入网格布局
		Glayout->addWidget(this->name_label_m, 0, 1, 1,1);
		Glayout->addWidget(this->icon_label_m, 0, 0, 2, 1, Qt::AlignTop);
		Glayout->addItem(Spacer, 2, 2, 1, 1);
		Glayout->addWidget(bubble_ml, 1, 1, 1, 1);
		Glayout->setColumnStretch(1, 3);//设置伸缩因子
		Glayout->setColumnStretch(2, 2);
	}
	this->setLayout(Glayout);//将网格布局添加
}


void ChatItemBase::set_user_icon(QPixmap user_icon)
{
	this->icon_label_m->setPixmap(user_icon);
}


void ChatItemBase::set_user_name(QString& name)
{
	this->name_label_m->setText(name);
}

void ChatItemBase::set_widget(QWidget* w)
{
	QGridLayout* grid_layout = (qobject_cast<QGridLayout*>(this->layout()));//获取网格布局
	grid_layout->replaceWidget(this->bubble_ml, w);//将原来气泡换为新的
	delete bubble_ml;//替换之后Qt不在管理被替换的控件的内存所以要手动管理
	this->bubble_ml = w;
}


