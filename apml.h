#ifndef __APML_H
#define __APML_H

#include <stdlib.h>
#include <string.h>

#ifndef apml_h_oom
#define apml_h_oom() exit(-1)
#endif

#ifndef MAX
#define MAX(A, B) ((A) > (B) ? (A) : (B))
#endif

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
        if (r->size < n) {
                r->alloc = n;
                r->num = realloc(n * sizeof(*r->num));
        }

        if (r->num == NULL) {
                apml_h_oom();
        }

        return r;
}

apml_t apml_init(apml_t r, const int base) {
        r->alloc = 4;
        r->size = 1;
        r->sign = 1;
        r->base = base;
        r->scale = 0;
        r->num = malloc(r->alloc * sizeof(*r->num));

        if (r->num == NULL) {
                apml_h_oom();
        }

        r->num[0] = 0;

        return r;
}

apml_t apml_free(apml_t r) {
        r->alloc = 0;
        r->size = 0;
        free(r->num);
        r->num = NULL;

        return r;
}

apml_t apml_set_scale(apml_t r, const int scale) {
        int offset = scale - r->scale;

        if (offset < 0) {
                memmove(r->num, r->num - offset, (r->size + offset) * sizeof(*r->num));
        } else {
                apml_realloc(r, r->size + offset);
                memmove(r->num + offset, r->num, r->size * sizeof(*r->num));
                memset(r->num, 0, offset * sizeof(*r->num));
        }

        r->size += offset;
        r->scale = scale;

        return r;
}

apml_t apml_c_mod(apml_t r, const apml_t a, const int c) {
        /* Not yet Implemented */
        exit(1);
}

apml_t apml_c_div(apml_t r, const apml_t a, const int c) {
        /* Not yet Implemented */
        exit(1);
}

apml_t apml_c_mult(apml_t r, const apml_t a, const int c) {
        int i;
        int carry = 0;
        long long tmp;

        apml_realloc(r, a->size + 1);

        for (i = 0; i < a->size; i++) {
                tmp = c;
                tmp *= a->num[i];
                tmp += carry;

                r->num[i] = tmp % r->base;
                carry = tmp / r->base;
        }

        if (carry > 0) {
                r->num[r->size++] = carry;
        }

        return r;
}

apml_t apml_set_base(apml_t r, const int base) {
        return -1;
}

apml_t apml_add(apml_t r, const apml_t a, const apml_t b) {
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
