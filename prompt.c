#include <stdio.h>

#define BUFSIZE 2048
#define SLUR "Slur"
#define SLUR_VER "v0.0.0.0.1"
#define EXIT_HINT "Press Ctrl+C to exit"
#define REPL_PROMPT "slur> "
#define MOCK_OUTPUT "No you're a"

int main(int argc, char** argv) {
	char input[BUFSIZE];

	/* Print version and exit information */
	printf("%s %s\n", SLUR, SLUR_VER);
	printf("%s\n", EXIT_HINT);

	for (;;) {
		fputs(REPL_PROMPT, stdout);
		fgets(input, BUFSIZE, stdin);
		printf("%s %s", MOCK_OUTPUT, input);
	}

	return 0;
}