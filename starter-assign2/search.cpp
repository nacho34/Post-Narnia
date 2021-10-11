// Contains code for a simple search engine using ADTs. Reads pages from a document and builds an index of urls for search terms. Supports
// using modifier characters such as '+' or '-' to improve search accuracy.

#include <iostream>
#include <fstream>
#include "error.h"
#include "filelib.h"
#include "map.h"
#include "search.h"
#include "set.h"
#include "strlib.h"
#include "vector.h"
#include "queue.h"
#include "testing/SimpleTest.h"
#include "simpio.h"
using namespace std;


/**
 * Returns a cleaned version of s. The returned string has all leading and ending punctuation removed from s. Punctuation is defined by ispunct().
 * The returned string is lower-case. The returned string must have at least one letter, otherwise an empty string is returned.
 */
string cleanToken(string s)
{
    //remove punctuation from front
    for(int i = 0; i < s.length(); i++) {
        if(!ispunct(s[i])) {
            s = s.substr(i,s.length());
            break;
        }
    }
    //remove punctuation from end
    for(int i = s.length()-1; i > 0; i--) {
        if(!ispunct(s[i])) {
            s = s.substr(0,i+1);
            break;
        }
    }
    //check that the string contains letters
    for(char c : s) {
        if(isalpha(c)) {
            return toLowerCase(s);
        }
    }
    return "";
}

/**
 * Takes a string of text and splits it into smaller tokens by spaces. Cleans every token according to cleanToken, and returns a set of unique tokens.
 * Keep in mind that different raw tokens could be identical once clean; these would only be counted once in the returned set.
 */
Set<string> gatherTokens(string text)
{
    Set<string> tokens;
    Vector<string> rawStrings = stringSplit(text, " ");
    for(string raw : rawStrings) {
        string cleaned = cleanToken(raw);
        if(cleaned != "") { //cleanToken() returns an empty string when raw contains no letters
            tokens += cleaned;
        }
    }
    return tokens;
}

/**
 * Builds an inverted index of webpages in dbfile. For each webpage, splits the text into tokens using gatherTokens(). For each token, records the corresponding
 * url into the Map index under index[token].
 */
int buildIndex(string dbfile, Map<string, Set<string>>& index)
{
    ifstream in;
    if (!openFile(in, dbfile))
        error("Cannot open file named " + dbfile);
    Vector<string> lines;
    readEntireFile(in, lines);

    //iterating over pairs of lines; each url line is followed by the body text line
    for(int line = 0; line < lines.size(); line += 2) {
        string url = lines[line];
        string webPage = lines[line+1];
        Set<string> tokens = gatherTokens(webPage);
        for(string token : tokens) {
            index[token] += url;
        }
    }
    return lines.size()/2;
}

/**
 * Removes and returns the modifier (either '+' or '-') from the front of a query term, if that term has a valid modifier.
 * If the term has no valid modifier returns the space character ' '.
 */
char processModifier(string& query) {
    if(query[0] == '+' || query[0] == '-') {
        char modifier = query[0];
        query = query.substr(1,query.length());
        return modifier;
    }
    return ' ';
}

/**
 * Represents a single term of a query, including the modifier and the cleaned term text. During construction, removes and stores the modifier for the term using processModifier.
 * Note that terms with no modifier will store the space character ' ' as the modifier.
 */
class queryTerm {
    char m = ' ';
    string q = "";
    public: queryTerm(string query) {
        m = processModifier(query);
        q = cleanToken(query);
    }
    char modifier() {
        return m;
    }
    string text() {
        return q;
    }
};

/**
 * Processes a raw query string into a Queue of term objects, splitting by the ' ' character. Each term contains a modifier and the cleaned term string (see queryTerm).
 */
Queue<queryTerm> processQuery(string rawQuery) {
    Queue<queryTerm> terms;
    Vector<string> rawTerms = stringSplit(rawQuery, " ");
    for(string rawTerm : rawTerms) {
        terms.enqueue(queryTerm(rawTerm));
    }
    return terms;
}

/**
 * Searches for all urls matching a given query. Terms in the query process a leading modifier character and clean the rest of the string using cleanToken()
 * Terms are then adjudicated from left to right, modifying the returned set of urls as follows:
 * - A term with no leading modifier will add all matching urls in index that are not already included in the set
 * - A term beginning with '+' will subtract all urls not matching the search term from the set
 * - A term beginning with '-' will subtract all matching urls that are included in the set
 */
Set<string> findQueryMatches(Map<string, Set<string>>& index, string query)
{
    Set<string> result;
    Queue<queryTerm> terms = processQuery(query);
    while(!terms.isEmpty()) {
        queryTerm term = terms.dequeue();
        Set<string> matches = index[term.text()];
        switch(term.modifier()) {
            //if the term had no valid modifier, it stores ' ' as its modifier
            case ' ':
                result.unionWith(matches);
                break;
            case '+':
                result.intersect(matches);
                break;
            case '-':
                result.difference(matches);
                break;
        }
    }
    return result;
}

