#include <cmath>
#include <string>
#include <cassert>
#include <ege.h>
#include "microui_impl_ege.h"
#include "microui.h"

using namespace std;

const int width = 400;
const int height = 300;

const int ROWS = 5;
const int COLS = 4;

// 按键状态
enum {
	BTN_SQRT = 10,
	BTN_AC,
	BTN_DEL,
	BTN_DIV,
	BTN_MUL,
	BTN_SUB,
	BTN_ADD,
	BTN_POINT,
	BTN_SIGN,
	BTN_CALC,
	BTN_MAX,
};

// 状态机状态
enum {
	INPUT_LHS,
	WAIT_RHS,
	INPUT_RHS,
	DISP_ANS,
};

static void mu_calc_display(mu_Context *ctx, const char digits[]) {
	auto r = mu_layout_next(ctx);
	ctx->draw_frame(ctx, r, MU_COLOR_PANELBG);
	mu_draw_control_text(ctx, digits, r, MU_COLOR_TEXT, MU_OPT_ALIGNRIGHT);
}

// 读数字（包括小数点和正负号）
static string input_number(int btn, string cdisp) {
	switch (btn) {
	case BTN_POINT: {
		if (cdisp.find('.') == string::npos) {
			cdisp += '.';
		}
		break;
	}
	case BTN_SIGN: {
		if (cdisp.front() == '-') {
			cdisp = cdisp.substr(1);
		} else {
			cdisp = "-" + cdisp;
		}
		break;
	}
	default: {
		assert(0 <= btn && btn <= 9);
		if (cdisp == "0") {
			cdisp.clear();
		}
		cdisp.push_back(btn + '0');
	}
	}
	return cdisp;
}

// 读运算符
inline char btn2op(int btn) {
	char op;
	switch (btn) {
	case BTN_ADD: {
		op = '+';
		break;
	}
	case BTN_SUB: {
		op = '-';
		break;
	}
	case BTN_MUL: {
		op = '*';
		break;
	}
	case BTN_DIV: {
		op = '/';
		break;
	}
	default: {
		assert(false);
	}
	}
	return op;
}

// 计算
float calc(char op, float lhs, float rhs) {
	float ans = 0;
	switch (op) {
	case '+': ans = lhs + rhs; break;
	case '-': ans = lhs - rhs; break;
	case '*': ans = lhs * rhs; break;
	case '/': ans = lhs / rhs; break;
	}
	return ans;
}

