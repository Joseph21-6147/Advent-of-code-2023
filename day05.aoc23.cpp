// AoC 2023 - Day 05 - If you give a seed a fertilizer
// ===================================================

// date:  2023-12-05
// by:    Joseph21 (Joseph21-6147)

#define DAY_STRING  "day05"

#include <iostream>
#include <fstream>
#include <algorithm>
#include <numeric>
#include <vector>
//#include <map>
//#include <set>
//#include <deque>
//#include <list>

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
// a vector of seeds (numbers, use long long since they are large) and 7 mappings
// each mapping is a pair of <dst-start> <src-str> <nrOf> numbers describing how source numbers must be mapped to dest numbers
// (if no mapping is specified, the mapping is trivial)
typedef struct sMappingStruct {
    long long lStartDst = -1;
    long long lStartSrc = -1;
    long long lRange = 0;
} MappingType;
typedef std::vector<MappingType> MappingStream;

enum eMappingSorts {
    SEED2SOIL = 0,
    SOIL2FERT,
    FERT2WATER,
    WATER2LGHT,
    LGHT2TEMP,
    TEMP2HUM,
    HUM2LOC
};

std::string Mapping2String( eMappingSorts m ) {
    switch (m) {
        case SEED2SOIL : return "SEED2SOIL";
        case SOIL2FERT : return "SOIL2FERT";
        case FERT2WATER: return "FERT2WATER";
        case WATER2LGHT: return "WATER2LGHT";
        case LGHT2TEMP : return "LGHT2TEMP";
        case TEMP2HUM  : return "TEMP2HUM";
        case HUM2LOC   : return "HUM2LOC";
    }
    return "MAP_ERROR";
}

typedef struct sDatumStruct {
    std::vector<long long> vSeeds;
    MappingStream aMapping[7];
} DatumType;
typedef std::vector<DatumType> DataStream;

// ==========   INPUT DATA FUNCTIONS

// hardcoded input - just to get the solution tested
void GetData_EXAMPLE( DataStream &dData ) {

    // hard code your examples here - use the same data for your test input file
    DatumType cDatum;
    cDatum = {  }; dData.push_back( cDatum );

// I don't think it's wise to invest into hardcoding today. Use TEST instead
}

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

    DatumType cDatum;

    // first read seed numbers
    getline( dataFileStream, sLine );
    std::string sIgnore1 = get_token_sized( 7, sLine );   // remove "seeds: "
    while ((int)sLine.length() > 0) {
        std::string sNumber = get_token_dlmtd( " ", sLine );
        cDatum.vSeeds.push_back( stoll( sNumber ));
    }
    // now read all the mappings
    int nMappingIndex = -1;
    bool bIgnoreFlag = true;   // used to ignore the first line in each line sequence

    while (getline( dataFileStream, sLine )) {  // file is not empty
        if (sLine.length() > 0) {    // non empty line
            if (bIgnoreFlag) {
                bIgnoreFlag = false;  // ignore this line, it's text that need not be parsed
            } else {
                // parse this line, it contains a mapping
                std::string sNr1 = get_token_dlmtd( " ", sLine );
                std::string sNr2 = get_token_dlmtd( " ", sLine );
                std::string sNr3 = get_token_dlmtd( " ", sLine );
                MappingType auxMapping = { stoll( sNr1 ), stoll( sNr2 ), stoll( sNr3 ) };

                cDatum.aMapping[nMappingIndex].push_back( auxMapping );
            }
        } else {
            nMappingIndex += 1;
            bIgnoreFlag = true;
        }
    }
    vData.push_back( cDatum );
    dataFileStream.close();
}

void GetData_TEST(   DataStream &dData ) { std::string sInputFile = DAY_STRING; sInputFile.append( ".input.test.txt"   ); ReadInputData( sInputFile, dData ); }
void GetData_PUZZLE( DataStream &dData ) { std::string sInputFile = DAY_STRING; sInputFile.append( ".input.puzzle.txt" ); ReadInputData( sInputFile, dData ); }

// ==========   CONSOLE OUTPUT FUNCTIONS

