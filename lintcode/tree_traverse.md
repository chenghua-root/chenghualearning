# 前序

[二叉树的前序遍历](https://www.lintcode.com/problem/binary-tree-preorder-traversal/description)

## 递归

```c++
vector<int> preorderTraversal(TreeNode *root) {
  vector<int> rst;
  preorderDFS(root, rst);
  return rst;
}
void preorderDFS(TreeNode *root, vector<int> &rst) {
  if (NULL == root) return;

  rst.push_back(root->val);
  preorderDFS(root->left, rst);
  preorderDFS(root->right, rst);
}
```

## 非递归

```c++
vector<int> preorderTraversal(TreeNode *root) {
  if (NULL == root) return vector<int>{};

  vector<int> rst;
  stack<TreeNode *> st;

  st.push(root);
  while(!st.empty()) {
    TreeNode * n = st.top(); st.pop();
    rst.push_back(n->val);
    if (n->right) st.push(n->right);
    if (n->left) st.push(n->left);
  }

  return rst;
}
```

# 中序

[二叉树的中序遍历](https://www.lintcode.com/problem/binary-tree-inorder-traversal/description)

## 非递归

```c++
vector<int> inorderTraversal(TreeNode * root) {
  if (NULL == root) return vector<int>{};

  vector<int> rst;
  stack<TreeNode *> st;

  TreeNode * cur = root;
  while(cur) {
    st.push(cur);
    cur = cur->left;
  }

  while(!st.empty()) {
     TreeNode * n = st.top(); st.pop();
     rst.push_back(n->val);
     cur = n->right;
     while(cur) {
       st.push(cur);
       cur = cur->left;
     }
  }

  return rst;
}
```

# 后序

[二叉树的后序遍历](https://www.lintcode.com/problem/binary-tree-postorder-traversal/description)

## 非递归

```c++
vector<int> postorderTraversal(TreeNode * root) {
  if (NULL == root) return vector<int>{};

  vector<int> rst;
  stack<TreeNode *> st;

  TreeNode * node = root;
  while(node) {
      st.push(node);
      node = node->left;
  }

  TreeNode * lastViste = NULL;
  while(!st.empty()) {
    node = st.top();
    if (NULL == node->right || lastViste == node->right) {
      rst.push_back(node->val);
      st.pop();
      lastViste = node;
    } else {
      node = node->right;
      while(node) {
        st.push(node);
        node = node->left;
      }
    }
  }

  return rst;
}
```

# 层次
[二叉树的层次遍历](https://www.lintcode.com/problem/binary-tree-level-order-traversal/description)

```c++
vector<vector<int>> levelOrder(TreeNode * root) {
  if (NULL == root) return vector<vector<int>>{};

  vector<vector<int>> rst;
  vector<TreeNode *> level;

  level.push_back(root);
  while(!level.empty()){
    vector<int> tmp_rst;
    vector<TreeNode *> next_level;
    for (auto it = level.begin(); it != level.end(); it++) {
      tmp_rst.push_back((*it)->val);
      if ((*it)->left) next_level.push_back((*it)->left);
      if ((*it)->right) next_level.push_back((*it)->right);
    }
    rst.push_back(tmp_rst);
    level.clear();
    level = next_level;
  }

  return rst;
}
```

# 构造
[前序遍历和中序遍历树构造二叉树](https://www.lintcode.com/problem/construct-binary-tree-from-preorder-and-inorder-traversal/description)

```c++
TreeNode *buildTree(vector<int> &preorder, vector<int> &inorder) {
    return doBuildTree(preorder, 0, preorder.size(), inorder, 0, inorder.size());
}

TreeNode *doBuildTree(vector<int> &pre, int a, int b, vector<int> &in, int c, int d) {
    // 左闭右开
    if (a >= b || c >= d) {
        return NULL;
    }
    TreeNode *node = new TreeNode(pre[a]);

    int k = c;
    for (; k < d; k++) {
        if (in[k] == pre[a]) {
            break;
        }
    }
    node->left = doBuildTree(pre, a+1, a+k-c+1, in, c, k);
    node->right = doBuildTree(pre, a+k-c+1, b, in, k+1, d);
    return node;
}
```

# 验证二叉查找树
[验证二叉查找树](https://www.lintcode.com/problem/validate-binary-search-tree/description)
## 递归
```
bool isValidBST(TreeNode * root) {
    int low, high;
    return doIsValidBST(root, low, high);
}

bool doIsValidBST(TreeNode * root, int &low, int &high) {
    if (NULL == root) return true;
    
    int a, b;
    if (root->left && root->right) {
        return doIsValidBST(root->left, low, a) && a < root->val
               && doIsValidBST(root->right, b, high) && b > root->val;
    } else if (root->left) {
        high = root->val;
        return doIsValidBST(root->left, low, a) && a < root->val;
    } else if (root->right) {
        low = root->val;
        return doIsValidBST(root->right, b, high) && b > root->val;
    } else {
        low = root->val;
        high = root->val;
        return true;
    }
    
    return false;
}
```
## 非递归
```
bool isValidBST(TreeNode * root) {
    if (NULL == root) return true;
    
    stack<TreeNode*> st;
    TreeNode * node = root;
    while (node) {
        st.push(node);
        node = node->left;
    }
    
    int lastVal = 0;
    bool first = true;
    while (!st.empty()) {
        node = st.top(); st.pop();
        if (node->val <= lastVal && !first) return false;
        else { lastVal = node->val; first = false;}
        
        node = node->right;
        while (node) {
            st.push(node);
            node = node->left;
        }
    }
    
    return true;
}
```
