#include <ege.h>
#include <assert.h>
#include <cstdio>
#include "microui_impl_ege.h"
#include "atlas.h"

const int width = 800;
const int height = 600;

static ege::PIMAGE textures;
static ege::PIMAGE src_rect;

void r_init(void) {
	// 绘图环境初始化
	ege::setinitmode(ege::INIT_ANIMATION);
	ege::initgraph(width, height);
	ege::setbkmode(TRANSPARENT);
	ege::setfont(16, 0, "Gadugi", NULL);

	textures = ege::newimage();
	src_rect = ege::newimage();
	//ege::setfont(16, 0, "Gadugi",src_rect);
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

static char* utf82gbk(const char u8str[], int len, char dest[], size_t bufsize) {
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
	utf82gbk(text, -1, gbkbuf, sizeof(gbkbuf));
	//ege::setcolor(EGERGB(color.r, color.g, color.b));
	//ege::outtextxy(r_pos.x, r_pos.y, gbkbuf);

	if (*text) { //宽度为0时bug
		const ege::color_t textcolor = EGERGB(color.r, color.g, color.b);
		const ege::color_t textbkcolor = ((textcolor == ege::BLACK) ? (ege::BLACK+1) : ege::BLACK);
		ege::resize(src_rect, r_get_text_width(text, strlen(text)), r_get_text_height());
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
		const unsigned char* tex_pix = atlas_texture[tex_src.y + y] + tex_src.x;
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

int r_get_text_width(const char *text, int len) {
	if (buf_len < len + 1) {
		buf = (char*)realloc(buf, len + 1);
		buf_len = len + 1;
	}
	strncpy(buf, text, len);
	buf[len] = '\0';
	return ege::textwidth(utf82gbk(buf, len, gbkbuf, sizeof(gbkbuf)));
}


int r_get_text_height() {
	return ege::textheight(' ');
}


void r_set_clip_rect(mu_Rect rect) {
	ege::setviewport(rect.x, rect.y, rect.x + rect.w, rect.y + rect.h);
}


void r_clear(mu_Color clr) {
	ege::setbkcolor_f(EGERGBA(clr.r, clr.g, clr.b, clr.a));
	ege::cleardevice();
}

void r_end(void) {
	free(buf);
	ege::delimage(textures);
	ege::closegraph();
}
