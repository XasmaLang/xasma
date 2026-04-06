#ifndef XASMA_H
#define XASMA_H

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

// --- 1. Helper Functions ---
static char* trim(char* s) {
    while (isspace((unsigned char)*s)) s++;
    if (*s == 0) return s;
    char* back = s + strlen(s) - 1;
    while (back > s && isspace((unsigned char)*back)) back--;
    *(back + 1) = '\0';
    return s;
}

// --- 2. State & Memory ---
typedef struct {
    char name[64]; // Increased size to support Object.Property notation
    int value;
} Variable;

Variable memory[200]; // Increased capacity for object properties
int var_count = 0;
int skip_until_brace = 0;

// Security: Track imported packages
char imported_pkgs[20][64];
int import_count = 0;

// --- 3. Memory Helpers ---
void set_var(char* name, int val) {
    char* n = trim(name);
    for(int i = 0; i < var_count; i++) {
        if(strcmp(memory[i].name, n) == 0) {
            memory[i].value = val;
            return;
        }
    }
    if (var_count < 200) {
        strcpy(memory[var_count].name, n);
        memory[var_count].value = val;
        var_count++;
    }
}

int get_var(char* name) {
    char* n = trim(name);
    // Support literal numbers or variable lookups
    if(isdigit(*n) || (*n == '-' && isdigit(*(n+1)))) return atoi(n);
    for(int i = 0; i < var_count; i++) {
        if(strcmp(memory[i].name, n) == 0) return memory[i].value;
    }
    return 0;
}

void execute_xasma_line(char* line);

// --- 4. Built-in Simulation (xasma.io) ---
void handle_io_module(char* call, char* original_cmd) {
    if (strstr(call, "filesystem.read")) {
        // Silent read simulation
    }
    else if (strstr(call, "subprocess.run")) {
        char* open_p = strchr(original_cmd, '(');
        char* close_p = strrchr(original_cmd, ')');
        if (open_p && close_p && (close_p > open_p + 1)) {
            char sys_cmd[256] = {0};
            strncpy(sys_cmd, open_p + 1, close_p - open_p - 1);
            system(sys_cmd);
        }
    }
    else if (strstr(call, "filesystem.write")) {
        // Silent write simulation
    }
}

// --- 5. Path & Module Support ---
void load_external_pkg(char* full_call_path, char* original_cmd) {
    char path[512] = {0};
    char temp[512];
    strcpy(temp, trim(full_call_path));

    if (strncmp(temp, "xasma.io", 8) == 0) {
        int allowed = 0;
        for(int i = 0; i < import_count; i++) {
            if(strcmp(imported_pkgs[i], "xasma.io") == 0) {
                allowed = 1;
                break;
            }
        }
        if(allowed) {
            handle_io_module(temp, original_cmd);
        } else {
            printf("Security Error: Module 'xasma.io' is not imported. Use 'pkg xasma.io' first.\n");
        }
        return;
    }

    char* p = temp;
    while(strncmp(p, "..", 2) == 0) {
        strcat(path, "../");
        p += 2;
        if(*p == '.') p++;
    }

    int dots = 0;
    for(int i = 0; p[i] != '\0'; i++) if(p[i] == '.') dots++;

    int dots_processed = 0;
    char filename[256] = {0};
    char pkg_name[256] = {0};
    int fi = 0;

    for(int i = 0; p[i] != '\0'; i++) {
        if (p[i] == '.') {
            dots_processed++;
            if (dots_processed > (dots - 2) && dots > 1) break;
            filename[fi++] = '/';
            pkg_name[i] = '.';
        } else {
            filename[fi++] = p[i];
            pkg_name[i] = p[i];
        }
    }

    int allowed = 0;
    for(int i = 0; i < import_count; i++) {
        if(strcmp(imported_pkgs[i], pkg_name) == 0) {
            allowed = 1;
            break;
        }
    }

    if(!allowed) {
        printf("Security Error: Module '%s' is not imported. Use 'pkg %s' first.\n", pkg_name, pkg_name);
        return;
    }

    strcat(path, filename);
    strcat(path, ".xa");

    FILE* file = fopen(path, "r");
    if (file) {
        char line[512];
        while (fgets(line, sizeof(line), file)) execute_xasma_line(line);
        fclose(file);
    } else {
        printf("Error: Module not found at %s\n", path);
    }
}

