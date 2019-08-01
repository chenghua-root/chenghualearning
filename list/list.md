## 翻转链表
[翻转链表](https://www.lintcode.com/problem/reverse-linked-list/description)
```
ListNode * reverse(ListNode * head) {
    if (NULL == head || NULL == head->next) return head;
    
    ListNode * now = head;
    ListNode * pre = now->next;
    now->next = NULL;
    while(pre) {
        ListNode *tmp = pre->next;
        pre->next = now;
        now = pre;
        pre = tmp;
    }
    return now;
}
```

## 合并K个排序链表
[合并K个排序链表](https://www.lintcode.com/problem/merge-k-sorted-lists/description)
### 非递归：利用堆栈
```
ListNode *mergeKLists(vector<ListNode *> &lists) {
    struct cmp {
         bool operator() (ListNode *a, ListNode *b) {
            return a->val > b->val;
         }
    };
    priority_queue<ListNode*, vector<ListNode*>, cmp> heap;
    
    for (auto iter = lists.begin(); iter < lists.end(); ++iter) {
        if (*iter) heap.push(*iter);
    }

    ListNode *head = NULL, *now = NULL;
    while(!heap.empty()) {
        ListNode *tmp = heap.top(); heap.pop();
        if (!head) {
            head = tmp;
            now = tmp;
        } else {
            now->next = tmp;
            now = tmp;
        }
        if (tmp->next) heap.push(tmp->next);
    }

    return head;
}
```
### 递归
```
ListNode *mergeKLists(vector<ListNode *> &lists) {
    return merge(lists, 0, lists.size() -1);
}

ListNode *merge(vector<ListNode*> &lists, int l, int r) {
    if (l > r) return NULL;
    if (l == r) return lists[l];
    
    ListNode *lNode = merge(lists, l, (l+r)/2);
    ListNode *rNode = merge(lists, (l+r)/2 + 1, r);
    
    if (!lNode) return rNode;
    if (!rNode) return lNode;
    
    ListNode *head = NULL, *cur = NULL;
    if (lNode->val <= rNode->val) {
        head = cur = lNode;
        lNode = lNode->next;
    } else {
        head = cur = rNode;
        rNode = rNode->next;
    }
    
    while(lNode && rNode) {
        if (lNode->val <= rNode->val) {
            cur->next = lNode;
            cur = cur->next;
            lNode = lNode->next;
        } else {
            cur->next = lNode;
            cur = cur->next;
            rNode = rNode->next;
        }
    }
    
    if (lNode) cur->next = lNode;
    if (rNode) cur->next = rNode;
    
    return head;
}
```
