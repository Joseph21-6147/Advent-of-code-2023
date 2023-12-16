// AoC 2023 - Day 12 - Hot springs
// ===============================

// date:  2023-12-16
// by:    Joseph21 (Joseph21-6147)

#define DAY_STRING  "day12"     //   <---  adapt this to the correct day!!

#include <iostream>
#include <fstream>
#include <algorithm>
#include <numeric>
#include <vector>
#include <map>

#include "../flcTimer.h"
#include "my_utility.h"

// ==========   PROGRAM PHASING

enum eProgPhase {     // what programming phase are you in - set at start of main()
    EXAMPLE = 0,      //    1. start with hard coded examples to test your implemented solution on small test sets
    TEST,             //    2. read the same small test set from file, to test on your file reading and parsing
    PUZZLE            //    3. put your algo to work on the full scale puzzle data
} glbProgPhase;

std::string ProgPhase2string() {
    switch (glbProgPhase) {
        case EXAMPLE: return "EXAMPLE";
        case TEST   : return "TEST"   ;
        case PUZZLE : return "PUZZLE" ;
    }
    return "unknown";
}

// ==========   INPUT DATA STRUCTURES          <<<<< ========== adapt to match columns of input file

// the data consists of recors that contain a (damaged) row of hot springs, and
// quantitative info on groups of damaged data fields
typedef struct sDatumStruct {
    std::string sRow;
    std::string sGroupInfo;
} DatumType;
typedef std::vector<DatumType> DataStream;

// ==========   INPUT DATA FUNCTIONS

// hardcoded input - just to get the solution tested
void GetData_EXAMPLE( DataStream &dData ) {

    // hard code your examples here - use the same data for your test input file
    DatumType cDatum;
    cDatum = { "???.###"            , "1,1,3"   }; dData.push_back( cDatum );
    cDatum = { ".??..??...?##."     , "1,1,3"   }; dData.push_back( cDatum );
    cDatum = { "?#?#?#?#?#?#?#?"    , "1,3,1,6" }; dData.push_back( cDatum );
    cDatum = { "????.#...#..."      , "4,1,1"   }; dData.push_back( cDatum );
    cDatum = { "????.######..#####.", "1,6,5"   }; dData.push_back( cDatum );
    cDatum = { "?###????????"       , "3,2,1"   }; dData.push_back( cDatum );
}

// Cuts of and returns the front token from "input_to_be_adapted", using "delim" as delimiter.
// If delimiter is not found, the complete input string is passed as a token. The delimiter string
// is discarded. The input string becomes shorter as a result, and may even become empty
std::string get_token_dlmtd( const std::string &delim, std::string &input_to_be_adapted ) {
    size_t splitIndex = input_to_be_adapted.find( delim );
    std::string token = input_to_be_adapted.substr( 0, splitIndex );
    input_to_be_adapted = (splitIndex == std::string::npos) ? "" : input_to_be_adapted.substr( splitIndex + delim.length());

    return token;
}

// Cuts of and returns a token of size nSize from the front of input_to_be_adapted.
// This input string becomes shorter as a result of this cut off, and may even become empty.
std::string get_token_sized( const int nSize, std::string &input_to_be_adapted ) {
    std::string token;
    token = input_to_be_adapted.substr( 0, nSize );
    input_to_be_adapted = (nSize == (int)std::string::npos) ? "" : input_to_be_adapted.substr( nSize );
    return token;
}

// file input - this function reads text file content one line at a time - adapt code to match your need for line parsing!
void ReadInputData( const std::string sFileName, DataStream &vData ) {

    std::ifstream dataFileStream( sFileName );
    vData.clear();
    std::string sLine;
    while (getline( dataFileStream, sLine )) {
        if (sLine.length() > 0) {    // non empty line
            DatumType cDatum;
            cDatum.sRow = get_token_dlmtd( " ", sLine );
            cDatum.sGroupInfo = sLine;
            vData.push_back( cDatum );
        }
    }
    dataFileStream.close();
}

void GetData_TEST(   DataStream &dData ) { std::string sInputFile = DAY_STRING; sInputFile.append( ".input.test.txt"   ); ReadInputData( sInputFile, dData ); }
void GetData_PUZZLE( DataStream &dData ) { std::string sInputFile = DAY_STRING; sInputFile.append( ".input.puzzle.txt" ); ReadInputData( sInputFile, dData ); }

// ==========   CONSOLE OUTPUT FUNCTIONS

