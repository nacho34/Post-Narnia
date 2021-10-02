/*
 * TODO: remove and replace this file header comment
 * This is a .cpp file you will edit and turn in.
 * Remove starter comments and add your own
 * comments on each function and on complex code sections.
 */
#include "testing/SimpleTest.h"
#include "strlib.h"
#include "filelib.h"
#include "simpio.h"
#include <fstream>
#include <cctype>
#include <string>
#include "vector.h"
using namespace std;

/* This function returns a string which
 * contains only the letter characters from the original. Formerly, there
 * was a bug where the first character would remain included even if it was
 * not a letter. However, this has been fixed to remove all non-letter characters
 * regardless of position within the string.
 */
string removeNonLetters(string s) {
    string result = "";
    for (int i = 0; i < s.length(); i++) {
        if (isalpha(s[i])) {
            result += s[i];
        }
    }
    return result;
}

/*
 * returns the result of converting every character in s into a digit according to the soundex table
 */
string getDigits(string s) {
    //soundex replaces a given character with the index of the string in this vector containing it
    const Vector<string> conversion = {"AEIOUHWY","BFPV","CGJKQSXZ","DT","L","MN","R"};
    string inputName = toUpperCase(s);
    string digits = "";
    for(char currentChar : inputName){
        for(int i = 0; i < conversion.size(); i++) {
            for(char key : conversion[i]) {
                if(currentChar == key) {
                    digits += integerToString(i);
                }
            }
        }
    }
    return digits;
}

/*
 * returns the result of removing any repeated digits from a string of digits; i.e. 0033335555 would become 035
 */
string coalesce(string s) {
    string coalesced = charToString(s[0]);
    char currentChar = s[0];
    for(int i = 1; i < s.length(); i++) {
        if(s[i] != currentChar) {
            coalesced += charToString(s[i]);
            currentChar = s[i];
        }
    }
    return coalesced;
}

/*
 * Written for soundex algorithm. If s has length less than 4, the function adds 0s to the end. If s has length greater than 4, the function truncates ending digits.
 */
string lengthFour(string s) {
    string fourLong = s;
    if(fourLong.length() < 4) {
        while(fourLong.length() < 4) {
            fourLong += "0";
        }
    } else if(fourLong.length() > 4) {
        fourLong = fourLong.substr(0,4);
    }
    return fourLong;
}

/*
 * Implements the historically-significant Soundex algorithm on the string s:
 * -discards non-letter characters
 * -encodes each letter as a digit
 * -combines repeated digits into a single digit
 * -replaces the first digit with the first letter of the name, uppercase
 * -removes all zeroes from the code
 * -changes the code to length four by concatenating zeroes if it is too short, or by truncating it if too long
 * The resultant code will be returned as something like C600 or L646.
 */
string soundex(string s) {
    string soundexed = removeNonLetters(s);
    soundexed = getDigits(soundexed);
    soundexed = coalesce(soundexed);
    soundexed[0] = toupper(s[0]);
    soundexed = stringReplace(soundexed,"0","");
    soundexed = lengthFour(soundexed);
    return soundexed;
}


/*
 * Reads the file at filepath into a vector of names. Converts that vector into a vector of soundex codes. Asks the user to enter a name, and displays all names
 * in the database with a matching soundex code. Repeats this process until the user asks to stop.
 */
void soundexSearch(string filepath) {
    // The proivded code opens the file with the given name
    // and then reads the lines of that file into a vector.
    ifstream in;
    Vector<string> databaseNames;

    if (openFile(in, filepath)) {
        readEntireFile(in, databaseNames);
    }
    cout << "Read file " << filepath << ", "
         << databaseNames.size() << " names found." << endl;

    // The names in the database are now stored in the provided
    // vector named databaseNames

    Vector<string> soundexNames;
    for(string name : databaseNames) {
        soundexNames.add(soundex(name));
    }

    while(true) {
        string name = soundex(getLine("Enter a name: "));
        Vector<string> candidates;
        for(int i = 0; i < soundexNames.size(); i++) {
            if(soundexNames[i] == name) {
                candidates.add(databaseNames[i]);
            }
        }
        candidates.sort();
        cout << "the following names are potential phonetic matches for the entered name: " << candidates.toString() << endl;
        string continuing = getLine("Would you like to continue soundex searching? type 'no' to end search, otherwise search will continue");
        if(continuing == "no") {break;}
    }

    /* TODO: Fill in the remainder of this function. */
}

