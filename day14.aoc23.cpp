// AoC 2023 - Day 14 - Parabolic reflector dish
// ============================================

// date:  2023-12-31
// by:    Joseph21 (Joseph21-6147)

#define DAY_STRING  "day14"     //   <---  adapt this to the correct day!!


/* IMPORTANT NOTE:
 * ==============
 *
 * Since I couldn't figure out a smart way of solving part 2, I decided to try and improve/enhance the brute force way.
 * Succeeded in this for about 20-25%, but not for the 90% I wanted (to reduce the estimated turn around time to an
 * acceptable 4 hours I needed 90% reduction of turnaround time). Because I couldn't get more out of it, I added the
 * feature to dump the input map to file after every 1 million iterations. This way, if anything would go wrong, I could
 * alter the start value for the iteration, and resume with the latest dump as input and prevent a lot of time loss.
 *
 * So two words of warning are appropriate:
 *   1. this code dumps the current state of the map to file after every 1 million iterations. This will result in
 *      (at most) 1,000 files.
 *   2. it took the code approx 36 hours on my setup to complete
 *
 * Other than that - have fun with it!
 * Joseph21
 */

#include <iostream>
#include <fstream>
#include <fstream>
#include <algorithm>
#include <numeric>
#include <vector>
#include <map>
#include <set>
#include <deque>
#include <list>

#include "my_utility.h"

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

// the data consists of a map, containing
//   - empty spaces '.',
//   - cube shaped rocks '#' that are fixed in place,
//   - and rounded rocks 'O' that roll around in any of the cardinal directions.
// This map is modeled as a vector of strings
typedef std::string DatumType;
typedef std::vector<DatumType> DataStream;

DataStream glbMap;   // attempt to gain on performance by making the input data a global var

int MapHeight() { return int( glbMap.size()); }
int MapWidth( ) { return int( glbMap[0].length()); }

char GetMapValue( int x, int y ) { return glbMap[y][x]; }
void SetMapValue( int x, int y, char cValue ) { glbMap[y][x] = cValue; }

bool IsEmpty( char cTile ) { return cTile == '.'; }
bool IsRound( char cTile ) { return cTile == 'O'; }
bool IsCube(  char cTile ) { return cTile == '#'; }

// ==========   INPUT DATA FUNCTIONS

