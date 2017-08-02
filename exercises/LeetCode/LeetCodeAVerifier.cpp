#include <sys/time.h>
#include <climits>
#include <vector>
#include <iostream>
using namespace std;

#include "gtest/gtest.h"
#include "LeetCodeA.h"
#include "Common.h"

static long getNowUs() {
	struct timeval tv;
	gettimeofday(&tv, NULL);
	return tv.tv_sec * 1000000ll + tv.tv_usec;
}
//1
TEST(LeetCodeA, twoSum) {

	LeetCodeA A;
	int array[] = {2, 7, 11, 15};

	std::vector<int> nums;
	nums.assign(array, array + sizeof(array)/sizeof(array[0]));
	std::vector<int> res = A.twoSum(nums, 9);
	EXPECT_TRUE(res.size() == 2);
	EXPECT_TRUE(res[0] == 0);
	EXPECT_TRUE(res[1] == 1);
}

TEST(LeetCodeA, addTwoNumbers){
	LeetCodeA A;
	const int a[] = {2, 4, 3};
	const int b[] = {5, 6, 4};
	const int c[] = {7, 0, 8};

	ListNode *head1 = new ListNode(0);
	ListNode *head2 = new ListNode(0);

	ListNode *cur = head1;
	for(size_t i = 0; i < sizeof(a)/sizeof(a[0]); i ++) {
		cur->next = new ListNode(a[i]);
		cur = cur->next;
	}

	cur = head2;
	for(size_t i = 0; i < sizeof(b)/sizeof(b[0]); i ++) {
		cur->next = new ListNode(b[i]);
		cur = cur->next;
	}

	ListNode *result = A.addTwoNumbers(head1->next, head2->next);
	int i = 0;
	for(ListNode *tmp = result; tmp != NULL; tmp = tmp->next, i++) {
		EXPECT_TRUE(tmp->val == c[i]);
	}
}

TEST(LeetCodeA, lengthOfLongestSubstring) {

	LeetCodeA A;

	int len = A.lengthOfLongestSubstring("b");
	EXPECT_TRUE(len == 1);

	len = A.lengthOfLongestSubstring("abcabcbb");
	EXPECT_TRUE(len == 3);

	len = A.lengthOfLongestSubstring("bbbbb");
	EXPECT_TRUE(len == 1);

	len = A.lengthOfLongestSubstring("pwwkew");
	EXPECT_TRUE(len == 3);

	len = A.lengthOfLongestSubstring("abcdef");
	EXPECT_TRUE(len == 6);
}

TEST(LeetCodeA, findMedianSortedArrays) {
	LeetCodeA A;
	vector<int> nums1 = {1, 3};
	vector<int> nums2 = {2};
	//EXPECT_EQ(A.findMedianSortedArrays(nums1, nums2), 2.0f);

	nums1 = {1, 2};
	nums2 = {3, 4};
	//EXPECT_EQ(A.findMedianSortedArrays(nums1, nums2), 2.5f);

}

//5
TEST(LeetCodeA, longestPalindrome) {

}

TEST(LeetCodeA, convert) {
	LeetCodeA A;
	EXPECT_EQ(A.convert(string(""), 1).compare(""), 0);
	EXPECT_EQ(A.convert(string("A"), 2).compare("A"), 0);
	EXPECT_EQ(A.convert(string("PAYPALISHIRING"), 1).compare("PAYPALISHIRING"), 0);
	EXPECT_EQ(A.convert(string("PAYPALISHIRING"), 3).compare("PAHNAPLSIIGYIR"), 0);
	EXPECT_EQ(A.convert(string("PAYPALISHIRING"), 5).compare("PHASIYIRPLIGAN"), 0);
}

TEST(LeetCodeA, reverseInteger) {
	LeetCodeA A;
	EXPECT_EQ(A.reverseInteger(123), 321);
	EXPECT_EQ(A.reverseInteger(-1234), -4321);
	EXPECT_EQ(A.reverseInteger(1534236469), 0);
}

