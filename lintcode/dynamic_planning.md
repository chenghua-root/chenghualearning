## 最长上升子序列
[76. 最长上升子序列](https://www.lintcode.com/problem/longest-increasing-subsequence/description)
长度为n的数组dp，dp[i]表示**以第i个数字结尾**的最长上升子序列的长度。
对于每个数字，枚举前面所有小于自己的数字j，dp[i] = max(dp[i], dp[j]+1).
dp[i]初始值为1
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
dp[i][j]表示到A[i], B[j]为止的最长公共子序列
if A[i]!=B[j], dp[i][j] = max(dp[i-1][j], dp[i][j-1])
if A[i]=B[j], dp[i][j] = max(dp[i-1][j], dp[i][j-1], dp[i][j])

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
dp[i][j]表示**包含A[i],B[j]**最长的公共子串
if A[i] == B[j]; f[i][j] = f[i-1][j-1]+1; rst = max(rst, f[i][j])
else f[i][j] = 0

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
if T[i] != S[j]; f[i][j] = f[i][j-1]
if T[i] == S[j]; f[i][j] = f[i][j-1] + f[i-1][j-1]

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
