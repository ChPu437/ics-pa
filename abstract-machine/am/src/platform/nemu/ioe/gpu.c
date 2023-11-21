/*
 * io_read宏展开到ioe_read()调用相应的handler，如io_read(AM_GPU_CONFIG)调用__am_gpu_config()
 * 相应的映射关系在ioe.c中定义
 */

#include <am.h>
#include <nemu.h>
#include <stdio.h>

#define SYNC_ADDR (VGACTL_ADDR + 4)

void __am_gpu_init() {
  // int i;
  int w = io_read(AM_GPU_CONFIG).width; // / 32 // DO: get the correct width
  int h = io_read(AM_GPU_CONFIG).height; // / 32 // DO: get the correct height
  // 800 * 600 会爆栈
  uint32_t *fb = (uint32_t *)(uintptr_t)FB_ADDR;
  // for (i = 0; i < w * 40; i ++) fb[i] = i / w * 20;
	for (int i = 0; i < h; i++) {
		for (int j = 0; j < w; j++) {
			fb[i * w + j] = i; 
		}
	}
  outl(SYNC_ADDR, 1); 
}

void __am_gpu_config(AM_GPU_CONFIG_T *cfg) {
  *cfg = (AM_GPU_CONFIG_T) {
    .present = true, .has_accel = false,
  // view_width = screen_w * sizeof(u32i) = 4 * screen_w
    .width = 400, // MUXDEF(CONFIG_VGA_SIZE_400x300, 400, 800),
    .height = 300, // MUXDEF(CONFIG_VGA_SIZE_400x300, 300, 600),
    // 硬件(vga)已实现，而软件(AM)未实现
    .vmemsz = 0
  };
}

void __am_gpu_fbdraw(AM_GPU_FBDRAW_T *ctl) {
  // Finishing its function to draw the pixels
  // x, y, pixels, w, h, sync
  int w = io_read(AM_GPU_CONFIG).width;
  // int h = io_read(AM_GPU_CONFIG).height;
  uint32_t *fb = (uint32_t *)(uintptr_t)FB_ADDR;
	for (int i = ctl->y; i < ctl->h; i++) {
		for (int j = ctl->x; j < ctl->w; j++) {
			// printf ("j: %d\n", j);
				fb[i * w + j] = ((uint32_t*)(ctl->pixels))[i * ctl->w + (j - ctl->x)];
		}
	}
	if (ctl->sync) {
    outl(SYNC_ADDR, 1);
  }
}

void __am_gpu_status(AM_GPU_STATUS_T *status) {
  status->ready = true;
}
