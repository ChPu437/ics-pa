#include <NDL.h>
#include <SDL.h>

int SDL_OpenAudio(SDL_AudioSpec *desired, SDL_AudioSpec *obtained) {
	NDL_TODO("");
  return 0;
}

void SDL_CloseAudio() {
	NDL_TODO("");
}

void SDL_PauseAudio(int pause_on) {
	NDL_TODO("");
}

void SDL_MixAudio(uint8_t *dst, uint8_t *src, uint32_t len, int volume) {
	NDL_TODO("");
}

SDL_AudioSpec *SDL_LoadWAV(const char *file, SDL_AudioSpec *spec, uint8_t **audio_buf, uint32_t *audio_len) {
	NDL_TODO("");
  return NULL;
}

void SDL_FreeWAV(uint8_t *audio_buf) {
	NDL_TODO("");
}

void SDL_LockAudio() {
	NDL_TODO("");
}

void SDL_UnlockAudio() {
	NDL_TODO("");
}
