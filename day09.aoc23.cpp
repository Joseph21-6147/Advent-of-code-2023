// AoC 2023 - Day 09 - Mirage Maintenance
// ======================================

// date:  2023-12-09
// by:    Joseph21 (Joseph21-6147)

#define DAY_STRING  "day09"     //   <---  adapt this to the correct day!!

#include <iostream>
#include <fstream>
#include <algorithm>
#include <numeric>
#include <vector>
#include <deque>

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

// the data consists of:
// sequences of numbers (started part 1 with std::vector for DatumType,
// switched to std::deque for part 2)
typedef std::deque<int> DatumType;
typedef std::vector<DatumType> DataStream;

// ==========   INPUT DATA FUNCTIONS

// hard coded input - just to get the solution tested
void GetData_EXAMPLE( DataStream &dData ) {

    // hard code your examples here - use the same data for your test input file
    DatumType cDatum;
    cDatum = {  0,  3,  6,  9, 12, 15 }; dData.push_back( cDatum );
    cDatum = {  1,  3,  6, 10, 15, 21 }; dData.push_back( cDatum );
    cDatum = { 10, 13, 16, 21, 30, 45 }; dData.push_back( cDatum );
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

// file input - this function reads text file content one line at a time - adapt code to match your need for line parsing!
void ReadInputData( const std::string sFileName, DataStream &vData ) {

    std::ifstream dataFileStream( sFileName );
    vData.clear();
    std::string sLine;
    while (getline( dataFileStream, sLine )) {
        if (sLine.length() > 0) {    // non empty line
            DatumType cDatum;
            while (sLine.length() > 0) {
                cDatum.push_back( stoi( get_token_dlmtd( " ", sLine )));
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
void PrintDatum( DatumType &iData, bool bEndl = true ) {
    // define your datum printing code here
    for (int i = 0; i < (int)iData.size(); i++) {
        std::cout << StringAlignedC( iData[i], 5 ) << " ";
    }
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

// use STL all_of() to check and return whether all values are 0
bool AllZeros( DatumType &vSeq ) {
    return std::all_of(
        vSeq.begin(),
        vSeq.end(),
        [=]( int n ) {
            return n == 0;
        }
    );
}

// This function performs the extrapolation exactly as it is described in the puzzle specs of today.
// The extrapolated value is returned as a result. Flag bBack determines extrapolation at the
// beginning or at the end of vSeq.
int ExtrapolateSequence( DatumType &vSeq, bool bBack, bool bDebug = false ) {

    DatumType vCur = vSeq;            // don't disturb input parameter
    std::vector<DatumType> vvSeqs;    // to build stack of sequences
    vvSeqs.push_back( vCur );

    if (bDebug) PrintDatum( vCur );

    // build stack of reduced sequences
    while (!AllZeros( vCur)) {
        // create a new sequence from the differences between elements of vCur
        DatumType vNew;
        for (int i = 1; i < (int)vCur.size(); i++) {
            vNew.push_back( vCur[i] - vCur[i - 1] );
        }
        // store it in the stack
        vCur = vNew;
        vvSeqs.push_back( vCur );

        if (bDebug) {
            for (int i = 0; i < (int)vvSeqs.size(); i++)
                std::cout << "  ";
            PrintDatum( vCur );
        }
    }

    // now we know for sure that the last sequence on the stack is all zero's
    // so work your way down, adding an element to each sequence
    for (int i = vvSeqs.size() - 1; i >= 0; i--) {
        int nExVal = -1;
        DatumType &vCur = vvSeqs[i];
        if (AllZeros( vCur )) {
            nExVal = 0;
        } else {
            if (bBack) {
                int nCurLastVal  = vCur.back();
                int nPrevLastVal = vvSeqs[ i + 1].back();
                nExVal = nCurLastVal + nPrevLastVal;
            } else {
                int nCurLastVal  = vCur.front();
                int nPrevLastVal = vvSeqs[ i + 1].front();
                nExVal = nCurLastVal - nPrevLastVal;
            }
        }
        if (bBack) {
            vCur.push_back( nExVal );
        } else {
            vCur.push_front( nExVal );
        }

        if (bDebug) {
            for (int j = 0; j < i; j++)
                std::cout << "  ";
            PrintDatum( vCur );
        }
    }

    // return the final added element to the first sequence as result
    return (bBack ? vvSeqs[0].back() : vvSeqs[0].front());
}

// ----------   part 2

// At first I created ExtrapolateSequenceFront() as a variant of the part 1
// ExtrapolateSequence(). After solving I combined this code into one function.

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

    // collect total value of all (right-) extrapolated values per sequence
    int nCumulate1 = 0;
    for (int i = 0; i < (int)inputData.size(); i++) {
        int nCurExVal = ExtrapolateSequence( inputData[i], true, glbProgPhase != PUZZLE );
        nCumulate1 += nCurExVal;
        if (glbProgPhase != PUZZLE) {
            std::cout << "Iteration: " << i << " yielded: " << nCurExVal << " cumulated val = " << nCumulate1 << std::endl;
        }
    }
    // report final answer
    std::cout << std::endl << "Answer to part 1: cumulated extrapolations = " << nCumulate1 << std::endl << std::endl;

/* ========== */   tmr.TimeReport( "Timing 1 - solving puzzle part 1 : " );   // =========================^^^^^vvvvv

    // part 2 code here

    // collect total value of all (left-) extrapolated values per sequence
    int nCumulate2 = 0;
    for (int i = 0; i < (int)inputData.size(); i++) {
        int nCurExVal = ExtrapolateSequence( inputData[i], false, glbProgPhase != PUZZLE );
        nCumulate2 += nCurExVal;
        if (glbProgPhase != PUZZLE) {
            std::cout << "Iteration: " << i << " yielded: " << nCurExVal << " cumulated val = " << nCumulate2 << std::endl;
        }
    }
    // report final answer
    std::cout << std::endl << "Answer to part 2: cumulated extrapolations = " << nCumulate2 << std::endl << std::endl;

/* ========== */   tmr.TimeReport( "Timing 2 - solving puzzle part 2 : " );   // ==============================^^^^^

    return 0;
}
