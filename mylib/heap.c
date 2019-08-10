#include<stdio.h>
#include<stdlib.h>
#include<stdbool.h>

#define OK 0
#define ERR -1
#define ERR_EMPTY -2

// 完全二叉树添加节点http://www.voidcn.com/article/p-qgerawwv-btg.html

int assert(int exp, int val) {
    if (exp != val) {
        printf("exp=%d, get=%d", exp, val);
        exit(-1);
    }
    return 0;
}

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
        heap->cnt++;
        return OK;
    }

    int parent_cnt = (heap->cnt+1)/2;
    int child = (heap->cnt+1)%2;
    int parent_offset = heapGetOffset(parent_cnt);
    HeapNode *now = heap->root;
    while (parent_offset > 0) {
        if (parent_cnt & parent_offset) {
            now = now->right;
        } else {
            now = now->left;
        }
        parent_offset >>= 1;
    }
    if (!child) {
        now->left = node;
    } else {
        now->right = node;
    }

    node->parent = now;
    heap->cnt++;

    heapAdjustUp(heap, node);

    return OK;
}

/**********************************************************************/

void heapAdjustDown(Heap *heap) {
    if (!heap || !heap->root) return;

    HeapNode * node = heap->root;
    while (node) {
        HeapNode * node_p = node->parent;
        HeapNode * node_left = node->left;
        HeapNode * node_right  = node->right;

        int cmp_left = 0;
        int cmp_right = 0;
        int cmp_left_right = 0;

        if (node_left) {
            cmp_left = heap->cmp(node->val, node_left->val);
        }
        if (node_right) {
            cmp_right = heap->cmp(node->val, node_right->val);
        }
        if (node_left && node_right) {
            cmp_left_right = heap->cmp(node_left->val, node_right->val);
        }

        bool ex_left = false;
        bool ex_right = false;

        // 默认大顶堆
        if (node_left && node_right) {
            if (cmp_left >= 0 && cmp_right >= 0) break;
            else if (cmp_left >= 0)  {
                // 交换右子树
                ex_right = true;
            } else if (cmp_right >= 0)  {
                // 交换左子树
                ex_left = true;
            } else if (cmp_left_right >= 0) {
                // 交换左子树
                ex_left = true;
            } else if (cmp_left_right < 0) {
                // 交换右子树
                ex_right = true;
            }
        } else if (node_left && cmp_left < 0)  {
            ex_left = true;
        } else if (node_right && cmp_right < 0)  {
            ex_right = true;
        }

        if (!ex_left && !ex_right) break;

        if (ex_left) {
            node->parent = node_left;
            node->left = node_left->left;
            node->right = node_left->right;
            if (node_p && node_p->left == node) node_p->left = node_left;
            else if (node_p && node_p->right == node) node_p->right = node_left;

            if (node_left->left) node_left->left->parent = node;
            if (node_left->right) node_left->right->parent = node;

            node_left->parent = node_p;
            node_left->left = node;
            node_left->right = node_right;

            node_right->parent = node_left;

            if (heap->root == node) heap->root = node_left;
        } else if (ex_right) {
            node->parent = node_right;
            node->left = node_right->left;
            node->right = node_right->right;
            if (node_p && node_p->left == node) node_p->left = node_right;
            else if (node_p && node_p->right == node) node_p->right = node_right;

            if (node_right->left) node_right->left->parent = node;
            if (node_right->right) node_right->right->parent = node;

            node_right->parent = node_p;
            node_right->left = node_left;
            node_right->right = node;

            node_left->parent = node_right;

            if (heap->root == node) heap->root = node_right;
        }
    }
}

void pre_traverse(HeapNode * root) {
    if (NULL == root) return;

    printf("%d ", root->val);
    pre_traverse(root->left);
    pre_traverse(root->right);
}


int heapGet(Heap *heap, int *val) {
    if (!heap || !heap->root) return ERR_EMPTY;
    HeapNode * root = heap->root;

    int offset = heapGetOffset(heap->cnt);
    HeapNode *last = heap->root;

    while (offset > 0) {
        if (heap->cnt & offset) {
            last = last->right;
        } else {
            last = last->left;
        }
        offset >>= 1;
    }

    HeapNode * last_parent = last->parent;
    if (last->parent && last->parent->left == last) last->parent->left = NULL;
    else if (last->parent && last->parent->right == last) last->parent->right = NULL;


    last->parent = NULL;
    last->left = root->left;
    last->right = root->right;

    if (root->left) root->left->parent = last;
    if (root->right) root->right->parent = last;

    if (root == last) heap->root = NULL;
    else heap->root = last;

    root->left = NULL;
    root->right = NULL;
    *val = root->val;
    heap->cnt--;
    free(root);

    heapAdjustDown(heap);

    return OK;
}

int main() {
    int i = 0;
    int v = 0;
    Heap *heap = heapCreate(heapCmpDo);
    int NUM = 10;
    for (i = 1; i <= NUM; ++i) {
        heapAdd(heap, i);
    }

    printf("pre traverse cnt=%d:\n", NUM);
    pre_traverse(heap->root);
    printf("\n");

    for (i = NUM; i >= 1; --i) {
        heapGet(heap, &v);
        assert(i, v);
    }
    assert(ERR_EMPTY, heapGet(heap, &v));
}
