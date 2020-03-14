
#include <ege.h>
#include "microui_impl_ege.h"
#include "microui.h"
#include "microui_demo.h"

const int width = 800;
const int height = 600;

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
		microui_impl_ege_process_events(ctx);

		/* process frame */
		process_frame(ctx);

		/* render */
		ege::setbkcolor_f(EGERGBA((int)bg[0], (int)bg[1], (int)bg[2], (int)bg[3]));
		ege::cleardevice();
		microui_impl_ege_draw_data(ctx);

		ege::delay_ms(0);
	}

	microui_impl_ege_shutdown();
	ege::closegraph();

	return 0;
}


