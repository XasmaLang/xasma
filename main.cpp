#include <stdio.h>
#include <stdlib.h>
#include "apis/lexers/xasma.h"

int main(int argc, char* argv[]) {
    // 1. Check if user provided a file
    if (argc < 2) {
        printf("Xasma Interpreter v1.0\n");
        printf("Usage: xasma <file.xa>\n");
        return 1;
    }

    // 2. Open the .xa file
    FILE* file = fopen(argv[1], "r");
    if (!file) {
        printf("Error: Could not find file '%s'\n", argv[1]);
        return 1;
    }

    char line[512];
    // 3. Read and execute every line immediately
    while (fgets(line, sizeof(line), file)) {
        // Pass the line to our "Lexer" to execute
        execute_xasma_line(line);
    }

    fclose(file);
    return 0;
}
