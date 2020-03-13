#ifndef RENDERER_H
#define RENDERER_H

#include "microui.h"
#include "ege.h"

// 初始化 EGE 与 mu_Context 后调用此初始化函数
void microui_impl_ege_init(mu_Context *ctx);

// 调用 ege::closegraph() 前调用此函数
void microui_impl_ege_shutdown();

// 通过 EGE 读取键盘鼠标输入。程序其它部分要用到键盘
// 鼠标输入的可通过 ctx 中相关成员获取
void microui_impl_ege_process_events(mu_Context *ctx);

// 从 microui 读取指令，绘制 UI 界面
void microui_impl_ege_draw_data(mu_Context *ctx);

#endif

