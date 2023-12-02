// AoC 2023 - Day 01 - Trebuchet
// =============================

// date:  2023-12-01
// by:    Joseph21 (Joseph21-6147)

#define DAY_STRING  "day01"

#include <iostream>
#include <fstream>
#include <algorithm>
#include <numeric>
#include <vector>

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

// the data consists of strings that may or may not contain digits
// these strings should a least contain 1 digit (for part 1 to work properly
// and should contain a number of spelled out digits for part 2
typedef std::string DatumType;
typedef std::vector<DatumType> DataStream;

// ==========   INPUT DATA FUNCTIONS

// hardcoded input - just to get the solution tested
void GetData_EXAMPLE( DataStream &dData ) {

    // hard code your examples here - use the same data for your test input file
    DatumType cDatum;
//    cDatum = { "1abc2"            }; dData.push_back( cDatum );    // part 1 examples
//    cDatum = { "pqr3stu8vwx"      }; dData.push_back( cDatum );
//    cDatum = { "a1b2c3d4e5f"      }; dData.push_back( cDatum );
//    cDatum = { "treb7uchet"       }; dData.push_back( cDatum );

    cDatum = { "two1nine"         }; dData.push_back( cDatum );    // part 2 examples
    cDatum = { "eightwothree"     }; dData.push_back( cDatum );
    cDatum = { "abcone2threexyz"  }; dData.push_back( cDatum );
    cDatum = { "xtwone3four"      }; dData.push_back( cDatum );
    cDatum = { "4nineeightseven2" }; dData.push_back( cDatum );
    cDatum = { "zoneight234"      }; dData.push_back( cDatum );
    cDatum = { "7pqrstsixteen"    }; dData.push_back( cDatum );
}

// file input - this function reads text file content one line at a time - adapt code to match your need for line parsing!
void ReadInputData( const std::string sFileName, DataStream &vData ) {

    std::ifstream dataFileStream( sFileName );
    vData.clear();
    std::string sLine;
    while (getline( dataFileStream, sLine )) {
        if (sLine.length() > 0) {    // non empty line
            vData.push_back( sLine );
        }
    }
    dataFileStream.close();
}

void GetData_TEST(   DataStream &dData ) { std::string sInputFile = DAY_STRING; sInputFile.append( ".input.test.txt"   ); ReadInputData( sInputFile, dData ); }
void GetData_PUZZLE( DataStream &dData ) { std::string sInputFile = DAY_STRING; sInputFile.append( ".input.puzzle.txt" ); ReadInputData( sInputFile, dData ); }

// ==========   CONSOLE OUTPUT FUNCTIONS

