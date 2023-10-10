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

#include <assert.h>
#include <isa.h>

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <iso646.h>
#include <regex.h>
#include <stdio.h>
#include <string.h>

enum {
  TK_NOTYPE = 256, 
  TK_EQ = 1, TK_NEQ = 2, TK_AND = 3, // Logic
  TK_PLUS = 4, TK_MINUS = 5, TK_MUL = 6, TK_DIV = 7, // Arith, minus can also be negative and multiply is also deref
  TK_LBRAC = 8, TK_RBRAC = 9, // Bracket
  TK_DEC = 10, TK_HEX = 11, // Number
  TK_REG = 12, // System
};

/* enum {
  TK_NOTYPE = 256, 
  TK_EQ, TK_NEQ, TK_AND, // Logic
  TK_PLUS, TK_MINUS, TK_MUL, TK_DIV, // Arith, minus can also be negative and multiply is also deref
  TK_LBRAC, TK_RBRAC, // Bracket
  TK_DEC, TK_HEX, // Number
  TK_REG, // System
}; */

static struct rule {
  const char *regex;
  int token_type;
} rules[] = {

  /* TODO: Add more rules.
   * Pay attention to the precedence level of different rules.
   */

  {" +", TK_NOTYPE},    // spaces
  // {"$\\\w+", TK_REG}, // TODO: register
  // {"0x\\w+", TK_HEX}, // TODO: hexcimal number
  {"[0-9]+", TK_DEC}, // decimal number
  {"\\+", TK_PLUS}, // plus
  {"-", TK_MINUS}, // minus ()
  {"\\*", TK_MUL}, // multiply (TODO: OR deref)
  {"/", TK_DIV}, // division
  {"\\(", TK_LBRAC}, // left bracket
  {"\\)", TK_RBRAC}, // right bracket
  // {"==", TK_EQ}, // TODO: equal
  // {"!=", TK_NEQ}, // TODO: not equal
  // {"&&", TK_AND}, // TODO: logical and
};

#define NR_REGEX ARRLEN(rules)

static regex_t re[NR_REGEX] = {};

/* Rules are used for many times.
 * Therefore we compile them only once before any usage.
 */
void init_regex() {
  int i;
  char error_msg[128];
  int ret;

  for (i = 0; i < NR_REGEX; i ++) {
    ret = regcomp(&re[i], rules[i].regex, REG_EXTENDED);
    if (ret != 0) {
      regerror(ret, &re[i], error_msg, 128);
      panic("regex compilation failed: %s\n%s", error_msg, rules[i].regex);
    }
  }
}

typedef struct token {
  int type;
  char str[32];
} Token;

static Token tokens[32] __attribute__((used)) = {};
static int nr_token __attribute__((used))  = 0;

static bool make_token(char *e) {
  int position = 0;
  int i;
  regmatch_t pmatch;

  nr_token = 0;

  while (e[position] != '\0') {
    /* Try all rules one by one. */
    for (i = 0; i < NR_REGEX; i ++) {
      if (regexec(&re[i], e + position, 1, &pmatch, 0) == 0 && pmatch.rm_so == 0) {
        char *substr_start = e + position;
        int substr_len = pmatch.rm_eo;

        Log("match rules[%d] = \"%s\" at position %d with len %d: %.*s",
            i, rules[i].regex, position, substr_len, substr_len, substr_start);

        position += substr_len;

        /* TODO: Now a new token is recognized with rules[i]. Add codes
         * to record the token in the array `tokens'. For certain types
         * of tokens, some extra actions should be performed.
         */

        switch (rules[i].token_type) {
          case TK_NOTYPE:
          	  break; // we want not space recorded for optimized memory use.
        	  // TODO: DONT throw "0x" of hexical here, throw at eval
        	  // also hexical overflow check can be hard!
		  case TK_DEC: // remember index handling and alphabic handling when handling hexical, or using sscanf 
        	  tokens[nr_token].type = TK_DEC;
			  assert(substr_len <= 32); // do not handle when buffer overflow for now
			  										// maybe ignore the overflowed upper bits
          	  for (int i = 0; i < substr_len; i++)
          	  	tokens[nr_token].str[i] = *(substr_start + i);
          	  ++nr_token;
		  	  break;
		  case TK_PLUS:
        	  tokens[nr_token].type = TK_PLUS;
          	  ++nr_token;
		  	  break;	
		  case TK_MINUS:
        	  tokens[nr_token].type = TK_MINUS;
          	  ++nr_token;
		  	  break;
		  case TK_MUL: // only record here, handle deref when calculating expr
        	  tokens[nr_token].type = TK_MUL;
          	  ++nr_token;
		  	  break;
		  case TK_LBRAC:
        	  tokens[nr_token].type = TK_LBRAC;
          	  ++nr_token;
		  	  break;
		  case TK_RBRAC:
        	  tokens[nr_token].type = TK_RBRAC;
          	  ++nr_token;
		  	  break;
		  default:
			  assert(0);
        }

        break;
      }
    }

    if (i == NR_REGEX) {
      printf("no match at position %d\n%s\n%*.s^\n", position, e, position, "");
      return false;
    }
  }

  return true;
}