STUDENT_TEST("DOES IT WORK? (yes)") {
    soundexSearch("res/surnames.txt");
}

/* * * * * * Test Cases * * * * * */
STUDENT_TEST("soundex test cases") {
    EXPECT_EQUAL(soundex("larlarb"),"L646");
}

STUDENT_TEST("test changing length of string to four") {
    EXPECT_EQUAL(lengthFour("1987345"),"1987");
    EXPECT_EQUAL(lengthFour("3"),"3000");
}

STUDENT_TEST("test conversion of a string of letters to digits") {
    EXPECT_EQUAL(getDigits("curie"),"20600");
    EXPECT_EQUAL(getDigits("Larlarb"),"4064061");
    EXPECT_EQUAL(getDigits("Angelou"),"0520400");
}

STUDENT_TEST("test coalescence of string of digits into non-repetitive string") {
    EXPECT_EQUAL(coalesce("20600"),"2060");
    EXPECT_EQUAL(coalesce("4064061"),"4064061");
    EXPECT_EQUAL(coalesce("10033335555010111"),"10350101");
}

PROVIDED_TEST("Test removing puntuation, digits, and spaces") {
    string s = "O'Hara";
    string result = removeNonLetters(s);
    EXPECT_EQUAL(result, "OHara");
    s = "Planet9";
    result = removeNonLetters(s);
    EXPECT_EQUAL(result, "Planet");
    s = "tl dr";
    result = removeNonLetters(s);
    EXPECT_EQUAL(result, "tldr");
    s = "/this used to fail due to issue removing first char";
    result = removeNonLetters(s);
    EXPECT_EQUAL(result, "thisusedtofailduetoissueremovingfirstchar");
}


PROVIDED_TEST("Sample inputs from handout") {
    EXPECT_EQUAL(soundex("Curie"), "C600");
    EXPECT_EQUAL(soundex("O'Conner"), "O256");
}

PROVIDED_TEST("hanrahan is in lowercase") {
    EXPECT_EQUAL(soundex("hanrahan"), "H565");
}

PROVIDED_TEST("DRELL is in uppercase") {
    EXPECT_EQUAL(soundex("DRELL"), "D640");
}

PROVIDED_TEST("Liu has to be padded with zeros") {
    EXPECT_EQUAL(soundex("Liu"), "L000");
}

PROVIDED_TEST("Tessier-Lavigne has a hyphen") {
    EXPECT_EQUAL(soundex("Tessier-Lavigne"), "T264");
}

PROVIDED_TEST("Au consists of only vowels") {
    EXPECT_EQUAL(soundex("Au"), "A000");
}

PROVIDED_TEST("Egilsdottir is long and starts with a vowel") {
    EXPECT_EQUAL(soundex("Egilsdottir"), "E242");
}

PROVIDED_TEST("Jackson has three adjcaent duplicate codes") {
    EXPECT_EQUAL(soundex("Jackson"), "J250");
}

PROVIDED_TEST("Schwarz begins with a pair of duplicate codes") {
    EXPECT_EQUAL(soundex("Schwarz"), "S620");
}

PROVIDED_TEST("Van Niekerk has a space between repeated n's") {
    EXPECT_EQUAL(soundex("Van Niekerk"), "V526");
}

PROVIDED_TEST("Wharton begins with Wh") {
    EXPECT_EQUAL(soundex("Wharton"), "W635");
}

PROVIDED_TEST("Ashcraft is not a special case") {
    // Some versions of Soundex make special case for consecutive codes split by hw
    // We do not make this special case, just treat same as codes split by vowel
    EXPECT_EQUAL(soundex("Ashcraft"), "A226");
}
