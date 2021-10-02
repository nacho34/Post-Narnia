/*
 * TODO: remove and replace this file header comment
 * This is a .cpp file you will edit and turn in.
 * Remove starter comments and add your own
 * comments on each function and on complex code sections.
 */
#include "console.h"
#include <iostream>
#include "testing/SimpleTest.h"
using namespace std;

/* This function takes one argument `n` and calculates the sum
 * of all proper divisors of `n` excluding itself. To find divisors
 * a loop iterates over all numbers from 1 to n-1, testing for a
 * zero remainder from the division.
 *
 * Note: long is a C++ type is a variant of int that allows for a
 * larger range of values. For all intents and purposes, you can
 * treat it like you would an int.
 */
long divisorSum(long n) {
    long total = 0;
    for (long divisor = 1; divisor < n; divisor++) {
        if (n % divisor == 0) {
            total += divisor;
        }
    }
    return total;
}

/* This function takes one argument `n` and returns a boolean
 * (true/false) value indicating whether or not `n` is perfect.
 * A perfect number is a non-zero positive number whose sum
 * of its proper divisors is equal to itself.
 */
bool isPerfect(long n) {
    return (n != 0) && (n == divisorSum(n));
}

/* This function does an exhaustive search for perfect numbers.
 * It takes one argument `stop` and searches the range 1 to `stop`,
 * checking each number to see whether it is perfect and if so,
 * printing it to the console.
 */
void findPerfects(long stop) {
    for (long num = 1; num < stop; num++) {
        if (isPerfect(num)) {
            cout << "Found perfect number: " << num << endl;
        }
        if (num % 10000 == 0) cout << "." << flush; // progress bar
    }
    cout << endl << "Done searching up to " << stop << endl;
}

/* This function takes one argument `n` and calculates the sum
 * of all proper divisors of `n` excluding itself. To find divisors
 * a loop iterates over all numbers from 1 to sqrt(n), testing for a
 * zero remainder from the division. If a divisor is found, the loop includes the
 * divisor as well as the paired divisor, n/divisor. In the case where sqrt(n)
 * is an integer, it is counted once. Negative integers, 1, and 0 will return 0.
 */
long smarterSum(long n) {
    //returning 0 for these values emulates the behavior of divisorSum
    if(n <= 1) {
        return 0;
    }
    //start with total = 1 since every integer greater than 1 has a valid divisor 1
    long total = 1;
    //this root is truncated to integer form
    long root = sqrt(n);
    //we can start with divisor = 2 here since we already accounted for the divisor = 1 case
    for (long divisor = 2; divisor <= root; divisor++) {
        if (n % divisor == 0) {
            //ensures we don't double count root, in the case that root is a rational root of n
            if(divisor == root && n/root == root) {
                total += root;
            } else {
                total += divisor;
                total += n / divisor;
            }
        }
    }
    return total;
}

/* This function takes one argument `n` and returns a boolean
 * (true/false) value indicating whether or not `n` is perfect.
 * A perfect number is a non-zero positive number whose sum
 * of its proper divisors is equal to itself.
*/
bool isPerfectSmarter(long n) {
     return (n != 0) && (n == smarterSum(n));
}

/* This function does an exhaustive search for perfect numbers.
 * It takes one argument `stop` and searches the range 1 to `stop`,
 * checking each number to see whether it is perfect and if so,
 * printing it to the console.
 */
void findPerfectsSmarter(long stop) {
    for (long num = 1; num < stop; num++) {
        if (isPerfectSmarter(num)) {
            cout << "Found perfect number: " << num << endl;
        }
        if (num % 10000 == 0) cout << "." << flush; // progress bar
    }
    cout << endl << "Done searching up to " << stop << endl;
}

/* This function iterates through k mersenne numbers of form 2^k - 1. If such a number is prime,
 * the function records the value of (2^k - 1) * 2^(k - 1). The function returns the nth such value, which is
 * the nth perfect number. Accepts integer values greater than 0 for n. Can only return perfect nums up to size LONG_MAX
 */
