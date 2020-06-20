#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "util.h"

int nextPowerOfTwo(int n) {
    if (0 == n) {
          return 1;
    }

    n -= 1;
    n |= (n >> 1);
    n |= (n >> 1);
    n |= (n >> 2);
    n |= (n >> 4);
    n |= (n >> 8);
    n |= (n >> 16);

    return ++n;
}

typedef struct kfifo kfifo;
struct kfifo {
    unsigned char *buf;
    unsigned int size;
    unsigned int in;
    unsigned int out;
};

kfifo *kfifo_init(unsigned int size) {
    kfifo *fifo = NULL;
    size_t buf_size = nextPowerOfTwo(size);

    fifo = (kfifo *)malloc(sizeof(kfifo));
    if (NULL == fifo) {
        return NULL;
    }
    fifo->buf = (unsigned char*)malloc(buf_size);
    if (NULL == fifo->buf) {
        free(fifo);
        return NULL;
    }
    fifo->size = buf_size;
    fifo->in = 0;
    fifo->out = 0;

    return fifo;
}

/* 学习点：
 * 1. 如何保证正确性，in超过unsigned int最大值后仍然正确, 即in < out时
 * 2. in/out与size的关系，size需为2的指数，
 *    in & (size - 1) 等价于in对size取余，需保证size是2的指数
 * 3. 内存屏蔽
 */

unsigned int kfifo_put(kfifo *fifo, const unsigned char *buf, unsigned int len) {
    unsigned int l;
    len = min(len, fifo->size - fifo->in + fifo->out);

    smp_mb();

    l = min(len, fifo->size - (fifo->in & (fifo->size - 1)));
    memcpy(fifo->buf + (fifo->in & (fifo->size - 1)), buf, l);

    memcpy(fifo->buf, buf + l, len - l);

    smp_wmb(); // 保证在更新fifo->in时，buf数据已经被写入
    fifo->in += len;

    return len;
}

unsigned int kfifo_get(kfifo *fifo, unsigned char *buf, unsigned int len) {
    unsigned int l;
    len = min(len, fifo->in - fifo->out);

    smp_rmb();

    l = min(len, fifo->size - (fifo->out & (fifo->size - 1)));
    memcpy(buf, fifo->buf + (fifo->out & (fifo->size - 1)), l);

    memcpy(buf + l, fifo->buf, len - l);

    smp_mb(); // 保证在更新fifo->out前，数据已经被拷贝
    fifo->out += len;

    return len;
}

int main() {
    kfifo *fifo = kfifo_init(588);
    assertBool(fifo);
    assert(1024, fifo->size);

    unsigned char A[600];
    for (int i = 0; i < sizeof(A); ++i) {
        A[i] = 'a';
    }
    unsigned char B[600];
    for (int i = 0; i < sizeof(B); ++i) {
        B[i] = 'a';
    }
    int a = kfifo_put(fifo, A, 600);
    assert(600, a);

    unsigned char A2[100];
    a = kfifo_get(fifo, A2, sizeof(A2));
    assert(100, a);

    int b = kfifo_put(fifo, B, 600);
    assert(1024 - (600-100), b);
}
