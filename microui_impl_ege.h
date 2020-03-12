#ifndef RENDERER_H
#define RENDERER_H

#include "microui.h"
#include "ege.h"

void r_init(int width, int height, int initmode=ege::INIT_ANIMATION);
void r_end(void);

 int r_get_text_width(mu_Font font, const char *text, int len);
 int r_get_text_height(mu_Font font);

void r_draw_rect(mu_Rect rect, mu_Color color);
void r_draw_text(const char *text, mu_Vec2 pos, mu_Color color);
void r_draw_icon(int id, mu_Rect rect, mu_Color color);
void r_flush(void);
void r_clear(mu_Color color);

void r_set_clip_rect(mu_Rect rect);

int ege2mu_key_map(int key);

void ege2mu_input_mouse(mu_Context *ctx, ege::mouse_msg mmsg);

void loop_process_kbhit(mu_Context * ctx);

#endif