// output to console for testing
void PrintDatum( DatumType &iData, bool bEndl = true ) {
    // define your datum printing code here
    std::cout << "row = _" << StringAlignedL( iData.sRow, 20 ) << "_ --> " << StringAlignedL( iData.sGroupInfo, 15 );
    if (bEndl) { std::cout << std::endl; }
}

// output to console for testing
void PrintDataStream( DataStream &dData ) {
    for (auto &e : dData) {
        PrintDatum( e );
    }
    std::cout << std::endl;
}

// ==========   PROGRAM PHASING

// populates input data, by calling the appropriate input function that is associated
// with the global program phase var
void GetInput( DataStream &dData, bool bDisplay = false ) {

    switch( glbProgPhase ) {
        case EXAMPLE: GetData_EXAMPLE( dData ); break;
        case TEST:    GetData_TEST(    dData ); break;
        case PUZZLE:  GetData_PUZZLE(  dData ); break;
        default: std::cout << "ERROR: GetInput() --> unknown program phase: " << glbProgPhase << std::endl;
    }
    // display to console if so desired (for debugging)
    if (bDisplay) {
        PrintDataStream( dData );
    }
}

// ==========   PUZZLE SPECIFIC SOLUTIONS

// ----------   part 1

// does string s start with a ','?
bool LeadingComma( std::string &s ) {
    return s[0] == ',';
}

// precondition is that string s starts with digit. The function splits string s into a number
// string, containing all contiguous digits, and a remainder string
void SplitOffNumberString( std::string &s, std::string &sNumber, std::string &sRemainder ) {
    if (LeadingComma( s )) {
        std::cout << "ERROR - SplitOffNumberString() called with leading comma ... ";
    } else {
        // find first character that is not a digit
        int nLen = 0;
        while (nLen < (int)s.length() && isdigit( s[nLen] )) {
            nLen += 1;
        }
        // cut of the part that has only digits
        sNumber = get_token_sized( nLen, s );
        sRemainder = s;
    }
}

// how many characters must be in the match string to make a match with sGI at least
// theoretically possible?
int MinRequiredMatchLength( std::string &sGI ) {
    int nResult = 0;
    if (sGI.length() > 0) {
        std::string sCopy = sGI;
        if (LeadingComma( sCopy )) {
            std::string sIgnore = get_token_sized( 1, sCopy );
            nResult = 1 + MinRequiredMatchLength( sCopy );
        } else {
            std::string sNr, sRem;
            SplitOffNumberString( sCopy, sNr, sRem );
            nResult += stoi( sNr ) + MinRequiredMatchLength( sRem );
        }
    }
    return nResult;
}

// auxiliary struct for memoization
typedef struct sKeyStruct {
    std::string sMatch, sGrpInf;
    bool bPrevDot;

    // this operator is needed to make find() possible
    bool operator == (const sKeyStruct &rhs) const {
        return (this->sMatch.compare(  rhs.sMatch  ) == 0 &&
                this->sGrpInf.compare( rhs.sGrpInf ) == 0 &&
                this->bPrevDot == rhs.bPrevDot);
    }

    // this operator is needed to make insert() possible
    bool operator < (const sKeyStruct &rhs) const {
        return ( this->sMatch.compare( rhs.sMatch ) <  0 ) ||
               ((this->sMatch.compare( rhs.sMatch ) == 0 ) &&
                    ((this->sGrpInf.compare( rhs.sGrpInf ) <  0) ||
                    ( this->sGrpInf.compare( rhs.sGrpInf ) == 0  && this->bPrevDot < rhs.bPrevDot )));
    }
} KeyType;

// std::map for memoization
typedef std::map<KeyType, long long> MemoType;

// global memoization store
MemoType glbMemory;

// If a memo containing this combination of values (that combined form the key) exists,
// return the associated answer. Otherwise return -1
long long GetMemo( std::string &sMatch, std::string &sGI, bool bPD ) {
    long long llResult = -1;
    KeyType curKey = { sMatch, sGI, bPD };

    MemoType::iterator iter = glbMemory.find( curKey );
    if (iter != glbMemory.end()) {
        llResult = (*iter).second;
    }
    return llResult;
}

// Combine the first three parameters into a key and store it together with the anwer,
// so that it can be retrieved later on
void PutMemo( std::string &sMatch, std::string &sGI, bool bPD, long long llAnswer ) {
    KeyType curKey = { sMatch, sGI, bPD };
    glbMemory.insert( { curKey, llAnswer } );
}

