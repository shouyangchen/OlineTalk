//
// Created by chenshouyang on 25-6-23.
//
#include "global.h"
std::function<void(QWidget*)> repolish_widget_func = [](QWidget*w)  {
    // Default implementation, can be overridden
   w->style()->unpolish(w);//卸载当前样式
   w->style()->polish(w); // 重新应用样式
};

QString gate_url_prefix = "";

