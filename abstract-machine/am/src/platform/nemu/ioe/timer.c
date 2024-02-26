#include <am.h>
#include <nemu.h>

void __am_timer_init() {
}

void __am_timer_uptime(AM_TIMER_UPTIME_T *uptime) {
  uptime->us = inl(RTC_ADDR); inl(RTC_ADDR + 4);
  // Not quite clear why must two reg be read at one time
}

void __am_timer_rtc(AM_TIMER_RTC_T *rtc) {
  rtc->second = inl(RTC_ADDR) / 1000; inl(RTC_ADDR + 4);
  // since we did not really implemented rtc yet
  rtc->minute = 0;
  rtc->hour   = 0;
  rtc->day    = 0;
  rtc->month  = 0;
  rtc->year   = 1900;
}
