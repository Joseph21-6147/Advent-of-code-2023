// AoC 2023 - Day 04 - Scratchcards
// ================================

// date:  2023-12-04
// by:    Joseph21 (Joseph21-6147)

#define DAY_STRING  "day04"

#include <iostream>
#include <fstream>
#include <algorithm>
#include <numeric>
#include <vector>
#include <set>

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

// the data consists of Cards with an ID, a set of winning nrs and a set of owned nrs
// for part 2 i added a nCopies field to keep track how many copies of the card you have
typedef struct sDatumStruct {
    int nId = 0;
    std::set<int> stWinning, stOwned;
    int nCopies = 1;
} DatumType;
typedef std::vector<DatumType> DataStream;

// ==========   INPUT DATA FUNCTIONS

// hardcoded input - just to get the solution tested
void GetData_EXAMPLE( DataStream &dData ) {

    // hard code your examples here - use the same data for your test input file
    DatumType cDatum;

    // for part 2 i added a nCopies field to keep track how many copies of the card you have
    //                                                                        V
    cDatum = { 1, { 41, 48, 83, 86, 17 }, { 83, 86,  6, 31, 17,  9, 48, 53 }, 1 }; dData.push_back( cDatum );
    cDatum = { 2, { 13, 32, 20, 16, 61 }, { 61, 30, 68, 82, 17, 32, 24, 19 }, 1 }; dData.push_back( cDatum );
    cDatum = { 3, {  1, 21, 53, 59, 44 }, { 69, 82, 63, 72, 16, 21, 14,  1 }, 1 }; dData.push_back( cDatum );
    cDatum = { 4, { 41, 92, 73, 84, 69 }, { 59, 84, 76, 51, 58,  5, 54, 83 }, 1 }; dData.push_back( cDatum );
    cDatum = { 5, { 87, 83, 26, 28, 32 }, { 88, 30, 70, 12, 93, 22, 82, 36 }, 1 }; dData.push_back( cDatum );
    cDatum = { 6, { 31, 18, 13, 56, 72 }, { 74, 77, 10, 23, 35, 67, 36, 11 }, 1 }; dData.push_back( cDatum );
}

// NOTE - in the input file I replaced every occurrence of two white spaces with just one (repeatedly).
//        for my code it's much easier to parse that way

// Cuts of and returns the front token from "input_to_be_adapted", using "delim" as delimiter.
// If delimiter is not found, the complete input string is passed as a token.
// The input string becomes shorter as a result, and may even become empty
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
            std::string sIgnore1 = get_token_sized( 5, sLine );   // strip off "Card "
            std::string sCrdId = get_token_dlmtd( ": ", sLine );
            cDatum.nId = stoi( sCrdId );
            while (sLine[0] != '|') {
                std::string sNr = get_token_dlmtd( " ", sLine );
                cDatum.stWinning.insert( stoi( sNr ));
            }
            std::string sIgnore2 = get_token_sized( 2, sLine );   // strip off "| "
            while (sLine.length() > 0) {
                std::string sNr = get_token_dlmtd( " ", sLine );
                cDatum.stOwned.insert( stoi( sNr ));
            }
            vData.push_back( cDatum );
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
    std::cout << "card nr: " << StringAlignedR( iData.nId, 3 ) << ", ";
    std::cout << "# copies: " << StringAlignedR( iData.nCopies, 5 ) << " --> ";
    for (auto e : iData.stWinning) {
        std::cout << StringAlignedR( e, 2 ) << " ";
    }
    std::cout << "| ";
    for (auto e : iData.stOwned) {
        std::cout << StringAlignedR( e, 2 ) << " ";
    }
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

// part 1

// works out the number of matches between the owned numbers and the winning numbers for this crd
int GetWinningNrs( DatumType &crd ) {
    int nResult = 0;
    for (auto e : crd.stOwned) {
        nResult += crd.stWinning.count( e );
    }
    return nResult;
}

// convenience function - needed a quick power of 2 function
int my_power2( int nExp ) {
    if (nExp <  0) { return 0; }
    if (nExp == 0) { return 1; }
    return 2 * my_power2( nExp - 1 );
}

// calculates the points (only used in part 1) according to puzzle rules
int GetPoints( DatumType &crd ) {
    int nCount = GetWinningNrs( crd );
    return my_power2( nCount - 1 );
}

// part 2

// applies the copying process as described in part 2
void ProcessPoints( DataStream &inputData, int nFromCrdID, int nrPoints ) {

    // note that cards are numbered from 1! So card n is at index n-1
    DatumType &curDatum = inputData[nFromCrdID - 1];
    // add copies to card range nFromCrdID + 1 upto and including nFromCrdID + 1 + nrPoints
    for (int i = 0; i < nrPoints; i++) {
        if (nFromCrdID + i >= (int)inputData.size()) {
            std::cout << "ERROR: ProcessPoints() --> resulting index out of range: card ID = " << nFromCrdID << ", nr points = " << nrPoints << ", size inputdata = " << inputData.size() << std::endl;
        } else {
            // note that cards are numbered from 1! So card n is at index n-1
//            inputData[ nFromCrdID - 1 + 1 + i ].nCopies += curDatum.nCopies;
            inputData[ nFromCrdID + i ].nCopies += curDatum.nCopies;
        }
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

    // iterate all input data and accumulate the points that are won per card
    int nTotalPoints = 0;
    for (int i = 0; i < (int)inputData.size(); i++) {
        DatumType &curDatum = inputData[i];
        int nPoints = GetPoints( curDatum );
        nTotalPoints += nPoints;

        if (glbProgPhase != PUZZLE ) {
            PrintDatum( curDatum );
            std::cout << "#winning nrs: " << GetWinningNrs( curDatum ) << ", yielding: " << nPoints << " points." << std::endl;
        }
    }
    // report the outcome
    std::cout << std::endl << "Answer to part 1: total points = " << nTotalPoints << std::endl << std::endl;

/* ========== */   tmr.TimeReport( "Timing 1 - solving puzzle part 1 : " );   // =========================^^^^^vvvvv

    // part 2 code here

    // iterate all input data and apply the copy process on it - print out state at each iteration
    if (glbProgPhase != PUZZLE ) {
        PrintDataStream( part2Data );
    }
    for (int i = 0; i < (int)part2Data.size(); i++) {
        DatumType &curDatum = part2Data[i];
        int nMatches = GetWinningNrs( curDatum );
        ProcessPoints( part2Data, curDatum.nId, nMatches );

        if (glbProgPhase != PUZZLE ) {
            std::cout << "processing card ID: " << curDatum.nId << " (which has index: " << i << ") - nr of matches = " << nMatches << std::endl;
            PrintDataStream( part2Data );
        }
    }

    // collect the total number of cards from the copy process
    int nTotalCards = 0;
    for (int i = 0; i < (int)part2Data.size(); i++) {
        nTotalCards += part2Data[i].nCopies;
    }
    // report the outcome
    std::cout << std::endl << "Answer to part 2: total scratch cards = " << nTotalCards << std::endl << std::endl;

/* ========== */   tmr.TimeReport( "Timing 2 - solving puzzle part 2 : " );   // ==============================^^^^^

    return 0;
}
