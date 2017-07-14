/*
 * Solution.h
 *
 *	https://leetcode.com/problemset/algorithms/
 *  Created on: May 9, 2017
 *      Author: sheldon
 */

#ifndef LEETCODE_H_
#define LEETCODE_H_
#include "LeetCodeDataStructure.h"

class LeetCodeA {
public:

    //1.Two Sum
	//Given an array of integers, return indices of the two numbers such that they
	//add up to a specific target.You may assume that each input would have exactly
	//one solution, and you may not use the same element twice.
    std::vector<int> twoSum(std::vector<int>& nums, int target);

    //2.Add Two Numbers
    //You are given two non-empty linked lists representing two non-negative integers.
    //The digits are stored in reverse order and each of their nodes contain a single digit.
    //Add the two numbers and return it as a linked list.
    //You may assume the two numbers do not contain any leading zero, except the number 0 itself.
    ListNode* addTwoNumbers(ListNode* l1, ListNode* l2);

    //3.Longest Substring Without Repeating Characters
    //Given a std::string, find the length of the longest substring without repeating characters.
    int lengthOfLongestSubstring(std::string s);

    //4.Median of Two Sorted Arrays
    //Find the median of the two sorted arrays.
    //The overall run time complexity should be O(log (m+n)).
    double findMedianSortedArrays(std::vector<int>& nums1, std::vector<int>& nums2);

    //5.Longest Palindromic Substring
    //Given a string s, find the longest palindromic substring in s.
    //You may assume that the maximum length of s is 1000.
    string longestPalindrome(string s);

    //6.ZigZag Conversion
    string convert(string s, int numRows);

    //7.Reverse Integer
    //Reverse digits of an integer.
    int reverseInteger(int x);

    //8.String to Integer (atoi)
    //Implement atoi to convert a string to an integer.
    int myAtoi(string str);

    //9.Palindrome Number
    //Determine whether an integer is a palindrome. Do this without extra space.
    bool isPalindrome(int x);

    //10. Regular Expression Matching
    //Implement regular expression matching with support for '.' and '*'.
    bool isRegularExpressionMatch(string s, string p);

    //11. Container With Most Water
    //Given n non-negative integers a1, a2, ..., an, where each represents a point
    //at coordinate (i, ai). n vertical lines are drawn such that the two endpoints
    //of line i is at (i, ai) and (i, 0). Find two lines, which together with x-axis
    //forms a container, such that the container contains the most water.
    int maxArea(vector<int>& height);

    //12. Integer to Roman
    //Given an integer, convert it to a roman numeral.
    string intToRoman(int num);

    //13.Roman to Integer
    //Given a roman numeral, convert it to an integer.
    int romanToInt(string s);

    //14. Longest Common Prefix
    //Write a function to find the longest common prefix string amongst an array of strings.
    string longestCommonPrefix(vector<string>& strs);

    //15. 3Sum
    //Given an array S of n integers, are there elements a, b, c in S such that a + b + c = 0?
    //Find all unique triplets in the array which gives the sum of zero.
    vector<vector<int>> threeSum(vector<int>& nums);

    //16. 3Sum Closest
    //Given an array S of n integers, find three integers in S such that the sum is closest to
    //a given number, target. Return the sum of the three integers. You may assume that each
    //input would have exactly one solution.
    int threeSumClosest(vector<int>& nums, int target);

    //17. Letter Combinations of a Phone Number
    //Given a digit string, return all possible letter combinations that the number could represent.
    vector<string> letterCombinations(string digits);

    //18. 4Sum
    //Given an array S of n integers, are there elements a, b, c, and d in S such that a + b + c + d = target?
    //Find all unique quadruplets in the array which gives the sum of target
    vector<vector<int>> fourSum(vector<int>& nums, int target);

    //19. Remove Nth Node From End of List
    //Given a linked list, remove the nth node from the end of list and return its head.
    ListNode* removeNthFromEnd(ListNode* head, int n);

    //20. Valid Parentheses
    bool isValid(string s);

    //21. Merge Two Sorted Lists
    //Merge two sorted linked lists and return it as a new list. The new list should be made
    //by splicing together the nodes of the first two lists.
    ListNode* mergeTwoLists(ListNode* l1, ListNode* l2);

    //22. Generate Parentheses
    //Given n pairs of parentheses, write a function to generate all combinations of well-formed parentheses
    vector<string> generateParenthesis(int n);

    //23. Merge k Sorted Lists
    //Merge k sorted linked lists and return it as one sorted list. Analyze and describe its complexity.
    ListNode* mergeKLists(vector<ListNode*>& lists);

    //24. Swap Nodes in Pairs
    //Given a linked list, swap every two adjacent nodes and return its head.
    ListNode* swapPairs(ListNode* head);

    //25. Reverse Nodes in k-Group
    //Given a linked list, reverse the nodes of a linked list k at a time and return its modified list.
    ListNode* reverseKGroup(ListNode* head, int k);

    //26. Remove Duplicates from Sorted Array
    //Given a sorted array, remove the duplicates in place such that each element appear
    //only once and return the new length.Do not allocate extra space for another array,
    //you must do this in place with constant memory.
    int removeDuplicates(vector<int>& nums);

    //27. Remove Element
    //Given an array and a value, remove all instances of that value in place and return the new length.
    //Do not allocate extra space for another array, you must do this in place with constant memory.
    //The order of elements can be changed. It doesn't matter what you leave beyond the new length.
    int removeElement(vector<int>& nums, int val);

