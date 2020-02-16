## 第k大元素
[第k大元素](https://www.lintcode.com/problem/kth-largest-element/description)
**partition** O(n)
```c++
int kthLargestElement(int n, vector<int> &nums) {
    if (nums.empty() || n > nums.size())  return -1;
    int idx = nums.size() - n;
    int l = 0, r = nums.size() - 1;
    while(1) {
        int p = partition(nums, l, r);
        if (p == idx) return nums[p];
        else if (p < idx) l = p + 1;
        else r = p - 1;
    }
}
int partition(vector<int> &nums, int l, int r) { // 左闭右闭
    int pivot = nums[l];
    while (l < r) {
        while(l < r && nums[r] >= pivot) r--;
        nums[l] = nums[r];
        while(l < r && nums[l] <= pivot) l++;
        nums[r] = nums[l];
    }
    nums[l] = pivot;
    return l;
}
```
## 合并有序数组
[合并有序数组](https://www.lintcode.com/problem/merge-two-sorted-arrays/description)
```c++
vector<int> mergeSortedArray(vector<int> &A, vector<int> &B) {
    vector<int> R;
    auto ita = A.begin();
    auto itb = B.begin();

    while (ita != A.end() && itb != B.end()) {
        if (*ita < *itb) R.push_back(*ita++);
        else R.push_back(*itb++);
    }
    if (ita == A.end()) R.insert(R.end(), itb, B.end());
    else R.insert(R.end(), ita, A.end());

    return R;
}
```
## 二分查找
```c++
[二分查找](https://www.lintcode.com/problem/first-position-of-target/description)
int binarySearch(vector<int> &nums, int target) {
    int l = 0, r = nums.size() - 1;

    while (l <= r) {
        int m = l + (r-l)/2;
        if (nums[m] == target) {
            if (m == 0 || nums[m-1] != target) return m;
            else r = m-1;
        } else if (nums[m] > target) {
            r = m-1;
        } else {
            l = m+1;
        }
    }

    return -1;
}
```
## 最大子数组
```c++
[最大子数组](https://www.lintcode.com/problem/maximum-subarray/description)
int maxSubArray(vector<int> &nums) {
    if (nums.empty()) return 0;
    int rst = nums[0];
    int sub_sum = 0;
    for (int i = 0; i < nums.size(); ++i) {
        if (sub_sum > 0 ) sub_sum += nums[i];
        else sub_sum = nums[i];
        rst = max(rst, sub_sum);
    }
    return rst;
}
```
## 最大子数组 II
[最大子数组 II](https://www.lintcode.com/problem/maximum-subarray-ii/description)
```c++
int maxTwoSubArrays(vector<int> &nums) {
    if (nums.empty()) return 0;
    else if (1 == nums.size()) return nums[0];
    else if (2 == nums.size()) return nums[0] + nums[1];

    vector<int> forword(nums.size(), 0);
    vector<int> backword(nums.size(), 0);

    int mv = nums[0];
    int last = 0;
    for (int i = 0; i < nums.size(); ++i) {
        if (last > 0) last += nums[i];
        else last = nums[i];
        mv = max(mv, last);
        forword[i] = mv;
    }

    mv = nums[nums.size()-1];
    last = 0;
    for (int i = nums.size()-1; i >= 0; --i) {
        if (last > 0) last += nums[i];
        else last = nums[i];
        mv = max(mv, last);
        backword[i] = mv;
    }

    int rst = forword[0]+backword[1];
    for (int i = 1; i < nums.size()-1; ++i) {
        rst = max(rst, forword[i]+backword[i+1]);
    }

    return rst;
}
```
## 两数之和
[两数之和](https://www.lintcode.com/problem/two-sum/description)
```Go
func twoSum (numbers []int, target int) []int {
    idxMap := make(map[int]int)
    for i := 0; i < len(numbers); i++ {
        idxMap[numbers[i]] = i
    }

    for i := 0; i < len(numbers); i++ {
        v := target - numbers[i]
        if idx, ok := idxMap[v]; ok {
            return []int{i, idx}
        }
    }
    return []int{-1, -1}
}
```
## 三数之和
[三数之和](https://www.lintcode.com/problem/3sum/description)
```c++
vector<vector<int>> threeSum(vector<int> &numbers) {
    // write your code here
    sort(numbers.begin(), numbers.end());
    vector<vector<int>> rst;

    for (int i = 0; i < numbers.size()-2; ++i) {
        if (i > 0 && numbers[i] == numbers[i-1]) {
            continue;
        }
        int start = i+1, end = numbers.size() - 1;
        while (start < end) {
            if (start > i+1 && numbers[start] == numbers[start-1]) {
                start++;
                continue;
            }
            if (numbers[start]+numbers[end] < 0 - numbers[i]) start++;
            else if (numbers[start] + numbers[end] > 0 - numbers[i]) end--;
            else rst.push_back(vector<int>{numbers[i], numbers[start++], numbers[end--]});
        }
    }

    return rst;
}
```
## 搜索旋转排序数组
[搜索旋转排序数组](https://www.lintcode.com/problem/search-in-rotated-sorted-array/description)
```c++
int search(vector<int> &A, int target) {
    for (int i = 0, j = A.size()-1; i <= j; ) {
        int mid = (i+j) / 2;
        if (A[mid] == target) {
            return mid;
        }

        if (A[0] < A[mid]) {
            if (target >= A[0] && target < A[mid] ) {
                j = mid -1;
            } else {
                i = mid + 1;
            }
        } else {
            if (target > A[mid] && target <= A[A.size()-1]) {
                i = mid + 1;
            } else {
                j = mid -1;
            }
        }
    }
    return -1;
}
```
## 两个排序数组的中位数
[两个排序数组的中位数](https://www.lintcode.com/problem/median-of-two-sorted-arrays/description)
```c++
double findMedianSortedArrays(vector<int> &A, vector<int> &B) {
    // write your code here
    if ((A.size()+B.size()) & 0x01) {
        return findKth(A, 0, A.size()-1, B, 0, B.size()-1, (A.size()+B.size())/2+1);
    } else {
        return (findKth(A, 0, A.size()-1, B, 0, B.size()-1, (A.size()+B.size())/2) +
            findKth(A, 0, A.size()-1, B, 0, B.size()-1, (A.size()+B.size())/2+1))/2;
    }
}

double findKth(vector<int> &A, int al, int ar, vector<int> &B, int bl, int br, int k) {
    //cout << al << " " << ar << " " << bl << " " << br << endl;
    if (br-bl < ar-al) {
        return findKth(B, bl, br, A, al, ar, k);
    }

    if (al > ar) return B[bl+k-1];

    if (k == 1) return min(A[al], B[bl]);

    int pa = min(ar-al+1, k/2);
    int pb = k - pa;

    if (A[al+pa-1] == B[bl+pb-1]) {
        return A[al+pa-1];
    } else if (A[al+pa-1] < B[bl+pb-1]) return findKth(A, al+pa, ar, B, bl, br, k-pa);
    else return findKth(A, al, ar, B, bl+pb, br, k-pb);
}
```

## 中位数
[中位数](https://www.lintcode.com/problem/median/description)
**partiton** O(n)
```c++
int median(vector<int> &nums) {
    // write your code here

    int m = 0;
    if (nums.size() & 0x01)  m = nums.size()/2;
    else m = nums.size()/2-1;

    int left = 0, right = nums.size()-1;
    while (1) {
        int p = partition(nums, left, right);
        if (p == m) return nums[p];
        else if (p < m) left = p + 1;
        else if (p > m) right = p - 1;
    }

    return 0;
}

int partition(vector<int> &nums, int left, int right) {
    int pivot = nums[left];
    while (left < right) {
        while (left < right && nums[right] >= pivot) right--;
        nums[left] = nums[right];
        while (left < right && nums[left] <= pivot) left++;
        nums[right] = nums[left];
    }
    nums[left] = pivot;
    return left;
}
```

## 买卖股票
### 买卖股票I
[买卖股票I](https://www.lintcode.com/problem/best-time-to-buy-and-sell-stock/description)
```c++
    int maxProfit(vector<int> &prices) {
        // write your code here
        int rst = 0;

        int p = 0;
        for (int i = 1; i < prices.size(); ++i) {
             if (p > 0) p += prices[i] - prices[i-1];
             else p = prices[i] - prices[i-1];
             rst = max(rst, p);
         }

        return rst;
    }
```
### 买卖股票II
[买卖股票II](https://www.lintcode.com/problem/best-time-to-buy-and-sell-stock-ii/description)
```c++
    int maxProfit(vector<int> &prices) {
        // write your code here
        int rst = 0;
        for (int i = 1; i < prices.size(); ++i) {
            rst += max(0, prices[i] - prices[i-1]);
        }
        return rst;
    }
```
### 买卖股票III
[买卖股票III](https://www.lintcode.com/problem/best-time-to-buy-and-sell-stock-iii/description)
```c++
// 最多两笔交易
    int maxProfit(vector<int> &prices) {
        if (prices.empty()) return 0;
        // write your code here
        vector<int> left(prices.size(), 0);
        int minPrice = prices[0];
        for (int i = 1; i < prices.size(); ++i) {
            if (prices[i] <= minPrice) {
                left[i] = left[i-1];
                minPrice = prices[i];
            } else {
                left[i] = max(left[i-1], prices[i] - minPrice);
            }
        }

        vector<int> right(prices.size(), 0);
        int maxPrice = prices[prices.size()-1];
        for (int i = prices.size()-2; i>=0; i--) {
            if (prices[i] >= maxPrice) {
                maxPrice = prices[i];
                right[i] = right[i+1];
            } else {
                right[i] = max(right[i+1], maxPrice - prices[i]);
            }
        }

        int rst = 0;
        for (int i = 0; i < prices.size(); ++i) {
            rst = max(rst, left[i] + right[i]);
        }
        
        return rst;
    }
```
## 装最多水的容器
[装最多水的容器](https://www.lintcode.com/problem/container-with-most-water/description)
```c++
    int maxArea(vector<int> &heights) {
        // write your code here
        int rst = 0;
        for (int i = 0, j = heights.size()-1; i < j; ) {
            rst = max(rst, min(heights[i], heights[j])*(j-i));
            if (heights[i] < heights[j]) i++;
            else j--;
        }
        return rst;
    }
```
## 连续子数组求和
[连续子数组求和](https://www.lintcode.com/problem/continuous-subarray-sum/description)
```c++
    vector<int> continuousSubarraySum(vector<int> &A) {
        // write your code here
        int l = 0, r = 0;
        int subVal = A[0];
        int maxVal = A[0];
        int lastIdx = 0;
        
        for (int i = 1; i < A.size(); ++i) {
            if (subVal < 0) {
                lastIdx = i;
                subVal = A[i];
            } else {
                subVal += A[i];
            }
            
            if (maxVal < subVal) {
                maxVal = subVal;
                l = lastIdx;
                r = i;
            }
        }
        
        return vector<int>{l, r};
    }
```

## 接雨水
[接雨水](https://www.lintcode.com/problem/trapping-rain-water/description)
```c++
    int trapRainWater(vector<int> &heights) {
        // write your code here
        if (heights.size() <= 2) return 0;
        
        vector<int> lw(heights.size(), 0);
        vector<int> rw(heights.size(), 0);
        
        int w = heights[0];
        for (int i = 1; i < heights.size()-1; ++i) {
            if (heights[i] > w) w = heights[i];
            else lw[i] = w - heights[i];
        }
        w = heights[heights.size()-1];
        for (int i = heights.size()-2; i > 0; --i) {
            if (heights[i] > w) w = heights[i];
            else rw[i] = w - heights[i];
        }
        
        int rst = 0;
        for (int i = 0; i < heights.size(); ++i) {
            rst += min(lw[i], rw[i]);
        }
        
        return rst;
    }
```
##
[]()
```c++
```
