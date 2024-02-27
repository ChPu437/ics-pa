#include <NDL.h>
#include <sdl-timer.h>
#include <stdio.h>

SDL_TimerID SDL_AddTimer(uint32_t interval, SDL_NewTimerCallback callback, void *param) {
	NDL_TODO("SDL_AddTimer");
  return NULL;
}

int SDL_RemoveTimer(SDL_TimerID id) {
	NDL_TODO("SDL_RemoveTimer");
  return 1;
}

uint32_t SDL_GetTicks() {
  return NDL_GetTicks();
}

void SDL_Delay(uint32_t ms) {
	NDL_TODO("SDL_Delay");
}