    //28. Implement strStr()
    //Implement strStr().
    //Returns the index of the first occurrence of needle in haystack,
    //or -1 if needle is not part of haystack.
    int strStr(string haystack, string needle);

    //29. Divide Two Integers
    //Divide two integers without using multiplication, division and mod operator.
    int divideIntegers(int dividend, int divisor);

    //30. Substring with Concatenation of All Words
    //You are given a string, s, and a list of words, words, that are all of the same length.
    //Find all starting indices of substring(s) in s that is a concatenation of each word in
    //words exactly once and without any intervening characters.
    vector<int> findSubstring(string s, vector<string>& words);

    //31. Next Permutation
    //Implement next permutation, which rearranges numbers into the lexicographically
    //next greater permutation of numbers. If such arrangement is not possible, it must
    //rearrange it as the lowest possible order (ie, sorted in ascending order).
    //The replacement must be in-place, do not allocate extra memory.
    void nextPermutation(vector<int>& nums);

    //32. Longest Valid Parentheses
    //Given a string containing just the characters '(' and ')', find the length of the
    //longest valid (well-formed) parentheses substring. For "(()", the longest valid
    //parentheses substring is "()", which has length = 2. Another example is ")()())",
    //where the longest valid parentheses substring is "()()", which has length = 4.
    int longestValidParentheses(string s);

    //33. Search in Rotated Sorted Array
    //Suppose an array sorted in ascending order is rotated at some pivot unknown to you beforehand.
    //(i.e., 0 1 2 4 5 6 7 might become 4 5 6 7 0 1 2).
    //You are given a target value to search. If found in the array return its index, otherwise return -1.
    int search(vector<int>& nums, int target);

    //34. Search for a Range
    //Given an array of integers sorted in ascending order, find the starting and ending position of
    //a given target value. Your algorithm's runtime complexity must be in the order of O(log n).
    //If the target is not found in the array, return [-1, -1].
    vector<int> searchRange(vector<int>& nums, int target);

    //35. Search Insert Position
    //Given a sorted array and a target value, return the index if the target is found.
    //If not, return the index where it would be if it were inserted in order.
    int searchInsert(vector<int>& nums, int target);

    //36. Valid Sudoku
    //Determine if a Sudoku is valid, according to: Sudoku Puzzles - The Rules.
    //The Sudoku board could be partially filled, where empty cells are filled with the character '.'.
    bool isValidSudoku(vector<vector<char>>& board);

    //37. Sudoku Solver
    //Write a program to solve a Sudoku puzzle by filling the empty cells.
    //Empty cells are indicated by the character '.'.
    //You may assume that there will be only one unique solution.
    void solveSudoku(vector<vector<char>>& board);

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
    string countAndSay(int n);

    //39. Combination Sum
    //Given a set of candidate numbers (C) (without duplicates) and a target number (T),
    //find all unique combinations in C where the candidate numbers sums to T.
    //The same repeated number may be chosen from C unlimited number of times.
    vector<vector<int>> combinationSum(vector<int>& candidates, int target);

    //40. Combination Sum II
    //Given a collection of candidate numbers (C) and a target number (T),
    //find all unique combinations in C where the candidate numbers sums to T.
    //Each number in C may only be used once in the combination.
    //Note:
    //All numbers (including target) will be positive integers.
    //The solution set must not contain duplicate combinations.
    vector<vector<int>> combinationSum2(vector<int>& candidates, int target);

    //41. First Missing Positive
    //Given an unsorted integer array, find the first missing positive integer.
    int firstMissingPositive(vector<int>& nums);

    //42. Trapping Rain Water
    //Given n non-negative integers representing an elevation map where the width of each bar is 1,
    //compute how much water it is able to trap after raining.
    int trap(vector<int>& height);

    //43. Multiply Strings
    //Given two non-negative integers num1 and num2 represented as strings, return the product of num1 and num2.
    //The length of both num1 and num2 is < 110.
    //Both num1 and num2 contains only digits 0-9.
    //Both num1 and num2 does not contain any leading zero.
    //You must not use any built-in BigInteger library or convert the inputs to integer directly.
    string multiply(string num1, string num2);

    //44. Wildcard Matching
    //Implement wildcard pattern matching with support for '?' and '*'.
    bool isWildcardMatch(string s, string p);

    //45. Jump Game II
    //Given an array of non-negative integers, you are initially positioned at the first index of the array.
    //Each element in the array represents your maximum jump length at that position.
    //Your goal is to reach the last index in the minimum number of jumps.
    int jump(vector<int>& nums);

    //46.Permutations
    //Given a collection of distinct numbers, return all possible permutations.
    vector<vector<int> > permute(vector<int>& nums);

    //47.Permutations II
    //Given a collection of numbers that might contain duplicates, return all possible unique permutations.
    vector<vector<int> > permuteUnique(vector<int>& nums);

    //48. Rotate Image
    //You are given an n x n 2D matrix representing an image.
    //Rotate the image by 90 degrees (clockwise).
    void rotate(vector<vector<int>>& matrix);

    //49. Group Anagrams
    //Given an array of strings, group anagrams together.
    vector<vector<string>> groupAnagrams(vector<string>& strs);

    //50. Pow(x, n)
    //Implement pow(x, n).
    double myPow(double x, int n);
private:
    void generateParenthesis_l(vector<string> &v, string str, int n, int m);
    void permute_l(vector<int>& nums, int start, vector<vector<int> > &result);
    void permuteUnique_l(vector<int> nums, int start, vector<vector<int> > &result);
};


#endif /* LEETCODE_H_ */