// this stuff is needed for tree like printing
int glbIndentCntr = 0;
int glbIndentSize = 3;
void Indent() {
    for (int i = 0; i < glbIndentCntr; i++) {
        for (int j = 0; j < glbIndentSize; j++) {
            std::cout << " ";
        }
    }
}

// set flag to true for debugging
#define bOutput false

// matches sMatch (row with unknown chars) with sGI (group info) string, and works out how many
// arrangements exists to do that
long long NrPossibleMatches( std::string &sMatch, std::string &sGI, bool bPrevDot ) {

    if (bOutput) {
        glbIndentCntr += 1;
        Indent();
        std::cout << "NPM_inner: ENTRY _" << sMatch << "_, _" << sGI << "_, " << PrintBoolToString( bPrevDot ) << std::endl;
    }

    // check if the answer is already in the memoization store
    long long llResult = GetMemo( sMatch, sGI, bPrevDot );
    if (llResult < 0) {
        // if not, calculate it, but first create copies of the input parameters to store
        // these with the answer in the memoization store after calculation
        std::string sCacheMatch = sMatch;
        std::string sCacheGI = sGI;
        bool bCachePrevDot = bPrevDot;

        llResult = 0;
        int nMatchLen = (int)sMatch.length();
        int nGroupLen = (int)sGI.length();

        if (nMatchLen == 0 && nGroupLen == 0) {
            // if both match string and group info string are empty at same time --> valid possibility
            llResult = 1;
        } else
        if (nMatchLen == 0 && nGroupLen > 0) {
            // if match string is empty, but group info string isn't --> no possibility
            llResult = 0;
        } else
        if (nMatchLen > 0 && nGroupLen == 0) {
            // if group info string is empty, but match string isn't, there's a possibility still unless leading char == '#'
            std::string sToken = get_token_sized( 1, sMatch );
            switch (sToken[0]) {
                case '.':
                case '?':
                    llResult = NrPossibleMatches( sMatch, sGI, true );
                    break;
                case '#':
                    llResult = 0;
                    break;
            }
        } else
        if (nMatchLen > 0 && nGroupLen > 0) {
            if (nMatchLen < MinRequiredMatchLength( sGI )) {
                llResult = 0;
            } else {
                if (LeadingComma( sGI )) {
                    std::string sIgnore = get_token_sized( 1, sGI ); // remove leading comma from sGI
                    switch (sMatch[0]) {
                        case '.':
                        case '?':
                            sIgnore = get_token_sized( 1, sMatch ); // remove leading '.' or '?' from sMatch and continue
                            llResult = NrPossibleMatches( sMatch, sGI, true );
                            break;
                        case '#':
                            llResult = 0; // comma in group info string does not match # in match string
                            break;
                    }
                } else {
                    switch (sMatch[0]) {
                        case '.': {
                            if (bPrevDot) {
                                std::string sIgnore = get_token_sized( 1, sMatch );
                                llResult = NrPossibleMatches( sMatch, sGI, true );
                            } else {
                                llResult = 0;
                            }
                        }
                        break;
                        case '#': {
                            std::string sNr, sRem;
                            SplitOffNumberString( sGI, sNr, sRem );

                            std::string sIgnore = get_token_sized( 1, sMatch );
                            int nNewNr = stoi( sNr ) - 1;
                            if (nNewNr > 0) {
                                sGI = std::to_string( nNewNr ) + sRem;
                            } else {
                                sGI = sRem;
                            }
                            llResult = NrPossibleMatches( sMatch, sGI, false );
                        }
                        break;
                        case '?': {
                            long long llResult1 = 0;
                            if (bPrevDot) {
                                // there are two options here: 1. treat the '?' as a '#', or 2. treat it as a '.'
                                std::string sGI_copy( sGI );
                                std::string sMatch_copy( sMatch );
                                sMatch_copy[0] = '.';
                                llResult1 = NrPossibleMatches( sMatch_copy, sGI_copy, true );        // treat it as a '.'
                            }
                            sMatch[0] = '#';
                            long long llResult2 = NrPossibleMatches( sMatch, sGI, false );        // treat it as a '#'
                            llResult = llResult1 + llResult2;
                        }
                        break;
                    }
                }
            }
        }
        // store the answer with the original input params for later use (memoization)
        PutMemo( sCacheMatch, sCacheGI, bCachePrevDot, llResult );
    }

    if (bOutput) {
        Indent();
        std::cout << "NPM_inner: EXIT - result = " << llResult << "strings: _" << sMatch << "_, _" << sGI << "_, " << PrintBoolToString( bPrevDot ) << std::endl;
        glbIndentCntr -= 1;
    }

    return llResult;
}

