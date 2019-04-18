
#include <ege.h>
#include "microui_impl_ege.h"
#include "microui.h"
#include "examples.h"

const int width = 800;
const int height = 600;

static int mu_key_map(int key) {
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

static void process_kbhit(mu_Context * ctx) {
	static char cbuf[100] = " ";
	static wchar_t wcbuf[50] = L" ";
	int len = 0;

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
				wcbuf[len++] = kmsg.key;
			}
			break;
		}
		case ege::key_msg_down: {
			int c = mu_key_map(kmsg.key);
			if (c)
				mu_input_keydown(ctx, c);
			break;
		}
		case ege::key_msg_up: {
			int c = mu_key_map(kmsg.key);
			if (c)
				mu_input_keyup(ctx, c);
			break;
		}
		}
	}
	if (len) {
		size_t bytes = ::WideCharToMultiByte(CP_UTF8, 0, wcbuf, len, cbuf, 100, NULL, NULL);
		if (bytes < 100)cbuf[bytes] = '\0';
		mu_input_text(ctx, cbuf);
	}

}

static void process_mouse_events(mu_Context *ctx) {

	while (ege::mousemsg()) {
		ege::mouse_msg mmsg = ege::getmouse();
		if (mmsg.is_move()) {
			mu_input_mousemove(ctx, mmsg.x, mmsg.y);
		}
		else if (mmsg.is_wheel()) {
			mu_input_mousewheel(ctx, mmsg.wheel / 120);
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
}


static int text_width(mu_Font font, const char *text, int len) {
	if (len == -1) { len = strlen(text); }
	return r_get_text_width(text, len);
}

static int text_height(mu_Font font) {
	return r_get_text_height();
}

static void process_frame(mu_Context *ctx) {
	mu_begin(ctx);
	test_window(ctx);
	log_window(ctx);
	style_window(ctx);
	mu_end(ctx);
}

int main(int argc, char **argv) {
	r_init(width, height);

	/* init microui */
	mu_Context ctx_c;
	mu_Context *ctx = &ctx_c;
	mu_init(ctx);
	ctx->text_width = text_width;
	ctx->text_height = text_height;

	/* main loop */
	while (ege::is_run()) {
		/* handle events */
		process_mouse_events(ctx);
		process_kbhit(ctx);

		/* process frame */
		process_frame(ctx);

		/* render */
		r_clear(mu_color(bg[0], bg[1], bg[2], 255));
		mu_Command *cmd = NULL;
		while (mu_next_command(ctx, &cmd)) {
			switch (cmd->type) {
			case MU_COMMAND_TEXT: r_draw_text(cmd->text.str, cmd->text.pos, cmd->text.color); break;
			case MU_COMMAND_RECT: r_draw_rect(cmd->rect.rect, cmd->rect.color); break;
			case MU_COMMAND_ICON: r_draw_icon(cmd->icon.id, cmd->icon.rect, cmd->icon.color); break;
			case MU_COMMAND_CLIP: r_set_clip_rect(cmd->clip.rect); break;
			}
		}
		r_flush();
	}

	r_end();

	return 0;
}


