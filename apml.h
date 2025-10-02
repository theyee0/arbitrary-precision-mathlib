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

#ifndef MIN
#define MAX(A, B) ((A) < (B) ? (A) : (B))
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

apml_t apml_shallow_copy(apml_t r, const apml_t a) {
        r->scale = a->scale;
        r->size = a->size;
        r->num = a->num;
        r->alloc = a->alloc;

        r->sign = a->sign;

        return r;
}

int apml_c_mod(const apml_t a, const int c) {
        int i;
        int m;
        int t;

        m = 0;
        t = 1;
        for (i = a->scale; i < a->size; i++) {
                m += ((a->num[i] % c) * (t % c)) % c;
                t *= a->base;
                t %= c;
        }

        return m;
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

apml_t apml_set_base(apml_t r, apml_t a, const int base) {
        /* Not yet implemented */
        return -1;
}

int apml_cmp(const apml_t a, const apml_t b) {
        int i;

        int av;
        int bv;

        if (a->base != b->base) {
                return -1;
        }

        if (a->size - a->scale != b->size - b->scale) {
                return (a->size - a->scale > b->size - b->scale) ? 1 : -1;
        } else {
                for (i = 1; i <= a->size - a->scale; i++) {
                        av = a->num[a->size - i];
                        bv = b->num[b->size - i];

                        if (av > bv) {
                                return 1;
                        } else if (av < bv) {
                                return -1;
                        }
                }

                for (i = 1; i < MIN(a->scale, b->scale); i++) {
                        av = a->num[a->scale - i];
                        bv = b->num[b->scale - i];

                        if (av > bv) {
                                return 1;
                        } else if (av < bv) {
                                return -1;
                        }
                }

                for (i = MIN(a->scale, b->scale); i < a->scale; i++) {
                        av = a->num[a->scale - i];

                        if (av > 0) {
                                return 1;
                        }
                }

                for (i = MIN(a->scale, b->scale); i < b->scale; i++) {
                        bv = b->num[b->scale - i];

                        if (bv > 0) {
                                return 1;
                        }
                }
        }

        return 0;
}

apml_t apml_add(apml_t r, const apml_t a, const apml_t b) {
        int i;
        int carry = 0;
        long long tmp;

        struct apml_struct t;

        if (a->base != b->base || a->scale != b->scale) {
                return NULL;
        }

        if (a->sign != b->sign) {
                apml_shallow_copy(&t, b);
                t.sign = a->sign;

                return apml_sub(r, a, &t);
        }

        r->base = a->base;
        r->sign = a->sign;
        apml_set_scale(r, a->scale);
        apml_reallocate(r, MAX(a->size, b->size) + 1);

        for (i = 0; i < MIN(a->size, b->size); i++) {
                tmp = carry;
                tmp += a->num[i];
                tmp += b->num[i];

                r->num[i] = tmp % r->base;
                carry = tmp / r->base;
        }

        if (a->size > b->size) {
                r->size = a->size;
                for (i = b->size; i < a->size; i++) {
                        tmp = carry;
                        tmp += a->num[i];

                        r->num[i] = tmp % r->base;
                        carry = tmp / r->base;
                }
        } else {
                r->size = b->size;
                for (i = a->size; i < b->size; i++) {
                        tmp = carry;
                        tmp += b->num[i];

                        r->num[i] = tmp % r->base;
                        carry = tmp / r->base;
                }
        }

        if (carry > 0) {
                r->num[r->size++] = carry;
        }

        return r;
}

apml_t apml_sub(apml_t r, const apml_t a, const apml_t b) {
        int i;
        int carry = 0;
        long long tmp;

        struct apml_struct t;

        if (a->base != b->base || a->scale != b->scale) {
                return NULL;
        }

        if (a->sign != b->sign) {
                apml_shallow_copy(&t, b);
                t.sign = a->sign;

                return apml_add(r, a, &t);
        }

        apml_realloc(r, MAX(a->size, b->size));
        r->base = a->base;

        if (apml_cmp(a, b) < 0) {
                apml_sub(r, b, a);
        } else {
                for (i = 0; i < MIN(a->size, b->size); i++) {
                        r->num[i] = a->num[i] - b->num[i] - carry;
                        if (r->num[i] < 0) {
                                r->num[i] += r->base;
                                carry = 1;
                        } else {
                                carry = 0;
                        }
                }

                while (carry > 0) {
                        r->num[i] = a->num[i] - carry;
                        if (r->num[i] < 0) {
                                r->num[i] += r->base;
                                carry = 1;
                        } else {
                                carry = 0;
                        }
                }
        }

        return r;
}

apml_t apml_mult(apml_t r, apml_t a, apml_t b) {
        return -1;
}

apml_t apml_div(apml_t r, apml_t a, apml_t b) {
        return -1;
}

#endif
