#include "apml.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main(void) {
        char stack[1024][1024];
        int n = 0;
        int tmp;
        int i;
        char input[1024];
        char *s;
        apml_t a = malloc(sizeof(*a));
        apml_t b = malloc(sizeof(*b));

        apml_init(a, 10);
        apml_init(b, 10);

        do {
                printf("--------------Stack Entries----------\n");
                for (i = 0; i < n; i++) {
                        printf("%s\n", stack[i]);
                }
                printf("-------------------------------------\n");

                fgets(input, 1024 * sizeof(input[0]), stdin);
                input[strcspn(input, "\n")] = '\0';

                if (input[0] == '+' && n >= 2) {
                        a = apml_fromstr(a, 10, stack[--n]);
                        b = apml_fromstr(b, 10, stack[--n]);

                        if (a == NULL || b == NULL) {
                                printf("Invalid entry found!\n");
                        }

                        apml_add_im(a, b);
                        apml_tostr(a, &s);

                        strcpy(stack[n++], s);
                        printf("%s\n", s);
                        free(s);
                } else if (input[0] == '-') {
                        b = apml_fromstr(a, 10, stack[--n]);
                        a = apml_fromstr(b, 10, stack[--n]);

                        if (a == NULL || b == NULL) {
                                printf("Invalid entry found!\n");
                        }

                        apml_add_im(a, b);
                        apml_tostr(a, &s);

                        strcpy(stack[n++], s);
                        printf("%s\n", s);
                        free(s);
                } else {
                        if (input[strspn(input, "0123456789.")] == '\0') {
                                tmp = strcspn(input, ".");

                                if (input[tmp] == '\0') {
                                        input[tmp] = '.';
                                        input[tmp + 1] = '\0';
                                }
                                strcpy(stack[n++], input);
                        } else {
                                printf("Invalid input!\n");
                        }
                }
        } while (n < 1024 && strcmp(input, "quit") != 0);

        apml_free(a);
        apml_free(b);
        free(a);
        free(b);

        return 0;
}
