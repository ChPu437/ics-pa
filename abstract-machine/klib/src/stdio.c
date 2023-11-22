#include <am.h>
#include <ctype.h>
#include <klib.h>
#include <klib-macros.h>
#include <math.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdlib.h>
#include <threads.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

#define BUF_SIZE 128
#define ISDIGIT(x) x>'0'&&x<'9'

#define BITMASK(bits) ((1ull << (bits)) - 1)
#define BITS(x, hi, lo) (((x) >> (lo)) & BITMASK((hi) - (lo) + 1)) // similar to x[hi:lo] in verilog

static void TODO(){
	panic("Not implemented.\n");
}


enum FORMAT_FLAGS {
	FLAG_LEFT_ALIGN, // '-', 在给定宽度内左对齐
	FLAG_FORCE_SIGN, // '+', 强制显示正负号
	FLAG_MARGIN_SIGN, // ' ', 无符号时空出符号位置
	FLAG_FORCE_DECO, // 强制输出十六进制数的0x/0X或浮点数的小数点(不用实现浮点数)
	FLAG_ZERO_PADDING, // '0', 用0填充右对齐时左侧的空位
	FLAG_NONE, // 没有提供 flag
};
enum FORMAT_SPECIFIER {
	SPEC_INT, // d
	SPEC_STR, // s
	SPEC_CHAR, // c
	SPEC_HEX, // x or X, unsigned hex
	// SPEC_FLOAT, // f // No need to implement float
};

static struct {
	enum FORMAT_FLAGS flag;
	int width;
	enum FORMAT_SPECIFIER spec;
} io_format;
static int cnt_buf = 0;
static char buf[BUF_SIZE];
static void buf_flush() {
	for (int i = 0; i < cnt_buf; i++) {
		putch(*(buf + i));
	}
	cnt_buf = 0;
}
/* static void buf_flush_str(char *out) { // out 应该是输出位置, 也即输出前的'\0'位置

} */

int vsprintf(char *out, const char *fmt, va_list ap) {
	int cnt_write = 0;

	for (int i = 0; *(fmt + i) != '\0'; i++) {
		if(*(fmt + i) != '%') {
			*(out + (cnt_write++)) = *(fmt + i);
		} else {
			int tmp;
			switch(*(fmt + i + 1)) {
				case '%': // '%' itself
					*(out + (cnt_write++)) = '%';
					break;
				case 's': // str (char*)
					tmp = 0;
					char* src = va_arg(ap, char*);
					while(*(src + tmp) != '\0') {
						*(out + (cnt_write++)) = *(src + tmp);
						++tmp;
					}
					break;
				case 'd': // 4-byte integer
					tmp = va_arg(ap, int);
					if (tmp == 0) {
						*(out + (cnt_write++)) = '0';
					} else {
						if (tmp < 0) { // check if is negative, we treat it as '-' + str(-tmp)
							*(out + (cnt_write++)) = '-';
							tmp = -tmp;
						}
						// Load the digit reversly and then swap it 
						// TODO: rewrite this with atoi()
						int len_int = 0;
						while(tmp) {
							*(out + cnt_write + (len_int++)) = tmp % 10 + '0';
							tmp /= 10;
						}
						for (int j = 0; j < len_int / 2; j++) {
							char *x = out + cnt_write + j;
							char *y = out + cnt_write + len_int - j - 1;
							char z;
							z = *x;
							*x = *y;
							*y = z;
						}
						cnt_write += len_int; // Update cnt_write
					}
					break;
				default:
					panic("Not implemented");
					break;
			}
			i++; // jump over the format specifier, need to be expand to a certain value when the length of specifier gt 1
		}	
	}

	va_end(ap);
	*(out + cnt_write) = '\0';
	// TODO: trim blank at back of `cnt_write`
	return cnt_write;
}

int sprintf(char *out, const char *fmt, ...) {
	va_list ap;
	va_start(ap, fmt);

	return vsprintf(out, fmt, ap);
}

