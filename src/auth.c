#include "hub.h"
#include <stdio.h>
#include <string.h>

#ifdef _WIN32
#include <conio.h>
#else
#include <termios.h>
#include <unistd.h>
#endif

#define USERS_FILE "data/users.txt"
#define MAX_U 50
#define NMAX  20
#define PMAX  32

typedef struct { char name[NMAX+1]; char pass[PMAX+1]; } Acc;
static Acc u[MAX_U];
static int n;

static void hide_pass(const char *pw, char *out) {
    int i; for (i = 0; pw[i] && i < PMAX; i++) out[i] = (char)(pw[i] ^ 0x5A);
    out[i] = 0;
}

static void load(void) {
    FILE *f = fopen(USERS_FILE, "r");
    n = 0;
    if (!f) return;
    while (n < MAX_U && fscanf(f, "%20s %32s", u[n].name, u[n].pass) == 2) n++;
    fclose(f);
}

static void save(void) {
    FILE *f = fopen(USERS_FILE, "w");
    if (!f) return;
    for (int i = 0; i < n; i++) fprintf(f, "%s %s\n", u[i].name, u[i].pass);
    fclose(f);
}

static Acc *find(const char *name) {
    for (int i = 0; i < n; i++) if (!strcmp(u[i].name, name)) return &u[i];
    return NULL;
}

static int read_star(const char *prompt, char *buf, int size) {
    int k = 0;
    printf("%s", prompt); fflush(stdout);
#ifdef _WIN32
    for (;;) {
        int c = _getch();
        if (c == '\r' || c == '\n') break;
        if ((c == 8 || c == 127) && k > 0) { k--; printf("\b \b"); fflush(stdout); continue; }
        if (k < size-1 && c >= 32 && c < 127) { buf[k++] = (char)c; putchar('*'); fflush(stdout); }
    }
#else
    struct termios o, t; tcgetattr(0, &o); t = o; t.c_lflag &= ~(ECHO|ICANON); tcsetattr(0, TCSANOW, &t);
    for (;;) {
        int c = getchar();
        if (c == '\n' || c == '\r' || c == EOF) break;
        if ((c == 127 || c == 8) && k > 0) { k--; printf("\b \b"); fflush(stdout); continue; }
        if (k < size-1 && c >= 32 && c < 127) { buf[k++] = (char)c; putchar('*'); fflush(stdout); }
    }
    tcsetattr(0, TCSANOW, &o);
#endif
    buf[k] = 0; printf("\n"); return k > 0;
}

int auth_login(char *name_out, int size) {
    char name[NMAX+1], pw[PMAX+1], line[8], hid[PMAX+1];
    load();
    printf("\n  --- GAME HUB login ---\n  [1] login  [2] register\n  choice: ");
    fflush(stdout);
    if (!fgets(line, sizeof line, stdin)) return 0;

    printf("  username: "); fflush(stdout);
    if (!fgets(name, sizeof name, stdin)) return 0;
    name[strcspn(name, "\r\n")] = 0;
    if (strlen(name) < 2) { printf("  name too short\n"); return 0; }

    if (line[0] == '2') {
        if (find(name)) { printf("  name taken\n"); return 0; }
        if (!read_star("  password: ", pw, sizeof pw)) return 0;
        if (n >= MAX_U) return 0;
        strncpy(u[n].name, name, NMAX);
        hide_pass(pw, u[n].pass);
        n++; save();
        printf("  registered, %s\n", name);
    } else {
        Acc *a = find(name);
        if (!a) { printf("  unknown user\n"); return 0; }
        if (!read_star("  password: ", pw, sizeof pw)) return 0;
        hide_pass(pw, hid);
        if (strcmp(a->pass, hid)) { printf("  wrong password\n"); return 0; }
        printf("  welcome, %s\n", name);
    }
    strncpy(name_out, name, size-1);
    name_out[size-1] = 0;
    return 1;
}

int auth_change_password(const char *name) {
    char oldp[PMAX+1], newp[PMAX+1], hid[PMAX+1];
    load();
    Acc *a = find(name);
    if (!a) { printf("  not found\n"); return 0; }
    if (!read_star("  current: ", oldp, sizeof oldp)) return 0;
    hide_pass(oldp, hid);
    if (strcmp(a->pass, hid)) { printf("  wrong password\n"); return 0; }
    if (!read_star("  new: ", newp, sizeof newp)) return 0;
    hide_pass(newp, a->pass);
    save();
    printf("  password changed\n");
    return 1;
}