bool check_parentheses(uint8_t p, uint8_t q) {
	if (tokens[p].type != TK_LBRAC) 
		return 0;

	for (int i = p + 1 ; i < q - 1; i++)
		if (tokens[i].type == TK_RBRAC) // if finding right bracket ahead of end 
				return 0;
	
	return tokens[q - 1].type == TK_RBRAC;
}

uint8_t check_precedence(uint8_t pos) {
	switch(tokens[pos].type) {
		case TK_LBRAC:
			return 0;
		case TK_RBRAC:
			return 0;
		case TK_EQ:
			return 1;
		case TK_NEQ:
			return 1;
		case TK_AND:
			return 1;
		case TK_MUL:
			return 2;
		case TK_DIV:
			return 2;
		case TK_PLUS:
			return 3;
		case TK_MINUS:
			return 3;
		default:
			assert(0);
	}
}

uint32_t eval(uint8_t p, uint8_t q) {
  if (p > q) {
    /* Bad expression */
	assert(0);
  }
  else if (p == q) { // empty token, do nothing.
	return 0;
  }
  else if (p == q - 1) {
    /* Single token.
     * For now this token should be a number.
     * Return the value of the number.
     */
	uint32_t ret;
    sscanf(tokens[p].str, "%u", &ret);
    // TODO: check negative number here;
    // or we don't do negative check, just convert regs and hexs
    // printf("!!!%s\n", tokens[p].str);
    return ret;
  }
  else if (check_parentheses(p, q) == true) {
    /* The expression is surrounded by a matched pair of parentheses.
     * If that is the case, just throw away the parentheses.
     */
    return eval(p + 1, q - 1);
  }
  else {
	uint8_t op = q; // position of the main operator
	for (int i = p; i < q; i++) {

		if (tokens[i].type == TK_PLUS || tokens[i].type == TK_MINUS || tokens[i].type == TK_MUL || tokens[i].type == TK_DIV || tokens[i].type == TK_EQ || tokens[i].type == TK_NEQ || tokens[i].type == TK_AND) {
			if (op == q) {
				printf("!@!!!!!\n");
				op = i;
			}
			else {
				if (check_precedence(op) <= check_precedence(i))
					op = i;
			}
		}
	}
	printf("!!!%d %d\n", op, q);
	assert(op != q); // we shouldn't find no main op since we got no single number here.
	assert(tokens[op].type !=  TK_LBRAC && tokens[op].type != TK_RBRAC); // also we shouldn't select bracket since we have thrown the surrounding ones before;

    uint32_t val1 = eval(p, op);
    uint32_t val2 = eval(op + 1, q);

    switch (tokens[op].type) {
      case TK_PLUS: return val1 + val2;
      case TK_MINUS: return val1 - val2;
      case TK_MUL: return val1 * val2;
      case TK_DIV: return val1 / val2;
      case TK_EQ: return val1 == val2;
      case TK_NEQ: return val1 != val2;
	  case TK_AND: return val1 && val2;
      default: assert(0);
    }
  }
}

word_t expr(char *e, bool *success) {
  if (!make_token(e)) {
    *success = false;
    return 0;
  }

  /* TODO: Insert codes to evaluate the expression. */

  // TODO: Change assert to success = 0;
  // do a quick scan first to check if the brackets are legal, using uint-simulated stack;
  // 同时末位的token不可以是运算符

  uint8_t cnt_bracket = 0;
  for (int i = 0; i < nr_token; i++) {
	  if (tokens[i].type == TK_RBRAC)
	  	  ++cnt_bracket;
	  if (tokens[i].type == TK_RBRAC) {
	  	if (!cnt_bracket)
	  		assert(0); // orphan rbrac
	  	else
	  		--cnt_bracket;
	  }
  }

  // printf("!!!%d\n", (tokens[nr_token - 1].type ==  TK_DEC));

  if (tokens[nr_token - 1].type != TK_DEC && tokens[nr_token - 1].type != TK_RBRAC && tokens[nr_token - 1].type != TK_HEX)
  	assert(0); // illegal end
  if (cnt_bracket)
  	assert(0); // open bracket

  return eval(0, nr_token); // 根据我们最后++nr_token的写法，eval函数左闭右开
}
