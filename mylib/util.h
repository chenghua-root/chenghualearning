#define offsetof(tp, field) ( (size_t)&((tp*)NULL)->field )
#define by_offset(p, offset) ( &((char*)(p))[offset] )
#define owner(p, tp, field) (tp*)by_offset(p, -offsetof(tp, field))


int assert(int exp, int val) {
    if (exp != val) {
        printf("exp=%d, get=%d!!!\n", exp, val);
        exit(-1);
    }
    return 0;
}

int assertBool(bool b) {
    if (!b) {
        printf("assert bool fail!!!\n");
        exit(-1);
    }
    return 0;
}