// hardcoded input - just to get the solution tested
void GetData_EXAMPLE( DataStream &dData ) {

    // hard code your examples here - use the same data for your test input file
    DatumType cDatum;
    cDatum = "O....#...."; dData.push_back( cDatum );
    cDatum = "O.OO#....#"; dData.push_back( cDatum );
    cDatum = ".....##..."; dData.push_back( cDatum );
    cDatum = "OO.#O....O"; dData.push_back( cDatum );
    cDatum = ".O.....O#."; dData.push_back( cDatum );
    cDatum = "O.#..O.#.#"; dData.push_back( cDatum );
    cDatum = "..O..#O..O"; dData.push_back( cDatum );
    cDatum = ".......O.."; dData.push_back( cDatum );
    cDatum = "#....###.."; dData.push_back( cDatum );
    cDatum = "#OO..#...."; dData.push_back( cDatum );
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

// See NOTE at start of code file

//void GetData_PUZZLE( DataStream &dData ) { std::string sInputFile = DAY_STRING; sInputFile.append( ".input.puzzle.txt" ); ReadInputData( sInputFile, dData ); }
void GetData_PUZZLE( DataStream &dData ) { std::string sInputFile = DAY_STRING; sInputFile.append( ".MapData_0998.txt" ); ReadInputData( sInputFile, dData ); }

// ==========   CONSOLE OUTPUT FUNCTIONS

// output to console for testing
void PrintDatum( DatumType &iData, bool bEndl = true ) {
    // define your datum printing code here
    std::cout << iData;
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
// a range descriptor describes a range of empty places between two bounds
// it can be either a horizontal or a vertical range
typedef struct sRangeDescriptor {
    int  nBound1, nBound2;   // nBound1 < nBound2 --> nr of places in between = nBound2 - nBound1 - 1;
    int  nPlaces;
    int  nRowOrCol;
    bool bVertical;     // if true,  then nRowOrCol denotes a column index, and nBound1 / nBound2 denote row indices
                        // if false, then nRowOrCol denotes a row    index, and nBound1 / nBound2 denote column indices
} RangeType;

void PrintRange( RangeType &r ) {
    std::cout << "bound1 = "       << StringAlignedR( r.nBound1  , 3 )
              << " bound2 = "      << StringAlignedR( r.nBound2  , 3 )
              << " nPlaces = "     << StringAlignedR( r.nPlaces  , 3 )
              << " orientation = " << ( r.bVertical ? "VERTICAL" : "HORIZONTAL" )
              << " nRowOrCol = "   << StringAlignedR( r.nRowOrCol, 3 )
              << std::endl;
}

typedef std::vector<RangeType> MultiRange;           // can describe multiple ranges for a row or column

typedef std::vector<MultiRange> DescriptorType;      // all descriptors for the map

DescriptorType glbRowDescriptor, glbColDescriptor;   // descriptors are either horizontally or vertically oriented

// for testing/debugging
void PrintDescriptor( DescriptorType &d ) {
    for (int i = 0; i < (int)d.size(); i++) {
        for (int j = 0; j < (int)d[i].size(); j++) {
            PrintRange( d[i][j] );
        }
    }
}

// fill the global variables glbCol / and glbRowDescriptor with the correct
// range descriptors
void InitDescriptors() {
    // process all columns and fill glbColDescriptor
    for (int x = 0; x < MapWidth(); x++) {
        MultiRange vColRanges;

        // first collect all fixed locations in this column
        std::vector<int> vFixedRocks;
        vFixedRocks.push_back( - 1 );
        for (int y = 0; y < MapHeight(); y++) {
            if (IsCube( GetMapValue( x, y ))) {
                vFixedRocks.push_back( y );
            }
        }
        vFixedRocks.push_back( MapHeight());

        // now process fixed positions list into range info
        for (int i = 1; i < (int)vFixedRocks.size(); i++) {
            int nBnd1 = vFixedRocks[i - 1];
            int nBnd2 = vFixedRocks[i    ];
            int nPlcs = nBnd2 - nBnd1 - 1;
            if (nPlcs > 0) {
                RangeType newRange = { nBnd1, nBnd2, nPlcs, x, true };
                vColRanges.push_back( newRange );
            }
        }
        glbColDescriptor.push_back( vColRanges );
    }

    // process all rows and fill glbRowDescriptor
    for (int y = 0; y < MapHeight(); y++) {
        MultiRange vRowRanges;

        // first collect all fixed locations in this row
        std::vector<int> vFixedRocks;
        vFixedRocks.push_back( - 1 );
        for (int x = 0; x < MapWidth(); x++) {
            if (IsCube( GetMapValue( x, y ))) {
                vFixedRocks.push_back( x );
            }
        }
        vFixedRocks.push_back( MapWidth());

        // now process fixed positions list into range info
        for (int i = 1; i < (int)vFixedRocks.size(); i++) {
            int nBnd1 = vFixedRocks[i - 1];
            int nBnd2 = vFixedRocks[i    ];
            int nPlcs = nBnd2 - nBnd1 - 1;
            if (nPlcs > 0) {
                RangeType newRange = { nBnd1, nBnd2, nPlcs, y, false };
                vRowRanges.push_back( newRange );
            }
        }
        glbRowDescriptor.push_back( vRowRanges );
    }
}

// get the number of round rocks in this VERTICAL range
int RangeCountVert( RangeType &r ) {
    int nResult = 0;
    for (int y = r.nBound1 + 1; y < r.nBound2; y++) {
        if (IsRound( GetMapValue( r.nRowOrCol, y ))) {
            nResult += 1;
        }
    }
    return nResult;
}

// get the number of round rocks in this HORIZONTAL range
int RangeCountHorz( RangeType &r ) {
    int nResult = 0;
    for (int x = r.nBound1 + 1; x < r.nBound2; x++) {
        if (IsRound( GetMapValue( x, r.nRowOrCol ))) {
            nResult += 1;
        }
    }
    return nResult;
}

// perform the tilt north operation, which lets all round rocks roll to north as much as possible
void RollAllNorth() {
    for (int x = 0; x < (int)glbColDescriptor.size(); x++) {
        for (int vRange = 0; vRange < (int)glbColDescriptor[x].size(); vRange++) {
            RangeType &curRange = glbColDescriptor[x][vRange];
            int nRRs = RangeCountVert( curRange );
            int i = 0;
            while (i < nRRs            ) { SetMapValue( x, curRange.nBound1 + i + 1, 'O' ); i++; }
            while (i < curRange.nPlaces) { SetMapValue( x, curRange.nBound1 + i + 1, '.' ); i++; }
        }
    }
}

// counts and returns the numbers of rounded Rocks in row sRow
int NrRoundRocksInRow( const std::string &sRow ) {
    int nResult = 0;
    for (int i = 0; i <(int)sRow.length(); i++) {
        if (IsRound( sRow[i] )) {
            nResult += 1;
        }
    }
    return nResult;
}

// uses NrRoundRocksInRow() to determine the load on the north side of the dish
int TotalLoadNorth() {
    int nResult = 0;
    for (int y = 0; y < MapHeight(); y++) {
        nResult += (NrRoundRocksInRow( glbMap[y] ) * (MapHeight() - y));
    }
    return nResult;
}

// ----------   part 2

// similar to RollAllNorth()
void RollAllSouth() {
    for (int x = 0; x < (int)glbColDescriptor.size(); x++) {
        for (int vRange = 0; vRange < (int)glbColDescriptor[x].size(); vRange++) {
            RangeType &curRange = glbColDescriptor[x][vRange];
            int nEmpty = curRange.nPlaces - RangeCountVert( curRange );
            int i = 0;
            while (i < nEmpty          ) { SetMapValue( x, curRange.nBound1 + i + 1, '.' ); i++; }
            while (i < curRange.nPlaces) { SetMapValue( x, curRange.nBound1 + i + 1, 'O' ); i++; }
        }
    }
}

// similar to RollAllNorth()
void RollAllWest() {
    for (int y = 0; y < (int)glbRowDescriptor.size(); y++) {
        for (int vRange = 0; vRange < (int)glbRowDescriptor[y].size(); vRange++) {
            RangeType &curRange = glbRowDescriptor[y][vRange];
            int nRRs = RangeCountHorz( curRange );
            int i = 0;
            while (i < nRRs            ) { SetMapValue( curRange.nBound1 + i + 1, y, 'O' ); i++; }
            while (i < curRange.nPlaces) { SetMapValue( curRange.nBound1 + i + 1, y, '.' ); i++; }
        }
    }
}

// similar to RollAllNorth()
void RollAllEast() {
    for (int y = 0; y < (int)glbRowDescriptor.size(); y++) {
        for (int vRange = 0; vRange < (int)glbRowDescriptor[y].size(); vRange++) {
            RangeType &curRange = glbRowDescriptor[y][vRange];
            int nEmpty = curRange.nPlaces - RangeCountHorz( curRange );
            int i = 0;
            while (i < nEmpty          ) { SetMapValue( curRange.nBound1 + i + 1, y, '.' ); i++; }
            while (i < curRange.nPlaces) { SetMapValue( curRange.nBound1 + i + 1, y, 'O' ); i++; }
        }
    }
}

// spin cycle is tilting north, west, south and east in that order
void OneSpinCycle() {
    RollAllNorth();
    RollAllWest( );
    RollAllSouth();
    RollAllEast( );
}

// functions to write intermediate maps to file

std::string GenerateFileName( int nMillionIterations ) {
    std::string sNr = "0000";
    int i = 0;
    while (nMillionIterations > 0 && i < 4) {
        char cDigit = char('0' + (nMillionIterations % 10));
        sNr[3 - i] = cDigit;
        i++;
        nMillionIterations /= 10;
    }
    return "MapData_" + sNr + ".txt";
}

void WriteMapToFile( const std::string &sFileName ) {
    std::ofstream outputFile;
    outputFile.open( sFileName );
    for (int y = 0; y < MapHeight(); y++) {
        for (int x = 0; x < MapWidth(); x++) {
            outputFile << GetMapValue( x, y );
        }
        outputFile << std::endl;
    }
    outputFile.close();
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

    std::cout << "Data stats - height of data stream " << inputData.size()      << std::endl;
    std::cout << "           - width  of data stream " << inputData[0].length() << std::endl << std::endl;

    glbMap = inputData;
    InitDescriptors();

/* ========== */   tmr.TimeReport( "Timing 0 - input data preparation: " );   // =========================^^^^^vvvvv

    // part 1 code here
    glbMap = inputData;
    RollAllNorth();

    std::cout << std::endl << "Answer to part 1: total load = " << TotalLoadNorth() << std::endl << std::endl;

/* ========== */   tmr.TimeReport( "Timing 1 - solving puzzle part 1: " );   // =========================^^^^^vvvvv

    // part 2 code here
    glbMap = inputData;

    // Timing information - EXAMPLE input
    // - for 1000000000 iterations it takes 450 seconds (7.5 minutes) to finish on the EXAMPLE case...
    // - that is about 4.5 seconds per 10 million iterations (needing 100 of those)
    // Timing information - PUZZLE input
    // - iteration   1 million: 148680 msec --> 149 seconds per 1 million iterations, and i need 1000 of these
    //                                          this would accumulate to > 41 hours...

    // If i can speed up the operations with an order of magnitude, it becomes acceptable (4 hour limit)
    // - update
    // iteration   1 10 thousand: 1172.92 msec --> 117 seconds per 1 million iterations, would still require > 32 hours

    int nNrIterations = 1000000000;

// See NOTE at start of code file - for initial value of i

    for (int i = 998000000; i < nNrIterations; i++) {
        if (i % 1000000 == 0 && i > 0) {
            int nMillionFold = i / 1000000;
            std::string sMsg = "iteration " + StringAlignedR( nMillionFold, 3 ) + " million: ";
            tmr.TimeReport( sMsg );
            WriteMapToFile( GenerateFileName( nMillionFold ));
        }

        OneSpinCycle();
    }
    std::cout << std::endl << "Answer to part 2: total load = " << TotalLoadNorth() << std::endl << std::endl;

/* ========== */   tmr.TimeReport( "Timing 2 - solving puzzle part 2 : " );   // ==============================^^^^^

    return 0;
}
