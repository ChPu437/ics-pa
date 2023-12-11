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

#include <isa.h>
#include "local-include/reg.h"
#include <string.h>

const char *regs[] = {
  "$0", "ra", "sp", "gp", "tp", "t0", "t1", "t2",
  "s0", "s1", "a0", "a1", "a2", "a3", "a4", "a5",
  "a6", "a7", "s2", "s3", "s4", "s5", "s6", "s7",
  "s8", "s9", "s10", "s11", "t3", "t4", "t5", "t6"
};

const uint32_t csr_idx[] = {
	0x305, 0x341, 0x300, 0x342
};
const char *csrs[] = {
	"mtvec", "mepc", "mstatus", "mcause"
};

void isa_reg_display() {
	for (uint8_t i = 0; i < sizeof(regs) / sizeof(void*); i++) {
		printf("%s: 0x%x\n", regs[i], cpu.gpr[i]);
	}
	for (uint8_t i = 0; i < sizeof(csrs) / sizeof(void*); i++) {
		printf("%s: 0x%x\n", csrs[i], cpu.csr[i]);
	}
}

word_t isa_reg_str2val(const char *s, bool *success) {
	for (uint8_t i = 0; i < 32; i++) {
		if (!strcmp(regs[i], s)) {
			*success = 1;
			return cpu.gpr[i];
		}
	}
	success = 0;
	return 0;
}
