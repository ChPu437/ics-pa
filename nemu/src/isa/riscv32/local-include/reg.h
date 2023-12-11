/***************************************************************************************
* Copyright (c) 2014-2022 Zihao Yu, Nanjing University
*
* NEMU is licensed under Mulan PSL v2.
* You can use this software according to the terms and conditions of the Mulan PSL v2.
* You may obtain a copy of Mulan PSL v2 at:
*          http://license.coscl.org.cn/MulanPSL2
*
* THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
* EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
* MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
*
* See the Mulan PSL v2 for more details.
***************************************************************************************/

#ifndef __RISCV32_REG_H__
#define __RISCV32_REG_H__

#include <common.h>

static inline int check_reg_idx(int idx) {
  IFDEF(CONFIG_RT_CHECK, assert(idx >= 0 && idx < 32));
  return idx;
}

#define gpr(idx) cpu.gpr[check_reg_idx(idx)]

static inline const char* reg_name(int idx, int width) {
  extern const char* regs[];
  return regs[check_reg_idx(idx)];
}

// we imitate the process above to implement our own csr helper function

#define DEBUG
#ifdef DEBUG
#include <isa.h>
extern const char* csrs[];
#endif

static inline int check_csr_idx(uint32_t idx) {
	extern const uint32_t csr_idx[4];
	for (int i = 0; i < 4; i++) {
		if (idx == csr_idx[i]) {
#ifdef DEBUG
			printf("%s: %d\n", csrs[i], cpu.csr[i]);
#endif
			return i;
		}
	}
  assert(0);
}

#define csr_read(idx) (const word_t)cpu.csr[check_csr_idx(idx)]
#define csr_write(idx) &cpu.csr[check_csr_idx(idx)]

static inline const char* csr_name(int idx, int width) {
  extern const char* csrs[];
  return csrs[check_csr_idx(idx)];
}

#endif
