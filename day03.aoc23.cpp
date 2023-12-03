// AoC 2023 - Day 03 - Gear ratios
// ===============================

// date:  2023-12-03
// by:    Joseph21 (Joseph21-6147)

#define DAY_STRING  "day03"

#include <iostream>
#include <fstream>
#include <algorithm>
#include <numeric>
#include <vector>
#include <set>

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

// the data consists of a 2d map, modelled as a vector of strings
typedef std::string DatumType;
typedef std::vector<DatumType> DataStream;

// ==========   INPUT DATA FUNCTIONS

// hardcoded input - just to get the solution tested
void GetData_EXAMPLE( DataStream &dData ) {

    // hard code your examples here - use the same data for your test input file
    DatumType cDatum;
    cDatum = "467..114.."; dData.push_back( cDatum );
    cDatum = "...*......"; dData.push_back( cDatum );
    cDatum = "..35..633."; dData.push_back( cDatum );
    cDatum = "......#..."; dData.push_back( cDatum );
    cDatum = "617*......"; dData.push_back( cDatum );
    cDatum = ".....+.58."; dData.push_back( cDatum );
    cDatum = "..592....."; dData.push_back( cDatum );
    cDatum = "......755."; dData.push_back( cDatum );
    cDatum = "...$.*...."; dData.push_back( cDatum );
    cDatum = ".664.598.."; dData.push_back( cDatum );
}

