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

#include <isa.h>
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

static inline int check_csr_idx(int idx) {
  IFDEF(CONFIG_RT_CHECK, assert(idx >= 0 && idx < 4));
  return idx;
}

word_t csr_read(uint32_t idx) {
	extern const uint32_t csr_idx[];
	for (int i = 0; i < 4; i++) {
		if (idx == csr_idx[i]) {
			return cpu.csr[i];
		}
	}
	assert(0); // invalid csr index
} // 读取csr中的值，所以回传值
	// 注意将传入的标准riscv_csr值转换到自定义的index

word_t* csr_write(uint32_t idx) {
	extern const uint32_t csr_idx[];
	for (int i = 0; i < 4; i++) {
		if (idx == csr_idx[i]) {
			return &cpu.csr[i];
		}
	}
	assert(0); // invalid csr index
} // 向csr写入值，所以回传指针
	// 解引用在使用的CSRW()宏中处理，此处不处理

static inline const char* csr_name(int idx, int width) {
  extern const char* csrs[];
  return csrs[check_csr_idx(idx)];
}

#endif
