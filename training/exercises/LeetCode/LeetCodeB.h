/*
 * LeetCodeB.h
 *
 *  Created on: Jul 27, 2017
 *      Author: sheldon
 */

#ifndef TRAINING_EXERCISES_LEETCODE_LEETCODEB_H_
#define TRAINING_EXERCISES_LEETCODE_LEETCODEB_H_

#include "LeetCodeDataStructure.h"


class LeetCodeB {
public:
    //53. Maximum Subarray
    //Find the contiguous subarray within an array (containing at least one number) which has the largest sum.
    int maxSubArray(vector<int>& nums);

    //58. Length of Last Word
    //Given a string s consists of upper/lower-case alphabets and empty space characters ' ', return the length of last word in the string.
    //If the last word does not exist, return 0.
    int lengthOfLastWord(string s);

    //61. Rotate List
    //Given a list, rotate the list to the right by k places, where k is non-negative.
    ListNode* rotateRight(ListNode* head, int k);

    //66. Plus One
    //Given a non-negative integer represented as a non-empty array of digits, plus one to the integer.
    //You may assume the integer do not contain any leading zero, except the number 0 itself.
    //The digits are stored such that the most significant digit is at the head of the list.
    vector<int> plusOne(vector<int>& digits);

    //100. Same Tree
    //Given two binary trees, write a function to check if they are equal or not.
    //Two binary trees are considered equal if they are structurally identical and the nodes have the same value.
    bool isSameTree(TreeNode* p, TreeNode* q);

    //101. Symmetric Tree
    //Given a binary tree, check whether it is a mirror of itself (ie, symmetric around its center).
    bool isSymmetric(TreeNode* root);

    //102. Binary Tree Level Order Traversal
    //Given a binary tree, return the level order traversal of its nodes' values. (ie, from left to right, level by level).
    vector<vector<int>> levelOrder(TreeNode* root);

    //107. Binary Tree Level Order Traversal II
    //Given a binary tree, return the bottom-up level order traversal of its nodes' values. (ie, from left to right, level by level from leaf to root).
    vector<vector<int>> levelOrderBottom(TreeNode* root);
private:
};

#endif /* TRAINING_EXERCISES_LEETCODE_LEETCODEB_H_ */