// file input - this function reads text file content one line at a time - adapt code to match your need for line parsing!
void ReadInputData( const std::string sFileName, DataStream &vData ) {

    std::ifstream dataFileStream( sFileName );
    vData.clear();
    std::string sLine;
    while (getline( dataFileStream, sLine )) {
        if (sLine.length() > 0) {    // non empty line
            DatumType cDatum = sLine;
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

// returns true if char c represents a digit
bool IsNumeric( char c ) {
    return ('0' <= c && c <= '9');
}

// I need a way to store numbers with their position and the length of their string representation
typedef struct sNumberStruct {
    int nVal =  0;
    int px   = -1;
    int py   = -1;
    int len  =  0;
} NumberType;

// creates a list of NumberType records using iData as input
void FindNumbers( DataStream &iData, std::vector<NumberType> &vResults ) {

    vResults.clear();
    // get map dimensions
    int nMapX = (int)iData[0].length();
    int nMapY = (int)iData.size();

    bool bParsing = false;
    int pos1 = 0;
    int pos2 = 0;

    for (int y = 0; y < nMapY; y++) {
        for (int x = 0; x < nMapX; x++) {
            char cVal = iData[y][x];
            if (IsNumeric( cVal )) {   // 1. START parsing
                if (!bParsing) {
                    pos1 = x;
                    pos2 = x;
                    bParsing = true;
                } else {               // 2. CONTINU parsing
                    pos2 = x;
                }
            } else {
                if (bParsing) {        // 3. STOP parsing, and store info
                    NumberType aux;
                    aux.len = pos2 - pos1 + 1;
                    aux.px  = pos1;
                    aux.py  = y;
                    std::string sNr = iData[y].substr( pos1, aux.len );
                    aux.nVal = stoi( sNr );
                    vResults.push_back( aux );

                    bParsing = false;
                } // else                 4. DO NOTHING
            }
        }
        // edge case - parsing wasn't finished and end of line reached - handle like 3. STOP parsing
        if (bParsing) {
            NumberType aux;
            aux.len = pos2 - pos1 + 1;
            aux.px = pos1;
            aux.py = y;
            std::string sNr = iData[y].substr( pos1, aux.len );
            aux.nVal = stoi( sNr );
            vResults.push_back( aux );

            bParsing = false;
        }
    }
}

// (px, py) is assumed to be a position in a number string in the map
// the function returns whether it is adjacent to some symbol (not being '.')
bool IsCharAdjacent( DataStream &iData, int px, int py ) {

    // get map dimensions
    int nMapX = (int)iData[0].length();
    int nMapY = (int)iData.size();
    // get corner points for area to check (8-connected around (px, py))
    int x1 = std::max(         0, px - 1 );
    int x2 = std::min( nMapX - 1, px + 1 );
    int y1 = std::max(         0, py - 1 );
    int y2 = std::min( nMapY - 1, py + 1 );
    // iterate over area and do the check
    bool bFound = false;
    for (int y = y1; y <= y2 && !bFound; y++) {
        for (int x = x1; x <= x2 && !bFound; x++) {
            // skip (px, py) itself, check only neighbours
            if (!(px == x && py == y)) {
                char cNghbVal = iData[y][x];
                // adjacent if not empty and not numerical
                bFound = !IsNumeric( cNghbVal ) && (cNghbVal != '.');
            }
        }
    }
    return bFound;
}

// a number is adjacent if any positions of its representing string is adjacent
bool IsNumberAdjacent( DataStream &iData, NumberType &nInfo ) {
    bool bResult = false;
    for (int i = 0; i < nInfo.len && !bResult; i++) {
        bResult = IsCharAdjacent( iData, nInfo.px + i, nInfo.py );
    }
    return bResult;
}

// ==========   PART 2   ==========

// Variant of FindNumbers() - creates a list of gears from iData in vResults
void FindGears( DataStream &iData, std::vector<NumberType> &vResults ) {

    vResults.clear();
    // get map dimensions
    int nMapX = (int)iData[0].length();
    int nMapY = (int)iData.size();

    for (int y = 0; y < nMapY; y++) {
        for (int x = 0; x < nMapX; x++) {
            char cVal = iData[y][x];
            if (cVal == '*') {
                    NumberType aux;
                    aux.len = 1;
                    aux.px = x;
                    aux.py = y;
                    aux.nVal = -10;
                    vResults.push_back( aux );
            }
        }
    }
}

// returns true if (px, py) is a location in the string representation of nNr
bool IsPosIn( NumberType &nNr, int px, int py ) {
    return (nNr.py == py) &&                                 // py must be same row as nNr
           (nNr.px <= px && px <= nNr.px + nNr.len - 1);     // px must be within range of nNr columns
}

// returns the index into vNrList where (px, py) is a position in
int GetNumberIndex( std::vector<NumberType> &vNrList, int px, int py ) {
    int nResult = -1;
    for (int i = 0; i < (int)vNrList.size() && nResult < 0; i++) {
        if (IsPosIn( vNrList[i], px, py )) {
            nResult = i;
        }
    }
    return nResult;
}

// a [potential] gear ('*' symbol) is at (px, py). Find out if it has exactly two neigbours, and
// return the product of their values if it has.
// return 0 otherwise
int GetGearRatio( DataStream &iData, std::vector<NumberType> &vNrList, int px, int py ) {

    // get map dimensions
    int nMapX = (int)iData[0].length();
    int nMapY = (int)iData.size();
    // get corner points for area to check (8-connected around (px, py))
    int x1 = std::max(         0, px - 1 );
    int x2 = std::min( nMapX - 1, px + 1 );
    int y1 = std::max(         0, py - 1 );
    int y2 = std::min( nMapY - 1, py + 1 );

    // use a std::set to get *unique* indices
    std::set<int> sResults;
    // iterate over area and collect indices of adjacent numbers
    for (int y = y1; y <= y2; y++) {
        for (int x = x1; x <= x2; x++) {
            // skip (px, py) itself, check only neighbours
            if (!(px == x && py == y)) {
                char cNghbVal = iData[y][x];
                // trigger if numerical
                if (IsNumeric( cNghbVal )) {
                    // found adjacency - find out to which number it belongs and insert its index into set
                    int nIndex = GetNumberIndex( vNrList, x, y );
                    sResults.insert( nIndex );
                }
            }
        }
    }
    // having the indices in the set, check if size is exactly two, and return gear ratio if it is (0 otherwise)
    int nResult = 0;
    if (sResults.size() == 2) {
        std::set<int>::iterator iter = sResults.begin();
        int nVal1 = vNrList[ *iter ].nVal;
        iter++;
        int nVal2 = vNrList[ *iter ].nVal;
        nResult = nVal1 * nVal2;
    }
    return nResult;
}

// ==========   MAIN()

int main()
{
    glbProgPhase = EXAMPLE;     // program phase to EXAMPLE, TEST or PUZZLE
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

    // collect list of numbers from map
    std::vector<NumberType> vNrList;
    FindNumbers( inputData, vNrList );

    // iterate all numbers and accumulate values that have adjacency
    int nSumParts = 0;
    for (int i = 0; i < (int)vNrList.size(); i++) {
        NumberType &curNr = vNrList[i];
        bool bAdjacent = IsNumberAdjacent( inputData, curNr );
        if (bAdjacent) {
            nSumParts += curNr.nVal;
        }
        // for debugging & testing
        if (glbProgPhase != PUZZLE) {
            std::cout << "Found: " << curNr.nVal << " (of length: " << curNr.len << ") at (" << curNr.px << ", " << curNr.py << ") - ";
            std::cout << (bAdjacent ? " IS " : " NOT ") << "adjacent" << std::endl;
        }
    }

    std::cout << std::endl << "    >> Answer to part 1: sum of part numbers = " << nSumParts << std::endl << std::endl;

/* ========== */   tmr.TimeReport( "Timing 1 - solving puzzle part 1 : " );   // =========================^^^^^vvvvv

    // part 2 code here

    // collect list of gears from map
    std::vector<NumberType> vGearList;
    FindGears( inputData, vGearList );

    // iterate all gears and collect accumulation of gear ratios
    int nSumGRs = 0;
    for (int i = 0; i < (int)vGearList.size(); i++) {
        NumberType &curGear = vGearList[i];
        int nGearRatio = GetGearRatio( inputData, vNrList, curGear.px, curGear.py );
        nSumGRs += nGearRatio;
        // for debugging & testing
        if (glbProgPhase != PUZZLE) {
            std::cout << "GEAR found at at (" << curGear.px << ", " << curGear.py << ") - ";
            std::cout << "gear ratio = " << nGearRatio << std::endl;
        }
    }

    std::cout << std::endl << "    >> Answer to part 2: sum of gear ratios = " << nSumGRs << std::endl << std::endl;

/* ========== */   tmr.TimeReport( "Timing 2 - solving puzzle part 2 : " );   // ==============================^^^^^

    return 0;
}
