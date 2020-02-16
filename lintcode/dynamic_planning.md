## 动态规划三步骤
1. 定义数组元素的含义
2. 确定数组元素之间的关系式（状态转义方程）
3. 找出初始值

## 最长上升子序列
[76. 最长上升子序列](https://www.lintcode.com/problem/longest-increasing-subsequence/description)
1. dp[n]，dp[i]表示**以第i个数字结尾**的最长上升子序列的长度。
2. 对于每个数字，枚举前面所有小于自己的数字j，dp[i] = max(dp[i], dp[j]+1).
3. dp[i]初始值为1
```c++
int longestIncreasingSubsequence(vector<int> &nums) {
    if (nums.empty()) return 0;
    int rst = 0;

    vector<int> dp(nums.size(), 1);
    for (int i = 1; i < nums.size(); ++i) {
        for (int j = 0; j < i; ++j) {
            if (nums[j] < nums[i]) dp[i] = max(dp[i], dp[j]+1);
            rst = max(rst, dp[i]);
        }
    }

    return rst;
}
```

## 最长公共子序列
[77. 最长公共子序列](https://www.lintcode.com/problem/longest-common-subsequence/description)
1. dp[m+1][n+1]，dp[i][j]表示到A[i], B[j]为止的最长公共子序列
2. if A[i]!=B[j], dp[i][j] = max(dp[i-1][j], dp[i][j-1])
   if A[i]=B[j], dp[i][j] = max(dp[i-1][j], dp[i][j-1], dp[i][j])
3. 初始值为0

```c++
    int longestCommonSubsequence(string &A, string &B) {
        if (A.empty() || B.empty()) return 0;

        vector<vector<int>> dp(A.size()+1, vector<int>(B.size()+1, 0));

        for (int i = 1; i <= A.size(); i++) {
            for (int j = 1; j <= B.size(); j++) {
                dp[i][j] = max(dp[i-1][j], dp[i][j-1]);
                if (A[i-1] == B[j-1]) {
                    dp[i][j] = max(dp[i][j], dp[i-1][j-1]+1);
                }

            }
        }

        return dp[A.size()][B.size()];
    }
```

## 最长公共子串
[79. 最长公共子串](https://www.lintcode.com/problem/longest-common-substring/description)
1. dp[m+1][n+1]，dp[i][j]表示**包含A[i],B[j]**最长的公共子串
2. if A[i] == B[j]; f[i][j] = f[i-1][j-1]+1; rst = max(rst, f[i][j])
   else f[i][j] = 0
3. 初始值为0

```c++
    int longestCommonSubstring(string &A, string &B) {
        if (A.empty() || B.empty()) return 0;

        vector<vector<int>> dp(A.size()+1, vector<int>(B.size()+1, 0));
        int ans = 0;

        for (int i = 1; i <= A.size(); i++) {
            for (int j = 1; j <= B.size(); j++) {
                if (A[i-1] == B[j-1]) {
                    dp[i][j] = dp[i-1][j-1]+1;
                    ans = max(ans, dp[i][j]);
                }
            }
        }

        return ans;
    }
```

## S中包含T的不同子序列个数
118. https://www.lintcode.com/problem/distinct-subsequences/description
**子序列字符串是原始字符串删除一些(或零个)字符之后得到的字符串**
1. dp[T.size()+1][S.size()+1], dp[i][j]表示S[0, j]包含T[0, i]的不同子序列个数
2. if T[i] != S[j]; f[i][j] = f[i][j-1]
   if T[i] == S[j]; f[i][j] = f[i][j-1] + f[i-1][j-1]
3. dp[0][j] = 1 // T为空时，S包含T的子序列个数为1

```c++
int numDistinct(string &S, string &T) {
        // write your code here
        if (T.empty()) return 1;

        std::vector<vector<int>> dp(T.size()+1, vector<int>(S.size()+1, 0));
        for (int j = 0; j <= S.size(); ++j) {
            dp[0][j] = 1; // T为空时，S包含T的子序列个数为1
        }

        for (int i = 1; i <= T.size(); ++i) {
            for (int j = i; j <= S.size(); ++j) {
                dp[i][j] = dp[i][j-1];
                if (T[i-1] == S[j-1]) {
                    dp[i][j] += dp[i-1][j-1];
                }
            }
        }
        return dp[T.size()][S.size()];
    }
```

## 编辑距离
119. https://www.lintcode.com/problem/edit-distance/description
1. dp[m+1][n+1], dp[i][j]表示word1[0,i-1]到word2[0, j-1]的编辑距离
2. if word1[i-1] == word2[j-1]; dp[i][j] = dp[i-1][j-1]
   else dp[i][j] = min(dp[i-1][j-1], min(dp[i-1][j], dp[i][j-1]))+1;
3. dp[i][0] = i; dp[0][j] = j;

```c++
    int minDistance(string &word1, string &word2) {
        // write your code here
        std::vector<vector<int>> dp(word1.size()+1, vector<int>(word2.size()+1, 0));
        for (int i = 0; i <= word1.size(); i++) {
            dp[i][0] = i;
        }
        for (int j = 0; j <= word2.size(); j++) {
            dp[0][j] = j;
        }

        for (int i = 1; i <= word1.size(); i++) {
            for (int j = 1; j <= word2.size(); j++) {
                if (word1[i-1] == word2[j-1]) {
                    dp[i][j] = dp[i-1][j-1];
                } else {
                    dp[i][j] = min(dp[i-1][j-1], min(dp[i-1][j], dp[i][j-1]))+1;
                }
            }
        }

        return dp[word1.size()][word2.size()];
    }
}
```
