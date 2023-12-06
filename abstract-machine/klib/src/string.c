#include <klib.h>
#include <klib-macros.h>
#include <stdint.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

size_t strlen(const char *s) {
  size_t len = 0;
  while(*(s + len) != '\0') len++;
  return len;
}

char *strcpy(char *dst, const char *src) {
  size_t len = strlen(src);
  for (int i = 0; i < len; i++) {
    *(dst + i) = *(src + i);
  }
  *(dst + len) = '\0';
  return dst;
}

char *strncpy(char *dst, const char *src, size_t n) {
  panic("Not implemented");
}

char *strcat(char *dst, const char *src) {
  size_t len_dst = strlen(dst);
  size_t len_src = strlen(src);
  for (int i = 0; i < len_src; i++) {
    *(dst + len_dst + i) = *(src + i);
  }
  *(dst + len_dst + len_src) = '\0';
  return dst;
}

int strcmp(const char *s1, const char *s2) {
  size_t len_s1 = strlen(s1);
  size_t len_s2 = strlen(s2);
  size_t upper_bound = len_s1 < len_s2 ? len_s1 : len_s2;
  for (int i = 0; i < upper_bound; i++) {
    if (*(s1 + i) == *(s2 + i)) continue;
    return *(s1 + i) - *(s2 + i);
  }
  return *(s1 + upper_bound) - *(s2 + upper_bound);
}

int strncmp(const char *s1, const char *s2, size_t n) {
  panic("Not implemented");
}

void *memset(void *s, int c, size_t n) {
  for (int i = 0; i < n; i++) {
    *((char*)s + i) = (unsigned char)c;
    // c is used as its unsigned char form
  }
  return s;
}

void *memmove(void *dst, const void *src, size_t n) {
  // dst <- src, 但是保证重叠块能够正常复制
	if (dst < src) { // 如有重叠发生在src前端
		// 此时从前往后复制
		for (int i = 0; i < n; i++) {
			*((char*)dst + i) = *((char*)src + i);
		}
	} else { // 如有重叠发生在src后端
	  // 此时从后往前复制
		for (int i = n - 1; i >= 0; i--) {
			*((char*)dst + i) = *((char*)src + i);
		}
	}
	return dst;
}

void *memcpy(void *out, const void *in, size_t n) {
	for (int i = 0; i < n; i++) {
		*((char*)out + i) = *((char*)in + i);
	}
	return out;
}

int memcmp(const void *s1, const void *s2, size_t n) {
  // char --> 1 byte
  for (int i = 0; i < n; i++) {
    if(*((char*)s1 + i) == *((char*)s2 + i)) continue;
	return *((char*)s1 + i) - *((char*)s2 + i);
  }
  return 0;
}

#endif