// ----------   part 2

// performs "unfold" operation on the input data - see puzzle specs of part 2
// both match strings and group info strings are repeated five times, where
// match strings are combined with a '?' in between, and group info strings with a ','
void UnfoldInputData( DataStream &org, DataStream &unfolded ) {
    unfolded.clear();
    for (int i = 0; i < (int)org.size(); i++) {
        DatumType &curOrg = org[i];
        DatumType newUnfd;
        for (int j = 0; j < 5; j++) {
            if (j > 0) {
                newUnfd.sRow.append( "?" );
                newUnfd.sGroupInfo.append( "," );
            }
            newUnfd.sRow.append( curOrg.sRow );
            newUnfd.sGroupInfo.append( curOrg.sGroupInfo );
        }
        unfolded.push_back( newUnfd );
    }
}

// ==========   MAIN()

int main()
{
    glbProgPhase = PUZZLE;     // program phase to EXAMPLE, TEST or PUZZLE
    std::cout << "Phase: " << ProgPhase2string() << std::endl << std::endl;
    flcTimer tmr;

/* ========== */   tmr.StartTiming();   // ============================================vvvvv

    // get input data, depending on the glbProgPhase (example, test, puzzle)
    DataStream inputData;
    GetInput( inputData, glbProgPhase != PUZZLE );
    DataStream part2Data = inputData;
    std::cout << "Data stats - size of data stream " << inputData.size() << std::endl << std::endl;

/* ========== */   tmr.TimeReport( "Timing 0 - input data preparation: " );   // =========================^^^^^vvvvv

    // part 1 code here

    int nTotalArr1 = 0;
    for (int i = 0; i < (int)inputData.size(); i++) {
        DatumType &curDatum = inputData[i];
        // create copies to not touch the original data
        std::string sMatchCopy = curDatum.sRow;
        std::string sGpInfCopy = curDatum.sGroupInfo;
        // determine nr of arrangement for current datum
        int nCurrArr = NrPossibleMatches( sMatchCopy, sGpInfCopy, true );
        // cumulate
        nTotalArr1 += nCurrArr;

        if (glbProgPhase != PUZZLE) {
            std::cout << "Index: " << StringAlignedR( i, 3 ) << " datum: ";
            PrintDatum( curDatum, false );
            std::cout << " # arrang. for datum: " << StringAlignedR( nCurrArr  , 3 )
                      << ", and cum.: "           << StringAlignedR( nTotalArr1, 6 ) << std::endl;
        }
    }

    std::cout << std::endl << "Answer to part 1: total nr arrangements = " << nTotalArr1 << std::endl << std::endl;

/* ========== */   tmr.TimeReport( "Timing 1 - solving puzzle part 1 : " );   // =========================^^^^^vvvvv

    // part 2 code here

    // perform unfolding process on input data
    UnfoldInputData( part2Data, inputData );

    // not sure this is needed
    glbMemory.clear();

    // nr of arrangements can become large so use long long iso int
    long long nTotalArr2 = 0;
    for (int i = 0; i < (int)inputData.size(); i++) {
        DatumType &curDatum = inputData[i];
        // create copies to not touch the original data
        std::string sMatchCopy = curDatum.sRow;
        std::string sGpInfCopy = curDatum.sGroupInfo;
        // determine nr of arrangement for current datum
        long long nCurrArr = NrPossibleMatches( sMatchCopy, sGpInfCopy, true );
        // cumulate
        nTotalArr2 += nCurrArr;

        if (glbProgPhase != PUZZLE) {
            std::cout << "Index: " << StringAlignedR( i, 3 );
            std::cout << " # arrangements for datum: " << StringAlignedR( nCurrArr  , 15 )
                      << ", and cumulative: "          << StringAlignedR( nTotalArr2, 20 ) << std::endl;
        }
    }

    std::cout << std::endl << "Answer to part 2: total nr arrangements = " << nTotalArr2 << std::endl << std::endl;

/* ========== */   tmr.TimeReport( "Timing 2 - solving puzzle part 2 : " );   // ==============================^^^^^

    return 0;
}
