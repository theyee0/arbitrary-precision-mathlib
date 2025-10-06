#ifndef __APML_H
#define __APML_H

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>

#ifndef apml_h_oom
#define apml_h_oom() exit(-1)
#endif

#ifndef MAX
#define MAX(A, B) ((A) > (B) ? (A) : (B))
#endif

#ifndef MIN
#define MIN(A, B) ((A) < (B) ? (A) : (B))
#endif

/* Underlying type of each apml "cell" */
typedef unsigned int __apml_unit;

/* Structure representing fixed-point number */
struct apml_struct {
        int alloc;        /* Current memory allocated */
        int size;         /* Current length of number */

        int sign;         /* Sign, +1 or -1 */
        int base;         /* Number base */
        int scale;        /* Number of digits after decimal point */
        __apml_unit *num; /* Actual data */
};

/* Typedef to abstract nature of fixed point numbers */
typedef struct apml_struct* apml_t;

/* Initialization/memory management functions */
apml_t  apml_realloc(apml_t r, const int n);
apml_t  apml_init(apml_t r, const int base);
apml_t  apml_free(apml_t r);
/* Export and import for testing */
apml_t  apml_fromstr(apml_t r, const int base, const char *s);
char   *apml_tostr(apml_t r, char **s);
/* Copy functions */
apml_t  apml_shallow_copy(apml_t r, const apml_t a);
apml_t  apml_copy(apml_t r, const apml_t a);
/* Change number settings */
apml_t  apml_set_scale(apml_t r, const int scale);
apml_t  apml_set_base(apml_t r, const apml_t a, const int base);
/* Constant operations */
int     apml_c_mod(const apml_t a, const int c);
apml_t  apml_c_div_im(apml_t r, const int c);
apml_t  apml_c_div(apml_t r, const apml_t a, const int c);
apml_t  apml_c_mult_im(apml_t r, const int c);
apml_t  apml_c_mult(apml_t r, const apml_t a, const int c);
/* Fixed point operations */
int     apml_cmp(const apml_t a, const apml_t b);
apml_t  apml_add_im(apml_t r, const apml_t a);
apml_t  apml_add(apml_t r, const apml_t a, const apml_t b);
apml_t  apml_sub_im(apml_t r, const apml_t a);
apml_t  apml_sub(apml_t r, const apml_t a, const apml_t b);
apml_t  apml_mult(apml_t r, const apml_t a, const apml_t b);
apml_t  apml_mult_im(apml_t r, const apml_t a);
apml_t  apml_div(apml_t r, const apml_t a, const apml_t b);
apml_t  apml_div_im(apml_t r, const apml_t a);

/* If necessary, reallocate memory in apml_t to meet a minimum threshold */
apml_t apml_realloc(apml_t r, const int n) {
        if (r->alloc < n) {
                r->alloc = n;
                r->num = realloc(r->num, n * sizeof(*r->num));
        }

        /* Ensure that allocation was successful */
        if (r->num == NULL) {
                apml_h_oom();
        }

        return r;
}

/* Initialize number to default values and allocate 4 digits */
apml_t apml_init(apml_t r, const int base) {
        r->alloc = 4;
        r->size = 0;
        r->sign = 1;
        r->base = base;
        r->scale = 0;

        r->num = malloc(r->alloc * sizeof(*r->num));

        /* Ensure that allocation was successful */
        if (r->num == NULL) {
                apml_h_oom();
        }

        r->num[0] = 0;

        return r;
}

/* Destroy number and free its memory */
apml_t apml_free(apml_t r) {
        r->alloc = 0;
        r->size = 0;
        free(r->num);
        r->num = NULL;

        return r;
}