void PrintMapping( MappingType &m ) {
    std::cout << StringAlignedR( m.lStartDst, 10 ) << ", "
              << StringAlignedR( m.lStartSrc, 10 ) << ", "
              << StringAlignedR( m.lRange   , 10 );

    std::cout << " [" << m.lStartSrc << "-" << m.lStartSrc + m.lRange - 1 << "] effect: " << m.lStartDst - m.lStartSrc << std::endl;
}

// output to console for testing
void PrintDatum( DatumType &iData ) {
    // define your datum printing code here
    std::cout << "Seeds: ";
    for (int i = 0; i < (int)iData.vSeeds.size(); i++) {
        std::cout << iData.vSeeds[i] << " ";
    }
    std::cout << std::endl;
    for (int j = 0; j < 7; j++) {
        std::cout << Mapping2String( (eMappingSorts)j ) << std::endl;
        for (int i = 0; i < (int)iData.aMapping[j].size(); i++) {
            PrintMapping( iData.aMapping[j][i] );
        }
        std::cout << std::endl;
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

long long GetMappingAttempt( MappingType &m, long long lSrcVal ) {
    long long lResult = -1;
    long long lStrt = m.lStartSrc;
    long long lStop = lStrt + m.lRange - 1;
    if (lStrt <= lSrcVal && lSrcVal <= lStop) {
        lResult = m.lStartDst + (lSrcVal - lStrt);
    }
    return lResult;
}

long long GetMapping( MappingStream &aMappS, long long lSrcVal ) {
    long long lResult = -1;
    for (int i = 0; i < (int)aMappS.size() && lResult < 0; i++) {
        lResult = GetMappingAttempt( aMappS[i], lSrcVal );
    }
    if (lResult < 0) {
        lResult = lSrcVal;
    }
    return lResult;
}

long long GetMappingChain( DatumType &iDatum, long long lSeedVal, bool bPrint = false ) {
    long long lResult = lSeedVal;
    if (bPrint) { std::cout << "Starting with seed: " << lSeedVal << std::endl; }
    for (int i = 0; i < 7; i++) {
        eMappingSorts mSort = (eMappingSorts)i;
        if (bPrint) { std::cout << "Applying mapping: " << Mapping2String( mSort ); }
        long long lTmp = GetMapping( iDatum.aMapping[i], lResult );
        if (bPrint) { std::cout << " from " << lResult << " to " << lTmp << std::endl; }
        lResult = lTmp;
    }
    return lResult;
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
    DatumType curDatum = inputData[0];

    std::vector<long long> vLocations;
    for (int i = 0; i < (int)curDatum.vSeeds.size(); i++) {
        long long lSeedNumber = curDatum.vSeeds[i];
        long long lLocation = GetMappingChain( curDatum, lSeedNumber );
        vLocations.push_back( lLocation );
    }

    std::vector<long long>::iterator iter = std::min_element( vLocations.begin(), vLocations.end());
    long long lLowLocation = *iter;
    std::cout << std::endl << "Answer to part 1: lowest location nr = " << lLowLocation << std::endl << std::endl;

/* ========== */   tmr.TimeReport( "Timing 1 - solving puzzle part 1 : " );   // =========================^^^^^vvvvv

    // part 2 code here
    curDatum = inputData[0];

    long long llMinLocation = LLONG_MAX;
    long long llMinSeed = LLONG_MAX;
    for (int i = 0; i < (int)curDatum.vSeeds.size(); i += 2) {
        std::cout << "seed: " << i % 2 + 1 << std::endl;
        long long lSeedNumberStart = curDatum.vSeeds[i];
        long long lSeedNumberStop  = lSeedNumberStart + curDatum.vSeeds[i + 1];
        for (long long j = lSeedNumberStart; j <= lSeedNumberStop; j++) {
            long long lLocation = GetMappingChain( curDatum, j );
            if (llMinLocation > lLocation) {
                llMinLocation = lLocation;
                llMinSeed = j;
            }
            if (j % 1000000 == 0) std::cout << ".";
        }
    }

    std::cout << std::endl << "Answer to part 2: lowest location nr = " << llMinLocation << " at seed: " << llMinSeed << std::endl << std::endl;

/* ========== */   tmr.TimeReport( "Timing 2 - solving puzzle part 2 : " );   // ==============================^^^^^

    return 0;
}
