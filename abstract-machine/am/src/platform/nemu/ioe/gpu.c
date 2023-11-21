/*
 * io_read宏展开到ioe_read()调用相应的handler，如io_read(AM_GPU_CONFIG)调用__am_gpu_config()
 * 相应的映射关系在ioe.c中定义
 */

#include <am.h>
#include <nemu.h>

#define SYNC_ADDR (VGACTL_ADDR + 4)

void __am_gpu_init() {
  int i;
  int w = io_read(AM_GPU_CONFIG).width; // / 32 // TODO: get the correct width
  int h = io_read(AM_GPU_CONFIG).height; // / 32 // TODO: get the correct height
  // 800 * 600 会爆栈
  uint32_t *fb = (uint32_t *)(uintptr_t)FB_ADDR;
  for (i = 0; i < w * h; i ++) fb[i] = i;
  outl(SYNC_ADDR, 1);
}

void __am_gpu_config(AM_GPU_CONFIG_T *cfg) {
  *cfg = (AM_GPU_CONFIG_T) {
    .present = true, .has_accel = false,
    .width = 400, .height = 300,
    // 硬件(vga)已实现，而软件(AM)未实现
    .vmemsz = 0
  };
}

void __am_gpu_fbdraw(AM_GPU_FBDRAW_T *ctl) {
  if (ctl->sync) {
    outl(SYNC_ADDR, 1);
  }
  // 软件(AM)已实现，而硬件(vga)未实现
}

void __am_gpu_status(AM_GPU_STATUS_T *status) {
  status->ready = true;
}
