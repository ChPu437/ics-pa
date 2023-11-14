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

#include "sdb.h"
#include <assert.h>
#include <stdint.h>
#include <string.h>
#include <malloc.h>

#define NR_WP 32

typedef struct watchpoint {
  int NO;
  struct watchpoint *next;

  /* TODO: Add more members if necessary */
  char* expr;
  uint32_t val; // value of current EXPR
} WP;

static WP wp_pool[NR_WP] = {};
static WP *head = NULL, *free_ = NULL;

void init_wp_pool() {
  int i;
  for (i = 0; i < NR_WP; i ++) {
    wp_pool[i].NO = i;
    wp_pool[i].next = (i == NR_WP - 1 ? NULL : &wp_pool[i + 1]);
  }

  head = NULL;
  free_ = wp_pool;
}

/* TODO: Implement the functionality of watchpoint */
int new_wp(char* EXPR) {
	assert(free_ != NULL);
	WP* new = free_;
	free_ = free_->next;

	new->next = NULL;
	if(head == NULL) {
		head = new;
		new->NO = 1;
	} else {
		WP* last = head;
		while(last->next != NULL)
			last = last->next;
		last->next = new;
		new->NO = last->NO + 1;
	}


	new->expr = malloc((strlen(EXPR) + 1) * sizeof(char)); // remember the end tag!
	strcpy(new->expr, EXPR);
	bool success = 0;
	new->val = expr(EXPR, &success); 
	assert(success);

	return 0;
}

int free_wp(int index) {
	assert(head != NULL);
	assert(index > head->NO); // if exceeded max index

	WP *last = NULL, *curr = head, *nnext = NULL;
	while (curr->NO != index) {
		last = curr;
		curr = curr -> next;
		nnext = curr -> next;
	}
	if(last) last->next = nnext;
	while (nnext != NULL) {
		(nnext->NO)--;
		nnext = nnext->next;
	}
	curr->val = 0;
	free(curr->expr);
	curr->expr = NULL;
	
	last = free_;
	while(last->next != NULL) last = last->next;
	last->next = curr;
	curr->NO = last->NO + 1;

	return 0;
}

void dump_wp() {
	WP* curr = head;
	while(curr != NULL) {
		printf("%d\t%s\t%u\n", curr->NO, curr->expr, curr->val);
		curr = curr->next;
	}
}

bool update_wp() {
//	WP* curr = head;
	bool ret = 0;
/*	while(curr != NULL) {
		bool success = 0;
		uint32_t new_val = expr(curr->expr, &success);
		assert(success);
		if (curr->val != new_val) {
			printf("Watchpoint %d triggered! %s: %u -> %u\n", curr->NO, curr->expr, curr->val, new_val);
			curr->val = new_val;
		}
		ret = 1;
	} */
	return ret;
}