long findNthPerfectEuclid(long n) {
    if(n <= 0) {
        cout << "n must be greater than 0 to find the nth perfect number" << endl;
        return -1;
    }
    int numsFound = 0;
    long perfectNum = 0;
    for(int k = 1; numsFound < n; k++) {
        long mersenneNum = pow(2, k) - 1;
        //if mersseneNum is prime
        if(smarterSum(mersenneNum) == 1) {
            perfectNum = mersenneNum * pow(2, k - 1);
            numsFound++;
        }
    }
    cout << "perfect num from findNthPerfectEuclid(" << n << "): " << perfectNum << endl;
    return perfectNum;
}


/* * * * * * Test Cases * * * * * */

PROVIDED_TEST("Confirm divisorSum of small inputs") {
    EXPECT_EQUAL(divisorSum(1), 0);
    EXPECT_EQUAL(divisorSum(6), 6);
    EXPECT_EQUAL(divisorSum(12), 16);
}

PROVIDED_TEST("Confirm 6 and 28 are perfect") {
    EXPECT(isPerfect(6));
    EXPECT(isPerfect(28));
}

PROVIDED_TEST("Confirm 12 and 98765 are not perfect") {
    EXPECT(!isPerfect(12));
    EXPECT(!isPerfect(98765));
}

PROVIDED_TEST("Test oddballs: 0 and 1 are not perfect") {
    EXPECT(!isPerfect(0));
    EXPECT(!isPerfect(1));
}

PROVIDED_TEST("Confirm 33550336 is perfect") {
    EXPECT(isPerfect(33550336));
}

STUDENT_TEST("Time trials of findPerfects on doubling input sizes") {
    TIME_OPERATION(32500, findPerfects(32500));
    TIME_OPERATION(65000, findPerfects(65000));
    TIME_OPERATION(130000, findPerfects(130000));
    TIME_OPERATION(260000, findPerfects(260000));
}

STUDENT_TEST("Time trials of findPerfectsSmarter on doubling input sizes") {
    TIME_OPERATION(5044864,  findPerfectsSmarter( 1312500));
    TIME_OPERATION(5044864,  findPerfectsSmarter( 2625000));
    TIME_OPERATION(5044864,  findPerfectsSmarter( 5250000));
    TIME_OPERATION(10493739, findPerfectsSmarter(10500000));
}

STUDENT_TEST("Check negative numbers can't be perfect") {
    EXPECT(!isPerfect(-12));
    EXPECT(!isPerfect(-36));
    EXPECT(!isPerfectSmarter(-12));
    EXPECT(!isPerfectSmarter(-36));
}

STUDENT_TEST("Check divisorSum and smarterSum have same behavior") {
    EXPECT_EQUAL(divisorSum(0),smarterSum(0));
    EXPECT_EQUAL(divisorSum(1),smarterSum(1));
    EXPECT_EQUAL(divisorSum(-1),smarterSum(-1));
    EXPECT_EQUAL(divisorSum(-10000),smarterSum(-10000));
    EXPECT_EQUAL(divisorSum(28),smarterSum(28));
    EXPECT_EQUAL(divisorSum(6),smarterSum(6));
    EXPECT_EQUAL(divisorSum(25),smarterSum(25));
    EXPECT_EQUAL(divisorSum(10000),smarterSum(10000));
}

STUDENT_TEST("check findNthPerfectEuclid gives expected values") {
    EXPECT(isPerfectSmarter(findNthPerfectEuclid(1)));
    EXPECT(isPerfectSmarter(findNthPerfectEuclid(2)));
    EXPECT(isPerfectSmarter(findNthPerfectEuclid(3)));
    EXPECT(isPerfectSmarter(findNthPerfectEuclid(4)));
    EXPECT(isPerfectSmarter(findNthPerfectEuclid(5)));
    EXPECT(!isPerfectSmarter(findNthPerfectEuclid(-3)));
}
