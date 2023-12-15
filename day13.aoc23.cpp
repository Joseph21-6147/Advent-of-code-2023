// AoC 2023 - Day 13 - Point of incidence
// ======================================

// date:  2023-12-15
// by:    Joseph21 (Joseph21-6147)

#define DAY_STRING  "day13"     //   <---  adapt this to the correct day!!

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

// the data consists of maps of # (rock) and . (ash) cells
typedef std::vector<std::string> DatumType;
typedef std::vector<DatumType> DataStream;

// ==========   INPUT DATA FUNCTIONS

// hardcoded input - just to get the solution tested
void GetData_EXAMPLE( DataStream &dData ) {

    // hard code your examples here - use the same data for your test input file
    DatumType cDatum;
    cDatum = {
        { "#.##..##." },
        { "..#.##.#." },
        { "##......#" },
        { "##......#" },
        { "..#.##.#." },
        { "..##..##." },
        { "#.#.##.#." },
    }; dData.push_back( cDatum );

    cDatum = {
        { "#...##..#" },
        { "#....#..#" },
        { "..##..###" },
        { "#####.##." },
        { "#####.##." },
        { "..##..###" },
        { "#....#..#" },
    }; dData.push_back( cDatum );
}

// convenience getters for map dimensions
int MapWidth(  DatumType &d ) { return int( d[0].length()); }
int MapHeight( DatumType &d ) { return int( d.size()); }

// file input - this function reads text file content one line at a time - adapt code to match your need for line parsing!
void ReadInputData( const std::string sFileName, DataStream &vData ) {

    std::ifstream dataFileStream( sFileName );
    vData.clear();
    std::string sLine;
    DatumType cDatum;
    while (getline( dataFileStream, sLine )) {
        if (sLine.length() > 0) {    // non empty line
            cDatum.push_back( sLine );
        } else {
            vData.push_back( cDatum );
            cDatum.clear();
        }
    }
    dataFileStream.close();
}

void GetData_TEST(   DataStream &dData ) { std::string sInputFile = DAY_STRING; sInputFile.append( ".input.test.txt"   ); ReadInputData( sInputFile, dData ); }
void GetData_PUZZLE( DataStream &dData ) { std::string sInputFile = DAY_STRING; sInputFile.append( ".input.puzzle.txt" ); ReadInputData( sInputFile, dData ); }

// ==========   CONSOLE OUTPUT FUNCTIONS

