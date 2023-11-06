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

#include "local-include/reg.h"
#include <cpu/cpu.h>
#include <cpu/ifetch.h>
#include <cpu/decode.h>
#include <stdint.h>

#define R(i) gpr(i)
#define Mr vaddr_read
#define Mw vaddr_write

enum {
  TYPE_I, TYPE_U, TYPE_S,
  TYPE_N, // none
  // Extended types
  TYPE_J,
  TYPE_R,
  TYPE_B,
};

#define src1R() do { *src1 = R(rs1); } while (0)
#define src2R() do { *src2 = R(rs2); } while (0)
#define immI() do { *imm = SEXT(BITS(i, 31, 20), 12); } while(0)
#define immU() do { *imm = SEXT(BITS(i, 31, 12), 20) << 12; } while(0)
#define immS() do { *imm = (SEXT(BITS(i, 31, 25), 7) << 5) | BITS(i, 11, 7); } while(0)
// Extended immediate type
// // ~~注意到immJ和immB应当最后(即左移前)做符号扩展，因为最高位不一定一定是符号位，即可能出现实际位数小于最大位数情况~~
// // 指令集支出符号位一定都在31位->方便硬件实现
// // 回去查一查B_imm是不是读错了，ISA说和S_type区别只是移位，查查S_type准确定义
// #define immJ() do { *imm = ((SEXT(BITS(i, 31, 31), 1) << 19) | (BITS(i, 19, 12) << 11) | (BITS(i, 20, 20) << 10) | BITS(i, 30, 21)) << 1;} while(0)
// #define immB() do { *imm = ((SEXT(BITS(i, 31, 31), 1) << 11) | (BITS(i, 7, 7) << 10) | (BITS(i, 30, 25) << 4) | BITS(i, 11, 8)) << 1;} while(0)
// #define immJ() do { *imm = SEXT((BITS(i, 31, 31) << 19) | (BITS(i, 19, 12) << 11) | (BITS(i, 20, 20) << 10) | BITS(i, 30, 21), 20) << 1;} while(0)
// #define immB() do { *imm = SEXT((BITS(i, 31, 31) << 11) | (BITS(i, 7, 7) << 10) | (BITS(i, 30, 25) << 4) | BITS(i, 11, 8), 12) << 1;} while(0)
#define immJ() do { *imm = ((SEXT(BITS(i, 31, 31), 1) << 20) | (BITS(i, 19, 12) << 12) | (BITS(i, 20, 20) << 11) | (BITS(i, 30, 21) << 1) | 0);} while(0)
#define immB() do { *imm = ((SEXT(BITS(i, 31, 31), 1) << 12) | (BITS(i, 7, 7) << 11) | (BITS(i, 30, 25) << 5) | (BITS(i, 11, 8) << 1) | 0);} while(0)
// #define immB() do { *imm = BITS(i, 30, 25);} while(0)
// #define immIz() do { *imm = BITS(i, 31, 20); } while(0)
// // 左移给后续字节留下空间
// // SEXT代表符号扩展

static void decode_operand(Decode *s, int *rd, word_t *src1, word_t *src2, word_t *imm, int type) {
  uint32_t i = s->isa.inst.val;
  int rs1 = BITS(i, 19, 15);
  int rs2 = BITS(i, 24, 20);
  *rd     = BITS(i, 11, 7);
  switch (type) {
    case TYPE_I: src1R();          immI(); break;
    case TYPE_U:                   immU(); break;
    case TYPE_S: src1R(); src2R(); immS(); break;
    // Extended type
	case TYPE_J:                   immJ(); break; 
	case TYPE_R: src1R(); src2R();         break;
	case TYPE_B: src1R(); src2R(); immB(); break;
//	case TYPE_Iz:src1R();          immIz();break;
  }
}

