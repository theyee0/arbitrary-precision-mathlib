#ifndef __APML_H
#define __APML_H

typedef unsigned int __apml_unit;

struct apml_struct {
        int alloc;
        int size;

        int sign;
        int base;
        int scale;
        __apml_unit *num;
};

typedef struct apml_struct* apml_t;

apml_t apml_realloc(apml_t r, const int n) {
        return -1;
}

apml_t apml_init(apml_t r) {
        return -1;
}

apml_t apml_free(apml_t r) {
        return -1;
}

apml_t apml_add(apml_t r, apml_t a, apml_t b) {
        return -1;
}

apml_t apml_sub(apml_t r, apml_t a, apml_t b) {
        return -1;
}

apml_t apml_mult(apml_t r, apml_t a, apml_t b) {
        return -1;
}

apml_t apml_div(apml_t r, apml_t a, apml_t b) {
        return -1;
}

#endif