TEST(LeetCodeA, myAtoi) {

	LeetCodeA A;
	EXPECT_EQ(A.myAtoi("1234"), 1234);
	EXPECT_EQ(A.myAtoi("     010"), 10);
	EXPECT_EQ(A.myAtoi("     1234"), 1234);
	EXPECT_EQ(A.myAtoi("     +1234"), 1234);
	EXPECT_EQ(A.myAtoi("-1234"), -1234);
	EXPECT_EQ(A.myAtoi("-21474836489"), INT_MIN);
	EXPECT_EQ(A.myAtoi("21474836478"), INT_MAX);
	EXPECT_EQ(A.myAtoi("2147483648"), INT_MAX);
	EXPECT_EQ(A.myAtoi("-2147483648"), INT_MIN);
	EXPECT_EQ(A.myAtoi("123  456"), 123);
}

TEST(LeetCodeA, isPalindrome) {
	LeetCodeA A;
	EXPECT_TRUE(A.isPalindrome(12321));
	EXPECT_TRUE(A.isPalindrome(1));
	EXPECT_TRUE(A.isPalindrome(0));
	EXPECT_FALSE(A.isPalindrome(10));
	EXPECT_FALSE(A.isPalindrome(10234));
}

//10
TEST(LeetCodeA, isMatch) {

}

TEST(LeetCodeA, maxArea) {
	LeetCodeA A;
	vector<int> v = {1, 3, 4, 3};
	EXPECT_EQ(A.maxArea(v), 6);
}

TEST(LeetCodeA, intToRoman) {
	LeetCodeA A;
	EXPECT_EQ(A.intToRoman(8).compare("VIII"), 0);
	EXPECT_EQ(A.intToRoman(400).compare("CD"), 0);
	EXPECT_EQ(A.intToRoman(1437).compare("MCDXXXVII"), 0);
	EXPECT_EQ(A.intToRoman(3333).compare("MMMCCCXXXIII"), 0);
	EXPECT_EQ(A.intToRoman(1996).compare("MCMXCVI"), 0);
	EXPECT_EQ(A.intToRoman(2000).compare("MM"), 0);
	EXPECT_EQ(A.intToRoman(1).compare("I"), 0);
}

TEST(LeetCodeA, romanToInt) {
	LeetCodeA A;
	EXPECT_EQ(A.romanToInt("VIII"), 8);
	EXPECT_EQ(A.romanToInt("CD"), 400);
	EXPECT_EQ(A.romanToInt("MCDXXXVII"), 1437);
	EXPECT_EQ(A.romanToInt("MMMCCCXXXIII"), 3333);
	EXPECT_EQ(A.romanToInt("MCMXCVI"), 1996);
}

TEST(LeetCodeA, longestCommonPrefix) {
	LeetCodeA A;
	vector<string> v;
	string str = A.longestCommonPrefix(v);

	v.push_back("1234545");
	v.push_back("123");
	v.push_back("123455");
	v.push_back("12345");
	EXPECT_TRUE(A.longestCommonPrefix(v).compare("123") == 0);
}

//15
TEST(LeetCodeA, threeSum) {
	LeetCodeA A;
	vector<int> v = {-1, 0, 1, 2, -1, -4};
	vector<vector<int> > result = A.threeSum(v);
	vector<vector<int> > answer = {{-1, -1, 2}, {-1, 0, 1}};
	EXPECT_EQ(result.size(), answer.size());
	EXPECT_TRUE(equal(result.begin(), result.end(), answer.begin()));
}

TEST(LeetCodeA, threeSumClosest) {
	LeetCodeA A;
	vector<int> v = {-1, 2, 1, -4};
	EXPECT_EQ(A.threeSumClosest(v, 1), 2);


	vector<int> v2 = {1,2,4,8,16,32,64,128};
	EXPECT_EQ(A.threeSumClosest(v2, 82), 82);
}

