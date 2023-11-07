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
		if(*(fmt + i) == '%') {
			switch(*(fmt + i + 1)) {
				case '%':
					*(out + (cnt_write++)) = '%';
					break;
				case 's':
					break;
				case 'd':
					break;
			}
			i++; // jump over the format specifier, need to be expand to a certain value when the length of specifier gt 1
		} else {
			*(out + (cnt_write++)) = *(fmt + i);
		}
	}

	va_end(ap);
	return cnt_write;
}

int snprintf(char *out, size_t n, const char *fmt, ...) {
  panic("Not implemented");
}

int vsnprintf(char *out, size_t n, const char *fmt, va_list ap) {
  panic("Not implemented");
}

#endif