// output to console for testing
void PrintDatum( DatumType &iData, bool bEndl = true ) {
    std::cout << "(" << MapWidth( iData ) << "x" << MapHeight( iData ) << ")" << std::endl;
    for (int i = 0; i < (int)iData.size(); i++) {
        std::cout << iData[i] << std::endl;
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

// a reflection line is either horizontal (a row) or vertical, that's the boolean
// the nSmallerIx is the (c++ style) smallest index adjacent to the reflection line
typedef struct sReflectionStruct {
    bool bIsRow;        // if true it's a hor. reflection, if false a vert.
    int  nSmallerIx;    // adjacent smallest row or column index (C++ style)
} ReflectionType;
// convenience typedef
typedef std::vector<ReflectionType> ReflectionStream;

// returns true if two reflections are equal
bool ReflectionsEqual( const ReflectionType &a, const ReflectionType &b ) {
    return (
        a.bIsRow     == b.bIsRow &&
        a.nSmallerIx == b.nSmallerIx
    );
}

// the "range" is the relevant part of the map to compare. For instance, if a horizontal
// reflection is right below row 0, the range is only 1, because there's only 1 row to compare
int GetRange( DatumType &dMap, int nMaxSize, int nSmallIx ) {
    int nBackMax = nSmallIx + 1;    // if index = 1, range is max 2 etc
    int nForwMax = nMaxSize - (nSmallIx + 1);
    return std::min( nBackMax, nForwMax );
}

// for testing / debugging
void PrintReflection( ReflectionType &r ) {
    std::cout << (r.bIsRow ? "HOR " : "VER ") << "reflection after index: " << r.nSmallerIx << std::endl;
}

// for testing / debugging
void PrintReflectionStream( ReflectionStream &vR ) {
    for (auto e : vR) {
        PrintReflection( e );
    }
}

// returns true if s1 and s2 are the same string
bool StringEqual( std::string &s1, std::string &s2 ) { return s1.compare( s2 ) == 0; }

// returns true if there's a horizontal reflection line between rows nUpIx and nDnIx
bool IsHorReflectionLine( DatumType &dMap, int nUpIx, int nDnIx ) {
    // work out over what range comparison is possible
    int nRange = GetRange( dMap, MapHeight( dMap ), nUpIx );
    // compare rows within range
    bool bResult = true;
    for (int i = 0; i < nRange && bResult; i++) {
        bResult = StringEqual( dMap[ nUpIx - i ], dMap[nDnIx + i ] );
    }
    return bResult;
}

// returns true if there's a vertical reflection line between columns nLtIx and nRtIx
bool IsVerReflectionLine( DatumType &dMap, int nLtIx, int nRtIx ) {
    // work out over what range comparison is possible
    int nRange = GetRange( dMap, MapWidth( dMap ), nLtIx );
    // compare columns within range
    bool bResult = true;
    for (int r = 0; r < MapHeight( dMap ) && bResult; r++) {
        // instead of string comparison between rows, compare characters between columns
        for (int c = 0; c < nRange && bResult; c++) {
            bResult = (dMap[r][nLtIx - c] == dMap[r][nRtIx + c] );
        }
    }
    return bResult;
}

// returns a list (ReflectionStream) of all horizontal reflections in dMap
ReflectionStream FindMirrorRows( DatumType &dMap ) {
    ReflectionStream vResult;
    for (int i = 0; i < MapHeight( dMap ) - 1; i++) {
        int nUpRow = i;
        int nDnRow = i + 1;
        if (IsHorReflectionLine( dMap, nUpRow, nDnRow )) {
            vResult.push_back( { true, nUpRow } );
        }
    }
    return vResult;
}

// returns a list (ReflectionStream) of all vertical reflections in dMap
ReflectionStream FindMirrorCols( DatumType &dMap ) {
    ReflectionStream vResult;
    for (int i = 0; i < MapWidth( dMap ) - 1; i++) {
        int nLtCol = i;
        int nRtCol = i + 1;
        if (IsVerReflectionLine( dMap, nLtCol, nRtCol )) {
            vResult.push_back( { false, nLtCol } );
        }
    }
    return vResult;
}

// ----------   part 2

// iterate all cells of the map, flip exactly one, and see if this results in exactly
// one reflection line where the cell location must be within the relevant range of
// the reflection line
void FindSmudge( DatumType &dMap, int &smudgeX, int &smudgeY, ReflectionType &r ) {

    // get row and col reflections for original map
    ReflectionStream vR1 = FindMirrorRows( dMap );
    ReflectionStream vC1 = FindMirrorCols( dMap );
    // let vAll1 be the combination of above two vectors
    ReflectionStream vAll1( vR1 );
    vAll1.insert( vAll1.end(), vC1.begin(), vC1.end());

    if (vAll1.size() != 1) {
        std::cout << "ERROR: FindSmudge() --> unchanged map has unexpected # reflections: " << vAll1.size() << std::endl;
    }

    bool bFound = false;
    for (int y = 0; y < MapHeight( dMap ) && !bFound; y++) {
        for (int x = 0; x < MapWidth( dMap ) && !bFound; x++) {

            // create a copy of the current map with exactly one cell flipped
            DatumType dCopy( dMap );
            switch (dMap[y][x]) {
                case '.': dCopy[y][x] = '#'; break;
                case '#': dCopy[y][x] = '.'; break;
                default: std::cout << "ERROR: FindSmudge() --> unrecognized cell type: " << dMap[y][x] << std::endl;
            }
            // check if any mirror row or colums are found
            ReflectionStream vR2 = FindMirrorRows( dCopy );
            ReflectionStream vC2 = FindMirrorCols( dCopy );
            // create a combined solution vector
            ReflectionStream vAll2( vR2 );
            vAll2.insert( vAll2.end(), vC2.begin(), vC2.end());

            // work out if flipping (x, y) in the map results in exactly one valid reflection line
            // (valid means not equal to the reflection line in the unsmudged map)
            switch (int( vAll2.size())) {
                case 1: // exactly one reflection - this is a solution iff it's NOT equal to original
                    bFound = !ReflectionsEqual( vAll1[0], vAll2[0] );
                    break;
                case 2: // exactly two reflections - this is a solution iff one of these two IS equal to original
                    bFound = ReflectionsEqual( vAll1[0], vAll2[0] ) ||
                             ReflectionsEqual( vAll1[0], vAll2[1] );
                    if (bFound) {
                        // remove the one solution that is equal to the original map solution
                        // thus keeping the other (valid) one
                        vAll2.erase(
                            std::remove_if(
                                vAll2.begin(),
                                vAll2.end(),
                                [vAll1]( ReflectionType &r ) {
                                    return ReflectionsEqual( vAll1[0], r );
                                }
                            ),
                            vAll2.end()
                        );
                    }
                    break;
                default: // < 1 or > 2 doesn't lead to a valid solution
                    break;
            }
            if (bFound) {
                // pass results back by ref. parameters
                r = vAll2[0];
                smudgeX = x;
                smudgeY = y;
            }
        }
    }
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

    // iterate over all input data
    int nSummarize1 = 0;
    for (int i = 0; i < (int)inputData.size(); i++) {
        // grab reference to current datum
        DatumType &curMap = inputData[i];
        // collect all valid reflections for this datum
        ReflectionStream vRows = FindMirrorRows( curMap );
        ReflectionStream vCols = FindMirrorCols( curMap );
        int nRows = vRows.size();
        int nCols = vCols.size();

        // there should be exactly one solution!!
        if (nCols + nRows != 1) {
            std::cout << "ERROR: main() --> situation should not occur!!" << std::endl;
        }
        if (nCols == 1) { nSummarize1 +=       (vCols[0].nSmallerIx + 1); }
        if (nRows == 1) { nSummarize1 += 100 * (vRows[0].nSmallerIx + 1); }

        if (glbProgPhase != PUZZLE) {
            std::cout << "Map: " << i << " original = " << std::endl;
            PrintDatum( curMap );
            for (int i = 0; i < (int)vRows.size(); i++) {
                std::cout << "HOR ref.line after row (c++ style): " << vRows[i].nSmallerIx << std::endl;
            }
            for (int i = 0; i < (int)vCols.size(); i++) {
                std::cout << "VER ref.line after col (c++ style): " << vCols[i].nSmallerIx << std::endl;
            }
            std::cout << std::endl;
        }
    }

    std::cout << std::endl << "Answer to part 1: summarized value = " << nSummarize1 << std::endl << std::endl;

/* ========== */   tmr.TimeReport( "Timing 1 - solving puzzle part 1 : " );   // =========================^^^^^vvvvv

    // part 2 code here

    // iterate over all input data
    int nSummarize2 = 0;
    for (int i = 0; i < (int)inputData.size(); i++) {
        // grab reference to current datum
        DatumType &curMap = inputData[i];
        // get the one valid solution by calling FindSmudge()
        ReflectionType resRefl;
        int smudgeX, smudgeY;
        FindSmudge( curMap, smudgeX, smudgeY, resRefl );

        // assume resRefl contains a valid resulting element (this is monitored
        // in FindSmudge()
        nSummarize2 += (resRefl.bIsRow ? 100 : 1) * (resRefl.nSmallerIx + 1);

        if (glbProgPhase != PUZZLE) {
            std::cout << "Map: " << i << " original = " << std::endl;
            PrintDatum( curMap );
            std::cout << "Found smudge at: (" << smudgeX << ", " << smudgeY << ")" << std::endl;
            std::cout << (resRefl.bIsRow ? " ROW " : " COL ") << resRefl.nSmallerIx;
            std::cout << std::endl << std::endl;
        }
    }

    std::cout << std::endl << "Answer to part 2: summarized value = " << nSummarize2 << std::endl << std::endl;

/* ========== */   tmr.TimeReport( "Timing 2 - solving puzzle part 2 : " );   // ==============================^^^^^

    return 0;
}
