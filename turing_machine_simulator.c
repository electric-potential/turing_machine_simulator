#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>

struct delta_node {
	char* state;
	char read;
	char write;
	int move;
	char* new_state;
	struct delta_node* next;
};

struct state_node {
	char* state;
	int accept;
	int start;
	struct state_node* next;
};

struct tape {
	char val;
	struct tape* right;
	struct tape* left;
};

int append_delta_node(struct delta_node** node, char* state, char read, char write, int move, char* new_state) {
	struct delta_node* new = malloc(sizeof(struct delta_node));
	new->state = malloc(sizeof(char)*strlen(state));
	strncpy(new->state, state, strlen(state));
	new->read = read;
	new->write = write;
	new->move = move;
	new->new_state = malloc(sizeof(char)*strlen(new_state));
	strncpy(new->new_state, new_state, strlen(new_state));
	new->next = (struct delta_node*)NULL;
	struct delta_node* temp = *node;
	while (temp->next != (struct delta_node*)NULL) {
		temp = temp->next;
	}
	temp->next = new;
	return 0;
}

int append_state_node(struct state_node** states, char* name, int accept, int start) {
	struct state_node* new = malloc(sizeof(struct state_node));
	new->state = malloc(sizeof(char)*strlen(name));
	strncpy(new->state, name, strlen(name));
	new->accept = accept;
	new->start = start;
	new->next = (struct state_node*)NULL;
	struct state_node* temp = *states;
	while (temp->next != (struct state_node*)NULL) {
		temp = temp->next;
	}
	temp->next = new;
	return 0;
}

int move_tape_right(struct tape** cell) {
	if ((*cell)->right == (struct tape*)NULL) {
		struct tape* new = malloc(sizeof(struct tape));
		new->val = ' ';
		new->left = *cell;
		new->right = (struct tape*)NULL;
		(*cell)->right = new;
		*cell = new;
	} else {
		*cell = (*cell)->right;
	}
	return 0;
}

int move_tape_left(struct tape** cell) {
	if ((*cell)->left == (struct tape*)NULL) {
		struct tape* new = malloc(sizeof(struct tape));
		new->val = ' ';
		new->left = (struct tape*)NULL;
		new->right = *cell;
		(*cell)->left = new;
		*cell = new;
	} else {
		*cell = (*cell)->left;
	}
	return 0;
}

int read_delta_node(FILE* f, char* state, char* read, char* write, int* move, char* new_state) {
	char single;
	char multi[50];
	int count = 0;
	while(fread(&single, sizeof(char), 1, f) != 0) {
		if (single == '{') {
			break;
		}
	}
	if (single != '{') {
		return 1;
	}
	single = '\0';
	if (fread(multi, sizeof(char), 9, f) != 9) {
		printf("read delta error 1\n");
		return -1;
	}
	multi[9] = '\0';
	if (strcmp(multi, "\n\tstate: ") != 0) {
		printf("read delta error 2\n");
		return -1;
	}
	while (single != '\n' && count < 99) {
		if (fread(&single, sizeof(char), 1, f) == 0) {
			printf("read delta error 3\n");
			return -1;
		}
		if (single != '\n') {
			state[count] = single;
			count++;
		}
	}
	if (count == 99) {
		printf("read delta error 4\n");
		return -1;
	}
	state[count] = '\0';
	if (single != '\n') {
		printf("read delta error 5\n");
		return -1;
	}
	if (fread(multi, sizeof(char), 7, f) != 7) {
		printf("read delta error 6\n");
		return -1;
	}
	multi[7] = '\0';
	if (strcmp(multi, "\tread: ") != 0) {
		printf("read delta error 7\n");
		return -1;
	}
	if (fread(&single, sizeof(char), 1, f) == 0) {
		printf("read delta error 8\n");
		return -1;
	}
	*read = single;
	if (fread(&single, sizeof(char), 1, f) == 0 || single != '\n') {
		printf("read delta error 9\n");
		return -1;
	}
	if (fread(multi, sizeof(char), 8, f) != 8) {
		printf("read delta error 10\n");
		return -1;
	}
	multi[8] = '\0';
	if (strcmp(multi, "\twrite: ") != 0) {
		printf("read delta error 11\n");
		return -1;
	}
	if (fread(&single, sizeof(char), 1, f) == 0) {
		printf("read delta error 12\n");
		return -1;
	}
	*write = single;
	if (fread(&single, sizeof(char), 1, f) == 0 || single != '\n') {
		printf("read delta error 13\n");
		return -1;
	}
	if (fread(multi, sizeof(char), 7, f) != 7) {
		printf("read delta error 14\n");
		return -1;
	}
	multi[7] = '\0';
	if (strcmp(multi, "\tmove: ") != 0) {
		printf("read delta error 15\n");
		return -1;
	}
	if (fread(&single, sizeof(char), 1, f) == 0) {
		printf("read delta error 16\n");
		return -1;
	}
	if (single == '0') {
		*move = 0;
	} else if (single == '1') {

		*move = 1;
	} else {
		printf("read delta error 17\n");
		return -1;
	}
	if (fread(&single, sizeof(char), 1, f) == 0 || single != '\n') {
		printf("read delta error 18\n");
		return -1;
	}
	single = '\0';
	count = 0;
	if (fread(multi, sizeof(char), 12, f) != 12) {
		printf("read delta error 19\n");
		return -1;
	}
	multi[12] = '\0';
	if (strcmp(multi, "\tnew state: ") != 0) {
		printf("read delta error 20\n");
		return -1;
	}
	while (single != '\n' && count < 99) {
		if (fread(&single, sizeof(char), 1, f) == 0) {
			printf("read delta error 21\n");
			return -1;
		}
		if (single != '\n') {
			new_state[count] = single;
			count++;
		}
	}
	if (count == 99) {
		printf("read delta error 22\n");
		return -1;
	}
	new_state[count] = '\0';
	if (single != '\n') {
		printf("read delta error 23\n");
		return -1;
	}
	if (fread(&single, sizeof(char), 1, f) == 0 || single != '}') {
		printf("read delta error 24\n");
		return -1;
	}
	return 0;
}