TEST(LeetCodeA, letterCombinations) {
	LeetCodeA A;
	vector<string> result = A.letterCombinations("23");
	for(int i = 0; i < result.size(); i ++) {
		cout << "[" << i <<"]:" << result[i] << endl;
	}
}

TEST(LeetCodeA, fourSum) {
	vector<int> v = {1, 0, -1, 0, -2, 2};
	LeetCodeA A;
	vector<vector<int> > result = A.fourSum(v, 0);
	vector<vector<int> > answer = {{-2, -1, 1, 2}, {-2,  0, 0, 2}, {-1,  0, 0, 1}};
	EXPECT_EQ(result.size(), answer.size());
	EXPECT_TRUE(equal(result.begin(), result.end(), answer.begin()));
}

TEST(LeetCodeA, removeNthFromEnd) {
	LeetCodeA A;
	const int a[] = {2, 3, 4, 4, 5, 6};
	const int _6th[] = {3, 4, 4, 5, 6};
	const int _1st[] = {3, 4, 4, 5};

	ListNode *head = new ListNode(0);

	int num = sizeof(a)/sizeof(a[0]);
	ListNode *cur = head;
	for(size_t i = 0; i < num; i ++) {
		cur->next = new ListNode(a[i]);
		cur = cur->next;
	}
	int i = 0;
	cur = A.removeNthFromEnd(head->next, num);
	for(ListNode *tmp = cur; tmp != NULL; tmp = tmp->next, i++) {
		std::cout << "val:" << tmp->val << ", " << i << " is " << _6th[i] << endl;
		EXPECT_TRUE(tmp->val == _6th[i]);
	}
	i = 0;
	cur = A.removeNthFromEnd(cur, 1);
	for(ListNode *tmp = cur; tmp != NULL; tmp = tmp->next, i++) {
		std::cout << "val:" << tmp->val << ", " << i << " is " << _1st[i] << endl;
		EXPECT_TRUE(tmp->val == _1st[i]);
	}
}

//20
TEST(LeetCodeA, isValidParentheses) {
	LeetCodeA A;
	long startTime = getNowUs();
	EXPECT_EQ(A.isValid("[(]{}"), false);
	EXPECT_EQ(A.isValid("[]{}"), true);
	EXPECT_EQ(A.isValid("[]{"), false);
	EXPECT_EQ(A.isValid("[([( )])]"), true);
	std::cout << "running time is " << getNowUs() - startTime << endl;

}

TEST(LeetCodeA, mergeTwoLists) {
	LeetCodeA A;
	const int a[] = {2, 3, 4};
	const int b[] = {4, 5, 6};
	const int c[] = {2, 3, 4, 4, 5, 6};

	ListNode *head1 = new ListNode(0);
	ListNode *head2 = new ListNode(0);

	ListNode *cur = head1;
	for(size_t i = 0; i < sizeof(a)/sizeof(a[0]); i ++) {
		cur->next = new ListNode(a[i]);
		cur = cur->next;
	}

	cur = head2;
	for(size_t i = 0; i < sizeof(b)/sizeof(b[0]); i ++) {
		cur->next = new ListNode(b[i]);
		cur = cur->next;
	}

	ListNode *result = A.mergeTwoLists(head1->next, head2->next);
	int i = 0;
	for(ListNode *tmp = result; tmp != NULL; tmp = tmp->next, i++) {
		EXPECT_TRUE(tmp->val == c[i]);
	}
}

TEST(LeetCodeA, generateParenthesis) {
	LeetCodeA A;
	vector<string> str = A.generateParenthesis(3);
	vector<string> answer = {"()()()", "()(())", "(())()", "(()())", "((()))"};
	for(int i = 0; i < str.size(); i ++) {
		cout << i << ":" <<  str.at(i) << endl;
	}
	EXPECT_EQ(str.size(), answer.size());
	EXPECT_TRUE(equal(str.begin(), str.end(), answer.begin()));
}

TEST(LeetCodeA, mergeKLists) {
}

