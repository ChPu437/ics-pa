#include <NDL.h>
#include <SDL.h>
#include <string.h>

#define keyname(k) #k,

static const char *keyname[] = {
  "NONE",
  _KEYS(keyname)
};

int SDL_PushEvent(SDL_Event *ev) {
	NDL_TODO("");
  return 0;
}

int SDL_PollEvent(SDL_Event *ev) {
	NDL_TODO("");
  return 0;
}

int SDL_WaitEvent(SDL_Event *event) {
	char buf[32];
	int* sym = &event->key.keysym.sym;
	while (!NDL_PollEvent(buf, sizeof(buf) + 1));
	printf("SDLK: %d\n", *sym);
	switch (buf[1]) {
		case 'd':
			event->type = SDL_KEYDOWN;
			// TODO: regex-ify the compare
			if (strcmp(buf, "kd UP")) 	*sym = SDLK_UP;
			/* if (strcmp(buf, "kd DOWN")) *sym = SDLK_DOWN;
			if (strcmp(buf, "kd 0")) 		*sym = SDLK_0;
			if (strcmp(buf, "kd 1")) 		*sym = SDLK_1;
			if (strcmp(buf, "kd 2")) 		*sym = SDLK_2;
			if (strcmp(buf, "kd 3")) 		*sym = SDLK_3;
			if (strcmp(buf, "kd 4")) 		*sym = SDLK_4;
			if (strcmp(buf, "kd 5")) 		*sym = SDLK_5;
			if (strcmp(buf, "kd 6")) 		*sym = SDLK_6;
			if (strcmp(buf, "kd 7")) 		*sym = SDLK_7;
			if (strcmp(buf, "kd 8")) 		*sym = SDLK_8;
			if (strcmp(buf, "kd 9")) 		*sym = SDLK_9;
			if (strcmp(buf, "kd G")) 		*sym = SDLK_G;
			if (strcmp(buf, "kd J")) 		*sym = SDLK_J;
			if (strcmp(buf, "kd K")) 		*sym = SDLK_K; */
			break;
		case 'u':
			event->type = SDL_KEYUP;
			break;
		default:
			printf("Event not implemented\n");
			NDL_TODO("");
			break;
	}
	printf("SDLK: %d\n", *sym);
  return 1;
}

int SDL_PeepEvents(SDL_Event *ev, int numevents, int action, uint32_t mask) {
	NDL_TODO("");
  return 0;
}

uint8_t* SDL_GetKeyState(int *numkeys) {
	NDL_TODO("");
  return NULL;
}
