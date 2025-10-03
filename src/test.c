#include <assert.h>
#include <stdio.h>
#include "apml.h"

int test_fromstr_01();
int test_fromstr_02();

int test_tostr_01();
int test_tostr_02();

int test_add_01();
int test_add_02();

int test_add_im_01();
int test_add_im_02();

int test_mult_01();
int test_mult_02();

int test_mult_im_01();
int test_mult_im_02();

int test_sub_01();
int test_sub_02();

int test_sub_im_01();
int test_sub_im_02();

int test_div_01();
int test_div_02();

int test_div_im_01();
int test_div_im_02();


int main(void) {
        assert("fromstr_01" && test_fromstr_01());
        assert("fromstr_02" && test_fromstr_02());

        assert("tostr_01" && test_tostr_01());
        assert("tostr_02" && test_tostr_02());

        assert("add_im_01" && test_add_im_01());
        assert("add_im_02" && test_add_im_02());

        assert("add_01" && test_add_01());
        assert("add_02" && test_add_02());

        printf("All tests completed successfully.\n");
        return 0;
}

int test_fromstr_01() {
        int i;
        int successful = 1;
        const unsigned int expected[] = {9, 0, 6, 8, 8, 5, 4, 3, 1, 6, 5, 4, 2, 5, 4, 3, 3};
        const int expected_scale = 5;
        const char *str = "334524561345.88609";

        apml_t num = malloc(sizeof(*num));

        apml_init(num, 10);
        apml_fromstr(num, 10, str);

        for (i = 0; i < num->size; i++) {
                if (num->num[i] != expected[i]) {
                        successful = 0;
                        break;
                }
        }

        successful = successful && (num->scale == expected_scale);

        apml_free(num);
        free(num);

        return successful;
}

int test_fromstr_02() {
        int i;
        int successful = 1;
        const unsigned int expected[] = {4, 5, 1, 4, 2, 1, 0, 2, 4, 2, 0, 1, 1, 1, 3};
        const int expected_scale = 11;
        const char *str = "3111.02420124154";

        apml_t num = malloc(sizeof(*num));

        apml_init(num, 5);
        apml_fromstr(num, 5, str);

        for (i = 0; i < num->size; i++) {
                if (num->num[i] != expected[i]) {
                        successful = 0;
                        break;
                }
        }
        successful = successful && (num->scale == expected_scale);

        apml_free(num);
        free(num);

        return successful;
}

int test_tostr_01() {
        int i;
        int successful = 1;
        const int expected[] = {9, 0, 6, 8, 8, 5, 4, 3, 1, 6, 5, 4, 2, 5, 4, 3, 3};
        const int expected_scale = 5;
        const char *str = "334524561345.88609";
        char *b;

        apml_t num = malloc(sizeof(*num));

        apml_init(num, 10);
        apml_set_scale(num, expected_scale);

        apml_realloc(num, 17);

        for (i = 0; i < 17; i++) {
                num->num[i] = expected[i];
        }

        num->size = 17;

        successful = strcmp(str, apml_tostr(num, &b)) == 0;

        apml_free(num);
        free(num);
        free(b);

        return successful;
}

int test_tostr_02() {
        int i;
        int successful = 1;
        const int expected[] = {4, 5, 1, 4, 2, 1, 0, 2, 4, 2, 0, 1, 1, 1, 3};
        const int expected_scale = 11;
        const char *str = "3111.02420124154";
        char *b;

        apml_t num = malloc(sizeof(*num));

        apml_init(num, 10);
        apml_set_scale(num, expected_scale);

        apml_realloc(num, 15);

        for (i = 0; i < 15; i++) {
                num->num[i] = expected[i];
        }

        num->size = 15;

        successful = strcmp(str, apml_tostr(num, &b)) == 0;

        apml_free(num);
        free(num);
        free(b);

        return successful;
}

int test_add_im_template(const char *n1, const char *n2, const char *expected, int base) {
        int successful;

        apml_t r = malloc(sizeof(*r));
        apml_t a = malloc(sizeof(*a));

        char *s;

        apml_init(r, base);
        apml_init(a, base);

        apml_fromstr(r, base, n1);
        apml_fromstr(a, base, n2);

        apml_add_im(r, a);

        successful = strcmp(expected, apml_tostr(r, &s)) == 0;

        apml_free(r);
        free(r);
        apml_free(a);
        free(a);

        free(s);

        return successful;
}

int test_add_im_01() {
        const char *n1 = "1243214.213414";
        const char *n2 = "1314154.231114";
        const char *expected = "3001412.444532";
        const int base = 6;

        return test_add_im_template(n1, n2, expected, base);
}

int test_add_im_02() {
        const char *n1 = "92522131112313412.213241";
        const char *n2 = "1314154.231114";
        const char *expected = "92522131113627566.444355";
        const int base = 10;

        return test_add_im_template(n1, n2, expected, base);
}

int test_add_template(const char *n1, const char *n2, const char *expected, int base) {
        int successful;

        apml_t r = malloc(sizeof(*r));
        apml_t a = malloc(sizeof(*a));
        apml_t b = malloc(sizeof(*b));

        char *s;

        apml_init(r, base);
        apml_init(a, base);
        apml_init(b, base);

        apml_fromstr(a, base, n1);
        apml_fromstr(b, base, n2);

        apml_add(r, a, b);

        successful = strcmp(expected, apml_tostr(r, &s)) == 0;

        apml_free(r);
        free(r);
        apml_free(a);
        free(a);
        apml_free(b);
        free(b);

        free(s);

        return successful;
}

int test_add_01() {
        const char *n1 = "1243214.213414";
        const char *n2 = "1314154.231114";
        const char *expected = "3001412.444532";
        const int base = 6;

        return test_add_template(n1, n2, expected, base);
}

int test_add_02() {
        const char *n1 = "92522131112313412.213241";
        const char *n2 = "1314154.231114";
        const char *expected = "92522131113627566.444355";
        const int base = 10;

        return test_add_template(n1, n2, expected, base);
}