// --- 6. The Interpreter Core ---
void execute_xasma_line(char* line) {
    char original_line[512];
    strcpy(original_line, line);

    char* cmd = trim(line);
    if (strlen(cmd) == 0) return;

    if (strcmp(cmd, "}") == 0) {
        skip_until_brace = 0;
        return;
    }

    if (skip_until_brace && !strstr(cmd, "else:")) return;

    // A. Registration: pkg
    if (strncmp(cmd, "pkg ", 4) == 0) {
        char* name = trim(cmd + 4);
        for(int i = 0; i < import_count; i++) {
            if(strcmp(imported_pkgs[i], name) == 0) return;
        }
        strcpy(imported_pkgs[import_count++], name);
        return;
    }

    // B. Object & Class Definitions
    else if (strncmp(cmd, "object class ", 13) == 0) {
        // Definitions are skipped in execution; they act as blueprints
        skip_until_brace = 1;
        return;
    }
    else if (strncmp(cmd, "object function ", 16) == 0) {
        // Internal object functions are skipped until called
        skip_until_brace = 1;
        return;
    }
    else if (strncmp(cmd, "instantiate ", 12) == 0) {
        // Simulation of creating an object instance
        return;
    }

    // C. Input: input("prompt") varName
    else if (strncmp(cmd, "input(", 6) == 0) {
        char prompt[128] = {0}, varName[64] = {0};
        char* open_p = strchr(cmd, '\"');
        char* close_p = strrchr(cmd, '\"');
        char* end_paren = strrchr(cmd, ')');

        if (open_p && close_p && end_paren && open_p != close_p) {
            strncpy(prompt, open_p + 1, close_p - open_p - 1);
            strcpy(varName, trim(end_paren + 1));

            if (strlen(varName) > 0) {
                int val;
                printf("%s ", prompt);
                if(scanf("%d", &val) == 1) {
                    set_var(varName, val);
                    while(getchar() != '\n');
                }
            }
        }
        return;
    }

    // D. Math & Variables (Supports Object.Property)
    else if (strncmp(cmd, "setcfg ", 7) == 0) {
        char varName[64], arg1[64], op[2], arg2[64];
        if (sscanf(cmd, "setcfg %s = %s %s %s", varName, arg1, op, arg2) == 4) {
            int v1 = get_var(arg1), v2 = get_var(arg2);
            if (op[0] == '+') set_var(varName, v1 + v2);
            else if (op[0] == '-') set_var(varName, v1 - v2);
            else if (op[0] == '*') set_var(varName, v1 * v2);
            else if (op[0] == '/') set_var(varName, v2 != 0 ? v1 / v2 : 0);
        } else {
            char* eq = strchr(cmd, '=');
            if(eq) {
                char name[64];
                if (sscanf(cmd, "setcfg %s", name) == 1) {
                    set_var(name, get_var(trim(eq + 1)));
                }
            }
        }
    }

    // E. Logic
    else if (strncmp(cmd, "if ", 3) == 0) {
        char varName[64]; int comp;
        if (sscanf(cmd, "if (%s == %d):", varName, &comp) == 2) {
            if (get_var(varName) != comp) skip_until_brace = 1;
        }
    }
    else if (strstr(cmd, "else:")) { skip_until_brace = !skip_until_brace; }

    // F. Output: println
    else if (strncmp(cmd, "println(", 8) == 0) {
        char content[256];
        char* start = strchr(cmd, '(');
        char* end = strrchr(cmd, ')');
        if (start && end) {
            int len = end - start - 1;
            strncpy(content, start + 1, len);
            content[len] = '\0';

            char temp_content[256];
            strcpy(temp_content, content);
            char* part = strtok(temp_content, ",");
            while (part != NULL) {
                char* p = trim(part);
                if (p[0] == '\"') {
                    char clean[256] = {0};
                    sscanf(p, "\"%[^\"]\"", clean);
                    printf("%s ", clean);
                } else {
                    printf("%d ", get_var(p));
                }
                part = strtok(NULL, ",");
            }
            printf("\n");
        }
    }

    // G. Function & Method Calls
    else if (strstr(cmd, ".") && strchr(cmd, '(')) {
        char full_call[256], args_buf[256];
        strcpy(full_call, cmd);

        char* open_p = strchr(full_call, '(');
        char* close_p = strrchr(full_call, ')');

        if (open_p && close_p) {
            *close_p = '\0';
            strcpy(args_buf, open_p + 1);
            *open_p = '\0';

            char* token = strtok(args_buf, ",");
            int arg_idx = 0;
            while (token != NULL) {
                char arg_key[32];
                sprintf(arg_key, "arg%d", arg_idx++);
                set_var(arg_key, get_var(trim(token)));
                token = strtok(NULL, ",");
            }
        }
        load_external_pkg(full_call, original_line);
    }
}

#endif
