#include <ege.h>
#include <assert.h>
#include <cstdio>
#include "microui_impl_ege.h"
#include "atlas.h"

static ege::PIMAGE src_rect;

void microui_impl_ege_init(mu_Context *ctx) {
	src_rect = ege::newimage();

	// 字体设置
	setfont(
		16,
		0,
		"Gadugi",
		0,
		0,
		0,
		false,
		false,
		false,
		DEFAULT_CHARSET,
		OUT_DEFAULT_PRECIS,
		CLIP_DEFAULT_PRECIS,
		NONANTIALIASED_QUALITY,
		DEFAULT_PITCH,
		src_rect
	);
	ctx->text_width = r_get_text_width;
	ctx->text_height = r_get_text_height;

	ege::setbkmode(TRANSPARENT,src_rect);
	ege::setfontbkcolor(ege::BLACK, src_rect);

	//ege::getimage(textures, "IMAGE", "IDR_IMAGE1");
	//ege::getimage(textures, "textures.jpg");

}

static mu_Vec2 real_pos(int px, int py) {
	static int orgn_x, orgn_y;
	ege::getviewport(&orgn_x, &orgn_y, NULL, NULL);
	return mu_vec2(px - orgn_x, py - orgn_y);
}

void r_draw_rect(mu_Rect rect, mu_Color color) {
	mu_Vec2 r_pos = real_pos(rect.x, rect.y);
	ege::resize(src_rect, rect.w, rect.h);
	ege::setbkcolor_f(EGERGB(color.r, color.g, color.b), src_rect);
	ege::cleardevice(src_rect);
	ege::putimage_alphablend(NULL, src_rect, r_pos.x, r_pos.y, color.a);
}

static char gbkbuf[128 * 3];

static char* utf82ansi(const char u8str[], int len, char dest[], size_t bufsize) {
	static wchar_t wcbuf[512];
	if (len < 0)
		len = strlen(u8str);
	int wclen = ::MultiByteToWideChar(CP_UTF8, 0, u8str, len, wcbuf, 512);
	wcbuf[wclen] = '\0';
	int clen = ::WideCharToMultiByte(CP_ACP, 0, wcbuf, wclen, dest, bufsize, NULL, NULL);
	dest[clen] = '\0';
	return dest;
}

void r_draw_text(const char *text, mu_Vec2 pos, mu_Color color) {
	mu_Vec2 r_pos = real_pos(pos.x, pos.y);
	utf82ansi(text, -1, gbkbuf, sizeof(gbkbuf));
	//ege::setcolor(EGERGB(color.r, color.g, color.b));
	//ege::outtextxy(r_pos.x, r_pos.y, gbkbuf);

	if (*text) { //宽度为0时bug
		const ege::color_t textcolor = EGERGB(color.r, color.g, color.b);
		const ege::color_t textbkcolor = ((textcolor == ege::BLACK) ? (ege::BLACK+1) : ege::BLACK);
		ege::resize(src_rect, r_get_text_width(nullptr, text, strlen(text)), r_get_text_height(nullptr));
		ege::setbkcolor_f(textbkcolor, src_rect);
		ege::cleardevice(src_rect);
		ege::setcolor(textcolor, src_rect);
		ege::outtext(gbkbuf, src_rect);
		ege::putimage_alphatransparent(NULL, src_rect, r_pos.x, r_pos.y, textbkcolor, color.a);
		//ege::putimage_transparent(NULL, r_pos.x, r_pos.y, src_rect);
	}
}

void r_draw_icon(int id, mu_Rect rect, mu_Color color) {
	const mu_Vec2 r_pos = mu_vec2(rect.x,rect.y);
	const mu_Rect tex_src = atlas[id];
	const int px = r_pos.x + (rect.w - tex_src.w) / 2;
	const int py = r_pos.y + (rect.h - tex_src.h) / 2;
	const float dst_alpha = color.a / 255.0f;
	const ege::color_t base_color = EGERGB(color.r, color.g, color.b);

	// 在src_rect上绘制ICON
	ege::resize(src_rect, tex_src.w, tex_src.h);
	for (int y = 0; y < tex_src.h; ++y) {
		const unsigned char* tex_pix = atlas_texture + (tex_src.y + y) * ATLAS_WIDTH + tex_src.x;
		for (int x = 0; x < tex_src.w; ++x) {
			const unsigned char alpha = *tex_pix * dst_alpha;
			ege::putpixel_f(x, y, EGECOLORA(base_color, alpha), src_rect);
			++tex_pix;
		}
	}
	
	ege::putimage_withalpha(NULL,src_rect,px, py);
}

void r_flush(void)
{
	ege::delay_ms(0);
}

static char* buf;
static int buf_len = 0;

int r_get_text_width(mu_Font font, const char *text, int len) {
	if (len == -1) { len = strlen(text); }
	if (buf_len < len + 1) {
		buf = (char*)realloc(buf, len + 1);
		buf_len = len + 1;
	}
	strncpy(buf, text, len);
	buf[len] = '\0';
	return ege::textwidth(utf82ansi(buf, len, gbkbuf, sizeof(gbkbuf)), src_rect);
}


