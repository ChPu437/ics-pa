#include <NDL.h>
#include <SDL.h>

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
	char buf[32], key[32];
	while (!NDL_PollEvent(buf, sizeof(buf) + 1));
	switch (buf[1]) {
		case 'd':
			event->type = SDL_KEYDOWN;
			event->key.keysym.sym;
			break;
		case 'u':
			event->type = SDL_KEYUP;
			break;
		default:
			printf("Event not implemented\n");
			NDL_TODO("");
			break;
	}
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