/* Set r based on a given string (base <= 10) */
apml_t apml_fromstr(apml_t r, const int base, const char *s) {
        int i;
        int l = strlen(s);
        int scale;

        assert(strchr(s, '.') != NULL);

        scale = l - (int)(strchr(s, '.') - s) - 1; /* Scale of `s` */

        /* If invalid base provided */
        if (base < 1 || 10 < base) {
                return NULL;
        }

        apml_realloc(r, l);
        apml_set_scale(r, scale);
        r->base = base;
        r->size = 0;

        for (i = l - 1; i >= 0; i--) {
                if (strchr("1234567890", s[i]) != NULL) {
                        r->num[r->size++] = s[i] - '0';
                }
        }

        return r;
}

/* Set r based on a given string (base <= 10) */
char *apml_tostr(apml_t r, char **s) {
        int i;
        int l = 0;

        /* If invalid base provided */
        if (r->base < 1 || 10 < r->base) {
                *s = NULL;
                return NULL;
        }

        *s = malloc((r->size + 2) * sizeof(**s));

        /* Ensure successful allocation */
        if (*s == NULL) {
                apml_h_oom();
        }

        /* Add digits to string */
        for (i = r->size - 1; i >= 0; i--) {
                (*s)[l++] = r->num[i] + '0';

                /* At the scale character, add decimal point */
                if (i == r->scale) {
                        (*s)[l++] = '.';
                }
        }

        (*s)[l] = '\0';

        return *s;
}

/* Change the number of digits after decimal point */
apml_t apml_set_scale(apml_t r, const int scale) {
        int offset = scale - r->scale; /* Number of digits to add to the scale */

        if (offset < 0) {
                /* Shift digits down to lower precision */
                memmove(r->num, r->num - offset, (r->size + offset) * sizeof(*r->num));
        } else {
                /* Shift digits up and fill the new spaces with zeroes */
                apml_realloc(r, r->size + offset);
                memmove(r->num + offset, r->num, r->size * sizeof(*r->num));
                memset(r->num, 0, offset * sizeof(*r->num));
        }

        /* Update size and scale of number */
        r->size += offset;
        r->scale = scale;

        return r;
}

/* Create a copy of a number, but don't copy memory */
apml_t apml_shallow_copy(apml_t r, const apml_t a) {
        r->scale = a->scale;
        r->size = a->size;
        r->num = a->num;
        r->alloc = a->alloc;

        r->sign = a->sign;

        return r;
}

/* Create a full copy of a number */
apml_t apml_copy(apml_t r, const apml_t a) {
        r->scale = a->scale;
        r->size = a->size;
        r->sign = a->sign;

        apml_realloc(r, a->alloc);

        memcpy(r->num, a->num, a->alloc * sizeof(*r->num));

        return r;
}

/* Calculate a number, mod an integer */
int apml_c_mod(const apml_t a, const int c) {
        int i;
        int m;
        int t;

        m = 0;
        t = 1;
        for (i = a->scale; i < a->size; i++) {
                /* Calculate mod of current digit with ((a % c) * (b % c)) % c = (a * b) % c */
                m += ((a->num[i] % c) * (t % c)) % c;
                t *= a->base;
                t %= c;
        }

        return m;
}

/* Divide number by an integer and store the result back */
apml_t apml_c_div_im(apml_t r, const int c) {
        /* Not yet implemented; print values and exit */
        printf("%p %d\n", (void*)r, c);
        exit(1);
}

/* Divide number by an integer and store the result in a new variable */
apml_t apml_c_div(apml_t r, const apml_t a, const int c) {
        /* Not yet implemented; print values and exit */
        printf("%p %p %d\n", (void*)r, (void*)a, c);
        exit(1);
}

/* Multiply number by integer and store the result back */
apml_t apml_c_mult_im(apml_t r, const int c) {
        int i;
        int carry = 0;
        int tmp;

        apml_realloc(r, r->size + 1);

        /* Use grade-school multiplication algorithm */
        for (i = 0; i < r->size; i++) {
                tmp = c;
                tmp *= r->num[i];
                tmp += carry;

                r->num[i] = tmp % r->base;
                carry = tmp / r->base;
        }

        /* Add extra digits if necessary */
        while (carry > 0) {
                apml_realloc(r, r->size + 1);
                r->num[r->size++] = carry % r->base;
                carry /= r->base;
        }

        return r;
}

