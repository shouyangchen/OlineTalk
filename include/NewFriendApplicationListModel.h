#ifndef NEWADDFRIENDAPPLICATIONMODEL_H
#define NEWADDFRIENDAPPLICATIONMODEL_H
#include <QAbstractListModel>
#include "global.h"


class NewFriendApplicationListModel : public QAbstractListModel
{
	Q_OBJECT
private:
	QList<NewFriendApplicationlNS::user_application> model;
public:
	enum FriendApplicationRoles {// 自定义角色，从Qt::UserRole开始，避免与Qt预定义角色冲突
		AvatarRole = Qt::UserRole + 14,
		UserNameRole,
		MsgRole,
		TimeRole,
		StatusRole,
		UserIdRole
	};
	explicit NewFriendApplicationListModel(QObject* parent = nullptr);
	~NewFriendApplicationListModel() override;
	void storeApplicationList()const;//永久化申请列表
	void getApplicationList();//获取申请列表
	void getApplicationListAvatar();//获取申请列表头像
	NewFriendApplicationlNS::user_application get_application_user_info(QModelIndex index)const;
	int rowCount(const QModelIndex& parent = QModelIndex()) const override;
	QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
	bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole) override;
	signals:
		void sig_need_loading_history_applications_async();//异步加载历史申请记录
		void sig_store_applications_avatar(const QList<NewFriendApplicationlNS::user_application>& application_list);//存储好友申请头像
		void sig_have_new_application(const NewFriendApplicationlNS::user_application&);//有新的好友申请
public slots:
	void slot_new_application_insert_model(const NewFriendApplicationlNS::user_application& application);
	void slot_new_application_list_insert_model(const std::vector<NewFriendApplicationlNS::user_application>& application_list);
	void slot_history_applications_loading_done(const QList<NewFriendApplicationlNS::user_application>& application_list);
	void slot_the_application_list_user_avatar_loading_done(const QList<std::pair<QPixmap, QString>>&user_avatar_list);
protected:


};
#endif