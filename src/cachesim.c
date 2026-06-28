#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
typedef struct {
    unsigned long t;
    bool v;
    int a;
} L;
typedef struct {
    L *l;
    int n;
} S;
typedef struct {
    S *s;
    int ns;
    int nw;
    int bs;
    int bo;
    int si;
    unsigned long m;
    bool lru;
} C;
typedef struct {
    int r;
    int w;
    int h;
    int m;
} T;
bool ip(int x) {
    return x && !(x & (x - 1));
}
int lg(int x) {
    int r = 0;
    while ((1 << r) < x) r++;
    return r;
}
unsigned long gsi(C *c, unsigned long a) {
    return (a >> c->bo) &c->m;
}
unsigned long gt(C *c, unsigned long a) {
    return a >> (c->bo + c->si);
}
C cc(int cs, int bs, int a, bool l) {
    C c;
    c.bs =bs;
    c.bo = lg(bs);
    c.nw = a;
    c.ns = cs / (bs * a);
    c.si = lg(c.ns);
    c.m = (1UL << c.si) - 1;
    c.lru = l;
    c.s = malloc(sizeof(S) * c.ns);
    for (int i = 0; i < c.ns; i++) {
        c.s[i].l = malloc(sizeof(L) * a);
        for (int j = 0; j < a; j++) {
            c.s[i].l[j].v = false;
            c.s[i].l[j].a = 0;
        }
        c.s[i].n = 0;
    }
    return c;
}
void fc(C *c) {
    for (int i = 0; i < c->ns; i++) {
        free(c->s[i].l);
    }
    free(c->s);
}
bool ac(C *c, T *t, unsigned long a, bool w, bool p) {
    unsigned long i= gsi(c, a);
    unsigned long g = gt(c, a);
    S *s = &c->s[i];
    int hi = -1;
    for (int i = 0; i < c->nw && hi == -1; i++) {
        if (s->l[i].v && s->l[i].t == g) {
            hi = i;
        }
    }
    if (hi !=-1) {
        t->h++;
        if (w) t->w++;
        if (c->lru) {
            s->l[hi].a = 0;
            for (int j = 0; j < c->nw; j++) {
                if (j !=hi && s->l[j].v) {
                    s->l[j].a++;
                }
            }
        }
        return false;
    }
    t->m++;
    t->r++;
    if (w) t->w++;
    int x =-1;
    if (c->lru) {
        for (int i = 0; i < c->nw; i++) {
            if (!s->l[i].v) {
                x = i;
                break;
            }
        }
        if (x ==-1) {
            int ot = -1;
            int ae = -1;
            for (int i = 0; i < c->nw; i++) {
                if (s->l[i].a > ae) {
                    ae = s->l[i].a;
                    ot = i;
                }
            }
            x =ot;
        }
    } else{
        for (int i = 0; i < c->nw; i++) {
            if (!s->l[i].v) {
                x = i;
                break;
            }
        }
        if (x == -1) {
            x = s->n;
            s->n = (s->n + 1) % c->nw;
        }
    }
    s->l[x].v = true;
    s->l[x].t = g;
    if (c->lru) {
        s->l[x].a = 0;
        for (int j = 0; j < c->nw; j++) {
            if (j != x && s->l[j].v) {
                s->l[j].a++;
            }
        }
    }
    return true;
}
void sa(C *c, T *t, unsigned long a, bool w, bool p) {
    if (!ac(c, t, a, w, p) || !p) return;
    unsigned long na =((a >> c->bo) + 1) << c->bo;
    unsigned long i = gsi(c, na);
    unsigned long g = gt(c, na);
    S *s = &c->s[i];
    int fd = 0;
    for (int i = 0; i < c->nw; i++) {
        if (s->l[i].v && s->l[i].t == g) {
            fd= 1;
            break;
        }
    }
    if (fd) return;
    t->r++;
    int x= -1;
    if (c->lru) {
        for (int i = 0; i < c->nw; i++) {
            if (!s->l[i].v) {
                x = i;
                break;
            }
        }
        if (x== -1) {
            int ma = -1;
            for (int i= 0; i < c->nw; i++) {
                if (s->l[i].a > ma) {
                    ma = s->l[i].a;
                    x = i;
                }
            }
        }
    } else {
        for (int i= 0; i < c->nw; i++) {
            if (!s->l[i].v) {
                x =i;
                break;
            }
        }
        if (x == -1) {
            x = s->n;
            s->n = (s->n + 1) % c->nw;
        }
    }
    s->l[x].v = true;
    s->l[x].t=g;
    if (c->lru) {
        s->l[x].a = 0;
        for (int j= 0; j < c->nw; j++) {
            if (j != x&& s->l[j].v) {
                s->l[j].a++;
            }
        }
    }
}
int main(int n, char *v[]) {
    if (n == 6) {
        int cs = atoi(v[1]);
        int bs = atoi(v[4]);
        bool l = strcmp(v[3], "lru") == 0;
        if (ip(cs) && ip(bs)) {
            int a;
            if (strcmp(v[2], "direct") == 0) {
                a = 1;
            } else if (strncmp(v[2], "assoc:", 6) == 0) {
                a = atoi(v[2] + 6);
                if (!ip(a)) {
                    fprintf(stderr, "Associativity must be a power of 2.\n");
                    return 1;
                }
            } else if (strcmp(v[2], "assoc") == 0) {
                a= cs / bs;
            } else {
                fprintf(stderr, "wrong format.\n");
                return 1;
            }
            C c0 = cc(cs, bs, a, l);
            C c1 = cc(cs, bs, a, l);
            T t0= {0}, t1 = {0};
            FILE *f = fopen(v[5], "r");
            if (f == NULL) {
                perror("Error w file");
                return 1;
            }
            char o;
            unsigned long ad;
            char lbuf[100];
            while (fgets(lbuf, sizeof(lbuf), f)) {
                if (lbuf[0] == '#') break;
                if (sscanf(lbuf, "%*x: %c %lx", &o, &ad) == 2) {
                    sa(&c0, &t0, ad, o== 'W', false);
                    sa(&c1, &t1, ad, o == 'W', true);
                }
            }
            fclose(f);
            printf("Prefetch 0\n");
            printf("Memory reads: %d\n", t0.r);
            printf("Memory writes: %d\n", t0.w);
            printf("Cache hits: %d\n", t0.h);
            printf("Cache misses: %d\n", t0.m);
            printf("Prefetch 1\n");
            printf("Memory reads: %d\n", t1.r);
            printf("Memory writes: %d\n", t1.w);
            printf("Cache hits: %d\n", t1.h);
            printf("Cache misses: %d\n", t1.m);
            fc(&c0);
            fc(&c1);
            return 0;
        } else {
            fprintf(stderr, "Cache/block size must be powers of 2.\n");
            return 1;
        }
    }
    fprintf(stderr, "Usage: %s <cache size> <assoc[:n]> <fifo|lru> <block size> <trace file>\n", v[0]);
    return 1;
}