/* Multiply number by an integer and store the result in a new variable */
apml_t apml_c_mult(apml_t r, const apml_t a, const int c) {
        apml_copy(r, a);

        return apml_c_mult_im(r, c);
}

/* Change the base of a number */
apml_t apml_set_base(apml_t r, const apml_t a, const int base) {
        int i, j, m;
        apml_t d = malloc(sizeof(*d));
        int tmp;

        r->size = 0;
        r->scale = a->scale;

        /* Compute integer part */
        for (i = a->scale; i < a->size; i++) {
                m = 0;
                tmp = 1;
                for (j = i; j < a->size; j++) {
                        m += ((a->num[j] % base) * (tmp % base)) % base; /* Calculate (a[i] * tmp) % c */
                        tmp *= a->base;
                        tmp %= base;
                }

                apml_realloc(r, r->size + r->scale + 1);
                r->num[r->size++ + r->scale] = tmp;
        }

        apml_init(d, base);
        apml_set_scale(d, a->scale + 1); /* Higher scale to minimize roundoff error */
        apml_realloc(d, d->scale + 2);
        d->size = d->scale + 1;

        /* Compute fractional component */
        for (i = 0; i < a->scale; i++) {
                d->num[d->size - 1] = a->num[i];

                for (j = 0; j < a->scale - i - 1; j++) {
                        apml_c_div_im(d, base);
                }

                apml_add_im(r, d);
        }

        apml_free(d);
        free(d);

        return r;
}

/* Standard C comparison function returning -1, 0, or 1 after comparing a and b */
int apml_cmp(const apml_t a, const apml_t b) {
        int i;

        int av; /* Value of a given digit of a */
        int bv; /* Value of a given digit of b */

        /* Assert that the bases are the same; TODO: Change scale to gracefully compare */
        if (a->base != b->base) {
                exit(-1);
        }

        if (a->size - a->scale != b->size - b->scale) {
                /* If one number is clearly greater than the other by power*/
                return (a->size - a->scale > b->size - b->scale) ? 1 : -1;
        } else {
                /* Compare integer parts */
                for (i = 1; i <= a->size - a->scale; i++) {
                        av = a->num[a->size - i];
                        bv = b->num[b->size - i];

                        if (av > bv) {
                                return 1;
                        } else if (av < bv) {
                                return -1;
                        }
                }

                /* Compare fractional parts */
                for (i = 1; i < MIN(a->scale, b->scale); i++) {
                        av = a->num[a->scale - i];
                        bv = b->num[b->scale - i];

                        if (av > bv) {
                                return 1;
                        } else if (av < bv) {
                                return -1;
                        }
                }

                /* If a has more decimal places, check if any of them are nonzero */
                for (i = MIN(a->scale, b->scale); i < a->scale; i++) {
                        av = a->num[a->scale - i];

                        if (av > 0) {
                                return 1;
                        }
                }

                /* If b has more decimal places, check if any of them are nonzero */
                for (i = MIN(a->scale, b->scale); i < b->scale; i++) {
                        bv = b->num[b->scale - i];

                        if (bv > 0) {
                                return 1;
                        }
                }
        }

        return 0;
}

