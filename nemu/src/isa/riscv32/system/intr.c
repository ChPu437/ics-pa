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

/*
在PA中, 我们只使用如下3个CSR寄存器:

    mepc寄存器 - 存放触发异常的PC
    mstatus寄存器 - 存放处理器的状态
    mcause寄存器 - 存放触发异常的原因

riscv32触发异常后硬件的响应过程如下:

    将当前PC值保存到mepc寄存器
    在mcause寄存器中设置异常号
    从mtvec寄存器中取出异常入口地址
    跳转到异常入口地址
 */

word_t isa_raise_intr(word_t NO, vaddr_t epc) {
  /* TODO: Trigger an interrupt/exception with ``NO''.
   * Then return the address of the interrupt/exception vector.
   */
  /*
   * 0: mtvec
   * 1: mepc
   * 2: mstatus
   * 3: mcause
   */
	cpu.csr[1] = epc;
	cpu.csr[3] = NO;
	cpu.csr[2] = nemu_state.state;
  return cpu.csr[0];
}

word_t isa_query_intr() {
  return INTR_EMPTY;
}

word_t isa_return_intr() {
	// 我们自行定义这个函数，来实现RISC“由软件决定下一步是回到发生中断的位置还是发生中断的下一个位置”
	switch(cpu.csr[3]) {
		case -1: // yield
		case  1: // SYS_yield
		case  2: // SYS_open
		case  3: // SYS_read
		case  4: // SYS_write
		case  7: // SYS_close
		case  8: // SYS_lseek
		case  9: // SYS_brk
			return cpu.csr[1] + 4;
		default: assert(0); // Not implemented
	}
}
