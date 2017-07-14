#include <stdio.h>
#include <climits>
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <iostream>
#include <algorithm>
#include <stack>
using namespace std;
#include "Common.h"
#include "LeetCodeA.h"

#define BEST_SOLUTION

//1.Two Sum
//Given an array of integers, return indices of the two numbers such that they
//add up to a specific target.You may assume that each input would have exactly
//one solution, and you may not use the same element twice.
std::vector<int> LeetCodeA::twoSum(std::vector<int>& nums, int target) {
    std::vector<int> result;

    std::unordered_map<int, int> map;
    int remain;
    for(size_t i = 0; i < nums.size(); i ++) {
    	remain = target - nums[i];
    	std::unordered_map<int, int>::const_iterator it;
    	it = map.find(remain);
    	if(it != map.end()) {
    		result.push_back(it->second);
    		result.push_back(i);
    		break;
    	}
    	map.insert(std::make_pair<int, int>((int)(nums.at(i)), i));
//    	map[nums[i]] = i;
    }
    return result;
}

//2.Add Two Numbers
//You are given two non-empty linked lists representing two non-negative integers.
//The digits are stored in reverse order and each of their nodes contain a single digit.
//Add the two numbers and return it as a linked list.
//You may assume the two numbers do not contain any leading zero, except the number 0 itself.
/*
 * Input: (2 -> 4 -> 3) + (5 -> 6 -> 4)
 * Output: 7 -> 0 -> 8
 */
ListNode* LeetCodeA::addTwoNumbers(ListNode* l1, ListNode* l2) {
	ListNode *p = l1;
	ListNode *q = l2;
	ListNode dummyHead(0);
	ListNode *cur = &dummyHead;

	int carry = 0;
	while (!((carry == 0) && (p == NULL) && (q == NULL))) {
		int res = 0;
		if (p != NULL) {
			res += p->val;
			p = p->next;
		}

		if (q != NULL) {
			res += q->val;
			q = q->next;
		}

		res += carry;
		carry = res / 10;
		res = res % 10;

		cur->next = new ListNode(res);
		cur = cur->next;
	};

	return dummyHead.next;
}

//3.Longest Substring Without Repeating Characters
//Given a std::string, find the length of the longest substring without repeating characters.
int LeetCodeA::lengthOfLongestSubstring(std::string s) {
	vector<int> dict(256, -1);
	int max = 0;
	int start = -1;
	for(int i = 0; i < s.length(); i ++) {
		if(dict[s[i]] > start) {
			//found repeating character, update start position of repeating character
			start = dict[s[i]];
		}
		dict[s[i]] = i;
		max = std::max(max, i - start);
	}
	return max;
}

//4.Median of Two Sorted Arrays
//Find the median of the two sorted arrays.
//The overall run time complexity should be O(log (m+n)).
double LeetCodeA::findMedianSortedArrays(std::vector<int>& nums1, std::vector<int>& nums2) {
	double median = 0.0;
	//TODO:finish it
	return median/2;
}

//5.Longest Palindromic Substring
//Given a string s, find the longest palindromic substring in s.
//You may assume that the maximum length of s is 1000.
string LeetCodeA::longestPalindrome(string s) {
	string str;
	//TODO:finish it
	return str;
}

//6.ZigZag Conversion
string LeetCodeA::convert(string s, int numRows) {
#if CLEAR
	string str;
	if(numRows == 0)
		return str;

	int interval = 1;
	if(numRows  > 1 )
		interval = 2 * (numRows - 1);

	int gap;
	int size = s.size();

	for(int row = 0; row < numRows; row ++) {
		gap = (interval - 2 * row);
		for(int j = row; j < size ; j += interval) {
			str.push_back(s.at(j));
			if(row != 0 && (row != numRows - 1) && (j + gap < size)) {
				str.push_back(s.at(j + gap));
			}
		}
	}
	return str;
#else
	string str;
	if(numRows == 0)
		return str;

	int interval = 1;
	if(numRows  > 1 )
		interval = 2 * (numRows - 1);

	int gap;
	int size = s.size();

	for(int row = 0; row < numRows; row ++) {
		gap = (interval - 2 * row);
		for(int j = row; j < size ; j += interval) {
			str.push_back(s.at(j));
			cout << s.at(j);
			int l = gap;
			while(l > 2){
				cout <<" ";
				l -= 2;
			}
			if(row != 0 && (row != numRows - 1) && (j + gap < size)) {
				str.push_back(s.at(j + gap));
				cout << s.at(j + gap);
			}
			l = 2;
			while(l + gap < interval) {
				cout <<" ";
				l += 2;
			}
		}
		cout << endl;
	}
	return str;
#endif
}

