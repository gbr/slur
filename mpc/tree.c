#include "mpc.h"

long number_of_leaves(mpc_ast_t* t) {
	if (t->children_num == 0) { return 1; }
	else {
		int total = 0;
		for (int i = 0; i < t->children_num; i++) {
			total += number_of_leaves(t->children[i]);
		}
		return total;
	}
}

long number_of_branches(mpc_ast_t* t) {
	if (t->children_num < 2) { return 0; }
	else {
		int total = 1;
		for (int i = 0; i < t->children_num; i++) {
			total += number_of_branches(t->children[i]);
		}
		return total;
	}
}

int number_of_nodes(mpc_ast_t* t) {
	if (t->children_num == 0) { return 1; }
	if (t->children_num >= 1) {
		int total = 1;
		for (int i = 0; i < t->children_num; i++) {
			total = total + number_of_nodes(t->children[i]);
		}
		return total;
	}
	return 0;
}

// 
long max_child_branch(mpc_ast_t* t) {
	if (t->children_num < 2) { return 0; }
	else {
		int max = 0;
		for (int i = 0; i < t->children_num; i++) {
			// if (number)
		}
		return max;
	}
}