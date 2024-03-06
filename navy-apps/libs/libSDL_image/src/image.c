#define SDL_malloc  malloc
#define SDL_free    free
#define SDL_realloc realloc

#define SDL_STBIMAGE_IMPLEMENTATION
#include "SDL_stbimage.h"

SDL_Surface* IMG_Load_RW(SDL_RWops *src, int freesrc) {
  assert(src->type == RW_TYPE_MEM);
  assert(freesrc == 0);
  return NULL;
}

SDL_Surface* IMG_Load(const char *filename) {
	printf("loading image!\n");
	FILE* img = fopen(filename, "r");
	// move the pointer to end to get file length;
	fseek(img, 0, SEEK_END);
	size_t img_size = ftell(img);
	fseek(img, 0, SEEK_SET);

	char* buf = malloc(img_size);
	fread(buf, sizeof(char), img_size, img);
	SDL_Surface* img_surface = STBIMG_LoadFromMemory(buf, img_size);
	free(buf);
	fclose(img);

	printf("Done loading image!\n");
  return img_surface;
}

int IMG_isPNG(SDL_RWops *src) {
  return 0;
}

SDL_Surface* IMG_LoadJPG_RW(SDL_RWops *src) {
  return IMG_Load_RW(src, 0);
}

char *IMG_GetError() {
  return "Navy does not support IMG_GetError()";
}