int r_get_text_height(mu_Font font) {
	return ege::textheight(' ', src_rect);
}


void r_set_clip_rect(mu_Rect rect) {
	ege::setviewport(rect.x, rect.y, rect.x + rect.w, rect.y + rect.h);
}


void r_clear(mu_Color clr) {
	ege::setbkcolor_f(EGERGBA(clr.r, clr.g, clr.b, clr.a));
	ege::cleardevice();
}

void microui_impl_ege_shutdown() {
	free(buf);
	ege::delimage(src_rect);
	ege::closegraph();
}

int ege2mu_key_map(int key) {
	int res;
	using namespace ege;
	switch (key)
	{
	case key_shift:
	case key_shift_l:
	case key_shift_r:
		res = MU_KEY_SHIFT;
		break;
	case key_control:
	case key_control_l:
	case key_control_r:
		res = MU_KEY_CTRL;
		break;
	case key_menu:
	case key_menu_l:
	case key_menu_r:
		res = MU_KEY_ALT;
		break;
	case key_enter:
		res = MU_KEY_RETURN;
		break;
	case key_back:
		res = MU_KEY_BACKSPACE;
		break;
	default:
		res = 0;
	}
	return res;
}

void ege2mu_input_mouse(mu_Context *ctx, ege::mouse_msg mmsg) {
	if (mmsg.is_move()) {
		mu_input_mousemove(ctx, mmsg.x, mmsg.y);
	}
	else if (mmsg.is_wheel()) {
		mu_input_scroll(ctx, 0, -mmsg.wheel / 10);
	}
	else if (mmsg.is_left()) {
		if (mmsg.is_down())
			mu_input_mousedown(ctx, mmsg.x, mmsg.y, MU_MOUSE_LEFT);
		else if (mmsg.is_up())
			mu_input_mouseup(ctx, mmsg.x, mmsg.y, MU_MOUSE_LEFT);
	}
	else if (mmsg.is_right()) {
		if (mmsg.is_down())
			mu_input_mousedown(ctx, mmsg.x, mmsg.y, MU_MOUSE_RIGHT);
		else if (mmsg.is_up())
			mu_input_mouseup(ctx, mmsg.x, mmsg.y, MU_MOUSE_RIGHT);
	}
	else if (mmsg.is_mid()) {
		if (mmsg.is_down())
			mu_input_mousedown(ctx, mmsg.x, mmsg.y, MU_MOUSE_MIDDLE);
		else if (mmsg.is_up())
			mu_input_mouseup(ctx, mmsg.x, mmsg.y, MU_MOUSE_MIDDLE);
	}
}

const size_t INPUTBUFSIZE = 128;

static char* ansi2utf8(const char ansistr[], int len, char dest[], size_t bufsize) {
	static wchar_t wcbuf[INPUTBUFSIZE];
	int wclen = ::MultiByteToWideChar(CP_ACP, 0, ansistr, len, wcbuf, INPUTBUFSIZE);
	wcbuf[wclen] = '\0';
	int clen = ::WideCharToMultiByte(CP_UTF8, 0, wcbuf, wclen, dest, bufsize, NULL, NULL);
	dest[clen] = '\0';
	return dest;
}

void loop_process_kbhit(mu_Context * ctx) {
	static char cbuf[INPUTBUFSIZE * 3] = " ";
	static wchar_t wcbuf[INPUTBUFSIZE] = L" ";
	static char mbcsbuf[INPUTBUFSIZE * 2] = " ";
	int len = 0;
	bool is_unicode_win = IsWindowUnicode(ege::getHWnd());

	while (ege::kbmsg()) {
		ege::key_msg kmsg = ege::getkey();
		switch (kmsg.msg)
		{
		case ege::key_msg_char: {
			switch (kmsg.key)
			{
			case '\n':
			case '\r':
			case '\b':
			case '\t':
				break;
			default:
				if (is_unicode_win) {
					wcbuf[len++] = kmsg.key;
				} else {
					mbcsbuf[len++] = kmsg.key;
				}
			}
			break;
		}
		case ege::key_msg_down: {
			int c = ege2mu_key_map(kmsg.key);
			if (c)
				mu_input_keydown(ctx, c);
			break;
		}
		case ege::key_msg_up: {
			int c = ege2mu_key_map(kmsg.key);
			if (c)
				mu_input_keyup(ctx, c);
			break;
		}
		}
	}
	if (len) {
		if (is_unicode_win) {
			size_t bytes = ::WideCharToMultiByte(CP_UTF8, 0, wcbuf, len, cbuf, INPUTBUFSIZE * 3, NULL, NULL);
			if (bytes < INPUTBUFSIZE * 3) {
				cbuf[bytes] = '\0';
			}
		} else {
			mbcsbuf[len] = '\0';
			ansi2utf8(mbcsbuf, -1, cbuf, INPUTBUFSIZE * 3);
		}
		mu_input_text(ctx, cbuf);
	}

}
