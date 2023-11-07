#include <am.h>
#include <klib.h>
#include <klib-macros.h>
#include <stdarg.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

int printf(const char *fmt, ...) {
  panic("Not implemented");
}

int vsprintf(char *out, const char *fmt, va_list ap) {
  panic("Not implemented");
}

int sprintf(char *out, const char *fmt, ...) {
	int cnt_write = 0;
	va_list ap;
	va_start(ap, fmt);

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
						int len_int = 0;
						while(tmp) {
							*(out + cnt_write + (len_int++)) = tmp % 10;
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
			}
			i++; // jump over the format specifier, need to be expand to a certain value when the length of specifier gt 1
		}	
	}

	va_end(ap);
	*(out + cnt_write) = '\0';

	// TODO: trim blank at back of `cnt_write`
	return cnt_write;
}

int snprintf(char *out, size_t n, const char *fmt, ...) {
  panic("Not implemented");
}

int vsnprintf(char *out, size_t n, const char *fmt, va_list ap) {
  panic("Not implemented");
}

#endif
