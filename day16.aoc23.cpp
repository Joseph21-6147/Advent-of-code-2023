// AoC 2023 - Day 16 - The floor will be lava
// ==========================================

// date:  2023-12-16
// by:    Joseph21 (Joseph21-6147)

#define DAY_STRING  "day16"     //   <---  adapt this to the correct day!!

#include <iostream>
#include <fstream>
#include <algorithm>
#include <numeric>
#include <vector>
#include <deque>

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

// the data consists of map info, being empty '.', mirrors '/' and '\', and splitters '|' and '-'
typedef std::string DatumType;
typedef std::vector<DatumType> DataStream;

int MapHeight( DataStream &iMap ) { return iMap.size(); }
int MapWidth(  DataStream &iMap ) { return iMap[0].length(); }

// ==========   INPUT DATA FUNCTIONS

// hardcoded input - just to get the solution tested
void GetData_EXAMPLE( DataStream &dData ) {

    // hard code your examples here - use the same data for your test input file
    DatumType cDatum;
    cDatum = ".|...\\....";  dData.push_back( cDatum );
    cDatum = "|.-.\\.....";  dData.push_back( cDatum );
    cDatum = ".....|-...";   dData.push_back( cDatum );
    cDatum = "........|.";   dData.push_back( cDatum );
    cDatum = "..........";   dData.push_back( cDatum );
    cDatum = ".........\\";  dData.push_back( cDatum );
    cDatum = "..../.\\\\.."; dData.push_back( cDatum );
    cDatum = ".-.-/..|..";   dData.push_back( cDatum );
    cDatum = ".|....-|.\\";  dData.push_back( cDatum );
    cDatum = "..//.|....";   dData.push_back( cDatum );
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
    std::cout << "(" << MapWidth( dData ) << "x" << MapHeight( dData ) << ")" << std::endl;
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

// I need some concept of direction
enum eDirection {
    EAST = 0,
    SOUTH,
    WEST,
    NORTH
};

// convenience getters
bool IsHorizontal( eDirection eTo ) { return (eTo == EAST  || eTo == WEST ); }
bool IsVertical(   eDirection eTo ) { return (eTo == NORTH || eTo == SOUTH); }

// for testing/debugging
std::string Dir2String( eDirection dir ) {
    switch (dir) {
        case EAST : return "EAST ";
        case SOUTH: return "SOUTH";
        case WEST : return "WEST ";
        case NORTH: return "NORTH";
    }
    return "?????";
}

// gonna build a list of locations that are visited
// it's also relevant in what direction they were visited
typedef struct sLocationStruct {
    int x = -1;
    int y = -1;
    bool bVisitedFrom[4] = { false };
} LocType;
typedef std::vector<LocType> LocStream;

// global var to keep the visited locations
LocStream glbEnergizedLocs;

// for testing/debugging
void PrintEnergizedLocs( int dimX, int dimY ) {
    // prepare the map in displayable form
    std::string stub;
    std::vector<std::string> myMap;
    for (int x = 0; x < dimX; x++) stub.append( "." );
    for (int y = 0; y < dimY; y++) myMap.push_back( stub );
    // populate it
    for (int i = 0; i < (int)glbEnergizedLocs.size(); i++) {
        LocType &curLoc = glbEnergizedLocs[i];
        myMap[curLoc.y][curLoc.x] = '#';
    }
    // output it
    for (int y = 0; y < dimY; y++) {
        std::cout << myMap[y] << std::endl;
    }
}

// returns the index of the element in glbEngergizedLocs with these coordinates
// returns -1 if it's not there
int GetLocIndex( int x, int y ) {
    int nResult = -1;
    for (int i = 0; i < (int)glbEnergizedLocs.size() && nResult < 0; i++) {
        LocType &curLoc = glbEnergizedLocs[i];
        if (curLoc.x == x && curLoc.y == y) {
            nResult = i;
        }
    }
    return nResult;
}

// for tracing the beam of light I need a queue, since it's possible that 2 directions
// emerge from 1 beam
typedef struct sQueueStruct {
    int x = -1;
    int y = -1;
    eDirection eTo;
} QueueNode;
typedef std::deque<QueueNode> QueueType;

// for testing/debugging
void PrintQueueNode( QueueNode &qN ) {
    std::cout << "[(" << qN.x << "," << qN.y << ")-" << Dir2String( qN.eTo ) << "] ";
}

// for testing/debugging
void PrintQueue( QueueType &q ) {
    for (auto e : q) {
        PrintQueueNode( e );
    }
    std::cout << std::endl;
}

// the core of the beam tracing code is in the three functions NextX(), NextY() and NextDir()
// They depend on the cell type, and determine how to follow the beam
// NOTE: splitter cell types are not supported, use a combination of mirror cell types for these
int NextX( char cCellType, int pX, int pY, eDirection eTo ) {
    int nResult;
    switch (cCellType) {
        case '.': {
                switch (eTo) {
                    case EAST : nResult = pX + 1; break;
                    case WEST : nResult = pX - 1; break;
                    case SOUTH: nResult = pX    ; break;
                    case NORTH: nResult = pX    ; break;
                }
            }
            break;
        case '\\': {
                switch (eTo) {
                    case EAST : nResult = pX    ; break;
                    case WEST : nResult = pX    ; break;
                    case SOUTH: nResult = pX + 1; break;
                    case NORTH: nResult = pX - 1; break;
                }
            }
            break;
        case '/': {
                switch (eTo) {
                    case EAST : nResult = pX    ; break;
                    case WEST : nResult = pX    ; break;
                    case SOUTH: nResult = pX - 1; break;
                    case NORTH: nResult = pX + 1; break;
                }
            }
            break;
        default: std::cout << "ERROR: NextX() --> can't handle this type: " << cCellType << std::endl;
    }
    return nResult;
}

int NextY( char cCellType, int pX, int pY, eDirection eTo ) {
    int nResult;
    switch (cCellType) {
        case '.': {
                switch (eTo) {
                    case EAST : nResult = pY    ; break;
                    case WEST : nResult = pY    ; break;
                    case SOUTH: nResult = pY + 1; break;
                    case NORTH: nResult = pY - 1; break;
                }
            }
            break;
        case '\\': {
                switch (eTo) {
                    case EAST : nResult = pY + 1; break;
                    case WEST : nResult = pY - 1; break;
                    case SOUTH: nResult = pY    ; break;
                    case NORTH: nResult = pY    ; break;
                }
            }
            break;
        case '/': {
                switch (eTo) {
                    case EAST : nResult = pY - 1; break;
                    case WEST : nResult = pY + 1; break;
                    case SOUTH: nResult = pY    ; break;
                    case NORTH: nResult = pY    ; break;
                }
            }
            break;
        default: std::cout << "ERROR: NextY() --> can't handle this type: " << cCellType << std::endl;
    }
    return nResult;
}

eDirection NextDir( char cCellType, int pX, int pY, eDirection eTo ) {
    eDirection eResult;
    switch (cCellType) {
        case '.': {
                eResult = eTo;
            }
            break;
        case '\\': {
                switch (eTo) {
                    case EAST : eResult = SOUTH; break;
                    case WEST : eResult = NORTH; break;
                    case SOUTH: eResult = EAST ; break;
                    case NORTH: eResult = WEST; break;
                }
            }
            break;
        case '/': {
                switch (eTo) {
                    case EAST : eResult = NORTH; break;
                    case WEST : eResult = SOUTH; break;
                    case SOUTH: eResult = WEST ; break;
                    case NORTH: eResult = EAST ; break;
                }
            }
            break;
        default: std::cout << "ERROR: NextDir() --> can't handle this type: " << cCellType << std::endl;
    }
    return eResult;
}

// performs one step in the beam tracing. The step is directed by parameters dMap, pX, pY and eTo
// if there are steps to follow up, they are put in wipQ
void OneStep( DataStream &dMap, int pX, int pY, eDirection eTo, QueueType &wipQ ) {

    // only process if the location is in bounds
    if (pX >= 0 && pX < MapWidth( dMap ) && pY >= 0 && pY < MapHeight( dMap )) {

        // check if this location already exists in the global list
        int nLocIndex = GetLocIndex( pX, pY );
        if (nLocIndex < 0) {
            // doesn't exist: create, add to list and set nLocIndex to it
            LocType curLoc;
            curLoc.x = pX;
            curLoc.y = pY;
            nLocIndex = glbEnergizedLocs.size();
            glbEnergizedLocs.push_back( curLoc );
        }
        // grab ref to current (certainly existing) location
        LocType &curLoc = glbEnergizedLocs[ nLocIndex ];

        // convenience lambda
        auto add_queue_node = [=]( QueueType &q, char cell, int x, int y, eDirection to ) {
            QueueNode newNode = {
                NextX(   cell, x, y, to ),
                NextY(   cell, x, y, to ),
                NextDir( cell, x, y, to )
            };
            q.push_back( newNode );
        };

        if (!curLoc.bVisitedFrom[eTo]) {      // prevent cycles (they will be infinite :) )

            curLoc.bVisitedFrom[eTo] = true;
            // decide what to do based on the cell type encountered
            char cCell = dMap[pY][pX];
            switch (cCell) {
                case '.' :
                case '\\':
                case '/' : {
                        add_queue_node( wipQ, cCell, pX, pY, eTo );
                    }
                    break;
                case '-': {
                        if (IsVertical( eTo )) {
                            add_queue_node( wipQ, '\\', pX, pY, eTo );  // split
                            add_queue_node( wipQ, '/' , pX, pY, eTo );
                        } else {
                            add_queue_node( wipQ, '.' , pX, pY, eTo );
                        }
                    }
                    break;
                case '|': {
                        if (IsHorizontal( eTo )) {
                            add_queue_node( wipQ, '\\', pX, pY, eTo );  // split
                            add_queue_node( wipQ, '/' , pX, pY, eTo );
                        } else {
                            add_queue_node( wipQ, '.' , pX, pY, eTo );
                        }
                    }
                    break;
                default: std::cout << "ERROR: OneStep() --> can't handle this cell type: " << cCell << std::endl;
            }
        }
    }
}

// uses initialNode to start tracing the beam into dMap. Uses a work in progress queue internally.
// Processing only stops if the queue is empty. Finding cycles or running out of bounds will stop
// tracing on a particular beam
void SimulateBeam( DataStream &dMap, QueueNode initialNode, bool bOutput = false ) {

    glbEnergizedLocs.clear();

    // enqueue the initial node
    QueueType qWIP;
    qWIP.push_back( initialNode );

    while (!qWIP.empty()) {
        // serve node from queue
        QueueNode curNode = qWIP.front();
        qWIP.pop_front();

        if (bOutput) {
            std::cout << "INFO: SimulateBeam() --> calling OneStep with location: (" << curNode.x << ", " << curNode.y
                      << "), with direction: " << Dir2String( curNode.eTo ) << std::endl;
        }

        OneStep( dMap, curNode.x, curNode.y, curNode.eTo, qWIP );

        if (bOutput) {
            std::cout << "INFO: SimulateBeam() --> WIP Queue status: " << std::endl;
            PrintQueue( qWIP );

            int nrEnergized = glbEnergizedLocs.size();

            std::cout << "INFO: SimulateBeam() --> nr cells energized: " << nrEnergized << std::endl;
            PrintEnergizedLocs( MapWidth( dMap ), MapHeight( dMap ));
        }
    }
}

// ----------   part 2

// I changed (the interface of) SimulateBeam() from part 1 to accommodate part 2.
// All other changes needed for part 2 are put in main()

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

    // start beam tracing process
    QueueNode startNode = { 0, 0, EAST };
    SimulateBeam( inputData, startNode );
    std::cout << std::endl << "Answer to part 1: nr of cells energized: " << glbEnergizedLocs.size() << std::endl << std::endl;

/* ========== */   tmr.TimeReport( "Timing 1 - solving puzzle part 1 : " );   // =========================^^^^^vvvvv

    // part 2 code here


    int nLargestYield = -1;
    // iterate north and south borders as starting positions
    for (int x = 0; x < MapWidth( inputData ); x++) {
        QueueNode startNodeDn = { x,                          0, SOUTH };
        QueueNode startNodeUp = { x, MapHeight( inputData ) - 1, NORTH };

        SimulateBeam( inputData, startNodeDn );
        int nYieldDn = glbEnergizedLocs.size();
        SimulateBeam( inputData, startNodeUp );
        int nYieldUp = glbEnergizedLocs.size();

        nLargestYield = std::max( nLargestYield, nYieldDn );
        nLargestYield = std::max( nLargestYield, nYieldUp );
    }
    // iterate east and west borders as starting positions
    for (int y = 0; y < MapHeight( inputData ); y++) {
        QueueNode startNodeLt = {                         0, y, EAST };
        QueueNode startNodeRt = { MapWidth( inputData ) - 1, y, WEST };

        SimulateBeam( inputData, startNodeLt );
        int nYieldLt = glbEnergizedLocs.size();
        SimulateBeam( inputData, startNodeRt );
        int nYieldRt = glbEnergizedLocs.size();

        nLargestYield = std::max( nLargestYield, nYieldLt );
        nLargestYield = std::max( nLargestYield, nYieldRt );
    }

    std::cout << std::endl << "Answer to part 2: max energy yield = " << nLargestYield << std::endl << std::endl;

/* ========== */   tmr.TimeReport( "Timing 2 - solving puzzle part 2 : " );   // ==============================^^^^^

    return 0;
}
