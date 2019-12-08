#include <pthread.h>
#include <semaphore.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

/* 创建线程:
 * int pthread_create(pthread_t *thread, const pthread_attr_t *attr,
                          void *(*start_routine) (void *), void *arg);
 *
 *
 */
#define atomic_inc(val) __sync_add_and_fetch((val), 1)

#define THREAD_NUM 10
#define TEST_CASE_NUM 1000

int assert(int exp, int val) {
    if (exp != val) {
        printf("exp=%d, get=%d", exp, val);
        exit(-1);
    }
    return 0;
}

/**************************pthread_rwlock_t***********************/
typedef struct PthreadRWLockArg PthreadRWLockArg;
struct PthreadRWLockArg {
    pthread_rwlock_t rwlock;
    int cnt;
};

void *pthread_rwlock_read(void *arg_) {
    PthreadRWLockArg *arg = arg_;
    int i = 0;
    for (; i < TEST_CASE_NUM; ++i) {
        pthread_rwlock_rdlock(&arg->rwlock);
        // read
        pthread_rwlock_unlock(&arg->rwlock);
    }
}

void *pthread_rwlock_write(void *arg_) {
    PthreadRWLockArg *arg = arg_;
    int i = 0;
    for (; i < TEST_CASE_NUM; ++i) {
        pthread_rwlock_wrlock(&arg->rwlock);
        arg->cnt++;
        pthread_rwlock_unlock(&arg->rwlock);
    }
}

void pthread_rwlock() {
    pthread_rwlockattr_t attr;
    pthread_rwlockattr_setkind_np(&attr, PTHREAD_RWLOCK_PREFER_WRITER_NP); // 设置不饿死

    PthreadRWLockArg arg = (PthreadRWLockArg){.cnt = 0};
    pthread_rwlock_init(&arg.rwlock, &attr);


    pthread_t read_threads[THREAD_NUM] = {0};
    pthread_t write_threads[THREAD_NUM] = {0};
    int i = 0;
    for (i = 0; i < THREAD_NUM; ++i) {
        pthread_create(&read_threads[i], NULL, pthread_rwlock_read, &arg);
    }
    for (i = 0; i < THREAD_NUM; ++i) {
        pthread_create(&write_threads[i], NULL, pthread_rwlock_write, &arg);
    }
    for (i = 0; i < THREAD_NUM; ++i) {
        pthread_join(read_threads[i], NULL);
    }
    for (i = 0; i < THREAD_NUM; ++i) {
        pthread_join(write_threads[i], NULL);
    }
    assert(THREAD_NUM * TEST_CASE_NUM,  arg.cnt);
}

/**************************pthread_spinlock_t***********************/

typedef struct PthreadSpinlockArg PthreadSpinlockArg;
struct PthreadSpinlockArg {
    pthread_spinlock_t lock;
    int cnt;
};

void *pthread_spinlock_do(void *arg_) {
    PthreadSpinlockArg *arg = arg_;
    int i = 0;
    for (; i < TEST_CASE_NUM; ++i) {
        pthread_spin_lock(&arg->lock);
        arg->cnt++;
        pthread_spin_unlock(&arg->lock);
    }
}

void pthread_spinlock() {
    PthreadSpinlockArg arg = (PthreadSpinlockArg){.cnt=0};
    pthread_spin_init(&arg.lock, PTHREAD_PROCESS_PRIVATE); // PTHREAD_PROCESS_SHARED

    pthread_t threads[THREAD_NUM] = {0};
    int i = 0;
    for (i = 0; i < THREAD_NUM; ++i) {
        pthread_create(&threads[i], NULL, pthread_spinlock_do, &arg);
    }
    for (i = 0; i < THREAD_NUM; ++i) {
        pthread_join(threads[i], NULL);
    }
    assert(THREAD_NUM * TEST_CASE_NUM, arg.cnt);

    pthread_spin_destroy(&arg.lock);
}


