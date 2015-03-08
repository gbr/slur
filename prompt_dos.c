#include <stdio.h>
#include <stdlib.h>

#ifdef _WIN32
#include <string.h>

/* Fake readline function */
char* readline(char* prompt) {
	fputs(prompt, stdout);
	fgets(buffer, sizeof buffer, stdin);

	size_t bufsiz = strlen(buffer);
	char* cpy = malloc(bufsiz + 1);
	strcpy(cpy, buffer);
	cpy[bufsize-1] = '\0';
	return cpy;
}

/* Fake add_history function */
void add_history(char* unused) {}

#ifdef _linux
#include <editline/history.h>
#endif

#else
#include <editline/readline.h>
#endif

#define SLUR "Slur"
#define SLUR_VER "v0.0.0.0.1"
#define EXIT_HINT "Press Ctrl+C to exit"
#define REPL_PROMPT "slur> "
#define MOCK_OUTPUT "No you're a"


int main(int argc, char** argv) {

	printf("%s %s\n", SLUR, SLUR_VER);
	printf("%s\n", EXIT_HINT);

	for (;;) {
		char* input = readline(REPL_PROMPT);
		add_history(input);

		printf("%s %s\n", MOCK_OUTPUT, input);
		free(input);
	}

	return 0;
}