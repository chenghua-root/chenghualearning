#include <pthread.h>
#include <stdio.h>
#include <semaphore.h>

#define TEST_NUM 6
#define THREAD_NUM 4
#define TEST_CASE_NUM 20

/*
 * 题目描述：
 *   有四个线程1、2、3、4。线程1的功能就是输出1，线程2的功能就是输出2，以此类推.........现在有四个文件ABCD。初始都为空。现要让四个文件呈如下格式：
 *   A：1 2 3 4 1 2....
 *   B：2 3 4 1 2 3....
 *   C：3 4 1 2 3 4....
 *   D：4 1 2 3 4 1....
 * 题目说明参考：http://www.fdlly.com/p/1576456673.html
 *
 */

typedef struct PrintArg PrintArg;
struct PrintArg {
    sem_t *sem;
    sem_t *next_sem;
    int idx;
    int num;
};

void * do_print(void *arg_) {
    PrintArg *arg = arg_;
    int i = 0;
    for (i = 0; i < arg->num; ++i) {
        sem_wait(arg->sem);
        printf("%d ", arg->idx);
        sem_post(arg->next_sem);
    }
}

void sync_print() {
    int k = 0;
    for (k = 0; k < TEST_NUM; ++k) {
        sem_t sems[THREAD_NUM];
        int i = 0;
        for (i = 0; i < THREAD_NUM; i++) {
            sem_init(&sems[i], 0, 0);
        }

        PrintArg args[THREAD_NUM];
        pthread_t threads[THREAD_NUM];
        for (i = 0; i < THREAD_NUM; i++) {
            args[i].sem = &sems[i];
            args[i].next_sem = &sems[(i+1)%THREAD_NUM];
            args[i].idx = i+1;
            args[i].num = TEST_CASE_NUM;

            pthread_create(&threads[i], NULL, do_print, &args[i]);
        }

        sem_post(&sems[k%THREAD_NUM]);

        for (i = 0; i < THREAD_NUM; ++i) {
            pthread_join(threads[i], NULL);
        }
        printf("\n");
    }
}

int main() {
    sync_print();

    return 0;
}