//7.Reverse Integer
//Reverse digits of an integer.
int LeetCodeA::reverseInteger(int x) {
	int res = 0;
	int result = 0;
	while(x) {
		res = result * 10 + x%10;
		if((res - x%10)/10 != result) {
			//must have been overflown
			result = 0;
			break;
		}
		result = res;
		x /= 10;
	}
	return result;
}

//8.String to Integer (atoi)
//Implement atoi to convert a string to an integer.
int LeetCodeA::myAtoi(string str) {
	int res = 0;
	int len = str.size();
	int sign = 1;
	int pos = 0;

	while(str.at(pos) == ' ' ) pos++;

	if(len > pos && (str.at(pos) == '-' || str.at(pos) == '+')) {
		sign = 1 - 2*(str.at(pos++) == '-');
	}

	bool overflow = false;
	int tail;
	while(pos < len) {
		if (str.at(pos) >= '0' && str.at(pos) <= '9') {
			tail = str.at(pos++) - '0';
			if(res > INT_MAX/10 || (res == INT_MAX/10 && (tail > INT_MAX%10))) {
				if(sign == 1) {
					return INT_MAX;
				} else {
					//this will treat INT_MIN as overflow, but return value is correct.
					return INT_MIN;
				}
			}
			res = res * 10 + tail;
		} else {
			break;
		}
	}
	return sign * res;

}

//9.Palindrome Number
//Determine whether an integer is a palindrome. Do this without extra space.
bool LeetCodeA::isPalindrome(int x) {
	if(x < 0 || (x != 0 && x%10 == 0))
		return false;

	int reverse;
	for(reverse = 0; reverse < x; x /= 10) {
		reverse = reverse * 10 + x % 10;
	}

	return (reverse/10 == x) || (x == reverse);
}
//10. Regular Expression Matching
//Implement regular expression matching with support for '.' and '*'.
bool LeetCodeA::isRegularExpressionMatch(string s, string p) {
	//TODO:finish it
	return false;
}

//11. Container With Most Water
//Given n non-negative integers a1, a2, ..., an, where each represents a point
//at coordinate (i, ai). n vertical lines are drawn such that the two endpoints
//of line i is at (i, ai) and (i, 0). Find two lines, which together with x-axis
//forms a container, such that the container contains the most water.
int LeetCodeA::maxArea(vector<int>& height) {
	int area = 0;
	int i = 0;
	int j = height.size() - 1;
	while(i < j) {
		int h = min(height.at(i), height.at(j));
		area = max(area, (j - i) * h);

		//find next taller height
		if(height.at(i) < height.at(j))
			i ++;
		else
			j--;
	}
	return area;
}

//12. Integer to Roman
//Given an integer, convert it to a roman numeral.
//input is between 1 and 3999
string LeetCodeA::intToRoman(int num) {
#ifdef BEST_SOLUTION
	string M[] = {"", "M", "MM", "MMM"};
	string C[] = {"", "C", "CC", "CCC", "CD", "D", "DC", "DCC", "DCCC", "CM"};
	string X[] = {"", "X", "XX", "XXX", "XL", "L", "LX", "LXX", "LXXX", "XC"};
	string I[] = {"", "I", "II", "III", "IV", "V", "VI", "VII", "VIII", "IX"};
	return M[num/1000] + C[(num/100)%10] + X[(num/10)%10] + I[num%10];
#else
	string result;
	unordered_map<int, char> T = {
			{  1 ,  'I'},
			{  5 ,  'V'},
			{  10 , 'X'},
			{  50 , 'L'},
			{  100 ,'C'},
			{  500 ,'D'},
			{  1000 ,'M'}
	};
	int factor = 1;
	int remain = num/10;
	while(remain && factor < 1000) {
		remain /= 10;
		factor *= 10;
	}
	while (factor) {
		remain = num / factor;
		remain %= 10;
		if (factor == 1000) {
			result.append(remain, T[factor]);
		} else {
			if (remain % 5 == 4) {
				//4 and 9
				result.push_back(T[factor]);
				result.push_back(T[(remain + 1) * factor]);
			} else {
				if (remain >= 5) {
					result.push_back(T[factor * 5]);
				}
				remain %= 5;
				result.append(remain, T[factor]);
			}
		}
		factor /= 10;
	}
	return result;
#endif
}