int read_state_node(FILE* f, char* state, int* accept, int* start) {
	char single;
	char multi[50];
	int count = 0;
	while(fread(&single, sizeof(char), 1, f) != 0) {
		if (single == '[') {
			break;
		}
	}
	if (single != '[') {
		return 1;
	}
	single = '\0';
	if (fread(multi, sizeof(char), 9, f) != 9) {
		printf("read state error 1\n");
		return -1;
	}
	multi[9] = '\0';
	if (strcmp(multi, "\n\tstate: ") != 0) {
		printf("read state error 2\n");
		return -1;
	}
	while (single != '\n' && count < 99) {
		if (fread(&single, sizeof(char), 1, f) == 0) {
			printf("read state error 3\n");
			return -1;
		}
		if (single != '\n') {
			state[count] = single;
			count++;
		}
	}
	if (count == 99) {
		printf("read state error 4\n");
		return -1;
	}
	state[count] = '\0';
	if (single != '\n') {
		printf("read state error 5\n");
		return -1;
	}
	if (fread(multi, sizeof(char), 9, f) != 9) {
		printf("read delta error 6\n");
		return -1;
	}
	multi[9] = '\0';
	if (strcmp(multi, "\taccept: ") != 0) {
		printf("read state error 7\n");
		return -1;
	}
	if (fread(&single, sizeof(char), 1, f) == 0) {
		printf("read state error 8\n");
		return -1;
	}
	if (single == '0') {
		*accept = 0;
	} else if (single == '1') {
		*accept = 1;
	} else if (single == '2') {
		*accept = 2;
	} else {
		printf("read state error 9\n");
		return -1;
	}
	if (fread(&single, sizeof(char), 1, f) == 0 || single != '\n') {
		printf("read delta error 10\n");
		return -1;
	}
	if (fread(multi, sizeof(char), 8, f) != 8) {
		printf("read delta error 11\n");
		return -1;
	}
	multi[8] = '\0';
	if (strcmp(multi, "\tstart: ") != 0) {
		printf("read delta error 12\n");
		return -1;
	}
	if (fread(&single, sizeof(char), 1, f) == 0) {
		printf("read delta error 13\n");
		return -1;
	}
	if (single == '0') {
		*start = 0;
	} else if (single == '1') {
		*start = 1;
	} else {
		printf("read delta error 14\n");
		return -1;
	}
	if (fread(&single, sizeof(char), 1, f) == 0 || single != '\n') {
		printf("read delta error 15\n");
		return -1;
	}
	if (fread(&single, sizeof(char), 1, f) == 0 || single != ']') {
		printf("read delta error 16\n");
		return -1;
	}
	return 0;
}

