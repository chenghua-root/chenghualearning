# 介绍
本目录代码用c语言。包含：

- 读写锁、互斥锁、条件变量、信号量的使用，pthread_lock_cond_sem.c
- 利用信号量来同步，pthread_sem_sync.c
- 哈希，hash.c
  - 支持接口的哈希，hash_interface.c
- 堆，heap.c
  - 支持接口的堆heap_interface.c

# c接口的实现
有两种接口的实现：
1. 包含式：
   把用户的数据用成员变量的方式包含在定义好的结构体中，如hash_interface.c的实现
2. 内嵌式：
   把库的结构体用成员变化的方式包含在用户的结构体中，如heap_interface.c的实现
内嵌式更灵活易用，推荐。