//13.Roman to Integer
//Given a roman numeral, convert it to an integer.
int LeetCodeA::romanToInt(string s) {
	//I:1, V:5, X:10, L:50, C:100, D:500, M:1000
#ifdef BEST_SOLUTION
	unordered_map<char, int> T = {
			{ 'I', 1 },
			{ 'V', 5 },
			{ 'X', 10 },
			{ 'L', 50 },
			{ 'C', 100 },
			{ 'D', 500 },
			{ 'M', 1000 }
	};
	int size = s.size();
	if(size < 1)
		return 0;

	int sum = T[s.back()];
	for (int i = size - 2; i >= 0; --i) {
		if (T[s[i]] < T[s[i + 1]]) {
			sum -= T[s[i]];
		} else {
			sum += T[s[i]];
		}
	}

	return sum;
#else
	unordered_map<char, int> romanMap;
	romanMap['I'] = 1;
	romanMap['V'] = 5;
	romanMap['X'] = 10;
	romanMap['L'] = 50;
	romanMap['C'] = 100;
	romanMap['D'] = 500;
	romanMap['M'] = 1000;

	int size = s.size();
	if(s.size() < 1)
		return 0;
	//MCMXCVI
	int cur;
	int val = 0;
	int prev = romanMap[s.at(0)];
	for(int i = 1; i < size; i ++) {
		char c = s.at(i);
		if(prev != 0 && romanMap[c] > prev) {
			val += romanMap[c] - prev;
			prev = 0;
		} else {
			val += prev;
			prev = romanMap[c];
		}
	}
	val += prev;
	return val;
#endif
}

//14. Longest Common Prefix
//Write a function to find the longest common prefix string amongst an array of strings.
string LeetCodeA::longestCommonPrefix(vector<string>& strs) {

	string str = "";
	if(strs.empty())
		return str;

	for(int i = 0; i < strs[0].size(); i ++) {
		for(int j = 1; j < strs.size(); j++) {
			if(i >= strs[j].size() || strs[j][i] != strs[0][i]) {
				return str;
			}
		}
		str += strs[0][i];
	}
	return str;
}

//15. 3Sum
//Given an array S of n integers, are there elements a, b, c in S such that a + b + c = 0?
//Find all unique triplets in the array which gives the sum of zero.
vector<vector<int> > LeetCodeA::threeSum(vector<int>& nums) {
	vector<vector<int> > result;
	int size = nums.size();
	if(size <= 2)
		return result;

	std::sort(nums.begin(), nums.end());
	for(int i = 0; i < size; ) {
		int left = i + 1;
		int right = size - 1;
		while(left < right) {
			int sum = nums.at(i) + nums.at(left) + nums.at(right);
			if(sum == 0) {
				result.push_back({nums.at(i), nums.at(left), nums.at(right)});
				//move to next different number from left side
			}

			if(sum <= 0) {
				//postive number is to small, decrease negtive number
				while(++left < right && nums[left] == nums[left - 1]);
			}

			if(sum >= 0) {
				while(--right > left && nums[right] == nums[right + 1]);
			}
		}

		//find next different number
		while(++i < size && nums[i] == nums[i - 1]);
	}
	return result;
}

//16. 3Sum Closest
//Given an array S of n integers, find three integers in S such that the sum is closest to
//a given number, target. Return the sum of the three integers. You may assume that each
//input would have exactly one solution.
int LeetCodeA::threeSumClosest(vector<int>& nums, int target) {
	int sum = 0;
	int size = nums.size();
	if(size < 3)
		return sum;

	std::sort(nums.begin(), nums.end());
	sum = nums.at(0) + nums.at(1) + nums.at(2);
	for(int i = 0; i < size; ) {
		int left = i + 1;
		int right = size - 1;

		while(left < right) {
			int tmp = nums.at(i) + nums.at(left) + nums.at(right);
			if(tmp == target) {
				sum = tmp;
				break;
			}
			if(abs(tmp - target) < abs(sum - target))
				sum = tmp;

			if(tmp < target) {
				while(++left < right && nums.at(left) == nums.at(left - 1));
			}
			if(tmp > target) {
				while(--right > left && nums.at(right) == nums.at(right + 1));
			}
		}
		while(++i < size && nums.at(i) == nums.at(i - 1));
	}
	return sum;
}

//17. Letter Combinations of a Phone Number
//Given a digit string, return all possible letter combinations that the number could represent.
vector<string> LeetCodeA::letterCombinations(string digits) {
	vector<string> v;
	if(digits.empty())
		return v;

	string buttons[] =  {"", "", "abc", "def", "ghi", "jkl", "mno", "pqrs", "tuv", "wxyz"};

	v.push_back("");
	vector<string> tmp;
	int size = digits.size();
	for(int i = 0; i < size; i ++) {
		if(digits.at(i) < '0' || digits.at(i) > '9')
			break;

		string &str = buttons[digits.at(i) - '0'];
		if(str.empty()) continue;
		tmp.clear();
		for(int j = 0; j < str.size(); j ++) {
			for(int k = 0; k < v.size(); k++) {
				tmp.push_back(v[k] + str[j]);
			}
		}
		v.swap(tmp);
	}

	return v;
}

