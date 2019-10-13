# 二叉树深度

[二叉树的最大深度](https://www.lintcode.com/problem/maximum-depth-of-binary-tree/description)

```c++
int maxDepth(TreeNode * root) {
    if (NULL == root) return 0;
    return max(maxDepth(root->left), maxDepth(root->right)) + 1;
}
```
# 二叉树最小深度
[二叉树最小深度](https://www.lintcode.com/problem/minimum-depth-of-binary-tree/description)
```
int minDepth(TreeNode * root) {
    return doMinDepth(root, 0);
}

int doMinDepth(TreeNode * root, int depth) {
    if (NULL == root) return depth;
    
    if (root->left && root->right) {
        return min(doMinDepth(root->left, depth+1), doMinDepth(root->right, depth+1));
    } else if (root->left) {
        return doMinDepth(root->left, depth+1);
    } else if (root->right) {
        return doMinDepth(root->right, depth+1);
    } else {
        return depth+1;
    }
}
```

# 最大路径

[二叉树中的最大路径和](https://www.lintcode.com/problem/binary-tree-maximum-path-sum/description)

```c++
int maxPathSum(TreeNode * root) {
    if (NULL == root) return 0;
    
    int maxSum = INT_MIN;
    maxPath(root, maxSum);
    
    return maxSum;
}

int maxPath(TreeNode * root, int &maxSum) {
    if (NULL == root) return 0;
    
    int l = maxPath(root->left, maxSum);
    int r = maxPath(root->right, maxSum);
    
    int sum = root->val;
    if (l > 0) sum += l;
    if (r > 0) sum += r;
    
    maxSum = max(maxSum, sum);
    
    return max(root->val, root->val+max(l, r));
}
```

# 二叉树拆成链表
[将二叉树拆成链表](https://www.lintcode.com/problem/flatten-binary-tree-to-linked-list/description)

先序遍历，用二叉树的 right 指针，来表示链表中的 next 指针。

## 递归1

```c++
void flatten(TreeNode * root) {
    // write your code here
    if (NULL == root) return;
    
    TreeNode * right = root->right;
    root->right = root->left;
    root->left = NULL;
    flatten(root->right);
    
    TreeNode * r = root;
    while(r && r->right) r = r->right;
    
    r->right = right;
    flatten(r->right);
}
```

## 递归2

```c++
void flatten(TreeNode * root) {
    stack<TreeNode *> st; // 利用堆栈来避免递归1中的while遍历
    flattenDFS(root, st);
}

void flattenDFS(TreeNode * root, stack<TreeNode *> &st) {
    if (NULL == root) return;
    
    TreeNode * r = root->right;
    root->right = root->left;
    root->left = NULL;
    if (r) st.push(r);
    
    if (root->right) {
        return flattenDFS(root->right, st);
    }
    if (NULL == root->right && !st.empty()) {
        root->right = st.top(); st.pop();
        return flattenDFS(root->right, st);
    }
}
```

##  利用堆栈

```c++
void flatten(TreeNode * root) {
    if (NULL == root) return;
    
    std::stack<TreeNode*> st;
    st.push(root);
    TreeNode * cur = NULL;
    while(!st.empty()) {
        TreeNode * n = st.top(); st.pop();
        if (!cur) cur = n;
        else {
            cur->left = NULL;
            cur->right = n;
            cur = cur->right;
        }
        if (n->right) st.push(n->right);
        if (n->left) st.push(n->left);
    }
}
```
