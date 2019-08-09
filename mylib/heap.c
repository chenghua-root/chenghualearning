#include<stdio.h>
#include<stdlib.h>

#define OK 0
#define ERR -1
#define ERR_EMPTY -2

// http://www.voidcn.com/article/p-qgerawwv-btg.html

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

/*
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
*/

void heapAdjustUp(Heap *heap, HeapNode * node) {
    while (node && node->parent) {
        HeapNode *p = node->parent;
        HeapNode *pp = p->parent;

        // if (p->val >= node->val) break;
        if (heap->cmp(p->val, node->val) >= 0) break;

        HeapNode *node_left = node->left;
        HeapNode *node_right = node->right;

        HeapNode *p_left = p->left;
        HeapNode *p_right = p->right;

        node->parent = pp;
        if (pp && pp->left == p) {
            pp->left = node;
        } else if (pp && pp->right == p) {
            pp->right = node;
        }

        if (p->left == node) {
            node->right = p_right;
            if (p_right) p_right->parent = node;
            node->left = p;
        } else if (p->right == node) {
            node->left = p_left;
            if (p_left) p_left->parent = node;
            node->right = p;
        }
        p->parent = node;

        p->left = node_left;
        p->right = node_right;

        if (node_left) node_left->parent = p;
        if (node_right) node_right->parent = p;
    }
    if (node && !node->parent) heap->root = node;
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
        printf("cnt=%d cur=%d\n", heap->cnt, heap->cnt+1);
        printf("hang root\n");
        heap->cnt++;
        return OK;
    }

    int parent_cnt = (heap->cnt+1)/2;
    int child = (heap->cnt+1)%2;
    int parent_offset = heapGetOffset(parent_cnt);
    HeapNode *now = heap->root;
    printf("cnt=%d cur=%d parent_cnt=%d parent_offset=%d\n", heap->cnt, heap->cnt+1, parent_cnt, parent_offset);
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

    heapAdjustUp(heap, node);

    return OK;
}

/*
void heapAdjustDown(Heap *heap) {
    if (!heap || !heap->root) return;

    HeapNode * node = heap->root;
    while (node) {
        HeapNode * node_left = node->left;
        HeapNode * node_right  = node->right;

        if (node_left && node_right) {
        }
    }
}

int heapGet(Heap *heap, int *val) {
    if (!heap || heap->root) return ERR_EMPTY;
    HeapNode * root = heap->root;

    int offset = heapGetOffset(heap->cnt);
    HeapNode *last = heap->root;

    while (offset > 0) {
        if (heap->cnt & offset) {
            printf("go right\n");
            last = last->right;
        } else {
            printf("go left\n");
            last = last->left;
        }
        offset >>= 1;
    }

    HeapNode * last_parent = last->parent;
    if (last->parent && last->parent->left == last->parent) last->parent->left = NULL;
    if (last->parent && last->parent->right == last->parent) last->parent->right = NULL;

    last->parent = NULL;
    last->left = root->left;
    last->right = root->right;

    if (root->left) root->left->parent = last;
    if (root->right) root->right->parent = last;

    heap->root = last;

    root->left = NULL;
    root->right = NULL;
    *val = root->val;
    heap->cnt--;
    free(root);

    return OK;
}
*/

void pre_traverse(HeapNode * root) {
    if (NULL == root) return;

    printf("%d ", root->val);
    pre_traverse(root->left);
    pre_traverse(root->right);
}

int main() {
    int i = 0;
    Heap *heap = heapCreate(heapCmpDo);
    for (i = 1; i < 10; ++i) {
        heapAdd(heap, i);
        heapAdd(heap, i);
    pre_traverse(heap->root);
    printf("\n");
    printf("\n");
    }
}