//18. 4Sum
//Given an array S of n integers, are there elements a, b, c, and d in S such that a + b + c + d = target?
//Find all unique quadruplets in the array which gives the sum of target
vector<vector<int> > LeetCodeA::fourSum(vector<int>& nums, int target) {
	vector<vector<int> > result;
	int size = nums.size();
	if(size < 4)
		return result;

	std::sort(nums.begin(), nums.end());
	for (int j = 0; j < size;) {
		for (int i = j + 1; i < size;) {
			int left = i + 1;
			int right = size - 1;
			while (left < right) {
				int sum = nums.at(j) + nums.at(i) + nums.at(left) + nums.at(right);
				if (sum == target) {
					result.push_back( {nums.at(j), nums.at(i), nums.at(left), nums.at(right) });
					//move to next different number from left side
				}

				if (sum <= target) {
					//postive number is to small, decrease negtive number
					while (++left < right && nums[left] == nums[left - 1]) ;
				}

				if (sum >= target) {
					while (--right > left && nums[right] == nums[right + 1]) ;
				}
			}
			//find next different number
			while (++i < size && nums[i] == nums[i - 1]) ;
		}
		while (++j < size && nums[j] == nums[j - 1]) ;
	}
	return result;
}

//19. Remove Nth Node From End of List
//Given a linked list, remove the nth node from the end of list and return its head.
ListNode* LeetCodeA::removeNthFromEnd(ListNode* head, int n) {
#ifdef BEST_SOLUTION
	ListNode** t1 = &head, *t2 = head;
	for (int i = 1; i < n; ++i) {
		t2 = t2->next;
	}

	while (t2->next != NULL) {
		t1 = &((*t1)->next);
		t2 = t2->next;
	}
	t2 = *t1;
	*t1 = (*t1)->next;
	delete t2;

	return head;

#else
	ListNode *cur = head;
	vector<ListNode *> v;
	while(cur != NULL) {
		v.push_back(cur);
		cur = cur->next;
	}
	if(n < v.size()) {
		ListNode *prev = v.at(v.size() - n - 1);
		cur = v.at(v.size() - n);
		prev->next = cur->next;
		delete cur;
	} else {
		return head->next;
	}
	return head;
#endif
}

//20. Valid Parentheses
bool LeetCodeA::isValid(string s) {
#ifdef BEST_SOLUTION
	bool valid = true;
    stack<char> st;
    for(char &c :s) {
    	switch(c) {
    	case '(':
    	case '[':
    	case '{':
		   st.push(c);
		   break;
    	case ')':
    		if(st.empty() || st.top() != '(') {
    			return false;
    		} else {
    			st.pop();
    		}
    		break;
    	case ']':
    		if(st.empty() || st.top() != '[') {
    			return false;
    		} else {
				st.pop();
			}
    		break;
    	case '}':
    		if(st.empty() || st.top() != '{') {
    			return false;
    		} else {
				st.pop();
			}
    		break;
    	default:
    		break;
    	};
    }
    return st.empty();
#else
	bool valid = true;
    stack<char> st;
    unordered_map<char, char> parentheses;
    parentheses[')'] = '(';
    parentheses[']'] = '[';
    parentheses['}'] = '{';

    for(int i = 0; i < s.size(); i ++) {
        if(s.at(i) == '(' || s.at(i) == '[' || s.at(i) == '{') {
            st.push(s.at(i));
        } else if(s.at(i) == ')' || s.at(i) == ']' || s.at(i) == '}') {
            char c = st.top();
            if(c != parentheses[s.at(i)]) {
                valid = false;
                break;
            }
            st.pop();
        }
    }
    if(!st.empty())
        valid = false;

    return valid;
#endif
}

//21. Merge Two Sorted Lists
//Merge two sorted linked lists and return it as a new list. The new list should be made
//by splicing together the nodes of the first two lists.
ListNode* LeetCodeA::mergeTwoLists(ListNode* l1, ListNode* l2) {
	ListNode *current, *l11, *l22;
	l11 = l1;
	l22 = l2;
	ListNode tmp(0);
	current = &tmp;

	while(l11 && l22) {
		if(l11->val > l22->val) {
			current->next = l22;
			l22 = l22->next;
		} else {
			current->next = l11;
			l11 = l11->next;
		}
		current = current->next;
	};
	current->next = l11 ? l11 : l22;
	return tmp.next;
}

