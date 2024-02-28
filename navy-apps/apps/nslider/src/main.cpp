#include <SDL.h>
#include <SDL_bmp.h>
#include <stdio.h>
#include <assert.h>

#define W 400
#define H 300

// USAGE:
//   j/down - page down
//   k/up - page up
//   gg - first page

// number of slides
const int N = 8;
// slides path pattern (starts from 0)
const char *path = "/share/slides/slides-%d.bmp";

static SDL_Surface *gimp = NULL;
static SDL_Surface *slide = NULL;
static int cur = 0;

void render() {
	printf("into render! slide = %p\n", slide);
  if (slide) {
    SDL_FreeSurface(slide);
  }
  char fname[256];
  sprintf(fname, path, cur);
	printf("into loadbmp!\n");
  slide = SDL_LoadBMP(fname);
  printf("into assert!\n");
  assert(slide);
  SDL_UpdateRect(slide, 0, 0, 0, 0);
}

void prev(int rep) {
  if (rep == 0) rep = 1;
  cur -= rep;
  if (cur < 0) cur = 0;
  render();
}

void next(int rep) {
  if (rep == 0) rep = 1;
  cur += rep;
  if (cur >= N) cur = N - 1;
  render();
}

int main() {
	assert(gimp==NULL);
	slide = NULL; // why?
	printf("into init!\n");
  SDL_Init(0);
	printf("into videomode!\n");

  SDL_Surface *screen = SDL_SetVideoMode(W, H, 32, SDL_HWSURFACE);
  int rep = 0, g = 0;

  printf("prepare render!\n");
  render();

  while (1) {
    SDL_Event e;
    SDL_WaitEvent(&e);

    if (e.type == SDL_KEYDOWN) {
      switch(e.key.keysym.sym) {
        case SDLK_0: rep = rep * 10 + 0; break;
        case SDLK_1: rep = rep * 10 + 1; break;
        case SDLK_2: rep = rep * 10 + 2; break;
        case SDLK_3: rep = rep * 10 + 3; break;
        case SDLK_4: rep = rep * 10 + 4; break;
        case SDLK_5: rep = rep * 10 + 5; break;
        case SDLK_6: rep = rep * 10 + 6; break;
        case SDLK_7: rep = rep * 10 + 7; break;
        case SDLK_8: rep = rep * 10 + 8; break;
        case SDLK_9: rep = rep * 10 + 9; break;
        case SDLK_J:
        case SDLK_DOWN: next(rep); rep = 0; g = 0; break;
        case SDLK_K:
        case SDLK_UP: prev(rep); rep = 0; g = 0; break;
        case SDLK_G:
          g ++;
          if (g > 1) {
            prev(100000);
            rep = 0; g = 0;
          }
          break;
      }
    }
  }

  return 0;
}
