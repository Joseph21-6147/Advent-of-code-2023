// AoC 2023 - Day 18 - Lavaduct lagoon
// =============================

// date:  2023-12-30
// by:    Joseph21 (Joseph21-6147)

#define DAY_STRING  "day18"     //   <---  adapt this to the correct day!!

#include <iostream>
#include <fstream>
#include <algorithm>
#include <numeric>
#include <vector>
#include <map>
#include <set>
#include <deque>
#include <list>

#include "my_utility.h"
#include "vector_types.h"

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

// the data consists of dig instructions, containing
//   - a direction to dig in (L, D, R or U)
//   - a distance to dig
//   - a colour string in a hex enconding
typedef struct sDatumStruct {
    char cDir = ' ';
    int nMtrs = 0;
    std::string sRGB;
} DatumType;
typedef std::vector<DatumType> DataStream;

// ==========   INPUT DATA FUNCTIONS

// hardcoded input - just to get the solution tested
void GetData_EXAMPLE( DataStream &dData ) {

    // hard code your examples here - use the same data for your test input file
    DatumType cDatum;
    cDatum = { 'R', 6, "70c710" }; dData.push_back( cDatum );
    cDatum = { 'D', 5, "0dc571" }; dData.push_back( cDatum );
    cDatum = { 'L', 2, "5713f0" }; dData.push_back( cDatum );
    cDatum = { 'D', 2, "d2c081" }; dData.push_back( cDatum );
    cDatum = { 'R', 2, "59c680" }; dData.push_back( cDatum );
    cDatum = { 'D', 2, "411b91" }; dData.push_back( cDatum );
    cDatum = { 'L', 5, "8ceee2" }; dData.push_back( cDatum );
    cDatum = { 'U', 2, "caa173" }; dData.push_back( cDatum );
    cDatum = { 'L', 1, "1b58a2" }; dData.push_back( cDatum );
    cDatum = { 'U', 2, "caa171" }; dData.push_back( cDatum );
    cDatum = { 'R', 2, "7807d2" }; dData.push_back( cDatum );
    cDatum = { 'U', 3, "a77fa3" }; dData.push_back( cDatum );
    cDatum = { 'L', 2, "015232" }; dData.push_back( cDatum );
    cDatum = { 'U', 2, "7a21e3" }; dData.push_back( cDatum );
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
            std::string sDir  = get_token_dlmtd( " "  , sLine );
            std::string sMtrs = get_token_dlmtd( " (#", sLine );
            std::string sRGB  = get_token_dlmtd( ")"  , sLine );
            cDatum.cDir  = sDir[0];
            cDatum.nMtrs = stoi( sMtrs );
            cDatum.sRGB  = sRGB;
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
    std::cout << iData.cDir << ", " << StringAlignedR( iData.nMtrs, 3 ) << " - " << iData.sRGB;
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

// dynamic map is modelled as a std::vector of CellType cells
typedef struct sCellStruct {
    vllong2d    pos1;
    vllong2d    pos2;       // added for part 2
    int         nInputIx;   // index from input data this cell was dug from
    std::string sCol;
    char        curDir;     // in-direction, the direction via which this cell was reached
    char        nextDir;    // out-direction, the direction via which this cell is left
    std::string sCrnrType = "??";
} CellType;
typedef std::deque<CellType> CellStream;

void PrintCell( CellType &c ) {
    std::cout <<   "location 1: ("   << StringAlignedR( c.pos1.x, 8 ) << ", " << StringAlignedR( c.pos1.y, 8 )
              << ") location 2: ("   << StringAlignedR( c.pos2.x, 8 ) << ", " << StringAlignedR( c.pos2.y, 8 )
              << ") - input index: " << StringAlignedR( c.nInputIx, 4 )
              << ", curDir = "       << std::string( 1, c.curDir )
              << ", nextDir = "      << std::string( 1, c.nextDir )
              << ", corner type: "   << c.sCrnrType
              << ", color: "         << c.sCol
              << std::endl;
}

void PrintCellStream( CellStream &cs ) {
    for (auto e : cs) {
        PrintCell( e );
    }
}

void GetMapBoundingBox( CellStream &cs, vllong2d &UL, vllong2d &DR ) {
    UL.x = UL.y = LLONG_MAX;
    DR.x = DR.y = LLONG_MIN;
    for (int i = 0; i < (int)cs.size(); i++) {
        UL.x = std::min( UL.x, cs[i].pos1.x );
        UL.y = std::min( UL.y, cs[i].pos1.y );
        DR.x = std::max( DR.x, cs[i].pos1.x );
        DR.y = std::max( DR.y, cs[i].pos1.y );
    }
}

void PrepareForInstructions( CellStream &cs ) {
    // "The digger starts in a 1 meter cube hole in the ground."
    CellType newCell = { { 0, 0 }, { 0, 0 }, -1, "ffffff", '?', '?', "??" };
    cs.push_back( newCell );
}

void PerformOneInstruction( DataStream &ds, int nInstrIx, CellStream &cs ) {
    DatumType &curInstr = ds[nInstrIx];
    // determine x and y offsets depending on direction char
    vllong2d v_offset = { 0, 0 };
    switch (curInstr.cDir) {
        case 'R': v_offset = { +1,  0 }; break;
        case 'D': v_offset = {  0, +1 }; break;
        case 'L': v_offset = { -1,  0 }; break;
        case 'U': v_offset = {  0, -1 }; break;
        default: std::cout << "ERROR: PerformOneInstruction() --> unknown direction: " << curInstr.cDir << std::endl;
    }
    // determine position to start from
    vllong2d pos_run = { 0, 0 };
    if (!cs.empty()) {
        pos_run = cs.back().pos1;
    }
    // now run dig instruction
    for (int i = 0; i < curInstr.nMtrs; i++) {
        pos_run += v_offset;
        CellType newCell = { pos_run, { -1, -1 }, nInstrIx, curInstr.sRGB, curInstr.cDir, '?', "??" };
        cs.push_back( newCell );
    }
}

void PerformAllInstructions( DataStream &ds, CellStream &cs ) {

    PrepareForInstructions( cs );

    for (int i = 0; i < (int)ds.size(); i++) {
        PerformOneInstruction( ds, i, cs );
    }
    // make sure directions for 0'th element are set correctly
    cs[0].nextDir = cs[1].curDir;
    cs[0].curDir  = cs[0].nextDir;
    cs[0].sCrnrType[0] = cs[0].curDir;
    cs[0].sCrnrType[1] = cs[0].nextDir;
    // set nextDir for all other elements
    for (int i = 1; i < (int)cs.size(); i++) {
        cs[i].nextDir = cs[(i + 1) % (int)cs.size()].curDir;
        cs[i].sCrnrType[0] = cs[i].curDir;
        cs[i].sCrnrType[1] = cs[i].nextDir;
    }
}

int *CreateCellMap( CellStream &cs, int &nMapWidth, int &nMapHeight ) {
    int *pResult = nullptr;
    if (!cs.empty()) {
        // determine corner points and map sizes
        vllong2d UL, DR;
        GetMapBoundingBox( cs, UL, DR );
        nMapWidth  = DR.x - UL.x + 1;
        nMapHeight = DR.y - UL.y + 1;

        // create map
        pResult = new int[ nMapWidth * nMapHeight ];
        if (pResult == nullptr) {
            std::cout << "ERROR: CreateCellMap() --> allocation failed!!!";
        }
        // initialize it
        for (int y = UL.y; y <= DR.y; y++) {
            for (int x = UL.x; x <= DR.x; x++) {
                int Xindex = x - UL.x;
                int Yindex = y - UL.y;
                pResult[ Yindex * nMapWidth + Xindex ] = -1;   // empty cell is denoted by -1
            }
        }
        // put dug holes in it
        for (int i = 0; i < (int)cs.size(); i++) {
            int Xindex = cs[i].pos1.x - UL.x;
            int Yindex = cs[i].pos1.y - UL.y;
            pResult[ Yindex * nMapWidth + Xindex ] = i;   // non empty cells get index in cs
        }
    }
    return pResult;
}

void PrintCellMap( int *myMap, int nMapWidth, int nMapHeight ) {
    if (myMap == nullptr) {
        std::cout << "ERROR: PrintCellMap() --> map is empty" << std::endl;
    } else {
        // display it
        for (int y = 0; y < nMapHeight; y++) {
            for (int x = 0; x < nMapWidth; x++) {
                std::cout << (myMap[ y * nMapWidth + x ] == -1 ? '.' : '#');
            }
            std::cout << std::endl;
        }
    }
}

void CountCells( CellStream &cs, int *myMap, int nMapWidth, int nMapHeight, int &nInside, int &nOutside, int &nDug ) {

    nInside = nOutside = nDug = 0;

    if (myMap != nullptr) {

        for (int y = 0; y < nMapHeight; y++) {

            bool bInside    = false;    // flags whether current state is inside or outside
            bool bPrevEmpty = true;     // flags whether previous cell was equal to '.'
            std::string sCornerCache = "xx";

            // process one row at a time, scanning left to right
            for (int x = 0; x < nMapWidth; x++) {

                if (myMap[ y * nMapWidth + x ] < 0) {    // empty cell
                    if (bInside) {
                        nInside += 1;
                    } else {
                        nOutside += 1;
                    }
                    bPrevEmpty = true;
                } else {                                 // non empty cell, value is index in cs

                    nDug += 1;

                    int nCSindex = myMap[ y * nMapWidth + x ];
                    CellType &curCell = cs[nCSindex];

                    if (bPrevEmpty) {               // edge from empty to non empty cell

                        if (curCell.curDir == curCell.nextDir) {
                            // case 1 - no corner encountered
                            bInside = !bInside;
                        } else {
                            // case 2 - corner encountered, cache its directions
                            sCornerCache[0] = curCell.curDir;
                            sCornerCache[1] = curCell.nextDir;
                        }
                    } else {                        // edge from non-empty to non-empty cell, could be other corner

                        if (curCell.curDir != curCell.nextDir) {   // other corner encountered
                            // acquire this corner's directions
                            std::string sCornerCur = "xx";
                            sCornerCur[0] = curCell.curDir;
                            sCornerCur[1] = curCell.nextDir;
                            // compare corners and toggle inside flag if appropriate
                            if (sCornerCache[0] == sCornerCur[1] && sCornerCache[1] == sCornerCur[0]) {
                                bInside = !bInside;
                            }
                        }
                    }
                    bPrevEmpty = false;
                }
            }
        }
    }
}

// ----------   part 2

// this corner descriptor is for part 2, but needs to be inserted in CellType
typedef struct sCornerDesc {
    int         nIndex;        // to identify a corner by
    vllong2d    pos;           // where is the corner positioned
    std::string sID = "??";    // denotes two directions: in-direction and out-direction
    vllong2d    offset;
} CornerType;
typedef std::deque<CornerType> CornerStream;

typedef std::map<std::string, vllong2d> CornerSet;

CornerSet glbCornerSet = {
    { "EN", { +1,  0 } },
    { "ES", { -1, +1 } },
    { "SE", { -1,  0 } },
    { "SW", { -1, -1 } },
    { "WS", { -1,  0 } },
    { "WN", { +1, -1 } },
    { "NE", { +1, +1 } },
    { "NW", { +1,  0 } },
};

void PrintCorner( CornerType &c ) {
    std::cout << "Corner ["   << StringAlignedR( c.nIndex, 3 )
              << "] at: ("    << StringAlignedR( c.pos.x, 8 ) << ", " << StringAlignedR( c.pos.y, 8 )
              << "), type: "  << c.sID
              << ", offset: " << c.offset.ToString()
              << std::endl;
}

void PrintCornerStream( CornerStream &cs ) {
    for (auto e : cs) {
        PrintCorner( e );
    }
}

// for map printing corners
char GetDisplayChar( std::string &s ) {
    if (s == "EN" || s == "SW") return 'J';
    if (s == "NW" || s == "ES") return '7';
    if (s == "WS" || s == "NE") return 'F';
    if (s == "SE" || s == "WN") return 'L';
    return '?';
}

// searches for and returns offset value belonging to sType, using glbCornerTypes (defined above)
vllong2d &GetCornerOffset( const std::string &sType ) {
    CornerSet::iterator iter = glbCornerSet.find( sType );
    if (iter == glbCornerSet.end()) {
        std::cout << "ERROR: GetCornerOffset() --> couldn't find match on search key: "  << sType << std::endl;
    }
    return (*iter).second;
}

// auxiliary function to DecodeHexField
char DigitToDir( char cDigit ) {
    switch (cDigit) {
        case '0': return 'R';
        case '1': return 'D';
        case '2': return 'L';
        case '3': return 'U';
    }
    return 'X';
}

// auxiliary function to DecodeHexField
long long HexDigitToInt( char cHexDigit ) {
    switch (cHexDigit) {
        case '0': case '1': case '2': case '3':
        case '4': case '5': case '6': case '7':
        case '8': case '9':
            return (long long)( cHexDigit - '0' );
        case 'a': case 'b': case 'c':
        case 'd': case 'e': case 'f':
            return (long long)( cHexDigit - 'a' + 10 );
        case 'A': case 'B': case 'C':
        case 'D': case 'E': case 'F':
            return (long long)( cHexDigit - 'A' + 10 );
        default: std::cout << "ERROR: HexDigitToInt() --> wrong input, asshole " << cHexDigit << std::endl;
    }
    return -1;
}

// sHexNr will be split according to puzzle specs. As a result:
// - cDir will be R, D, L or U (for Right, Down, Left or Up) thus denoting direction
// - llDist will be a long long integer denoting the amount of meters to dig in that direction
void DecodeHexField( std::string &sHexNr, char &cDir, long long &llDist ) {
    if (sHexNr.length() != 6) {
        std::cout << "ERROR: DecodeHexField() --> argument has incorrect length: " << sHexNr.length() << std::endl;
    } else {
        std::string sCopy = sHexNr;   // prevent sHexNr to be altered
        std::string sDistHex = get_token_sized( 5, sCopy );
        cDir = DigitToDir( sCopy[0] );
        long long llFactor = 1;
        llDist = 0;
        for (int i = 0; i < 5; i++) {
            llDist   += HexDigitToInt( sDistHex[ 4 - i ] ) * llFactor;
            llFactor *= 16;
        }
    }
}

void PerformOneInstruction2( DataStream &ds, int nInstrIx, CellStream &cs ) {
    // grab instruction under consideration
    DatumType &curInstr = ds[nInstrIx];
    // get direction and distance from hex field
    char      cCurDir;
    long long llCurDist;
    DecodeHexField( curInstr.sRGB, cCurDir, llCurDist );
    // get offset vector from direction char
    vllong2d v_offset = { 0, 0 };
    switch (cCurDir) {
        case 'U': v_offset = {  0, -1 }; break;
        case 'D': v_offset = {  0, +1 }; break;
        case 'L': v_offset = { -1,  0 }; break;
        case 'R': v_offset = { +1,  0 }; break;
        default: std::cout << "ERROR: PerformOneInstruction2() --> unknown direction: " << curInstr.cDir << std::endl;
    }
    // determine position to start from
    vllong2d old_pos = { 0, 0 };
    if (!cs.empty()) {
        old_pos = cs.back().pos2;
    }
    // now perform  dig instruction
    vllong2d new_pos = old_pos + v_offset * llCurDist;
    CellType newCell = { old_pos, new_pos, nInstrIx, curInstr.sRGB, cCurDir, '?', "??" };
    cs.push_back( newCell );
}

long long GetInitialDugVolume( DataStream &ds ) {
    long long llResult = 0;
    for (int i = 0; i < (int)ds.size(); i++) {
        DatumType &curInstr = ds[i];
        // get direction and distance from hex field
        char      cCurDir;
        long long llCurDist;
        DecodeHexField( curInstr.sRGB, cCurDir, llCurDist );
        llResult += llCurDist;
    }
    return llResult;
}

char ConvertDir( char cDir ) {
    switch (cDir) {
        case 'R': return 'E';
        case 'L': return 'W';
        case 'D': return 'S';
        case 'U': return 'N';
    }
    return '?';
}

// creates a corner container with CornerType info from the input data ds
void PerformAllInstructions2( DataStream &ds, CornerStream &crnrs ) {

    CellStream cs;
    PrepareForInstructions( cs );

    for (int i = 0; i < (int)ds.size(); i++) {
        PerformOneInstruction2( ds, i, cs );
    }
    // make sure directions for 0'th element are set correctly
    cs[0].nextDir = cs[1].curDir;
    cs[0].curDir  = cs[0].nextDir;
    cs[0].sCrnrType[0] = cs[0].curDir;
    cs[0].sCrnrType[1] = cs[0].nextDir;
    // set nextDir for all other elements
    for (int i = 1; i < (int)cs.size(); i++) {
        cs[i].nextDir = cs[(i + 1) % (int)cs.size()].curDir;
        cs[i].sCrnrType[0] = cs[i].curDir;
        cs[i].sCrnrType[1] = cs[i].nextDir;
    }
    // remove the now redundant start point
    cs.pop_front();

    // convert cell stream to corner stream
    crnrs.clear();
    for (int i = 0; i < (int)cs.size(); i++) {
        CellType &curCell = cs[i];
        // fill corner ID string
        std::string sDirs = "xx";
        sDirs[0] = ConvertDir( curCell.curDir  );
        sDirs[1] = ConvertDir( curCell.nextDir );
        vllong2d crnrOffset = GetCornerOffset( sDirs );
        CornerType newCorner = { -1, curCell.pos2, sDirs, crnrOffset };
        crnrs.push_back( newCorner );
    }
    // get last element to front of crnrs deque
    crnrs.push_front( crnrs.back() );
    crnrs.pop_back();
    // set the index for all corners according to their ordering in the deque
    for (int i = 0; i < (int)crnrs.size(); i++) {
        crnrs[i].nIndex = i;
    }
}


void GetMapBoundingBox2( CornerStream &cs, vllong2d &UL, vllong2d &DR ) {
    UL.x = UL.y = LLONG_MAX;
    DR.x = DR.y = LLONG_MIN;
    for (int i = 0; i < (int)cs.size(); i++) {
        UL.x = std::min( UL.x, cs[i].pos.x );
        UL.y = std::min( UL.y, cs[i].pos.y );
        DR.x = std::max( DR.x, cs[i].pos.x );
        DR.y = std::max( DR.y, cs[i].pos.y );
    }
}

char *CreateCellMap2( CornerStream &crnrs, int nReductionFactor, int &nMapWidth, int &nMapHeight ) {

    CornerStream localCopy = crnrs;
    for (auto &e : localCopy) e.pos /= nReductionFactor;

    char *pResult = nullptr;
    if (!localCopy.empty()) {
        // determine corner points and map sizes
        vllong2d UL, DR;
        GetMapBoundingBox2( localCopy, UL, DR );
        nMapWidth  = DR.x - UL.x + 1;
        nMapHeight = DR.y - UL.y + 1;

        // create map
        pResult = new char[ nMapWidth * nMapHeight ];
        if (pResult == nullptr) {
            std::cout << "ERROR: CreateCellMap2() --> allocation failed!!!";
        }
        // initialize it
        for (int y = UL.y; y <= DR.y; y++) {
            for (int x = UL.x; x <= DR.x; x++) {
                int Xindex = x - UL.x;
                int Yindex = y - UL.y;
                pResult[ Yindex * nMapWidth + Xindex ] = '.';   // empty cell
            }
        }
        // put corners in it
        for (int i = 0; i < (int)localCopy.size(); i++) {
            int Xindex = localCopy[i].pos.x - UL.x;
            int Yindex = localCopy[i].pos.y - UL.y;
            pResult[ Yindex * nMapWidth + Xindex ] = GetDisplayChar( localCopy[i].sID );
        }
    }
    return pResult;
}

void PrintCellMap2( char *myMap, int nMapWidth, int nMapHeight ) {
    if (myMap == nullptr) {
        std::cout << "ERROR: PrintCellMap() --> map is empty" << std::endl;
    } else {
        // display it
        for (int y = 0; y < nMapHeight; y++) {
            for (int x = 0; x < nMapWidth; x++) {
                std::cout << myMap[ y * nMapWidth + x ];
            }
            std::cout << std::endl;
        }
    }
}

// see: https://en.wikipedia.org/wiki/Shoelace_formula
long long TrapezoidFormula( CornerStream &cs ) {
    long long A = 0;
    for (int i = 0; i < (int)cs.size(); i++) {
        int iNext = (i + 1) % cs.size();
        A += ((cs[i].pos.y + cs[iNext].pos.y) * (cs[i].pos.x - cs[iNext].pos.x));
    }
    A /= 2;
    return A;
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

    CellStream glbCS;

    // create cell stream from input data
    PerformAllInstructions( inputData, glbCS );
    // create map from cell stream
    int *glbMap = nullptr;
    int glbMapX, glbMapY;
    glbMap = CreateCellMap( glbCS, glbMapX, glbMapY );
    // count inside, outside and dug cells
    int nInside, nOutside, nDug;
    CountCells( glbCS, glbMap, glbMapX, glbMapY, nInside, nOutside, nDug );

    if (glbProgPhase != PUZZLE) {
        std::cout << std::endl;
        PrintCellMap( glbMap, glbMapX, glbMapY );
        std::cout << std::endl;
        PrintCellStream( glbCS );

        std::cout << std::endl;
        std::cout << "Map Width  = " << glbMapX << std::endl;
        std::cout << "Map Height = " << glbMapY << std::endl;
        std::cout << "Map size   = " << glbMapX * glbMapY << std::endl;

        std::cout << "Total outside cells = " << nOutside << std::endl;
        std::cout << "Total inside  cells = " << nInside  << std::endl;
        std::cout << "Total dug volume    = " << nDug     << std::endl;
    }

    delete glbMap;
    glbMap = nullptr;

    std::cout << std::endl << "Answer to part 1: total volume = " << nDug + nInside << std::endl << std::endl;

/* ========== */   tmr.TimeReport( "Timing 1 - solving puzzle part 1 : " );   // =========================^^^^^vvvvv

    // part 2 code here

    // get fresh copy of inputdata
    inputData = part2Data;

    // create list of corners from input data
    CornerStream glbCorners;
    PerformAllInstructions2( inputData, glbCorners );

    // I had to tweak a little with the boundary points to get the example value right
    long long llNrBoundaryPoints = (GetInitialDugVolume( inputData ) + 2 ) / 2;
    long long llNrInteriorPoints = TrapezoidFormula( glbCorners );

    if (glbProgPhase != PUZZLE) {
        std::cout << std::endl;
        PrintCornerStream( glbCorners );

        std::cout << std::endl;
        std::cout << "Nr of boundary points = " << llNrBoundaryPoints << std::endl;
        std::cout << "Nr of interior points = " << llNrInteriorPoints << std::endl;
        std::cout << "grand total           = " << llNrBoundaryPoints + llNrInteriorPoints << std::endl;
    }

    std::cout << std::endl << "Answer to part 2: total dug area = " << llNrBoundaryPoints + llNrInteriorPoints << std::endl << std::endl;

/* ========== */   tmr.TimeReport( "Timing 2 - solving puzzle part 2 : " );   // ==============================^^^^^

    return 0;
}



