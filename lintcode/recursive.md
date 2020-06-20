https://mp.weixin.qq.com/s/mJ_jZZoak7uhItNgnfmZvQ
## 递归结题步骤
以斐波拉契数列为例。
1. 明确递归函数的功能
   int f(int n) {}。功能为求第n项的值。
2. 找出递归结束条件
   if (n <= 2) return 1; // 结束条件为f(1)=f(2)=1
3. 找出递归关系
   递归关系的目的是缩小递归参数，使其向递归结束条件靠拢
   f(n) = f(n-1)+f(n-2)
## 优化
  递归存在重复计算的问题。需要优化（如：剪枝）
  f(n) = f(n-1) + f(n-2)
  f(n-1) = f(n-2) + f(n-3)
  递归就是自顶向下，上面两个式子中的f(n-2)都会被独立计算。
  解决方法：
    1. 把已经计算的中间结果记录下来，如果已经存在则直接返回
    2. 自底向上递推

## 翻转链表
https://www.lintcode.com/problem/reverse-linked-list/description

```c++
/*
1. 明确函数功能
   ListNode * reverser(ListNode * node); // 返回node的翻转链表
2. 结束条件
  if (NULL == head || NULL == head->next) return head;
3. 递归关系
  ListNode * new_head = reverse(head->next); // 翻转后链表尾节点为head->next
  ListNode * next = head->next; // head->next没有变
  next->next = head; // 把head接在尾节点
  head->next = NULL;
*/
ListNode * reverse(ListNode * head) {
    if (NULL == head || NULL == head->next) return head;

    ListNode *new_head = reverse(head->next);
    head->next->next = head;
    head->next = NULL;

    return new_head;
}
```
## 二叉树拆成链表
https://www.lintcode.com/problem/flatten-binary-tree-to-linked-list/description

```c++
/*
1. 明确函数功能
  void flatten(TreeNode * root); // 把二叉树变成以root为头结点的链表
2. 结束条件
  if (NULL == root) return;
3. 递归关系
  flatten(root->left);
  flatten(root->right);
  TreeNode *right = root->right;
  root->right = root->left;
  root->left = NULL;
  TreeNode *iter = root;
  while (iter && iter->right) iter = iter->right;
  iter->right = right;
*/
void flatten(TreeNode * root) {
    if (NULL == root) return;

    flatten(root->left);
    flatten(root->right);

    TreeNode * right = root->right;
    root->right = root->left;
    root->left = NULL;

    TreeNode * r = root;
    while(r && r->right) r = r->right;
    r->right = right;
}
```

## 平衡二叉树
https://www.lintcode.com/problem/balanced-binary-tree/description

```c++
bool isBalanced(TreeNode * root) {
    int high = 0;
    return doIsBalanced(root, high);
}

bool doIsBalanced(TreeNode * root, int &high) {
    if (NULL == root) {
        high = 0;
        return true;
    }

    int lh = 0, rh = 0;
    bool lb = doIsBalanced(root->left, lh);
    bool rb = doIsBalanced(root->right, rh);

    high = max(lh, rh) + 1;

    return lb && rb && abs(lh - rh) <= 1;
}
```
