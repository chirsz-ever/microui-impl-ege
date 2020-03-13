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

void r_draw_rect(mu_Rect rect, mu_Color color);
void r_draw_text(const char *text, mu_Vec2 pos, mu_Color color);
void r_draw_icon(int id, mu_Rect rect, mu_Color color);

void r_set_clip_rect(mu_Rect rect);

#endif

