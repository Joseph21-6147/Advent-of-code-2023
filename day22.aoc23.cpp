// AoC 2023 - Day 22 - Sand slabs
// ==============================

// date:  2023-12-22
// by:    Joseph21 (Joseph21-6147)

#define DAY_STRING  "day22"     //   <---  adapt this to the correct day!!

#include <iostream>
#include <fstream>
#include <algorithm>
#include <numeric>
#include <vector>
#include <deque>

#include "../flcTimer.h"
#include "my_utility.h"
#include "vector_types.h"

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

// the data consists of two points in discrete (integer) 3d space. The points represent the endpoints
// of a brick (beam like structure) of sand.
typedef struct sDatumStruct {
    vi3d ptA, ptB;
} DatumType;
typedef std::vector<DatumType> DataStream;

// ==========   INPUT DATA FUNCTIONS

// hardcoded input - just to get the solution tested
void GetData_EXAMPLE( DataStream &dData ) {

    // hard code your examples here - use the same data for your test input file
    DatumType cDatum;
    cDatum = { { 1, 0, 1 }, { 1, 2, 1 } }; dData.push_back( cDatum );
    cDatum = { { 0, 0, 2 }, { 2, 0, 2 } }; dData.push_back( cDatum );
    cDatum = { { 0, 2, 3 }, { 2, 2, 3 } }; dData.push_back( cDatum );
    cDatum = { { 0, 0, 4 }, { 0, 2, 4 } }; dData.push_back( cDatum );
    cDatum = { { 2, 0, 5 }, { 2, 2, 5 } }; dData.push_back( cDatum );
    cDatum = { { 0, 1, 6 }, { 2, 1, 6 } }; dData.push_back( cDatum );
    cDatum = { { 1, 1, 8 }, { 1, 1, 9 } }; dData.push_back( cDatum );
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
            std::string sX1 = get_token_dlmtd( ",", sLine );
            std::string sY1 = get_token_dlmtd( ",", sLine );
            std::string sZ1 = get_token_dlmtd( "~", sLine );
            std::string sX2 = get_token_dlmtd( ",", sLine );
            std::string sY2 = get_token_dlmtd( ",", sLine );
            std::string sZ2 =                       sLine;
            DatumType cDatum = {
                { stoi( sX1 ), stoi( sY1 ), stoi( sZ1 ) },
                { stoi( sX2 ), stoi( sY2 ), stoi( sZ2 ) }
            };
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
    std::cout << "pt. A: " << iData.ptA.ToString() << " - pt. B: " << iData.ptB.ToString();
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

// decided to create a class for the brick (of sand) object, to encapsulate most of the calculations and manipulations needed
class Brick {
public:
    Brick() {}
    ~Brick() {}

    // give the brick it's 3d integer points
    void Init( vi3d &A, vi3d &B ) {
        ptA = A;
        ptB = B;
    }

    // for testing/debugging
    void Print() {
        std::cout << "pt A: " << ptA.ToString() << " - pt B: " << ptB.ToString() << std::endl;
    }

    // returns true if the projection of (x, y) hits this brick, etc
    bool ClipsXYpos( int x, int y ) { return RangeOverlap( this->ptA.x, this->ptB.x, x, x ) && RangeOverlap( this->ptA.y, this->ptB.y, y, y ); }
    bool ClipsXZpos( int x, int z ) { return RangeOverlap( this->ptA.x, this->ptB.x, x, x ) && RangeOverlap( this->ptA.z, this->ptB.z, z, z ); }
    bool ClipsYZpos( int y, int z ) { return RangeOverlap( this->ptA.y, this->ptB.y, y, y ) && RangeOverlap( this->ptA.z, this->ptB.z, z, z ); }

    // returns whether this and rhs are clipping in x y plane (so looking from above)
    bool IsClipping( Brick &rhs ) {
        // there's overlap in the xy plane
        return RangeOverlap( this->ptA.x, this->ptB.x, rhs.ptA.x, rhs.ptB.x ) &&
               RangeOverlap( this->ptA.y, this->ptB.y, rhs.ptA.y, rhs.ptB.y );
    }

    // returns true if this brick is supported by rhs
    bool IsSupportedBy( Brick &rhs ) {
        // there's overlap in the xy plane and rhs.z + 1 == this->z
        return IsClipping( rhs ) && rhs.GetMaxZ() + 1 == this->GetMinZ();
    }

    // returns true if this brick is grounded (rests on / supported by the ground)
    bool IsGrounded() {
        return GetMinZ() == 1;
    }

    // needed to use stl containers with this class
    bool operator == ( Brick &rhs ) const {
        return (this->ptA == rhs.ptA && this->ptB == rhs.ptB) ||
               (this->ptA == rhs.ptB && this->ptB == rhs.ptA);
    }

    bool operator != ( Brick &rhs ) const {
        return !(*this == rhs);
    }

    // getters/setters for the 3d points
    vi3d GetA() { return ptA; }
    vi3d GetB() { return ptB; }
    void SetA( vi3d &loc ) { ptA = loc; }
    void SetB( vi3d &loc ) { ptB = loc; }

    // convenience method used in consolidation of the stack of bricks
    void LowerByOne() {
        ptA.z -= 1;
        ptB.z -= 1;
    }

    // convenience getters on the 3d points
    int GetMinX() { return std::min( ptA.x, ptB.x ); }
    int GetMaxX() { return std::max( ptA.x, ptB.x ); }
    int GetMinY() { return std::min( ptA.y, ptB.y ); }
    int GetMaxY() { return std::max( ptA.y, ptB.y ); }
    int GetMinZ() { return std::min( ptA.z, ptB.z ); }
    int GetMaxZ() { return std::max( ptA.z, ptB.z ); }

private:
    vi3d ptA, ptB;

    // nA1-nA2 and nB1-nB2 are two intervals (ranges): returns true if there's NO overlap
    bool SeparatedRanges( int nA1, int nA2, int nB1, int nB2 ) {
        return (nA2 < nB1) || (nB2 < nA1);
    }

    // nA1-nA2 and nB1-nB2 are two intervals (ranges): returns true if there IS overlap
    bool RangeOverlap( int nA1, int nA2, int nB1, int nB2 ) {
        return !SeparatedRanges( nA1, nA2, nB1, nB2 );
    }
};

// all the processing will be done on a list of bricks, built up from the input data
typedef std::vector<Brick> BrickList;

// initialises bl from input data stream iData
void InitBrickList( DataStream &iData, BrickList &bl ) {
    for (int i = 0; i < (int)iData.size(); i++) {
        DatumType &curDatum = iData[i];
        Brick newBrick;
        newBrick.Init( curDatum.ptA, curDatum.ptB );
        bl.push_back( newBrick );
    }
}

// for testing/debugging
void PrintBrickList( BrickList &bl ) {
    for (int i = 0; i < (int)bl.size(); i++) {
        std::cout << "Label: " << char( 'A' + i ) << ": ";
        Brick &curBrick = bl[i];
        curBrick.Print();
    }
}

// needed for projection printing - I could put this as lambda's in the corresponding printer functions ig

// returns the list of bricks that are hit by the line (x, z)
BrickList BricksInXZpos( BrickList &bl, int x, int z ) {
    BrickList vResult;
    for (int i = 0; i < (int)bl.size(); i++) {
        if (bl[i].ClipsXZpos( x, z )) {
            vResult.push_back( bl[i] );
        }
    }
    return vResult;
}

// returns the list of bricks that are hit by the line (y, z)
BrickList BricksInYZpos( BrickList &bl, int y, int z ) {
    BrickList vResult;
    for (int i = 0; i < (int)bl.size(); i++) {
        if (bl[i].ClipsYZpos( y, z )) {
            vResult.push_back( bl[i] );
        }
    }
    return vResult;
}

// returns the index of brick b in bricklist bl
int GetBrickIndex( BrickList &bl, Brick &b ) {
    int nResult = -1;
    for (int i = 0; i < (int)bl.size() && nResult < 0; i++) {
        Brick &curBrick = bl[i];
        if (curBrick == b) {
            nResult = i;
        }
    }
    return nResult;
}

// print to screen for comparison with examples - ignore y coordinates, only use x and z
void PrintProjectionXZ( BrickList &bl ) {
    // work out min and max values for x and z
    int minX = INT_MAX, minZ = INT_MAX, maxX = INT_MIN, maxZ = INT_MIN;
    for (int i = 0; i < (int)bl.size(); i++) {
        minX = std::min( minX, bl[i].GetMinX());
        maxX = std::max( maxX, bl[i].GetMaxX());
        minZ = std::min( minZ, bl[i].GetMinZ());
        maxZ = std::max( maxZ, bl[i].GetMaxZ());
    }
    // create a map to draw on
    int nMapW = (maxX - minX) + 1;
    int nMapH = (maxZ - minZ) + 1;
    char *pMap = new char[nMapW * nMapH];
    // fill the map with the right characters
    for (int h = 0; h < nMapH; h++) {
        for (int w = 0; w < nMapW; w++) {
            BrickList aux = BricksInXZpos( bl, w + minX, h + minZ );
            char cToDisplay = '!';
            switch (aux.size()) {
                case  0: cToDisplay = '.'                              ; break;  // no bricks
                case  1: cToDisplay = 'A' + GetBrickIndex( bl, aux[0] ); break;  // exactly one brick
                default: cToDisplay = '?';                                       // multiple bricks
            }
            pMap[h * nMapW + w] = cToDisplay;
        }
    }
    // display it on screen
    for (int h = nMapH - 1; h >= 0; h--) {
        for (int w = 0; w < nMapW; w++) {
            std::cout << pMap[ h * nMapW + w ];
        }
        std::cout << std::endl;
    }
    for (int w = 0; w < nMapW; w++) {  // ground
        std::cout << '-';
    }
    std::cout << std::endl;
    // dispose it after use
    delete pMap;
}

// print to screen for comparison with examples - ignore x coordinates, only use y and z
void PrintProjectionYZ( BrickList &bl ) {
    // work out min and max values for x and z
    int minY = INT_MAX, minZ = INT_MAX, maxY = INT_MIN, maxZ = INT_MIN;
    for (int i = 0; i < (int)bl.size(); i++) {
        minY = std::min( minY, bl[i].GetMinY());
        maxY = std::max( maxY, bl[i].GetMaxY());
        minZ = std::min( minZ, bl[i].GetMinZ());
        maxZ = std::max( maxZ, bl[i].GetMaxZ());
    }
    // create a map to draw on
    int nMapW = (maxY - minY) + 1;
    int nMapH = (maxZ - minZ) + 1;
    char *pMap = new char[nMapW * nMapH];
    // fill the map with the right characters
    for (int h = 0; h < nMapH; h++) {
        for (int w = 0; w < nMapW; w++) {
            BrickList aux = BricksInYZpos( bl, w + minY, h + minZ );
            char cToDisplay = '!';
            switch (aux.size()) {
                case  0: cToDisplay = '.'                              ; break;  // no bricks
                case  1: cToDisplay = 'A' + GetBrickIndex( bl, aux[0] ); break;  // exactly one brick
                default: cToDisplay = '?';                                       // multiple bricks
            }
            pMap[ h * nMapW + w ] = cToDisplay;
        }
    }
    // display it on screen
    for (int h = nMapH - 1; h >= 0; h--) {
        for (int w = 0; w < nMapW; w++) {
            std::cout << pMap[ h * nMapW + w ];
        }
        std::cout << std::endl;
    }
    for (int w = 0; w < nMapW; w++) {  // ground
        std::cout << '-';
    }
    std::cout << std::endl;
    // dispose it after use
    delete pMap;
}

// collect and return the list of all bricks that are supporting (!!) b
BrickList GetSupporters( BrickList &bl, Brick &b ) {
    BrickList vResult;
    for (int i = 0; i < (int)bl.size(); i++) {
        Brick &curBrick = bl[i];
        if (curBrick != b && b.IsSupportedBy( curBrick )) {
            vResult.push_back( curBrick );
        }
    }
    return vResult;
}

// collect and return the list of bricks that are supported by (!!) b
BrickList GetSupportees( BrickList &bl, Brick &b ) {
    BrickList vResult;
    for (int i = 0; i < (int)bl.size(); i++) {
        Brick &curBrick = bl[i];
        if (curBrick != b && curBrick.IsSupportedBy( b )) {
            vResult.push_back( curBrick );
        }
    }
    return vResult;
}

// returns true if there's no supporter for a (and a isn't grounded either)
bool IsBrickFloating( BrickList &bl, Brick &a ) {
    BrickList vSupporters = GetSupporters( bl, a );
    return !a.IsGrounded() && vSupporters.size() == 0;
}

// since bricks can be floating in the air in the input data, first make sure they are all
// stably stacked
void ConsolidateBrickList( BrickList &bl ) {

    // bringing down one brick may cause space for another, so use a stable flag
    // to signal whether any change is still occurring
    bool bStable;
    do {
        // bring bricks down if they are not supported
        bStable = true;
        for (int i = 0; i < (int)bl.size(); i++) {
            if (IsBrickFloating( bl, bl[i] )) {
                bStable = false;
                bl[i].LowerByOne();
            }
        }
    } while (!bStable);
}

// returns true if either
//   * b doesnt support anything or
//   * b supports only bricks that are also supported elsewhere
bool CanBeDisintegrated( BrickList &bl, Brick &b ) {
    bool bFlag = false;
    // get list of all bricks supported by b
    BrickList vSupportees = GetSupportees( bl, b );
    if (vSupportees.size() == 0) {
        // no bricks are supported by b
        bFlag = true;
    } else {
        // there are bricks supported by b, check if they each have other supporters
        bFlag = true;
        for (int i = 0; i < (int)vSupportees.size() && bFlag; i++) {
            // collect all supporters for this supportee of b
            BrickList vSupporters = GetSupporters( bl, vSupportees[i] );
            // if more than one supporter for this supportee of b, than still safe to remove b
            bFlag = vSupporters.size() > 1;
        }
    }
    return bFlag;
}

// ----------   part 2

// is brick e contained in brick list bl?
bool IsElement( Brick &e, BrickList &bl ) {
    return GetBrickIndex( bl, e ) != -1;
}

// returns true if a is a subset of b (so each element of a is also an element of b)
bool IsSubset( BrickList &a, BrickList &b ) {
    bool bResult = true;
    for (int i = 0; i < (int)a.size() && bResult; i++) {
        bResult = IsElement( a[i], b );
    }
    return bResult;
}

// Would brick b fall if all the bricks in vFalling were desintegrated?
// In other words - is the list of supporters of brick b a subset of vFalling?
bool WouldBrickFall( BrickList &bl, Brick &b, BrickList &vFalling ) {
    BrickList vSupprtrs = GetSupporters( bl, b );
    return IsSubset( vSupprtrs, vFalling );
}

typedef std::deque<Brick> BrickQueue;

// return the number of bricks that would fall if brick b was to be desintegrated
int NrFallingForBrick( BrickList &bl, Brick &b ) {

    BrickQueue bQ;
    bQ.push_back( b );

    // build up vFalling list, using a queue of still to process bricks
    BrickList vFalling;
    while (!bQ.empty()) {
        // get new element from queue in curBrick
        Brick curBrick = bQ.front();
        bQ.pop_front();
        // push it to the vFalling list
        vFalling.push_back( curBrick );

        // collect all bricks that are supported by curBrick
        BrickList vSupportees = GetSupportees( bl, curBrick );
        // examine them to check if they would fall
        for (int j = 0; j < (int)vSupportees.size(); j++) {
            Brick &curSupportee = vSupportees[j];

            if (!IsElement( curSupportee, vFalling )) {  // prevent double couting bricks
                if (WouldBrickFall( bl, curSupportee, vFalling )) {
                    // if it would fall using this vFalling list, enqueue it for processing
                    bQ.push_back( curSupportee );
                }
            }
        }
    }
    return vFalling.size() - 1;   // - 1 because the trigger brick is also in the list
}

// get the nr of bricks that would fall upon desintegrating one brick, and do that for
// all bricks in the list. collect the sum as the final answer
int AnalyseBrickList( BrickList &bl ) {

    int nCount = 0;
    for (int i = (int)bl.size() - 1; i >= 0; i--) {
        Brick &curBrick = bl[i];

        int nLocalScore = NrFallingForBrick( bl, curBrick );
        nCount += nLocalScore;

        if (glbProgPhase != PUZZLE) {
            std::cout << "Index: " << i << ", label: " << char( 'A' + i )
                      << ": would upon desintegration cause chain reaction of: " << nLocalScore << " cumulative: " << nCount << std::endl;
        }
    }
    return nCount;
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

    // create the working ds from the input data
    BrickList glbBrickList;
    InitBrickList( inputData, glbBrickList );
    if (glbProgPhase != PUZZLE) {
        std::cout << std::endl << "Brick list after initialization: " << std::endl;
        PrintBrickList( glbBrickList );
        std::cout << std::endl << "X-Z view: " << std::endl;
        PrintProjectionXZ( glbBrickList );
        std::cout << std::endl << "Y-Z view: " << std::endl;
        PrintProjectionYZ( glbBrickList );
    }

    // make sure no bricks are floating anymore
    ConsolidateBrickList( glbBrickList );
    if (glbProgPhase != PUZZLE) {
        std::cout << std::endl << "Brick list after consolidation: " << std::endl;
        PrintBrickList( glbBrickList );
        std::cout << std::endl << "X-Z view: " << std::endl;
        PrintProjectionXZ( glbBrickList );
        std::cout << std::endl << "Y-Z view: " << std::endl;
        PrintProjectionYZ( glbBrickList );
    }

    // get test output to compare with puzzle description
    if (glbProgPhase != PUZZLE) {
        for (int i = 0; i < (int)glbBrickList.size(); i++) {
            if (glbProgPhase != PUZZLE) {
                std::cout << "Brick: " << char( 'A' + i ) << std::endl;
                BrickList vSupportees = GetSupportees( glbBrickList, glbBrickList[i] );
                if (vSupportees.size() == 0) {
                    std::cout << " supports none" << std::endl;
                } else {
                    for (int j = 0; j < (int)vSupportees.size(); j++) {
                        Brick &curSupportee = vSupportees[j];
                        BrickList vSupporters = GetSupporters( glbBrickList, curSupportee );
                        int nLabelIx = GetBrickIndex( glbBrickList, curSupportee );
                        std::cout << " is 1 of " << vSupporters.size()
                                  << " supporters of brick " << char( 'A' + nLabelIx )
                                  << std::endl;
                    }
                }
            }
        }
    }

    // collect the answer and report it
    int nCount = 0;
    for (int i = 0; i < (int)glbBrickList.size(); i++) {
        bool bCandidate = CanBeDisintegrated( glbBrickList, glbBrickList[i] );

        if (glbProgPhase != PUZZLE) {
            std::cout << "Brick: " << char( 'A' + i ) << " can " << (bCandidate ? "INDEED" : "NOT   ") << " be desintegrated" << std::endl;
        }

        nCount += (bCandidate ? 1 : 0);
    }
    std::cout << std::endl << "Answer to part 1: disintegratable bricks = " << nCount << std::endl << std::endl;

/* ========== */   tmr.TimeReport( "Timing 1 - solving puzzle part 1 : " );   // =========================^^^^^vvvvv

    // part 2 code here

    // all the work for part 2 is done in one function - run it and report the answer
    int nAnswer2 = AnalyseBrickList( glbBrickList );
    std::cout << std::endl << "Answer to part 2: total chain reaction bricks: " << nAnswer2 << std::endl << std::endl;

/* ========== */   tmr.TimeReport( "Timing 2 - solving puzzle part 2 : " );   // ==============================^^^^^

    return 0;
}
