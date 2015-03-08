#include <stdio.h>
#include <stdlib.h>

#include <editline/readline.h>
#ifdef LINUX
#include <editline/history.h>
#endif

#define BUFSIZE 2048
#define SLUR "Slur"
#define SLUR_VER "v0.0.0.0.1"
#define EXIT_HINT "Press Ctrl+C to exit"
#define REPL_PROMPT "slur> "
#define MOCK_OUTPUT "No you're a"

int main(int argc, char** argv) {
	/* Print version and exit information */
	printf("%s %s\n", SLUR, SLUR_VER);
	printf("%s\n", EXIT_HINT);

	for (;;) {
		char* input = readline(REPL_PROMPT);
		add_history(input);
		printf("%s %s", MOCK_OUTPUT, input);
		free(input);
	}

	return 0;
}