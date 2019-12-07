# 下一个排列
199. 下一个排列 https://www.lintcode.com/problem/next-permutation-ii/description

 1, 3, 6, 5, 4, 2 => 1, 4, 2, 3, 5, 6
  * 从后往前找到第一个降序的index（元素3）
  * 从后往前找到第一个比index元素大的biggerIndex（元素4）
  * 交换index和biggerIndex元素，reverse(index+1…)（交换元素3和元素4，并reverse[6, 5, 3, 2]）

```c++
void reverse(vector<int> &nums, int begin, int end) {
        for (int i = begin, j = end; i < j; ++i, --j) {
            swap(nums[i], nums[j]);
        }
    }

    void nextPermutation(vector<int> &nums) {
        // 1. from the back forword find the first decrease index
        int index = -1;
        for (int i = nums.size() - 2; i >= 0; --i) {
            if (nums[i] < nums[i+1]) {
                index = i;
                break;
            }
        }

        if (-1 == index) {
            reverse(nums, 0, nums.size()-1);
            return;
        }

        // 2. from the back forword find the first bigger index
        int biggerIndex = nums.size()-1;
        for (int i = nums.size()-1; i > index; --i) {
            if (nums[i] > nums[index]) {
                biggerIndex = i;
                break;
            }
        }

        // 3. swap the elem, reverse the [index+1, ...]
        swap(nums[index], nums[biggerIndex]);
        reverse(nums, index+1, nums.size()-1);

        return;
    }
```

# 组合
152. 组合 https://www.lintcode.com/problem/combinations/description

[1, 2, ..., n]中包含k个数字的所有组合

```c++
vector<vector<int>> combine(int n, int k) {
        vector<vector<int>> rst;
        vector<int> one;
        recursive(1, n, k, one, rst);
        return rst;
    }

    void recursive(int begin, int end, int cnt, vector<int> &one, vector<vector<int>> &rst) {
        if (0 == cnt) {
            rst.push_back(one);
            return;
        }
        for (int i = begin; i <= end; ++i) {
            one.push_back(i);
            recursive(i+1, end, cnt-1, one, rst);
            one.pop_back();
        }
        return;
    }
```


# 数字组合
135. 数字组合 https://www.lintcode.com/problem/combination-sum/description

```c++
    vector<vector<int>> combinationSum(vector<int> &candidates, int target) {
        sort(candidates.begin(), candidates.end());
        vector<vector<int>> rst;
        vector<int> one;
        recursive(candidates, 0, candidates.size()-1, target, one, 0, rst);
        return rst;
    }

    void recursive(vector<int> &candidates, int begin, int end, int target, vector<int> &one, int sum, vector<vector<int>> &rst) {
        if (target == sum) {
            rst.push_back(one);
            return;
        }
        if (sum > target) return;

        for (int i = begin; i <= end; ++i) {
            // 跳过候选列表中的重复数字，以避免出现重复的组合
            if (i > begin && candidates[i] == candidates[i-1]) continue;
            one.push_back(candidates[i]);
            /*
             * recursive(i, ...)，表示每个数字在一次组合中可以被重复使用
             * recursive(i+1, ...)，表示每个数字在一次组合中只能被使用一次
             */
            recursive(candidates, i, end, target, one, sum+candidates[i], rst);
            one.pop_back();
        }
        return;
    }
```

