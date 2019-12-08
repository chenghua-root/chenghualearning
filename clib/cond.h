#include <pthread.h>
#include <stdint.h>
#include <stdlib.h>
#include <errno.h>

//#define atomic_store(x, v) __atomic_store_n((x), (v), __ATOMIC_SEQ_CST)

typedef struct Cond Cond;
struct Cond {
  pthread_mutex_t mtx_;
  pthread_cond_t cond_;
  int64_t cnt; // 避免丢失唤醒
};

Cond *cond_construct() {
  Cond *c = malloc(sizeof(Cond));
  return c;
}

int cond_init(Cond *c) {
  if (NULL == c) {
     return -1;
  }

  pthread_cond_init(&c->cond_, NULL);
  pthread_mutex_init(&c->mtx_, NULL);
  c->cnt = 0;

  return 0;
}

void cond_destruct(Cond *c) {
  if (NULL != c) {
    free(c);
  }
}

void cond_destroy(Cond *c) {
  if (NULL == c) {
    return;
  }
  pthread_cond_destroy(&c->cond_);
  pthread_mutex_destroy(&c->mtx_);
}

void cond_wakeup(Cond *c) {
  pthread_mutex_lock(&c->mtx_);
  c->cnt++;
  pthread_cond_signal(&c->cond_);
  pthread_mutex_unlock(&c->mtx_);
}

int cond_wait(Cond *c) {
  pthread_mutex_lock(&c->mtx_);
  while(0 == c->cnt) {
      pthread_cond_wait(&c->cond_, &c->mtx_);
  }
  c->cnt--;
  pthread_mutex_unlock(&c->mtx_);
}
