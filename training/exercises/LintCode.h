/*
 * LintCode.h
 *
 *  Created on: Jun 29, 2017
 *      Author: sheldon
 */

#ifndef LINTCODE_H_
#define LINTCODE_H_

class LintCode {
public:
	LintCode();
	~LintCode();
    /*
     * param k : As description.
     * param n : As description.
     * return: How many k's between 0 and n.
     */
    int digitCounts(int k, int n);

    // param n : description of n
    // return: description of return
    long trailingZeros(long n);

    /*
     * @param a: The first integer
     * @param b: The second integer
     * @return: The sum of a and b
     */
    int aplusb(int a, int b);

private:
	LintCode(const LintCode&);
	LintCode& operator=(const LintCode&);
};


#endif /* LINTCODE_H_ */