/**************************condition***********************/
/*
 * 条件变量是边缘触发, 先sign再wait的话会丢失，将丢失唤醒信号
 *
 * cond需要配合mutex使用的原因：
 *   1. 保护condition？NO
 *   2. 防止唤醒丢失？ YES
 *       pthread_cond_wait(&cond)（线程A）判定cond不满足，准备等待(放入唤醒队列)。
 *       在睡眠之前pthread_cond_signal(线程B)发出了唤醒信号，此时还未线程A还未开始放入队列, 丢失信号。
 *
 */

typedef struct PthreadCondArg PthreadCondArg;
struct PthreadCondArg {
    pthread_mutex_t mutex;
    pthread_cond_t cond;
    bool condition; // 避免丢失唤醒
    int cnt;
};

void * pthread_cond_do_sign(void *arg_) {
    PthreadCondArg *arg = arg_;
    int i = 0;
    pthread_mutex_lock(&arg->mutex);
    arg->condition = true;
    pthread_cond_signal(&arg->cond);
    pthread_mutex_unlock(&arg->mutex);
}

void * pthread_cond_do_wait(void *arg_) {
    PthreadCondArg *arg = arg_;
    int i = 0;
    pthread_mutex_lock(&arg->mutex);
    while(!arg->condition) {
        pthread_cond_wait(&arg->cond, &arg->mutex);
    }
    arg->condition = false;
    arg->cnt++;
    pthread_mutex_unlock(&arg->mutex);
}

void pthread_cond() {
    PthreadCondArg *arg = &(PthreadCondArg){.condition=false, .cnt=0};
    pthread_mutex_init(&arg->mutex, NULL);
    pthread_cond_init(&arg->cond, NULL);

    pthread_t cond_thread;
    pthread_t sign_thread;
    pthread_create(&cond_thread, NULL, pthread_cond_do_wait, arg);
    assert(0, arg->cnt);
    usleep(1);

    pthread_create(&sign_thread, NULL, pthread_cond_do_sign, arg);

    pthread_join(cond_thread, NULL);
    pthread_join(sign_thread, NULL);
    assert(1, arg->cnt);
}

/**************************sem***********************/

typedef struct PthreadSemArg PthreadSemArg;
struct PthreadSemArg {
    sem_t sem;
    int cnt;
};

void * pthread_sem_wait(void *arg_) {
    PthreadSemArg *arg = arg_;
    int i = 0;
    for (; i < TEST_CASE_NUM; ++i) {
        sem_wait(&arg->sem);
        atomic_inc(&arg->cnt);
    }
}

void * pthread_sem_post(void *arg_) {
    PthreadSemArg *arg = arg_;
    int i = 0;
    for (; i < TEST_CASE_NUM; ++i) {
        sem_post(&arg->sem);
    }
}

void pthread_sem() {
    PthreadSemArg *arg = &(PthreadSemArg){.cnt=0};
    sem_init(&arg->sem, PTHREAD_PROCESS_PRIVATE, 0);

    pthread_t wait_threads[THREAD_NUM];
    pthread_t post_threads[THREAD_NUM];
    int i = 0;
    for (i = 0; i < THREAD_NUM; ++i) {
        pthread_create(&wait_threads[i], NULL, pthread_sem_wait, arg);
    }
    for (i = 0; i < THREAD_NUM; ++i) {
        pthread_create(&post_threads[i], NULL, pthread_sem_post, arg);
    }
    for (i = 0; i < THREAD_NUM; ++i) {
        pthread_join(wait_threads[i], NULL);
    }
    for (i = 0; i < THREAD_NUM; ++i) {
        pthread_join(post_threads[i], NULL);
    }
    assert(THREAD_NUM * TEST_CASE_NUM, arg->cnt);

    sem_destroy(&arg->sem);
}


int main() {
    int i = 0;
    for (; i < 10; ++i) {
        pthread_rwlock();
        pthread_spinlock();
        pthread_cond();
        pthread_sem();
    }
    return 0;
}

