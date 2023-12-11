// AoC 2023 - Day 11 - Cosmic expansion
// ====================================

// date:  2023-12-11
// by:    Joseph21 (Joseph21-6147)

#define DAY_STRING  "day11"     //   <---  adapt this to the correct day!!

#include <iostream>
#include <fstream>
#include <algorithm>
#include <numeric>
#include <vector>
#include <map>
#include <set>
#include <deque>
#include <list>

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

// the data consists of map data, modeled as a vector of strings
typedef std::string DatumType;
typedef std::vector<DatumType> DataStream;

// ==========   INPUT DATA FUNCTIONS

// hardcoded input - just to get the solution tested
void GetData_EXAMPLE( DataStream &dData ) {

    // hard code your examples here - use the same data for your test input file
    DatumType cDatum;
    cDatum = "...#......"; dData.push_back( cDatum );
    cDatum = ".......#.."; dData.push_back( cDatum );
    cDatum = "#........."; dData.push_back( cDatum );
    cDatum = ".........."; dData.push_back( cDatum );
    cDatum = "......#..."; dData.push_back( cDatum );
    cDatum = ".#........"; dData.push_back( cDatum );
    cDatum = ".........#"; dData.push_back( cDatum );
    cDatum = ".........."; dData.push_back( cDatum );
    cDatum = ".......#.."; dData.push_back( cDatum );
    cDatum = "#...#....."; dData.push_back( cDatum );
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

/* Needed:
 *   1. functions to determine whether a row or column is completely empty
 *   2. a function to expand the map
 *   3. a function to get the distance between two locations (manhattan)
 *   4. a way to collect all pairs of distances - implemented this by
 *          creating a list of galaxies from the map
 *          making a non overlapping cartesian product of this list
 */

// functions to determine whether a row or column is completely empty
bool MapRowEmpty( DataStream &iData, int nRow ) {
    bool bResult = true;
    for (int i = 0; i < (int)iData[nRow].size() && bResult; i++) {
        bResult = iData[nRow][i] == '.';
    }
    return bResult;
}

bool MapColEmpty( DataStream &iData, int nCol ) {
    bool bResult = true;
    for (int i = 0; i < (int)iData.size() && bResult; i++) {
        bResult = iData[i][nCol] == '.';
    }
    return bResult;
}

// convenience map size getters
int MapWidth(  DataStream &iData ) { return iData[0].size(); }
int MapHeight( DataStream &iData ) { return iData.size(); }

// returns whether n is contained in v
bool Contains( int n, std::vector<int> &v ) {
    std::vector<int>::iterator iter = std::find( v.begin(), v.end(), n );
    return iter != v.end();
}

// function to expand the map
void ExpandMap1( DataStream &inData, DataStream &outData ) {
    // first collect insight into empty rows and columns
    std::vector<int> vEmptyRows, vEmptyCols;
    for (int y = 0; y < MapHeight( inData ); y++) {
        if (MapRowEmpty( inData, y )) {
            vEmptyRows.push_back( y );
        }
    }
    for (int x = 0; x < MapWidth( inData ); x++) {
        if (MapColEmpty( inData, x )) {
            vEmptyCols.push_back( x );
        }
    }
    // create new map of the right size
    std::string sStub;
    for (int i = 0; i < MapWidth( inData ) + (int)vEmptyCols.size(); i++) {
        sStub.append( "." );
    }
    outData.clear();
    for (int i = 0; i < MapHeight( inData ) + (int)vEmptyRows.size(); i++) {
        outData.push_back( sStub );
    }

    // copy from inData to Outdata, keeping track of double rows and columns
    int outRowIx = 0;
    for (int inRowIx = 0; inRowIx < MapHeight( inData ); inRowIx++) {

        int outColIx = 0;
        for (int inColIx = 0; inColIx < MapWidth( inData ); inColIx++) {
            // make a copy
            outData[outRowIx][outColIx] = inData[inRowIx][inColIx];

            if (Contains( inColIx, vEmptyCols )) {
                outColIx += 1;
            }
            outColIx += 1;
        }
        if (Contains( inRowIx, vEmptyRows )) {
            outRowIx += 1;
        }
        outRowIx += 1;
    }
}

// To create a list of galaxies
typedef struct sGalaxyStruct {
    int x, y;
} GalaxyType;
typedef std::vector<GalaxyType> GalStream;

// can be used for sorting galaxy list
bool IsSmaller( GalaxyType &a, GalaxyType &b ) {
    return (a.y < b.y) || (a.y == b.y && a.x < b.x);
}

// manhattan distance
int GetDistance( int x1, int y1, int x2, int y2 ) {
    return (abs( x1 - x2 ) + abs( y1 - y2 ));
}

// creates galaxy list from input data
void MakeGalaxyList( DataStream &iData, GalStream &vGal ) {
    vGal.clear();
    for (int y = 0; y < MapHeight( iData ); y++) {
        for (int x = 0; x < MapWidth( iData ); x++) {
            if (iData[y][x] == '#') {
                vGal.push_back( { x, y } );
            }
        }
    }
}

// ----------   part 2

// the map is not actually expanded, rather data is collected which rows / columns are empty
// (this data is needed in the new GetDistance2() function)
void ExpandMap2( DataStream &inData, std::vector<int> &vEmptyRows, std::vector<int> &vEmptyCols ) {
    // first collect insight into empty rows and columns
    for (int y = 0; y < MapHeight( inData ); y++) {
        if (MapRowEmpty( inData, y )) {
            vEmptyRows.push_back( y );
        }
    }
    for (int x = 0; x < MapWidth( inData ); x++) {
        if (MapColEmpty( inData, x )) {
            vEmptyCols.push_back( x );
        }
    }
}

// works out the distance according to puzzle specs part 2. Each empty row or column represents an additional
// distance of nFactor
long long GetDistance2( int x1, int y1, int x2, int y2, std::vector<int> &vEmptyRows, std::vector<int> &vEmptyCols, int nFactor ) {
    // get range to iterate horizontally
    int nStrtX = std::min( x1, x2 );
    int nStopX = std::max( x1, x2 );
    // work out horizontal distance
    long long llHorDist = 0;
    for (int x = nStrtX; x < nStopX; x++) {
        if (Contains( x, vEmptyCols )) {
            llHorDist += (long long)nFactor;
        } else {
            llHorDist += 1;
        }
    }
    // get range to iterate vertically
    int nStrtY = std::min( y1, y2 );
    int nStopY = std::max( y1, y2 );
    // work out vertical distance
    long long llVerDist = 0;
    for (int y = nStrtY; y < nStopY; y++) {
        if (Contains( y, vEmptyRows )) {
            llVerDist += (long long)nFactor;
        } else {
            llVerDist += 1;
        }
    }

    return llHorDist + llVerDist;
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

    if (glbProgPhase != PUZZLE) PrintDataStream( inputData );
    // expand the map into a new map
    DataStream expandedData;
    ExpandMap1( inputData, expandedData );
    if (glbProgPhase != PUZZLE) PrintDataStream( expandedData );
    // get galaxy list from expanded map
    GalStream glbGalList;
    MakeGalaxyList( expandedData, glbGalList );

    if (glbProgPhase != PUZZLE) {
        for (int i = 0; i < (int)glbGalList.size(); i++) {
            std::cout << "Galaxy nr: " << i << " is at (" << glbGalList[i].x << ", " << glbGalList[i].y << ")" << std::endl;
        }
    }

    // work out distances between every two galaxies, and total them
    int nTotalDistance = 0;
    int nNrPairs = 0;
    for (int i = 0; i < (int)glbGalList.size() - 1; i++) {
        for (int j = i + 1; j < (int)glbGalList.size(); j++) {
            int nDistance = GetDistance( glbGalList[i].x, glbGalList[i].y, glbGalList[j].x, glbGalList[j].y );
            nTotalDistance += nDistance;
            nNrPairs += 1;

            if (glbProgPhase != PUZZLE) {
                std::cout << "Distance between: " << i << " and: " << j << " = " << nDistance << ", (sub)total = " << nTotalDistance << std::endl;
            }

        }
    }

    std::cout << std::endl << "Answer to part 1: total distance = " << nTotalDistance << " between: " << nNrPairs << " pairs" << std::endl << std::endl;

/* ========== */   tmr.TimeReport( "Timing 1 - solving puzzle part 1 : " );   // =========================^^^^^vvvvv

    // part 2 code here

    // get a fresh copy of input data
    inputData = part2Data;

    if (glbProgPhase != PUZZLE) PrintDataStream( inputData );
    // the map is not really expanded, rather data on empty rows and columns is collected
    std::vector<int> vRowsEmpty, vColsEmpty;
    ExpandMap2( inputData, vRowsEmpty, vColsEmpty );
    if (glbProgPhase != PUZZLE) PrintDataStream( inputData );
    // get galaxy list
    glbGalList.clear();
    MakeGalaxyList( inputData, glbGalList );

    if (glbProgPhase != PUZZLE) {
        for (int i = 0; i < (int)glbGalList.size(); i++) {
            std::cout << "Galaxy nr: " << i << " is at (" << glbGalList[i].x << ", " << glbGalList[i].y << ")" << std::endl;
        }
    }

    // work out distances between every two galaxies, and total them
    // let each empty column or row count as "nFactorEmpty" distance

    int nFactorEmpty = 1000000;

    long long nTotalDistance2 = 0;
    nNrPairs = 0;
    for (int i = 0; i < (int)glbGalList.size() - 1; i++) {
        for (int j = i + 1; j < (int)glbGalList.size(); j++) {
            long long nDistance = GetDistance2(
                glbGalList[i].x, glbGalList[i].y,
                glbGalList[j].x, glbGalList[j].y,
                vRowsEmpty, vColsEmpty,
                nFactorEmpty
            );
            nTotalDistance2 += nDistance;
            nNrPairs += 1;

            if (glbProgPhase != PUZZLE) {
                std::cout << "Distance between: " << i << " and: " << j << " = " << nDistance << ", (sub)total = " << nTotalDistance2 << std::endl;
            }
        }
    }

    std::cout << std::endl << "Answer to part 2: total distance = " << nTotalDistance2 << " between: " << nNrPairs << " pairs" << std::endl << std::endl;


/* ========== */   tmr.TimeReport( "Timing 2 - solving puzzle part 2 : " );   // ==============================^^^^^

    return 0;
}