int snprintf(char *out, size_t n, const char *fmt, ...) {
  panic("Not implemented");
}

int vsnprintf(char *out, size_t n, const char *fmt, va_list ap) {
  panic("Not implemented");
}

int printf(const char *fmt, ...) {
	int cnt_write = 0;
	va_list ap;
	va_start(ap, fmt);

	for (int _i = 0; *(fmt + _i) != '\0'; _i++) {
		if(*(fmt + _i) != '%') {
				++cnt_write;
				putch(*(fmt + _i));
		} else {
			++_i; // jump over the starting '%'
			// %[flags][width][.precision][length]specifier
			// need to implement first: '-' and [width]
			if (*(fmt + _i) == '%') { // "%%"
				++cnt_write;
				putch('%');
				continue;
			}	

			bool flag_s = 1;
			while (flag_s) {
				switch(*(fmt + (_i++))) { // match flag
					case '-':
						io_format.flag |= FLAG_LEFT_ALIGN;
						break;
					case '+':
						io_format.flag |= FLAG_FORCE_SIGN;
						break;
					case ' ':
						io_format.flag |= FLAG_MARGIN_SIGN;
						break;
					case '#':
						io_format.flag |= FLAG_FORCE_DECO;
						break;
					case '0':
						io_format.flag |= FLAG_ZERO_PADDING;
						break;
					default:
						--_i; // return to last position
						io_format.flag = FLAG_NONE;
						flag_s = 0;
						break;
				}
			}

			io_format.width = 0; // 0 is refered as default
			while(ISDIGIT(*(fmt + _i))) {
				io_format.width = io_format.width * 10 + (*(fmt + _i) - '0');
				++_i;
			}

			switch(*(fmt + _i)) { // match specifier
				case 'd':
					io_format.spec = SPEC_INT;
					int tmp_d = va_arg(ap, int);

					cnt_buf = sprintf(buf, "%d", tmp_d);
					break;
				case 'u':
					TODO();
					break;
				case 's':
					io_format.spec = SPEC_STR;
					char* tmp_s = va_arg(ap, char*);
					for (int i = 0; *(tmp_s + i) != '\0'; i++) {
						buf[cnt_buf++] = *(tmp_s + i);
					}
					break;
				case 'c':
					io_format.spec = SPEC_CHAR;
					buf[cnt_buf++] = (char)va_arg(ap, int);
					break;
				case 'o':
					TODO();
					break;
				case 'x':
				case 'X':
					io_format.spec = SPEC_HEX;
					uint32_t tmp_x = va_arg(ap, unsigned int);
					for (int i = 8; i > 0; i--) {
						uint32_t tmp_buf = BITS(tmp_x, 4 * i - 1, 4 * (i - 1));
						if(!tmp_buf) continue;
						if(tmp_buf <= 9) {
							buf[cnt_buf++] = tmp_buf + '0';
						} else {
							buf[cnt_buf++] = tmp_buf - 9 + 'A' - 1;
						}
					}
					break;
				case 'p':
					TODO();
					break;
				/* case 'f': // No need to implement float */
				default:
					putch('\n'); putch('!'); putch(*(fmt + _i));
					panic("\nNot implemented\n");
					break;
			}

			// flush buffer and output to stdout
			cnt_write += cnt_buf;
			if (io_format.flag & FLAG_LEFT_ALIGN) {
				if(cnt_buf < io_format.width) {
					for (int i = io_format.width - cnt_buf; i > 0; i--) {
						putch(io_format.flag & FLAG_ZERO_PADDING ? '0' : ' ');
					}
				}
				buf_flush();
			} else {
				if(cnt_buf < io_format.width) {
					int i = io_format.width - cnt_buf;
					buf_flush();
					for ( ; i > 0; i--) {
						putch(' ');
					}
				}
			}
		}
	}

	va_end(ap);

	return cnt_write;
}


#endif