//22. Generate Parentheses
//Given n pairs of parentheses, write a function to generate all combinations of well-formed parentheses
void LeetCodeA::generateParenthesis_l(vector<string> &v, string str, int n, int m) {
	if (n == 0 && m == 0) {
		v.push_back(str);
		return;
	}
	if (m > 0) {
		generateParenthesis_l(v, str + ")", n, m - 1);
	}
	if (n > 0) {
		generateParenthesis_l(v, str + "(", n - 1, m + 1);
	}
}
vector<string> LeetCodeA::generateParenthesis(int n) {
	vector<string> str;
	string s;
	generateParenthesis_l(str, s, n, 0);
	return str;
}

//23. Merge k Sorted Lists
//Merge k sorted linked lists and return it as one sorted list. Analyze and describe its complexity.
ListNode* LeetCodeA::mergeKLists(vector<ListNode*>& lists) {
	ListNode dummyHead(0);
	//TODO:finish it
	return dummyHead.next;
}

//24. Swap Nodes in Pairs
//Given a linked list, swap every two adjacent nodes and return its head.
ListNode* LeetCodeA::swapPairs(ListNode* head) {
#if 0
	ListNode dummy(0);
	ListNode *prev = &dummy;
	ListNode *cur, *next;
	cur = head;
	while(cur && cur->next) {
		next = cur->next;

		prev->next = next;

		cur->next = next->next;
		next->next = cur;
		cur = cur->next;
		prev = cur;
	}
	prev->next = cur;
	return dummy.next;
#else
	ListNode **h = &head;
	ListNode *cur = head;
	ListNode *next;
	while(cur && cur->next) {
		next = cur->next;
		cur->next = next->next;
		next->next = cur;
		*h = next;
		h = &(cur->next);
		cur = cur->next;
	}
	return head;
#endif
}

//25. Reverse Nodes in k-Group
//Given a linked list, reverse the nodes of a linked list k at a time and return its modified list.
ListNode* LeetCodeA::reverseKGroup(ListNode* head, int k) {
	ListNode dummyHead(0);
	//TODO:finish it
	return dummyHead.next;
}

//26. Remove Duplicates from Sorted Array
//Given a sorted array, remove the duplicates in place such that each element appear
//only once and return the new length.Do not allocate extra space for another array,
//you must do this in place with constant memory.
int LeetCodeA::removeDuplicates(vector<int>& nums) {
	int size = nums.size();
	int len = 0;
	for (int i = 1; i < size; i++) {
		if (nums.at(i) == nums.at(i - 1)) {
			len++;
		} else {
			nums.at(i - len) = nums.at(i);
		}
	}
	return size - len;
}

//27. Remove Element
//Given an array and a value, remove all instances of that value in place and return the new length.
//Do not allocate extra space for another array, you must do this in place with constant memory.
//The order of elements can be changed. It doesn't matter what you leave beyond the new length.
int LeetCodeA::removeElement(vector<int>& nums, int val) {
	int start = 0;
	int end = nums.size() - 1;

	while(start <= end) {
		if(nums.at(start) == val) {
			while(end > start && nums.at(end) == val) end--;
			if(end <= start) {
				break;
			}
			swap(nums.at(end), nums.at(start));
		}
		start ++;
	}
	return start;
}

//28. Implement strStr()
//Implement strStr().
//Returns the index of the first occurrence of needle in haystack,
//or -1 if needle is not part of haystack.
int LeetCodeA::strStr(string haystack, string needle) {

	int m = haystack.size();
	int n = needle.size();
	if(n == 0)
		return 0;

	int pos = -1;
	for(int i = 0; i < m - n + 1; i ++) {
		int j;
		for(j = 0; j < n; j ++) {
			if(needle.at(j) != haystack.at(i + j))
				break;
		}
		if(j == needle.size()) {
			pos = i;
			break;
		}
	}
	return pos;
}

//29. Divide Two Integers
//Divide two integers without using multiplication, division and mod operator.
//overflow
int LeetCodeA::divideIntegers(int dividend, int divisor) {
	if (!divisor || (dividend == INT_MIN && divisor == -1))
		return INT_MAX;
	int sign = ((dividend < 0) ^ (divisor < 0)) ? -1 : 1;
	long long dvd = labs(dividend);
	long long dvs = labs(divisor);
	int res = 0;
	while (dvd >= dvs) {
		long long temp = dvs, multiple = 1;
		while (dvd >= (temp << 1)) {
			temp <<= 1;
			multiple <<= 1;
		}
		dvd -= temp;
		res += multiple;
	}
	return sign == 1 ? res : -res;
}