int main (int argc, char** argv) {
	int ret;
	struct delta_node* node = (struct delta_node*)NULL;
	char state[100];
	char read = '\0';
	char write = '\0';
	int move = 0;
	char new_state[100];
	strncpy(state, "\0", 1);
	strncpy(new_state, "\0", 1);
	if (argc != 3 && argc != 2) {
		printf("usage:\n\t./[executable file name] [turing machine file] [input string]\n");
		return 1;
	}
	struct stat s;
	lstat(argv[1], &s);
	if (errno == ENOENT) {
		printf("file %s does not exist\n", argv[1]);
		return 1;
	}
	FILE* f = fopen(argv[1], "r");
	if (f == NULL) {
		printf("error opening file %s\n", argv[1]);
		return 1;
	}
	ret = read_delta_node(f, state, &read, &write, &move, new_state);
	while (ret == 0) {
		if (node == (struct delta_node*)NULL) {
			node = malloc(sizeof(struct delta_node));
			node->state = malloc(sizeof(char)*strlen(state));
			strncpy(node->state, state, strlen(state));
			node->read = read;
			node->write = write;
			node->move = move;
			node->new_state = malloc(sizeof(char)*strlen(new_state));
			strncpy(node->new_state, new_state, strlen(new_state));
			node->next = (struct delta_node*)NULL;
		} else {
			append_delta_node(&node, state, read, write, move, new_state);
		}
		ret = read_delta_node(f, state, &read, &write, &move, new_state);
	}
	if (node == (struct delta_node*)NULL) {
		printf("no delta functions found\n");
		return 1;
	}
	if (ret == -1) {
		return 1;
	}
	if (fseek(f, 0, SEEK_SET) != 0) {
		printf("error resetting file pointer in %s", argv[1]);
		return 1;
	}
	struct state_node* states = (struct state_node*)NULL;
	char name[100];
	int accept = 0;
	int start = 0;
	strncpy(name, "\0", 1);
	ret = read_state_node(f, name, &accept, &start);
	while (ret == 0) {
		if (states == (struct state_node*)NULL) {
			states = malloc(sizeof(struct state_node));
			states->state = malloc(sizeof(char)*strlen(name));
			strncpy(states->state, name, strlen(name));
			states->accept = accept;
			states->start = start;
			states->next = (struct state_node*)NULL;
		} else {
			append_state_node(&states, name, accept, start);
		}
		ret = read_state_node(f, name, &accept, &start);
	}
	if (states == (struct state_node*)NULL) {
		printf("no state declarations found\n");
		return 1;
	}
	if (ret == -1) {
		return 1;
	}
	if (fclose(f) != 0) {
		printf("error closing file %s\n", argv[1]);
		return 1;
	}
	struct tape* cell = (struct tape*)NULL;
	if (argc == 2) {
		cell = malloc(sizeof(struct tape));
		cell->val = ' ';
		cell->left = (struct tape*)NULL;
		cell->right = (struct tape*)NULL;
	} else {
		for (int i = 0; i < strlen(argv[2]); i++) {
			if (cell == (struct tape*)NULL) {
				cell = malloc(sizeof(struct tape));
				cell->val = argv[2][i];
				cell->left = (struct tape*)NULL;
				cell->right = (struct tape*)NULL;
				move_tape_right(&cell);
			} else {
				cell->val = argv[2][i];
				move_tape_right(&cell);
			}
		}
		while (cell->left != (struct tape*)NULL) {
			move_tape_left(&cell);
		}
	}
	struct state_node* current_state = states;
	struct delta_node* current_delta = node;
	while (current_state->start != 1) {
		current_state = current_state->next;
	}
	if (current_state == (struct state_node*)NULL) {
		printf("no start state was specified\n");
		return 1;
	}
	printf("initial tape:\n");
	printf("          ");
	while(cell->right != (struct tape*)NULL) {
		printf("%c", cell->val);
		move_tape_right(&cell);
	}
	printf("%c\n\n\n", cell->val);
	while(cell->left != (struct tape*)NULL) {
		move_tape_left(&cell);
	}
	for (int i = 0; i < 10; i++) {
		move_tape_left(&cell);
	}
	for (int i = 0; i < 20; i++) {
		printf("%c", cell->val);
		move_tape_right(&cell);
	}
	for (int i = 0; i < 10; i++) {
		move_tape_left(&cell);
	}
	printf("\n          ^\n");
	while (current_state != (struct state_node*)NULL && current_state->accept == 0) {
		current_delta = node;
		while (current_delta != (struct delta_node*)NULL) {
			if (current_delta->read == cell->val && strcmp(current_delta->state, current_state->state) == 0) {
				cell->val = current_delta->write;
				if (current_delta->move == 0) {
					move_tape_left(&cell);
				} else {
					move_tape_right(&cell);
				}
				current_state = states;
				while (current_state != (struct state_node*)NULL && strcmp(current_state->state, current_delta->new_state) != 0) {
					current_state = current_state->next;
				}
				current_delta = node;
				break;
			} else {
				current_delta = current_delta->next;
			}
		}
		if (current_delta == (struct delta_node*)NULL) {
			current_state = (struct state_node*)NULL;
			break;
		}
		printf("\n");
		for (int i = 0; i < 10; i++) {
			move_tape_left(&cell);
		}
		for (int i = 0; i < 20; i++) {
			printf("%c", cell->val);
			move_tape_right(&cell);
		}
		for (int i = 0; i < 10; i++) {
			move_tape_left(&cell);
		}
		printf("\n          ^\n");
	}
	if (current_state == (struct state_node*)NULL) {
		printf("\n\nreject (missing delta function)\n");
	} else if (current_state->accept == 1) {
		printf("\n\naccept\n");
	} else {
		printf("\n\nreject\n");
	}
	printf("final tape:\n");
	while(cell->left != (struct tape*)NULL) {
		move_tape_left(&cell);
	}
	while(cell->right != (struct tape*)NULL) {
		printf("%c", cell->val);
		move_tape_right(&cell);
	}
	printf("%c\n", cell->val);
	return 0;
}