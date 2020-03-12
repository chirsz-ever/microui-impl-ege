
#include <ege.h>
#include "microui_impl_ege.h"
#include "microui.h"
#include "microui_demo.h"

const int width = 800;
const int height = 600;

static void process_mouse_events(mu_Context *ctx) {
	while (ege::mousemsg()) {
		ege::mouse_msg mmsg = ege::getmouse();
		ege2mu_input_mouse(ctx, mmsg);
	}
}

static void process_frame(mu_Context *ctx) {
	mu_begin(ctx);
	test_window(ctx);
	log_window(ctx);
	style_window(ctx);
	mu_end(ctx);
}

int main(int argc, char **argv) {
	// EGE 初始化
	ege::setinitmode(ege::INIT_ANIMATION);
	ege::initgraph(width, height);
	ege::setbkmode(TRANSPARENT);

	// 初始化 microui
	mu_Context ctx_c;
	mu_Context *ctx = &ctx_c;
	mu_init(ctx);

	// 适配 EGE 平台
	microui_impl_ege_init(ctx);

	/* main loop */
	while (ege::is_run()) {
		/* handle events */
		process_mouse_events(ctx);
		loop_process_kbhit(ctx);

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

	microui_impl_ege_shutdown();

	return 0;
}