//30. Substring with Concatenation of All Words
//You are given a string, s, and a list of words, words, that are all of the same length.
//Find all starting indices of substring(s) in s that is a concatenation of each word in
//words exactly once and without any intervening characters.
vector<int> LeetCodeA::findSubstring(string s, vector<string>& words) {
	vector<int> v;
	//TODO:finish it
	return v;
}

//31. Next Permutation
//Implement next permutation, which rearranges numbers into the lexicographically
//next greater permutation of numbers. If such arrangement is not possible, it must
//rearrange it as the lowest possible order (ie, sorted in ascending order).
//The replacement must be in-place, do not allocate extra memory.
void LeetCodeA::nextPermutation(vector<int>& nums) {
	int size = nums.size();
	if(size <= 1)
		return ;

	int i;
    for(i = size - 1; i >= 1; i --) {
        if(nums.at(i) > nums.at(i - 1)) {
            //find last i that makes nums[i - 1] < nums[i]
            for(int j = size - 1; j >= i; j--){
                if(nums.at(j) > nums.at(i - 1)) {
                    //find last j that makes nums[i - 1] < nums[j]
                    swap(nums.at(i - 1), nums.at(j));
                    std::reverse(nums.begin() + i, nums.end());
                    break;
                }
            }
            break;
        }
    }
    if(i < 1) {
    	std::reverse(nums.begin(), nums.end());
    }
}

//32. Longest Valid Parentheses
//Given a string containing just the characters '(' and ')', find the length of the
//longest valid (well-formed) parentheses substring. For "(()", the longest valid
//parentheses substring is "()", which has length = 2. Another example is ")()())",
//where the longest valid parentheses substring is "()()", which has length = 4.
int LeetCodeA::longestValidParentheses(string s) {
	//TODO:finish it
	return 0;
}

//33. Search in Rotated Sorted Array
//Suppose an array sorted in ascending order is rotated at some pivot unknown to you beforehand.
//(i.e., 0 1 2 4 5 6 7 might become 4 5 6 7 0 1 2). You are given a target value to search.
//If found in the array return its index, otherwise return -1.
//You may assume no duplicate exists in the array.
int LeetCodeA::search(vector<int>& nums, int target) {
	int size = nums.size();
	int start = 0;
	int end = size - 1;

	if(size <= 0) {
		return -1;
	}

	while(++start < size && nums.at(start) > nums.at(start - 1));
	if(start >= size) {
		//doesn't rotate, array is sorted
		start = 0;
	}

	if(target > nums.at(end)) {
		end = start -1;
		start = 0;
	}
	int index = -1;
	while(start <= end) {
		int mid = (start + end)/2;
		int cmp = nums.at(mid) - target;
		if(cmp == 0) {
			index = mid;
			break;
		} else if(cmp > 0) {
			end = mid - 1;
		} else {
			start = mid + 1;
		}
	}
	return index;
}

//34. Search for a Range
//Given an array of integers sorted in ascending order, find the starting and ending position of
//a given target value. Your algorithm's runtime complexity must be in the order of O(log n).
//If the target is not found in the array, return [-1, -1].
vector<int> LeetCodeA::searchRange(vector<int>& nums, int target) {
#ifdef BEST_SOLUTION
	vector<int> ans = {-1, -1};
	int lo = 0;
	int hi = nums.size() - 1;
	while(lo < hi) {
		int mid = lo + (hi - lo)/2;
		if(nums.at(mid) < target)
			lo = mid + 1;
		else
			hi = mid;
	}
	if(nums.at(lo) == target) {
		ans[0] = lo;
	} else {
		return ans;
	}

	hi = nums.size() - 1;
	while(lo < hi) {
		//mid biased to right to avoid stuck
		int mid = (hi + lo + 1) / 2;
		if(nums.at(mid) > target)
			hi = mid - 1;
		else
			lo = mid;
	}
	ans[1] = hi;

	return ans;
#else
	vector<int> ans = {-1, -1};
	int lo = 0;
	int hi = nums.size() - 1;
	int mid;
	while(lo <= hi) {
		mid = (lo + hi) / 2;
		if(nums.at(mid) == target) {
			ans.assign(2, mid);
			break;
		} else if (nums.at(mid) > target) {
			hi = mid - 1;
		} else {
			lo = mid + 1;
		}
	}

	if(ans[0] != -1) {
		//[0, mid - 1]
		int lo1 = lo;
		int hi1 = mid - 1;
		while(lo1 <= hi1) {
			int tmp = (lo1 + hi1)/2;
			if(nums.at(tmp) == target) {
				ans[0] = tmp;
				hi1 = tmp - 1;
			} else {
				//tmp < target
				lo1 = tmp + 1;
			}
		}
		//[mid + 1, nums.size() -1 ]
		lo1 = mid + 1;
		hi1 = hi;
		while(lo1 <= hi1) {
			int tmp = (lo1 + hi1)/2;
			if(nums.at(tmp) == target) {
				ans[1] = tmp;
				lo1 = tmp + 1;
			} else {
				//tmp > target;
				hi1 = tmp - 1;
			}
		}
	}
	return ans;
#endif
}