/* Add two numbers and store the result back */
apml_t apml_add_im(apml_t r, const apml_t a) {
        int i;
        int carry = 0;
        int tmp;

        apml_t t = NULL;

        /* Ensure that the numbers can be compared; TODO: Add graceful handling */
        if (r->base != a->base || r->scale != a->scale) {
                return NULL;
        }

        /* If the signs are different, treat as subtraction */
        if (r->sign != a->sign) {
                t = malloc(sizeof(*t));
                apml_shallow_copy(t, a);
                t->sign = a->sign;

                if (apml_sub_im(r, t)) {
                        apml_free(t);
                        free(t);
                        return r;
                } else {
                        apml_free(t);
                        free(t);
                        return NULL;
                }
        }

        apml_realloc(r, MAX(r->size, a->size) + 1);

        /* Add common digits */
        for (i = 0; i < MIN(r->size, a->size); i++) {
                tmp = carry;
                tmp += r->num[i];
                tmp += a->num[i];

                r->num[i] = tmp % r->base;
                carry = tmp / r->base;
        }

        /* Add remaining digits if the numbers differ in length */
        if (r->size > a->size) {
                i = a->size;
                while (carry > 0) {
                        r->num[i] += carry;
                        carry = r->num[i] / r->base;
                        r->num[i] %= r->base;
                        i++;
                }

                r->size = MAX(i + 1, r->size);
        } else {
                for (i = r->size; i < a->size; i++) {
                        tmp = carry;
                        tmp += a->num[i];

                        r->num[i] = tmp % r->base;
                        carry = tmp / r->base;
                }

                r->size = a->size;

                while (carry > 0) {
                        r->num[r->size] += carry;
                        carry = r->num[r->size] / r->base;
                        r->num[r->size] %= r->base;
                        r->size++;
                }
        }

        return r;
}

/* Add numbers and store the result in a new variable */
apml_t apml_add(apml_t r, const apml_t a, const apml_t b) {
        apml_copy(r, a);
        return apml_add_im(r, b);
}

/* Subtract two numbers and store the result back */
apml_t apml_sub_im(apml_t r, const apml_t a) {
        int i;
        int carry = 0;
        int tmp;
        apml_t t = NULL;

        /* Ensure that the numbers can be compared; TODO: Add graceful handling */
        if (r->base != a->base || r->scale != a->scale) {
                return NULL;
        }

        /* If the signs are different, treat as addition */
        if (r->sign != a->sign) {
                t = malloc(sizeof(*t));
                apml_shallow_copy(t, a);
                t->sign = a->sign;

                if (apml_add_im(r, t)) {
                        apml_free(t);
                        free(t);
                        return r;
                } else {
                        apml_free(t);
                        free(t);
                        return NULL;
                }
        }

        apml_realloc(r, MAX(a->size, r->size));
        r->base = a->base;

        /* Swap numbers around if the first number is less than the second */
        if (apml_cmp(r, a) < 0) {
                t = malloc(sizeof(*t));
                apml_init(t, r->base);
                apml_copy(t, r);
                apml_copy(r, a);

                if (apml_sub_im(r, t)) {
                        r->sign *= -1;
                        apml_free(t);
                        free(t);
                        return r;
                } else {
                        apml_free(t);
                        free(t);
                        return NULL;
                }
        } else {
                /* Subtract common digits */
                for (i = 0; i < MIN(a->size, r->size); i++) {
                        tmp = r->num[i] - a->num[i] - carry;
                        if (tmp < 0) {
                                tmp += r->base;
                                carry = 1;
                        } else {
                                carry = 0;
                        }

                        r->num[i] = tmp;
                }

                /* Sort out remaining borrows */
                while (carry > 0) {
                        tmp = r->num[i] - carry;

                        if (tmp < 0) {
                                tmp += r->base;
                                carry = 1;
                        } else {
                                carry = 0;
                        }

                        r->num[i] = tmp;
                }
        }

        return r;
}

/* Subtract two variables and store the result back */
apml_t apml_sub(apml_t r, const apml_t a, const apml_t b) {
        apml_copy(r, a);

        return apml_sub_im(r, b);
}

/* Multiply two numbers and store the result in a new variable */
apml_t apml_mult(apml_t r, apml_t a, apml_t b) {
        /* Not yet implemented; print values and exit */
        printf("%p %p %p\n", (void*)r, (void*)a, (void*)b);
        exit(1);
}

/* Divide two numbers and store the result in a new variable */
apml_t apml_div(apml_t r, apml_t a, apml_t b) {
        /* Not yet implemented; print values and exit */
        printf("%p %p %p\n", (void*)r, (void*)a, (void*)b);
        exit(1);
}

#endif