// output to console for testing
void PrintDatum( DatumType &iData ) {
    // define your datum printing code here
    std::cout << iData;
    std::cout << std::endl;
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

// returns true if c is a numerical digit
bool IsNumeric( char c ) { return ('0' <= c && c <= '9'); }

// returns position of first numerical character found in s
int GetFstNumericPos( std::string &s ) {
    int nResult = -1;
    for (int i = 0; i < (int)s.length() && nResult < 0; i++) {
        if (IsNumeric( s[i] )) {
            nResult = i;
        }
    }
    return nResult;
}

// returns position of last numerical character found in s
int GetLstNumericPos( std::string &s ) {
    int nResult = -1;
    for (int i = (int)s.length() - 1; i >= 0 && nResult < 0; i--) {
        if (IsNumeric( s[i] )) {
            nResult = i;
        }
    }
    return nResult;
}

// returns integer value of first numerical character found in s
int GetFirstDigitVal( std::string &s ) {
    int nIndex = GetFstNumericPos( s );
    char cDigit = s[nIndex];
    return int( cDigit - '0' );
}

// returns integer value of last numerical character found in s
int GetLastDigitVal( std::string &s ) {
    int nIndex = GetLstNumericPos( s );
    char cDigit = s[nIndex];
    return int( cDigit - '0' );
}

// ----- PART 2

// struct to group spelled out digits (like "one"), digits in string form (like "1")
// and length of spelled out word
typedef struct sNumberStruct {
    std::string sNumber;
    int nLen;
    std::string sDigit;
} NumberType;

// Note: "zero" is only added for ease of indexing. It should NOT BE USED!
std::vector<NumberType> glbNrList = {
    { "zero" , 4, "0" },   // DON'T USE THIS ONE (its not in the puzzle specs!)
    { "one"  , 3, "1" },
    { "two"  , 3, "2" },
    { "three", 5, "3" },
    { "four" , 4, "4" },
    { "five" , 4, "5" },
    { "six"  , 3, "6" },
    { "seven", 5, "7" },
    { "eight", 5, "8" },
    { "nine" , 4, "9" },
};

// creates and returns a new string from s, where the left most digit that is written out in
// characters is replaced by its digit character ("one" is replaced by "1" in the string)
std::string PreProcessLeft( std::string &s ) {

    std::string sResult = s;

    // working from LEFT to RIGHT, find LEFT MOST written digit and replace it
    // ========================================================================

    // check all matches, and replace the one that has lowest position in the string
    std::string::size_type sPos[10] = { std::string::npos };
    std::string::size_type sMinPos = sResult.length();    // to keep track of lowest position
    int sMinIndex = -1;                                   // keeps track which digit is candidate for replacement

    // find matches for all digits (except "zero")
    for (int i = 1; i < 10; i++) {
        sPos[i] = sResult.find( glbNrList[i].sNumber );

        if (sPos[i] != std::string::npos) {
            // if match found, check if it's the (current) lowest one
            if (sMinPos > sPos[i]) {
                // if it is, update min. position and candidate index
                sMinPos = sPos[i];
                sMinIndex = i;
            }
        }
    }
    if (sMinIndex != -1) {
        // if (at least one) match found, replace match with lowest position by it's corresponding digit
        sResult.replace( sPos[sMinIndex], glbNrList[sMinIndex].nLen, glbNrList[sMinIndex].sDigit );
    }

    return sResult;
}

// creates and returns a new string from s, where the right most digit that is written out in
// characters is replaced by its digit character ("one" is replaced by "1" in the string)
std::string PreProcessRight( std::string &s ) {

    std::string sResult = s;

    // working from RIGHT to LEFT, find RIGHT MOST written digit and replace it
    // ========================================================================

    // check all matches, and replace the one that has highest position in the string
    std::string::size_type sPos[10] = { std::string::npos };
    std::string::size_type sMaxPos = std::string::npos;    // to keep track of highest position
    int sMaxIndex = -1;

    // find matches for all digits (except "zero")
    for (int i = 1; i < 10; i++) {
        sPos[i] = sResult.find( glbNrList[i].sNumber );

        if (sPos[i] != std::string::npos) {
            // ADDITIONAL CODE - make sure this is in fact the last occurrence...!!!
            // Note: this check is not needed in the PreProcessLeft() function
            std::string::size_type aux;
            do {
                aux = sResult.find( glbNrList[i].sNumber, sPos[i] + 1 );
                if (aux != std::string::npos) {    // there was another occurrence
                    sPos[i] = aux;
                }
            } while (aux != std::string::npos);

            // if match found, check if it's the (current) highest one
            if (sMaxPos == std::string::npos || sMaxPos < sPos[i]) {
                sMaxPos = sPos[i];
                sMaxIndex = i;
            }
        }
    }
    if (sMaxIndex != -1) {
        // if (at least one) match found, replace match with highest position by it's corresponding digit
        sResult.replace( sPos[sMaxIndex], glbNrList[sMaxIndex].nLen, glbNrList[sMaxIndex].sDigit );
    }

    return sResult;
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
    int nTotalCalValues = 0;
    for (int i = 0; i < (int)inputData.size(); i++) {

        std::string sCur = inputData[i];
        int nCurCalValue = GetFirstDigitVal( sCur ) * 10 + GetLastDigitVal( sCur );
        nTotalCalValues += nCurCalValue;

        if (glbProgPhase != PUZZLE) {
            std::cout << StringAlignedL( sCur, 20 )
                      << " - Fst num.pos at: " << StringAlignedL( GetFstNumericPos( sCur ), 3 )
                      << ", lst num.pos at: "  << StringAlignedR( GetLstNumericPos( sCur ), 3 )
                      << " combined value = "  << StringAlignedR( nCurCalValue            , 6 )
                      << std::endl;
        }
    }

    std::cout << std::endl << "Answer to part 1: total calibration values = " << nTotalCalValues << std::endl << std::endl;

/* ========== */   tmr.TimeReport( "Timing 1 - solving puzzle part 1 : " );   // =========================^^^^^vvvvv

    // part 2 code here

    nTotalCalValues = 0;
    for (int i = 0; i < (int)inputData.size(); i++) {

        std::string sCurLeft = PreProcessLeft(  inputData[i] );
        std::string sCurRght = PreProcessRight( inputData[i] );

        int nCurCalValue = GetFirstDigitVal( sCurLeft ) * 10 + GetLastDigitVal( sCurRght );
        nTotalCalValues += nCurCalValue;

        if (glbProgPhase != PUZZLE) {
            std::cout << StringAlignedL( inputData[i], 20 )
                      << " --> "
                      << StringAlignedL( sCurLeft, 20 )
                      << " | "
                      << StringAlignedL( sCurRght, 20 )
                      << " - Fst num.pos at: " << StringAlignedL( GetFstNumericPos( sCurLeft ), 3 )
                      <<  ", lst num.pos at: " << StringAlignedR( GetLstNumericPos( sCurRght ), 3 )
                      << " combined value = "  << StringAlignedR( nCurCalValue                , 6 )
                      << std::endl;
        }
    }

    std::cout << std::endl << "Answer to part 2: total calibration values = " << nTotalCalValues << std::endl << std::endl;

/* ========== */   tmr.TimeReport( "Timing 2 - solving puzzle part 2 : " );   // ==============================^^^^^

    return 0;
}