//35. Search Insert Position
//Given a sorted array and a target value, return the index if the target is found.
//If not, return the index where it would be if it were inserted in order.
int LeetCodeA::searchInsert(vector<int>& nums, int target) {
	int pos = 0;
	int lo = 0;
	int hi = nums.size() - 1;
	while(lo <= hi) {
		int mid = (lo + hi)/2;
		if(nums.at(mid) == target) {
			pos = mid;
			break;
		} else if(nums.at(mid) < target) {
			lo = mid + 1;
		} else if(nums.at(mid) > target) {
			hi = mid - 1;
		}
		pos = lo;
	}
	return pos;
}

//36. Valid Sudoku
//Determine if a Sudoku is valid, according to: Sudoku Puzzles - The Rules.
//The Sudoku board could be partially filled, where empty cells are filled with the character '.'.
bool LeetCodeA::isValidSudoku(vector<vector<char>>& board) {
	//TODO:
	bool valid = true;
	return valid;
}

//37. Sudoku Solver
//Write a program to solve a Sudoku puzzle by filling the empty cells.
//Empty cells are indicated by the character '.'.
//You may assume that there will be only one unique solution.
void LeetCodeA::solveSudoku(vector<vector<char>>& board) {
	//TODO:
}

//38. Count and Say
//The count-and-say sequence is the sequence of integers with the first five terms as following:
/*
 * 	1.     1
 	2.     11
	3.     21
	4.     1211
	5.     111221
 *
 */
//Given an integer n, generate the nth term of the count-and-say sequence.
string LeetCodeA::countAndSay(int n) {
	string res = "";
	if(n == 0) {
		return res;
	}

	res.append(std::to_string(1));
	while (--n) {
		string str = "";
		int count = 1;
		int i;
		for (i = 1; i < res.size(); i++) {
			if (res.at(i) == res.at(i - 1)) {
				count++;
			} else {
				str.append(to_string(count) + res.at(i - 1));
				count = 1;
			}
		}
		str.append(to_string(count) + res.at(i - 1));
		res.swap(str);
	}
	return res;
}


//39. Combination Sum
//Given a set of candidate numbers (C) (without duplicates) and a target number (T),
//find all unique combinations in C where the candidate numbers sums to T.
//The same repeated number may be chosen from C unlimited number of times.
vector<vector<int> > LeetCodeA::combinationSum(vector<int>& candidates, int target) {
	vector<vector<int> > sum ;
	return sum;
}

//40. Combination Sum II
//Given a collection of candidate numbers (C) and a target number (T),
//find all unique combinations in C where the candidate numbers sums to T.
//Each number in C may only be used once in the combination.
//Note:
//All numbers (including target) will be positive integers.
//The solution set must not contain duplicate combinations.
vector<vector<int> > LeetCodeA::combinationSum2(vector<int>& candidates, int target) {
	vector<vector<int> > sum ;
	return sum;
}

//41. First Missing Positive
//Given an unsorted integer array, find the first missing positive integer.
int LeetCodeA::firstMissingPositive(vector<int>& nums) {
	int missing = 0;
	return missing;
}

//42. Trapping Rain Water
//Given n non-negative integers representing an elevation map where the width of each bar is 1,
//compute how much water it is able to trap after raining.
int LeetCodeA::trap(vector<int>& height) {
	int n = 0;
	return n;
}

//43. Multiply Strings
//Given two non-negative integers num1 and num2 represented as strings, return the product of num1 and num2.
//The length of both num1 and num2 is < 110.
//Both num1 and num2 contains only digits 0-9.
//Both num1 and num2 does not contain any leading zero.
//You must not use any built-in BigInteger library or convert the inputs to integer directly.
string LeetCodeA::multiply(string num1, string num2) {
	string ans = "0";
	//TODO:
	return ans;
}


//44. Wildcard Matching
//Implement wildcard pattern matching with support for '?' and '*'.
bool LeetCodeA::isWildcardMatch(string s, string p) {
	bool match = true;
	//TODO:
	return match;
}