int main(int argc, char **argv) {
	// EGE 初始化
	ege::setinitmode(ege::INIT_ANIMATION);
	ege::initgraph(width, height);
	ege::setbkmode(TRANSPARENT);
	ege::setbkcolor(EGERGB(0, 0, 255));

	// 初始化 microui
	mu_Context ctx_c;
	mu_Context *ctx = &ctx_c;
	mu_init(ctx);

	// 适配 EGE 平台
	microui_impl_ege_init(ctx);

	static int buttons[BTN_MAX];
	static std::string disp = "0";
	static int status = 0;
	static char binop;
	static float lhs;

	// 主循环
	while (ege::is_run()) {
		// 处理输入
		microui_impl_ege_process_events(ctx);

		// 定义 GUI 界面
		mu_begin(ctx);
		if (mu_begin_window(ctx, "Calculator", mu_rect(50, 50, 300, 200))) {
			int width = -1;
			mu_layout_row(ctx, 1, &width, 0);
			mu_calc_display(ctx, disp.c_str());

			// buttons
			auto wrect = mu_get_current_container(ctx)->body;
			int padding = ctx->style->padding;
			auto size = ctx->style->size;
			int spacing = ctx->style->spacing;
			float sw = (wrect.w - padding * 2 - spacing * (COLS - 1)) / float(COLS);
			float sh = (wrect.h - (size.y + padding * 2) - padding * 2 - spacing * ROWS) / float(ROWS);
			int widths[4] = { int(sw), int(ceil(sw)), int(sw), -1 };
			mu_layout_row(ctx, 4, widths, sh);

			buttons[BTN_SQRT]  = mu_button(ctx, "√");
			buttons[BTN_AC]    = mu_button(ctx, "AC");
			buttons[BTN_DEL]   = mu_button(ctx, "←");
			buttons[BTN_DIV]   = mu_button(ctx, "÷");

			buttons[7]         = mu_button(ctx, "7");
			buttons[8]         = mu_button(ctx, "8");
			buttons[9]         = mu_button(ctx, "9");
			buttons[BTN_MUL]   = mu_button(ctx, "×");

			buttons[4]         = mu_button(ctx, "4");
			buttons[5]         = mu_button(ctx, "5");
			buttons[6]         = mu_button(ctx, "6");
			buttons[BTN_SUB]   = mu_button(ctx, "-");

			buttons[1]         = mu_button(ctx, "1");
			buttons[2]         = mu_button(ctx, "2");
			buttons[3]         = mu_button(ctx, "3");
			buttons[BTN_ADD]   = mu_button(ctx, "+");

			buttons[0]         = mu_button(ctx, "0");
			buttons[BTN_POINT] = mu_button(ctx, ".");
			buttons[BTN_SIGN]  = mu_button(ctx, "+/-");
			buttons[BTN_CALC]  = mu_button(ctx, "=");

			mu_end_window(ctx);
		} else {
			// 里窗口关闭时外窗口也关闭
			mu_end(ctx);
			break;
		}
		mu_end(ctx);

		// 实际绘制
		ege::cleardevice();
		microui_impl_ege_draw_data(ctx);

		ege::delay_ms(0);

		// 用状态机运行计算器
		int btn_last = -1;
		for (int i = 0; i < BTN_MAX; ++i) {
			if (buttons[i]) {
				btn_last = i;
				break;
			}
		}
		if (btn_last == -1) {
			continue;
		}
		if (btn_last == BTN_AC) {
			disp = "0";
			status = INPUT_LHS;
			continue;
		}
		switch (status) {
		case INPUT_LHS: {
			switch (btn_last) {
			case BTN_SQRT: {
				disp = to_string(sqrt(stof(disp)));
				status = DISP_ANS;
				break;
			}
			case BTN_DEL: {
				if (disp.size() == 1) {
					disp = "0";
				} else {
					disp = disp.substr(0, disp.size() - 1);
				}
				status = INPUT_LHS;
				break;
			}
			case BTN_CALC: {
				status = INPUT_LHS;
				break;
			}
			case BTN_ADD: case BTN_SUB: case BTN_MUL: case BTN_DIV: {
				lhs = stof(disp);
				binop = btn2op(btn_last);
				status = WAIT_RHS;
				break;
			}
			// 输入数字
			default: {
				disp = input_number(btn_last, disp);
				status = INPUT_LHS;
			}
			}
			break;
		}
		case WAIT_RHS: {
			switch (btn_last) {
			case BTN_SQRT: {
				lhs = sqrt(lhs);
				disp = to_string(lhs);
				status = DISP_ANS;
				break;
			}
			case BTN_DEL: {
				disp = "0";
				status = INPUT_RHS;
				break;
			}
			case BTN_CALC: {
				status = DISP_ANS;
				break;
			}
			case BTN_ADD: case BTN_SUB: case BTN_MUL: case BTN_DIV: {
				binop = btn2op(btn_last);
				break;
			}
			default: {
				disp = input_number(btn_last, "0");
				status = INPUT_RHS;
			}
			}
			break;
		}
		case INPUT_RHS: {
			switch (btn_last) {
			case BTN_SQRT: {
				lhs = sqrt(stof(disp));
				disp = to_string(lhs);
				status = DISP_ANS;
				break;
			}
			case BTN_DEL: {
				if (disp.size() == 1) {
					disp = "0";
				} else {
					disp = disp.substr(0, disp.size() - 1);
				}
				status = INPUT_RHS;
				break;
			}
			case BTN_CALC: {
				lhs = calc(binop, lhs, stof(disp));
				disp = to_string(lhs);
				status = DISP_ANS;
				break;
			}
			case BTN_ADD: case BTN_SUB: case BTN_MUL: case BTN_DIV: {
				lhs = calc(binop, lhs, stof(disp));
				binop = btn2op(btn_last);
				disp = to_string(lhs);
				status = WAIT_RHS;
				break;
			}
			// 输入数字
			default: {
				disp = input_number(btn_last, disp);
				status = INPUT_RHS;
			}
			}
			break;
		}
		case DISP_ANS: {
			switch (btn_last) {
			case BTN_SQRT: {
				disp = to_string(sqrt(stof(disp)));
				status = DISP_ANS;
				break;
			}
			case BTN_DEL: {
				disp = "0";
				status = INPUT_LHS;
				break;
			}
			case BTN_CALC: {
				status = DISP_ANS;
				break;
			}
			case BTN_ADD: case BTN_SUB: case BTN_MUL: case BTN_DIV: {
				lhs = stof(disp);
				binop = btn2op(btn_last);
				status = WAIT_RHS;
				break;
			}
			// 输入数字
			default: {
				disp = input_number(btn_last, "0");
				status = INPUT_LHS;
			}
			}
			break;
		}
		}
	}

	microui_impl_ege_shutdown();
	ege::closegraph();

	return 0;
}


