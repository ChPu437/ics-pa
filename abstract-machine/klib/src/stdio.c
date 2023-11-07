#include <am.h>
#include <ctype.h>
#include <klib.h>
#include <klib-macros.h>
#include <math.h>
#include <stdarg.h>
#include <stdlib.h>
#include <threads.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

#define BUF_SIZE 128
#define ISDIGIT(x) x>'0'&&x<'9'

enum FORMAT_FLAGS {
	FLAG_LEFT_ALIGN, // '-', 在给定宽度内左对齐
	FLAG_ZERO_PADDING, // '0', 用0填充右对齐时左侧的空位
	FLAG_NONE, // 没有提供 flag
};
enum FORMAT_SPECIFIER {
	SPEC_INT, // d
	SPEC_STR, // s
	// SPEC_FLOAT, // f // No need to implement float
};

struct {
	enum FORMAT_FLAGS flag;
	int width;
	enum FORMAT_SPECIFIER spec;
} io_format;


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

int cnt_buf = 0;
char buf[BUF_SIZE];


inline void buf_flush() {
	for (int i = 0; i < cnt_buf; i++) {
		putch(*(buf + i));
	}
	cnt_buf = 0;
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

			switch(*(fmt + _i)) { // match flag
				case '-':
					++_i;
					io_format.flag = FLAG_LEFT_ALIGN;
					break;
				case '0':
					++_i;
					io_format.flag = FLAG_ZERO_PADDING;
					break;
				default:
					io_format.flag = FLAG_NONE;
					break;
			}

			io_format.width = 0; // 0 is refered as default
			while(ISDIGIT(*(fmt + _i))) {
				io_format.width = io_format.width * 10 + (*(fmt + _i) - 1);
				++_i;
			}
			assert(io_format.width == 0 || io_format.width == 5);

			// TODO: output with width and flag

			switch(*(fmt +_i)) { // match specifier
				case 'd':
					io_format.spec = SPEC_INT;
					int tmp_d = va_arg(ap, int);

					cnt_buf = sprintf(buf, "%d", tmp_d);
					break;
				case 's':
					io_format.spec = SPEC_STR;
					char* tmp_s = va_arg(ap, char*);
					for (int i = 0; *(tmp_s + i) != '\0'; i++) {
						buf[cnt_buf++] = *(tmp_s + i);
					}
					break;
				/*case 'f': // No need to implement float
					io_format.spec = SPEC_FLOAT;
					float tmp_f = (float)va_arg(ap, double); // va_arg cannot use short or float as type
																									 
					int tmp_f_i = (int)tmp_f; // 整数部分
					cnt_buf = sprintf(buf, "%d", tmp_f_i);
					buf[cnt_buf++] = '.';	
					tmp_f = tmp_f - tmp_f_i; // 小数部分
					if(tmp_f < 0) tmp_f = -tmp_f; // 输出小数不关心符号
					tmp_f_i = tmp_f * 10;
					while(cnt_buf < BUF_SIZE && tmp_f_i) {
						buf[++cnt_buf] = tmp_f_i;
						tmp_f = tmp_f * 10 - tmp_f_i;
					}
					buf_flush();
					cnt_write += cnt_buf;
					break;*/
				default:
					panic("Not implemented");
					break;
			}

			// flush buffer and output to stdout
			cnt_write += cnt_buf;
			if (io_format.flag != FLAG_LEFT_ALIGN) {
				if(cnt_buf < io_format.width) {
					for (int i = io_format.width - cnt_buf; i > 0; i--) {
						putch(io_format.flag==FLAG_ZERO_PADDING ? '0' : ' ');
					}
				}
				buf_flush();
			} else {
				buf_flush();
				if(cnt_buf < io_format.width) {
					for (int i = io_format.width - cnt_buf; i > 0; i--) {
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