//45. Jump Game II
//Given an array of non-negative integers, you are initially positioned at the first index of the array.
//Each element in the array represents your maximum jump length at that position.
//Your goal is to reach the last index in the minimum number of jumps.
int LeetCodeA::jump(vector<int>& nums) {
	//TODO:
	int ans = 0;
	return ans;
}

//46.Permutations
//Given a collection of distinct numbers, return all possible permutations.
vector<vector<int> > LeetCodeA::permute(vector<int>& nums) {
	vector<vector<int> > result;
	permute_l(nums, 0, result);
	return result;
}

void LeetCodeA::permute_l(vector<int>& nums, int start, vector<vector<int> > &result) {
	if(start >= nums.size() - 1) {
		result.push_back(nums);
		return ;
	}
	for(int i = start; i < nums.size(); i ++) {
		swap(nums.at(start), nums.at(i));
		permute_l(nums, start + 1, result);
		swap(nums.at(start), nums.at(i));
	}
}

//47.Permutations II
//Given a collection of numbers that might contain duplicates, return all possible unique permutations.
vector<vector<int> > LeetCodeA::permuteUnique(vector<int>& nums) {

    vector<vector<int> > result;
    if(nums.size() <= 1) {
        result.push_back(nums);
        return result;
    }

    sort(nums.begin(), nums.end());

    bool next = false;
    do {
        result.push_back(nums);
        next = false;
        for(int i = nums.size() - 1; i >= 1; i --) {
            if(nums.at(i) > nums.at(i - 1)) {
                //find last i that makes nums[i - 1] < nums[i]
                for(int j = nums.size() - 1; j >= i; j--){
                    if(nums.at(j) > nums.at(i - 1)) {
                        //find last j that makes nums[i - 1] < nums[j]
                        swap(nums.at(i - 1), nums.at(j));
                        std::reverse(nums.begin() + i, nums.end());
                        next = true;
                        break;
                    }
                }
                break;
            }
        }
    } while(next);

	return result;
}

//this costs O(n) space
void LeetCodeA::permuteUnique_l(vector<int> num, int start, vector<vector<int> > &result) {
	if (start == num.size() - 1) {
		result.push_back(num);
		return;
	}
	for (int k = start; k < num.size(); k++) {
		if (start != k && num[start] == num[k])
			continue;
		swap(num[start], num[k]);
		permuteUnique_l(num, start + 1, result);
	}
}

//48. Rotate Image
//You are given an n x n 2D matrix representing an image.
//Rotate the image by 90 degrees (clockwise).
void LeetCodeA::rotate(vector<vector<int>>& matrix) {
#ifdef BEST_SOLUTION
	/*
	 * clockwise rotate
	 * first reverse up to down, then swap the symmetry
	 * 1 2 3     7 8 9     7 4 1
	 * 4 5 6  => 4 5 6  => 8 5 2
	 * 7 8 9     1 2 3     9 6 3
	*/

	std::reverse(matrix.begin(), matrix.end());
	for (int i = 0; i < matrix.size(); ++i) {
		//for(int j = 0; j < i; j ++)
		for (int j = i + 1; j < matrix[i].size(); ++j) {
			swap(matrix[i][j], matrix[j][i]);
		}
	}
	/*
	 * anticlockwise rotate
	 * first reverse left to right, then swap the symmetry
	 * 1 2 3     3 2 1     3 6 9
	 * 4 5 6  => 6 5 4  => 2 5 8
	 * 7 8 9     9 8 7     1 4 7
	*/
	/*
	for (auto& vi : matrix)
		reverse(vi.begin(), vi.end());
	for (int i = 0; i < matrix.size(); ++i) {
		for (int j = i + 1; j < matrix[i].size(); ++j)
			swap(matrix[i][j], matrix[j][i]);
	}
	*/
#else
	vector<vector<int> > res;
	vector<int> line;
	int row = matrix.at(0).size();
	int col = matrix.size();
	for(int i = 0; i < row; i ++) {
		line.clear();
		for(int j = col - 1; j >= 0; j--) {
			line.push_back(matrix.at(j).at(i));
		}
		res.push_back(line);
	}
	matrix.swap(res);
#endif
}

//49. Group Anagrams
//Given an array of strings, group anagrams together.
vector<vector<string> > LeetCodeA::groupAnagrams(vector<string>& strs) {
	vector<vector<string> > str;
	//TODO:
	return str;
}


//50. Pow(x, n)
//Implement pow(x, n).
double LeetCodeA::myPow(double x, int n) {
	double p = 0;
	//TODO:
	return p;
}

