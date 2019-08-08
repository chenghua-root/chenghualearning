#include<stdio.h>
#include<stdlib.h>

#define OK 0
#define ERR -1

typedef struct HeapNode HeapNode;
struct HeapNode {
    struct HeapNode *parent;
    struct HeapNode *left;
    struct HeapNode *right;
    int val;
};

typedef int(*heapCmp)(int a, int b);

typedef struct Heap Heap;
struct Heap {
    HeapNode * root;
    int cnt;
    heapCmp cmp;
};

int heapCmpDo(int a, int b) {
    if(a < b) return -1;
    else if (a > b) return 1;
    else return 0;
}

Heap * heapCreate(heapCmp cmp) {
    Heap * h = malloc(sizeof(Heap));
    if (!h) return NULL;

    h->root = NULL;
    h->cnt = 0;
    h->cmp = cmp;
    return h;
}

int heapGetOffset(int parent_cnt) {
    int offset = 1;
    parent_cnt >>= 1;
    while (offset <= parent_cnt) offset <<= 1;
    return offset >> 1;
}

void heapAdjust(Heap *heap, HeapNode * node) {
    while (node) {
        HeapNode * parent = node->parent;
        HeapNode * pp = parent->parent;
        HeapNode * node_left = node->left;
        HeapNode * node_right = node->right;

        if (parent->left == node) {
            node->left = parent;
            node->right = parent->right;
        } else {
            node->right = parent;
            node->left = parent->left;
        }
        node->parent = pp;
        if (pp && pp->left == parent) {
            pp->left = node;
        } else if (pp && pp->right == parent) {
            pp->right = node;
        }

        parent->parent = node;
        parent->left = node_left;
        parent->right = node_right;
    }
}

int heapAdd(Heap *heap, int val) {
    if (!heap) return ERR;

    HeapNode *node = malloc(sizeof(HeapNode));
    if (!node) return ERR;

    node->parent = NULL;
    node->left = NULL;
    node->right = NULL;
    node->val = val;

    if (!heap->root) {
        heap->root = node;
        heap->cnt++;
        return OK;
    }

    int parent_cnt = (heap->cnt+1)/2;
    int child = (heap->cnt+1)%2;
    int parent_offset = heapGetOffset(parent_cnt);
    HeapNode *now = heap->root;
    printf("cnt=%d parent_cnt=%d parent_offset=%d\n", heap->cnt, parent_cnt, parent_offset);
    while (parent_offset > 0) {
        if (parent_cnt & parent_offset) {
            printf("go right\n");
            now = now->right;
        } else {
            printf("go left\n");
            now = now->left;
        }
        parent_offset >>= 1;
    }
    if (!child) {
        now->left = node;
        printf("hang left\n");
    } else {
        now->right = node;
        printf("hang right\n");
    }

    node->parent = now;
    heap->cnt++;

    return OK;
}


void pre_traverse(HeapNode * root) {
    if (NULL == root) return;

    printf("%d ", root->val+1);
    pre_traverse(root->left);
    pre_traverse(root->right);
}

int main() {
    int i = 0;
    Heap *heap = heapCreate(heapCmpDo);
    for (i = 0; i < 13; ++i) {
        heapAdd(heap, i);
        printf("\n");
    }
    pre_traverse(heap->root);
    printf("\n");
}
