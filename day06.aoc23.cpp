// AoC 2023 - Day 06 - Wait for it
// ===============================

// date:  2023-12-06
// by:    Joseph21 (Joseph21-6147)

#define DAY_STRING  "day06"

#include <iostream>
#include <fstream>
#include <algorithm>
#include <numeric>
#include <vector>

#include "../flcTimer.h"

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

// the data consists of combinations of times (in milliseconds/not relevant) and distances (in millimeters/not relevant)
// Since the puzzle input is very limited, I decided not to parse it from file today, but just hard code it in
typedef struct sDatumStruct {
    int nTime = 0;
    int nDist = 0;
} DatumType;
typedef std::vector<DatumType> DataStream;

// ==========   INPUT DATA FUNCTIONS

// hardcoded input - just to get the solution tested
void GetData_EXAMPLE( DataStream &dData ) {

    // hard code your examples here - use the same data for your test input file
    DatumType cDatum;
    cDatum = {  7,   9 }; dData.push_back( cDatum );
    cDatum = { 15,  40 }; dData.push_back( cDatum );
    cDatum = { 30, 200 }; dData.push_back( cDatum );
}

// not gonna do any parsing today since the input data is so simple
// ================================================================

void GetData_TEST(   DataStream &dData ) { GetData_EXAMPLE( dData ); }

// hardcoded input - just to get the solution tested
void GetData_PUZZLE( DataStream &dData ) {

    // hard code your examples here - use the same data for your test input file
    DatumType cDatum;
    cDatum = { 62,  553 }; dData.push_back( cDatum );
    cDatum = { 64, 1010 }; dData.push_back( cDatum );
    cDatum = { 91, 1473 }; dData.push_back( cDatum );
    cDatum = { 90, 1074 }; dData.push_back( cDatum );
}

// ==========   CONSOLE OUTPUT FUNCTIONS

// output to console for testing
void PrintDatum( DatumType &iData ) {
    // define your datum printing code here
    std::cout << "Time: " << iData.nTime << ", distance: " << iData.nDist;
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

// ----------   part 1

// the speed you have is equal to the time you kept the button pressed
// So distance is just product of speed and time available
int GetDistanceAttempt( int nPressed, int nTimeAvail ) { return nPressed * nTimeAvail; }
// determine for all possible values in the range [1, iData.nTime] if it is a winning scenario,
// and return the number of winning scenarios
int GetNrOfWins( DatumType &iData ) {
    int nResult = 0;
    for (int i = 1; i < iData.nTime; i++) {
        if (GetDistanceAttempt( i, iData.nTime - i ) > iData.nDist) {
            nResult += 1;
        }
    }
    return nResult;
}

// ----------   part 2

// create and return a large time number from a couple of loose time values
long long GetKernedTime( DataStream &iData ) {
    std::string sNumber = "";
    for (int i = 0; i < (int)iData.size(); i++) {
        sNumber.append( std::to_string( iData[i].nTime ));
    }
    return stoll( sNumber );
}

// same as GetKernedTime() but now for distance values
long long GetKernedDistance( DataStream &iData ) {
    std::string sNumber = "";
    for (int i = 0; i < (int)iData.size(); i++) {
        sNumber.append( std::to_string( iData[i].nDist ));
    }
    return stoll( sNumber );
}

// identical to int variants - I was afraid ints would lead to overflow
long long GetDistanceAttempt2( long long nPressed, long long nTimeAvail ) { return nPressed * nTimeAvail; }
long long GetNrOfWins2( long long llTime, long long llDist ) {
    long long nResult = 0;
    for (long long i = 1; i < llTime; i++) {
        if (GetDistanceAttempt2( i, llTime - i ) > llDist) {
            nResult += 1;
        }
    }
    return nResult;
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

    // calculate the product of the # of winning scenarios for all races
    long long llAnswer1 = 1;
    for (int i = 0; i < (int)inputData.size(); i++) {
        int nNrOfWins = GetNrOfWins( inputData[i] );
        llAnswer1 *= (long long)nNrOfWins;

        if (glbProgPhase != PUZZLE) {
            PrintDatum( inputData[i] );
            std::cout << " # different ways to win: " << nNrOfWins << ", error margin: " << llAnswer1 << std::endl;
        }
    }

    std::cout << std::endl << "Answer to part 1: total error margin = " << llAnswer1 << std::endl << std::endl;

/* ========== */   tmr.TimeReport( "Timing 1 - solving puzzle part 1 : " );   // =========================^^^^^vvvvv

    // part 2 code here

    // get the kerned values (see puzzle spec) and work out the nr of wins for them
    long long llGlbTime = GetKernedTime(     inputData );
    long long llGlbDist = GetKernedDistance( inputData );

    long long llAnswer2 = GetNrOfWins2( llGlbTime, llGlbDist );

    std::cout << std::endl << "Answer to part 2: total error margin = " << llAnswer2 << std::endl << std::endl;

/* ========== */   tmr.TimeReport( "Timing 2 - solving puzzle part 2 : " );   // ==============================^^^^^

    return 0;
}
