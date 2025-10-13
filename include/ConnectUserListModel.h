#ifndef CONNECTUSERLISTMODEL_H
#define CONNECTUSERLISTMODEL_H

#include <QPixmap>
#include <QAbstractListModel>
#include <QList>
#include <QString>
#include <QVariant>
#include <QTimer>
#include <QHash>
#include "global.h"

class ConnectUserListModel : public QAbstractListModel
{
    Q_OBJECT

private:
    QHash<QString, connectUserList::user_info> model; // 用hash存储用户信息，key为用户id
    QList<QString> user_name_model; // 存储用户id的列表，作为model的索引
    QTimer* get_user_icon_timer; // 定时获取用户头像
	

public:
    ConnectUserListModel(QObject* parent = nullptr);
    ~ConnectUserListModel() override = default;

    enum connectListModelRole : quint16
    {
        UserNameRole = Qt::UserRole + 11, // 避免与RecentChatListModel冲突
        UserIdRole,
        UserAvatarRole
    };

    // QAbstractListModel 接口实现
    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole) override;

    // 公共接口
    void refreshUserList(); // 手动刷新用户列表
    void refreshUserIcons(); // 手动刷新用户头像
	void get_user_icon(); // 初始化并启动定时获取用户头像

signals:
    void sig_need_loading_user_icon_or_update_user_icon(const QList<QString>&); // 需要加载用户头像或者更新用户头像
    void userListUpdated(int userCount); // 用户列表更新完成
    void iconsUpdated(int iconCount); // 头像更新完成
	void sig_connect_users_name_loading_done(const QList<std::pair<QString, QString>>);//用户名称加载完成
	void sig_connect_user_name_changed(const std::pair<QString, QString>&); // 用户名称改变

public slots:
    void slot_update_connect_user_icon(const QList<std::pair<QPixmap, QString>>& icon_list); // 更新联系人列表头像
    void slot_update_connect_user_list(const QList<connectUserList::user_info>& user_list); // 更新联系人列表
	void slot_get_users_name(const QList<QString>& user_id_list);// 获取用户名称
	void slot_update_user_name(const std::pair<QString, QString>&); // 更新单个用户名称
};

#endif // CONNECTUSERLISTMODEL_H

