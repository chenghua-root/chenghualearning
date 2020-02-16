#include <pthread.h>
#include <stdio.h>
#include <unistd.h>

#include "cond.h"
#include "list.h"

#define offsetof(tp, field) ( (size_t)&((tp*)NULL)->field )
#define by_offset(p, offset) ( &((char*)(p))[offset] )
#define owner(p, tp, field) (tp*)by_offset(p, -offsetof(tp, field))

/***************task********************/

typedef void (*ThreadTaskWorkFunc)(void *);

typedef struct ThreadTask ThreadTask;
struct ThreadTask {
  ListNode node_;
  int val;

  ThreadTaskWorkFunc work_func_;
  void *work_func_arg_;
};

typedef struct ThreadQueue ThreadQueue;
struct ThreadQueue {
  pthread_t pid;
  Cond cond_;

  pthread_spinlock_t lock_;
  List tasks;
};

/*
void *thread_queue_work_func(void *arg) {
  ThreadQueue *tq = arg;
  ListNode *node = NULL;
  ThreadTask *task = NULL;

  do {
    do {
      if (NULL != node) {
        task = owner(node, ThreadTask, node_);
        task->work_func_(task->work_func_arg_);
      }

      pthread_spin_lock(&tq->lock_);

      node = list_remove_head(&tq->tasks);

      pthread_spin_unlock(&tq->lock_);

    } while(NULL != node);

    cond_wait(&tq->cond_);
  } while (1);
}
*/

void *thread_queue_work_func(void *arg) {
  ThreadQueue *tq = arg;
  ListNode *node = NULL;
  ThreadTask *task = NULL;

  do {
    do {
      pthread_spin_lock(&tq->lock_);
      node = list_remove_head(&tq->tasks);
      pthread_spin_unlock(&tq->lock_);

      if (NULL == node) {
        break;
      } else {
        task = owner(node, ThreadTask, node_);
        task->work_func_(task->work_func_arg_);
      }
    } while(1);

    cond_wait(&tq->cond_);
  } while (1);
}
/*
*/

int thread_queue_init(ThreadQueue *tq) {
  int ret = 0;

  ret = cond_init(&tq->cond_);
  if (0 != ret) {
    return ret;
  }

  pthread_spin_init(&tq->lock_, PTHREAD_PROCESS_PRIVATE);

  list_init(&tq->tasks);

  ret = pthread_create(&tq->pid, NULL, thread_queue_work_func, tq);
  if (0 != ret) {
    return ret;
  }

  return 0;
}

int thread_queue_push(ThreadQueue *tq, ThreadTask *task) {
    if (NULL == tq || NULL == task || NULL == task->work_func_) {
      return -1;
    }

    pthread_spin_lock(&tq->lock_);
    list_add_tail(&tq->tasks, &task->node_);
    pthread_spin_unlock(&tq->lock_);

    cond_wakeup(&tq->cond_);

    return 0;
}

/***************main********************/

void PrintInt(void *arg) {
  int v = *(int*)arg;
  printf("int v=%d\n", v);
}

void PrintString(void *arg) {
  char *str = arg;
  printf("string v=%s\n", str);
}

int main() {
    List list;
    list_init(&list);

    ThreadTask task0 = {.val = 0};
    ThreadTask task1 = {.val = 1};
    ThreadTask task2 = {.val = 2};
    ThreadTask task3 = {.val = 3};

    // 2, 1, 0, 3
    list_add(&list, &task0.node_);
    list_add(&list, &task1.node_);
    list_add(&list, &task2.node_);
    list_add_tail(&list, &task3.node_);

    // 2, 1, 3
    list_del(&task0.node_);

    // 1, 3
    list_remove_head(&list);

    ListNode *iter_node = NULL;
    list_for_each(iter_node, &list) {
      ThreadTask *task = owner(iter_node, ThreadTask, node_);
      printf("%d ", task->val);
    }
    printf("\n");

    /***********/

    ThreadQueue tq;
    thread_queue_init(&tq);

    int t0_arg = 1;
    ThreadTask t0 = {
      .work_func_ = PrintInt,
      .work_func_arg_ = &t0_arg,
    };
    char t1_arg[] = "abc";
    ThreadTask t1 = {
      .work_func_ = PrintString,
      .work_func_arg_ = t1_arg,
    };

    thread_queue_push(&tq, &t0);
    thread_queue_push(&tq, &t1);

    do {
      pthread_spin_lock(&tq.lock_);
      if (list_empty(&tq.tasks)) {
        break;
      }
      pthread_spin_unlock(&tq.lock_);
    } while(1);

    return 0;
}
