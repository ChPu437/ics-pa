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
	while (!NDL_PollEvent(buf, sizeof(buf) + 1));
	// printf("SDLK: %d\n", event->key.keysym.sym);
	switch (buf[1]) {
		case 'd':
			event->type = SDL_KEYDOWN;
			// TODO: regex-ify the compare
			if (strcmp(buf, "kd UP")) event->key.keysym.sym = SDLK_UP;
			if (strcmp(buf, "kd DOWN")) event->key.keysym.sym = SDLK_DOWN;
			if (strcmp(buf, "kd 0")) event->key.keysym.sym = SDLK_0;
			if (strcmp(buf, "kd 1")) event->key.keysym.sym = SDLK_1;
			if (strcmp(buf, "kd 2")) event->key.keysym.sym = SDLK_2;
			if (strcmp(buf, "kd 3")) event->key.keysym.sym = SDLK_3;
			if (strcmp(buf, "kd 4")) event->key.keysym.sym = SDLK_4;
			if (strcmp(buf, "kd 5")) event->key.keysym.sym = SDLK_5;
			if (strcmp(buf, "kd 6")) event->key.keysym.sym = SDLK_6;
			if (strcmp(buf, "kd 7")) event->key.keysym.sym = SDLK_7;
			if (strcmp(buf, "kd 8")) event->key.keysym.sym = SDLK_8;
			if (strcmp(buf, "kd 9")) event->key.keysym.sym = SDLK_9;
			if (strcmp(buf, "kd G")) event->key.keysym.sym = SDLK_G;
			if (strcmp(buf, "kd J")) event->key.keysym.sym = SDLK_J;
			if (strcmp(buf, "kd K")) event->key.keysym.sym = SDLK_K;
			break;
		case 'u':
			event->type = SDL_KEYUP;
			break;
		default:
			printf("Event not implemented\n");
			NDL_TODO("");
			break;
	}
	// printf("SDLK: %d\n", event->key.keysym.sym);
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