TEST(LeetCodeA, swapPairs) {
	LeetCodeA A;
	const int a[] = {1, 2, 3, 4, 5};
	const int b[] = {2, 1, 4, 3, 5};
	ListNode *head = new ListNode(0);

	int num = sizeof(a)/sizeof(a[0]);
	ListNode *cur = head;
	for(size_t i = 0; i < num; i ++) {
		cur->next = new ListNode(a[i]);
		cur = cur->next;
	}

	int i = 0;
	cur = A.swapPairs(head->next);
	for(ListNode *tmp = cur; tmp != NULL; tmp = tmp->next, i++) {
		std::cout << "val:" << tmp->val << " vs " << b[i] << endl;
		EXPECT_TRUE(tmp->val == b[i]);
	}
}

//25
TEST(LeetCodeA, reverseKGroup) {
	LeetCodeA A;
	ListNode head(0);
	ListNode *cur = &head;
	for(int i = 0; i < 5; i ++) {
		cur->next = new ListNode(i + 1);
		cur = cur->next;
	}
	cur = A.reverseKGroup(head.next, 2);
}

TEST(LeetCodeA, removeDuplicates) {
	LeetCodeA A;
	int array[] = {1, 1, 2};
	int newarray[] = {1, 2};
	std::vector<int> nums;
	nums.assign(array, array + sizeof(array)/sizeof(array[0]));
	int len = A.removeDuplicates(nums);
	EXPECT_EQ(len, sizeof(newarray)/sizeof(newarray[0]));

	EXPECT_TRUE(equal(nums.begin(), nums.begin() + len,
			vector<int>(newarray, newarray + sizeof(newarray)/sizeof(newarray[0])).begin()));
}

TEST(LeetCodeA, removeElement) {
	LeetCodeA A;
	int array[] = {3,2,2,3};
	int newarray[] = {2, 2};
	std::vector<int> nums;
	nums.assign(array, array + sizeof(array)/sizeof(array[0]));
	int len = A.removeElement(nums, 3);
	EXPECT_EQ(len, sizeof(newarray)/sizeof(newarray[0]));

	EXPECT_TRUE(equal(nums.begin(), nums.begin() + len,
			vector<int>(newarray, newarray + sizeof(newarray)/sizeof(newarray[0])).begin()));

	nums = {2};
	int newarray1[] = {2};

	len = A.removeElement(nums, 3);
	EXPECT_EQ(len, sizeof(newarray1)/sizeof(newarray1[0]));

	EXPECT_TRUE(equal(nums.begin(), nums.begin() + len,
			vector<int>(newarray1, newarray1 + sizeof(newarray1)/sizeof(newarray1[0])).begin()));
}

TEST(LeetCodeA, strStr) {
	LeetCodeA A;
	string str = "adfjaldjflajdf;adf";
	string s = "jaldjflajdf;adf";
	EXPECT_EQ(A.strStr(str, s), 3);

	EXPECT_EQ(A.strStr("", ""), 0);
	EXPECT_EQ(A.strStr("adfja", "adfadfadf"), -1);
}

TEST(LeetCodeA, divideIntegers) {
	LeetCodeA A;
	EXPECT_EQ(A.divideIntegers(10, 0), INT_MAX);
	EXPECT_EQ(A.divideIntegers(10, 1), 10);
	EXPECT_EQ(A.divideIntegers(10, 3), 3);
	EXPECT_EQ(A.divideIntegers(10, -3), -3);
	EXPECT_EQ(A.divideIntegers(-10, -3), 3);
	EXPECT_EQ(A.divideIntegers(-10, 3), -3);
}

//30
TEST(LeetCodeA, findSubstring) {

}

TEST(LeetCodeA, nextPermutation) {
	LeetCodeA A;
	vector<int> v1 = {1,2,3};
	int v1p[] = {1, 3, 2};
	A.nextPermutation(v1);
	ostream_iterator<int> out(cout, " ");
	copy(v1.begin(), v1.end(), out);
	cout << endl;

	EXPECT_TRUE(std::equal(v1.begin(), v1.end(), v1p));

	vector<int> v2 = {3, 2, 1};
	vector<int> v2p = {1,2,3};
	A.nextPermutation(v2);
	EXPECT_TRUE(equal(v2.begin(), v2.end(), v2p.begin()));

	vector<int> v3 = {1,1,5};
	vector<int> v3p = {1,5,1};
	A.nextPermutation(v3);
	EXPECT_TRUE(equal(v3.begin(), v3.end(), v3p.begin()));
}

