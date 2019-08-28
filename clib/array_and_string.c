#include <stdio.h>
#include <string.h>

int main() {
    /*
     * 数组
     * int A[4];
     * 数组名和指针不能混用的地方：
     *     sizeof(A): 表示数组A的整个空间
     *     &A：表示指向int[4]类型的指针
     */
    int A[] = {1, 2, 3, 4, 5};
    int *pA = A;
    printf("sizeof(A[])=%d, sizeof(pA)=%d, sizeof(*pA)=%d\n", sizeof(A), sizeof(pA), sizeof(*pA));
    // sizeof(A[])=20, sizeof(pA)=8, sizeof(*pA)=4
    int i = 0;
    for (i = 0; i < sizeof(A)/sizeof(int); i++) {
        printf("%d ", A[i]);
        printf("%d ", *(A+i));
        printf("%d\n", *(pA+i));
    }
    printf("\n==========================\n\n");

    /*
     * 二维数组
     *
     */
    int AA[][3] = {{1, 2, 3}, {4, 5, 6}};
    int (*pAA)[3] = AA;
    printf("sizeof(AA)=%d, sizeof(AA[0])=%d\n", sizeof(AA), sizeof(AA[0]));
    // sizeof(AA)=24, sizeof(AA[0])=12

    printf("*AA=%d, **AA=%d, *AA[1]=%d, *(*(AA+1)+1)=%d\n", *AA, **AA, *AA[1], *(*(AA+1)+1));
    // *AA=-605870176, **AA=1, *AA[1]=4, *(*(AA+1)+1)=5

    printf("*pAA=%d, **pAA=%d, **(pAA+1)=%d, *(*(pAA+1)+1)=%d\n", *pAA, **pAA, **(pAA+1), *(*(pAA+1)+1));
    // *pAA=-605870176, **pAA=1, **(pAA+1)=4, *(*(pAA+1)+1)=5

    printf("\n==========================\n\n");

    /*
     * 字符串
     * C没有string类型
     */
    char s1[] = "abcdefghij";
    char *s2 = "abcdefghij";
    printf("s[]=%s, *s=%s\n", s1, s2);
    // s[]=abcdefghij, *s=abcdefghij

    printf("sizeof(s[])=%d, sizeof(s)=%d\n", sizeof(s1), sizeof(s2));
    // sizeof(s[])=11, sizeof(s)=8

    printf("\n==========================\n\n");

    /*
     * 字符串连接
     * char *strcat(char *dest, const char *src);
     * The  strcat()  function  appends  the  src  string to the dest string,
     * overwriting the null byte ('\0') at the end of dest, and then adds a terminating null byte.
     * The strings may not overlap, and the dest string must have ENOUGH space for the result.
     */
    char c1[10] = "abc";
    char *c2 = "def";
    strcat(c1, c2);
    printf("c1+c2=%s\n", c1);

    /*
     * 字符串数组
     */
    char *AS[] = {"abc", "1234", "hello, world!"};
    printf("sizeof(char*AS[]):%d\n", sizeof(AS));
    for (i = 0; i < sizeof(AS)/sizeof(char*); ++i) {
        printf("array elem:%s\n", *(AS+i));
        printf("array elem:%s\n", AS[i]);
    }

    printf("\n==========================\n");
}