/**
 * Reads a file containing alternating urls and associated body text. Builds an index which looks up all urls containing a given word. Words are processed
 * in lowercase form without leading or ending punctuation (see cleanToken()). Allows a user to enter a search query containing multiple tokens:
 * -A token starting with a '+' restricts the current search pool to associated urls.
 * -A token starting with a '-' removes all associated urls from the search pool.
 * -A token starting with neither of the aformentioned characters expands the search pool by including all associated urls.
 */
void searchEngine(string dbfile)
{
    cout << "Stand by while building index..." << endl;
    Map<string, Set<string>> index;
    int numFiles = buildIndex(dbfile, index);
    cout << "Indexed " << numFiles << " pages containing " << index.size() << " unique terms" << endl;
    while(true) {
        string query = getLine("Enter query sentence (RETURN/ENTER to quit): ");
        if(query == "") {
            break;
        }
        Set<string> matches = findQueryMatches(index, query);
        cout << "Found " << matches.size() << " matching pages" << endl;
        cout << matches.toString() << "\n" << endl;
    }
}

/* * * * * * Test Cases * * * * * */

PROVIDED_TEST("cleanToken on strings with no punctuation at beginning or end") {
    EXPECT_EQUAL(cleanToken("hello"), "hello");
    EXPECT_EQUAL(cleanToken("WORLD"), "world");
    EXPECT_EQUAL(cleanToken("CS*106B"), "cs*106b");
}

PROVIDED_TEST("cleanToken on strings with some punctuation at beginning and end") {
    EXPECT_EQUAL(cleanToken("/hello/"), "hello");
    EXPECT_EQUAL(cleanToken("~woRLD!"), "world");
}

PROVIDED_TEST("cleanToken on non-word strings (no letters)"){
    EXPECT_EQUAL(cleanToken("106"), "");
    EXPECT_EQUAL(cleanToken("~!106!!!"), "");
}

PROVIDED_TEST("gatherTokens from simple string") {
    Set<string> expected = {"go", "gophers"};
    EXPECT_EQUAL(gatherTokens("go go go gophers"), expected);
}

PROVIDED_TEST("gatherTokens correctly cleans tokens") {
    Set<string> expected = {"i", "love", "cs*106b"};
    EXPECT_EQUAL(gatherTokens("I _love_ CS*106B!"), expected);
}

PROVIDED_TEST("gatherTokens from seuss, 5 unique words, mixed case, punctuation") {
    Set<string> tokens = gatherTokens("One Fish Two Fish *Red* fish Blue fish ** 10 RED Fish?");
    EXPECT_EQUAL(tokens.size(), 5);
    EXPECT(tokens.contains("fish"));
    EXPECT(!tokens.contains("Fish"));
}

PROVIDED_TEST("buildIndex from tiny.txt, 4 pages, 11 unique tokens") {
    Map<string, Set<string>> index;
    int nPages = buildIndex("res/tiny.txt", index);
    EXPECT_EQUAL(nPages, 4);
    EXPECT_EQUAL(index.size(), 11);
    EXPECT(index.containsKey("fish"));
}

PROVIDED_TEST("findQueryMatches from tiny.txt, single word query") {
    Map<string, Set<string>> index;
    buildIndex("res/tiny.txt", index);
    Set<string> matchesRed = findQueryMatches(index, "red");
    EXPECT_EQUAL(matchesRed.size(), 2);
    EXPECT(matchesRed.contains("www.dr.seuss.net"));
    Set<string> matchesHippo = findQueryMatches(index, "hippo");
    EXPECT(matchesHippo.isEmpty());
}

PROVIDED_TEST("findQueryMatches from tiny.txt, compound queries") {
    Map<string, Set<string>> index;
    buildIndex("res/tiny.txt", index);
    Set<string> matchesRedOrFish = findQueryMatches(index, "red fish");
    EXPECT_EQUAL(matchesRedOrFish.size(), 4);
    Set<string> matchesRedAndFish = findQueryMatches(index, "red +fish");
    EXPECT_EQUAL(matchesRedAndFish.size(), 1);
    Set<string> matchesRedWithoutFish = findQueryMatches(index, "red -fish");
    EXPECT_EQUAL(matchesRedWithoutFish.size(), 1);
}

STUDENT_TEST("findQueryMatches from tiny.txt, more compound queries") {
    Map<string, Set<string>> index;
    buildIndex("res/tiny.txt", index);
    Set<string> matchesFishAndEatOrGreen = findQueryMatches(index, "Fish +,,eat././ ///grEEn*");
    EXPECT(matchesFishAndEatOrGreen.contains("www.bigbadwolf.com"));
    EXPECT(matchesFishAndEatOrGreen.contains("www.rainbow.org"));
    EXPECT_EQUAL(matchesFishAndEatOrGreen.size(), 2);
}