static int decode_exec(Decode *s) {
  int rd = 0;
  word_t src1 = 0, src2 = 0, imm = 0;
  // NOTICE there use UNSIGNED by default and 
  // this may affect the result of comparation
  // but no effect on calculation since we stored
  // these value by SEXT and we "give" them type when
  // fetch them from reg, but comparation is made in-place
  // and they are made to be intepreted as unsigned
  s->dnpc = s->snpc;

#define INSTPAT_INST(s) ((s)->isa.inst.val)
#define INSTPAT_MATCH(s, name, type, ... /* execute body */ ) { \
  decode_operand(s, &rd, &src1, &src2, &imm, concat(TYPE_, type)); \
  __VA_ARGS__ ; \
}

  INSTPAT_START();
  INSTPAT("??????? ????? ????? ??? ????? 01101 11", lui    , U, R(rd) = imm);
  INSTPAT("??????? ????? ????? 010 ????? 00000 11", lw     , I, R(rd) = Mr(src1 + imm, 4));
  INSTPAT("??????? ????? ????? 010 ????? 01000 11", sw     , S, Mw(src1 + imm, 4, src2));

  // Extended instructions
  
  INSTPAT("0000000 ????? ????? 000 ????? 01100 11", add    , R, R(rd) = src1 + src2);
  INSTPAT("0100000 ????? ????? 000 ????? 01100 11", sub    , R, R(rd) = src1 - src2);
  INSTPAT("0000000 ????? ????? 100 ????? 01100 11", xor    , R, R(rd) = src1 ^ src2);
  INSTPAT("0000000 ????? ????? 110 ????? 01100 11", or     , R, R(rd) = src1 | src2);
  INSTPAT("0000000 ????? ????? 111 ????? 01100 11", and    , R, R(rd) = src1 & src2);
  INSTPAT("0000000 ????? ????? 001 ????? 01100 11", sll    , R, R(rd) = src1 << src2);
  INSTPAT("0000000 ????? ????? 101 ????? 01100 11", srl    , R, R(rd) = ((word_t)src1) >> src2);
  INSTPAT("0100000 ????? ????? 101 ????? 01100 11", sra    , R, R(rd) = ((sword_t)src1) >> src2);
  INSTPAT("0000000 ????? ????? 010 ????? 01100 11", slt    , R, R(rd) = ((sword_t)src1 < (sword_t)src2 ? 1 : 0));
  INSTPAT("0000000 ????? ????? 011 ????? 01100 11", sltu   , R, R(rd) = ((word_t)src1 < (word_t)src2 ? 1 : 0));

  INSTPAT("0000001 ????? ????? 000 ????? 01100 11", mul    , R, R(rd) = src1 * src2);
  INSTPAT("0000001 ????? ????? 001 ????? 01100 11", mulh   , R, R(rd) = (SEXT(src1, 32) * SEXT(src2, 32)) >> 32);
  INSTPAT("0000001 ????? ????? 100 ????? 01100 11", div    , R, R(rd) = src1 / src2);
  INSTPAT("0000001 ????? ????? 101 ????? 01100 11", divu   , R, R(rd) = (word_t)src1 / (word_t)src2);
  INSTPAT("0000001 ????? ????? 110 ????? 01100 11", rem    , R, R(rd) = src1 % src2);
  INSTPAT("0000001 ????? ????? 111 ????? 01100 11", remu   , R, R(rd) = (word_t)src1 % (word_t)src2);


  INSTPAT("??????? ????? ????? 000 ????? 00100 11", addi   , I, R(rd) = src1 + imm);
  INSTPAT("??????? ????? ????? 100 ????? 00100 11", xori   , I, R(rd) = src1 ^ imm);
  INSTPAT("??????? ????? ????? 111 ????? 00100 11", andi   , I, R(rd) = src1 & imm);
  INSTPAT("0000000 ????? ????? 001 ????? 00100 11", slli   , I, R(rd) = src1 << (imm & 31)); // only use imm[0:4]
  INSTPAT("0000000 ????? ????? 101 ????? 00100 11", srli   , I, R(rd) = ((word_t)src1) >> (imm & 31)); // only use imm[0:4]
  INSTPAT("0100000 ????? ????? 101 ????? 00100 11", srai   , I, R(rd) = ((sword_t)src1) >> (imm & 31)); // only use imm[0:4]
  INSTPAT("??????? ????? ????? 011 ????? 00100 11", sltiu  , I, R(rd) = ((word_t)src1 < (word_t)imm ? 1 : 0)); // notice this should treat as unsigned though sign-extended

  // // Note that the last process of running a instruction is pc = dnpc, so we should give the value to dnpc instead of pc.
  INSTPAT("??????? ????? ????? 000 ????? 11000 11", beq    , B, s->dnpc = (src1 == src2 ? s->pc + imm : s->dnpc));
  INSTPAT("??????? ????? ????? 001 ????? 11000 11", bne    , B, s->dnpc = (src1 != src2 ? s->pc + imm : s->dnpc));
  INSTPAT("??????? ????? ????? 100 ????? 11000 11", blt    , B, s->dnpc = ((sword_t)src1 < (sword_t)src2 ? s->pc + imm : s->dnpc));
  INSTPAT("??????? ????? ????? 101 ????? 11000 11", bge    , B, s->dnpc = ((sword_t)src1 >= (sword_t)src2 ? s->pc + imm : s->dnpc));
  INSTPAT("??????? ????? ????? 110 ????? 11000 11", bltu   , B, s->dnpc = ((word_t)src1 < (word_t)src2 ? s->pc + imm : s->dnpc));
  INSTPAT("??????? ????? ????? 111 ????? 11000 11", bgeu   , B, s->dnpc = ((word_t)src1 >= (word_t)src2 ? s->pc + imm : s->dnpc));


  INSTPAT("??????? ????? ????? ??? ????? 11011 11", jal    , J, R(rd) = s->snpc, s->dnpc = s->pc + imm);
  INSTPAT("??????? ????? ????? 000 ????? 11001 11", jalr   , I, R(rd) = s->snpc, s->dnpc = src1 + imm);

  INSTPAT("??????? ????? ????? ??? ????? 00101 11", auipc  , U, R(rd) = imm + s->pc);
  // Extended instruction end

  // Testing extended instruction
  																											   
  // // hello-str -> TODO implemention of string 
  // // string    -> TODO inplemention of string




  // // bit
  INSTPAT("??????? ????? ????? 100 ????? 00000 11", lbu    , I, R(rd) = ((word_t)Mr(src1 + imm, 1))); // what is zero-extend?
  INSTPAT("??????? ????? ????? 001 ????? 01000 11", sh     , S, Mw(src1 + imm, 2, src2));
  INSTPAT("??????? ????? ????? 000 ????? 01000 11", sb     , S, Mw(src1 + imm, 1, src2));

  // // load-store -- fail
  INSTPAT("??????? ????? ????? 001 ????? 00000 11", lh     , I, R(rd) = Mr(src1 + imm, 2));

  // // // maybe B[U] is wrongly processed but get 'right' result
  // // // FACT: ONLY B[U] was processed right XD

  // // movsx --fail

  // Testing extended instruction end

  INSTPAT("0000000 00001 00000 000 00000 11100 11", ebreak , N, NEMUTRAP(s->pc, R(10))); // R(10) is $a0
  																						 
  // Notice that you SHOULDN'T place ANY instruction under the instruction below, since it matches ANY sequence
  INSTPAT("??????? ????? ????? ??? ????? ????? ??", inv    , N, INV(s->pc));
  
  // Extended pseudo-instructions
  // // We don't need this since pseudo-instructions are translated into "real" instructions
  // // this should have been done by compiler.
  INSTPAT_END();

  R(0) = 0; // reset $zero to 0

  return 0;
}

int isa_exec_once(Decode *s) {
  s->isa.inst.val = inst_fetch(&s->snpc, 4);
  return decode_exec(s);
}