TEST(LeetCodeA, search) {
	LeetCodeA A;
	vector<int> v = {4, 5, 6, 7, 0, 1, 2};
	EXPECT_EQ(A.search(v, 5), 1);
	EXPECT_EQ(A.search(v, 1), 5);
	EXPECT_EQ(A.search(v, 8), -1);
	v.clear();
	EXPECT_EQ(A.search(v, 8), -1);
}

TEST(LeetCodeA, searchRange) {
	LeetCodeA A;
	vector<int> v = {5, 7, 7, 8, 8, 10};
	vector<int> r = A.searchRange(v, 8);
	vector<int> a = {3, 4};
	dumpVector(" ", r);
	EXPECT_TRUE(equal(r.begin(), r.end(), a.begin()));
	a = {-1, -1};
	r = A.searchRange(v, 100);
	dumpVector(" ", r);
	EXPECT_TRUE(equal(r.begin(), r.end(), a.begin()));
}

//35
TEST(LeetCodeA, searchInsert) {
	LeetCodeA A;
	vector<int> v = {1,3,5,6};
	EXPECT_EQ(A.searchInsert(v, 5), 2);
	EXPECT_EQ(A.searchInsert(v, 2), 1);
	EXPECT_EQ(A.searchInsert(v, 7), 4);
	EXPECT_EQ(A.searchInsert(v, 0), 0);
	vector<int> v2 = {1,3};
	EXPECT_EQ(A.searchInsert(v2, 2), 1);
}

TEST(LeetCodeA, countAndSay) {
	LeetCodeA A;
	EXPECT_TRUE(A.countAndSay(0).empty());
	EXPECT_EQ(A.countAndSay(1).compare("1"), 0);
	EXPECT_EQ(A.countAndSay(5).compare("111221"), 0);
	EXPECT_EQ(A.countAndSay(6).compare("312211"), 0);
}

TEST(LeetCodeA, isWildcardMatch) {

}

//45
TEST(LeetCodeA, jump) {

}

TEST(LeetCodeA, permute) {
	LeetCodeA A;
	vector<int> nums = {1,2,3};
	vector<vector<int> > result = A.permute(nums);
	EXPECT_EQ(result.size(), 6);
	for(int i = 0; i < result.size(); i ++) {
		vector<int> permutations = result.at(i);
		for(int j = 0; j < permutations.size(); j ++) {
			cout << permutations.at(j) << " " ;
		}
		cout << endl;
	}
	cout << endl;
}

TEST(LeetCodeA, permuteUnique) {
	LeetCodeA A;
	vector<int> nums = {1,1,2};
	vector<vector<int> > result = A.permuteUnique(nums);
	for(int i = 0; i < result.size(); i ++) {
		vector<int> permutations = result.at(i);
		for(int j = 0; j < permutations.size(); j ++) {
			cout << permutations.at(j) << " " ;
		}
		cout << endl;
	}
	cout << endl;
}

TEST(LeetCodeA, rotate) {
	LeetCodeA A;
	int n = 5;
	vector<vector<int> > answer;
	vector<vector<int> > matrix;
	for(int i = 0; i < n; i ++) {
		vector<int> row;
		for(int j = n - 1; j >= 0; j--) {
			row.push_back(j);
		}
		answer.push_back(row);
		matrix.push_back(vector<int>(n, i));
	}
	for(int i = 0; i < n; i ++) {
		answer.push_back(vector<int>(n, i));
	}
	A.rotate(matrix);

	EXPECT_TRUE(equal(matrix.begin(), matrix.end(), answer.begin()));
}


