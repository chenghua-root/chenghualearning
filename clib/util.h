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

 #define min(a, b)             \
   ({ __typeof__ (a) _a = (a); \
   __typeof__ (b) _b = (b);    \
   _a > _b ? _b : _a; })

 #define max(a, b)              \
   ({ __typeof__ (a) _a = (a); \
   __typeof__ (b) _b = (b);    \
   _a > _b ? _a : _b; })

#define barrier() __asm__ __volatile__("": : :"memory")

#ifdef CONFIG_X86_32
#define mb() alternative("lock; addl $0,0(%%esp)", "mfence", X86_FEATURE_XMM2)
#define rmb() alternative("lock; addl $0,0(%%esp)", "lfence", X86_FEATURE_XMM2)
#define wmb() alternative("lock; addl $0,0(%%esp)", "sfence", X86_FEATURE_XMM)
#else
#define mb()    asm volatile("mfence":::"memory")
#define rmb()   asm volatile("lfence":::"memory")
#define wmb()   asm volatile("sfence" ::: "memory")
#endif

#ifdef CONFIG_SMP
#define smp_mb() mb()
#define smp_rmb() rmb()
#define smp_wmb() wmb()
#define smp_read_barrier_depends() read_barrier_depends()
#define set_mb(var, value) do { (void) xchg(&var, value); } while (0)
#else
#define smp_mb() barrier()
#define smp_rmb() barrier()
#define smp_wmb() barrier()
#define smp_read_barrier_depends() do { } while(0)
#define set_mb(var, value) do { var = value; barrier(); } while (0)
#endif
