// AoC 2023 - Day 23 - A long walk
// ===============================

// date:  2023-12-23
// by:    Joseph21 (Joseph21-6147)

#define DAY_STRING  "day23"     //   <---  adapt this to the correct day!!

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

// the data consists of a map that indicates paths (.), forest (#), and steep slopes (^, >, v, and <).
typedef std::string DatumType;
typedef std::vector<DatumType> DataStream;

// this little group of convenience functions grew over the course of AoC 2023 and is helpful

int MapWidth(  DataStream &dMap ) { return dMap[0].length(); }
int MapHeight( DataStream &dMap ) { return dMap.size(); }

bool MapInBounds( DataStream &dMap, int x, int y ) {
    return (0 <= x && x < MapWidth(  dMap ) &&
            0 <= y && y < MapHeight( dMap ));
}

char MapValue( DataStream &dMap, int x, int y ) {
    if (!MapInBounds( dMap, x, y )) {
        std::cout << "ERROR: MapValue() --> coordinates out of range" << std::endl;
    }
    return dMap[y][x];
}

// overloaded versions for vi2d argument
bool MapInBounds( DataStream &dMap, vi2d loc ) { return MapInBounds( dMap, loc.x, loc.y ); }
char MapValue(    DataStream &dMap, vi2d loc ) { return MapValue(    dMap, loc.x, loc.y ); }

// ==========   INPUT DATA FUNCTIONS

// hardcoded input - just to get the solution tested
void GetData_EXAMPLE( DataStream &dData ) {

    // hard code your examples here - use the same data for your test input file
    DatumType cDatum;
    cDatum = "#.#####################"; dData.push_back( cDatum );
    cDatum = "#.......#########...###"; dData.push_back( cDatum );
    cDatum = "#######.#########.#.###"; dData.push_back( cDatum );
    cDatum = "###.....#.>.>.###.#.###"; dData.push_back( cDatum );
    cDatum = "###v#####.#v#.###.#.###"; dData.push_back( cDatum );
    cDatum = "###.>...#.#.#.....#...#"; dData.push_back( cDatum );
    cDatum = "###v###.#.#.#########.#"; dData.push_back( cDatum );
    cDatum = "###...#.#.#.......#...#"; dData.push_back( cDatum );
    cDatum = "#####.#.#.#######.#.###"; dData.push_back( cDatum );
    cDatum = "#.....#.#.#.......#...#"; dData.push_back( cDatum );
    cDatum = "#.#####.#.#.#########v#"; dData.push_back( cDatum );
    cDatum = "#.#...#...#...###...>.#"; dData.push_back( cDatum );
    cDatum = "#.#.#v#######v###.###v#"; dData.push_back( cDatum );
    cDatum = "#...#.>.#...>.>.#.###.#"; dData.push_back( cDatum );
    cDatum = "#####v#.#.###v#.#.###.#"; dData.push_back( cDatum );
    cDatum = "#.....#...#...#.#.#...#"; dData.push_back( cDatum );
    cDatum = "#.#########.###.#.#.###"; dData.push_back( cDatum );
    cDatum = "#...###...#...#...#.###"; dData.push_back( cDatum );
    cDatum = "###.###.#.###v#####v###"; dData.push_back( cDatum );
    cDatum = "#...#...#.#.>.>.#.>.###"; dData.push_back( cDatum );
    cDatum = "#.###.###.#.###.#.#v###"; dData.push_back( cDatum );
    cDatum = "#.....###...###...#...#"; dData.push_back( cDatum );
    cDatum = "#####################.#"; dData.push_back( cDatum );
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

// these functions are written after analysis of the EXAMPLE and PUZZLE input data
vi2d GetStrtPos( DataStream dMap ) { return {                    1,                     0 }; }
vi2d GetStopPos( DataStream dMap ) { return { MapWidth( dMap ) - 2, MapHeight( dMap ) - 1 }; }

// weighted directed graph implementation - could probably be better, but good enough for now
// ======================================

// edge record - edges are identified by the combination of their start and stop location
typedef struct sGraphEdgeStruct {
    vi2d strtNd, stopNd;
    int nWeight = 1;
} GraphEdge;
typedef std::vector<GraphEdge> EdgeList;

// for testing/debugging
void PrintGraphEdge( GraphEdge &gE ) {
    std::cout << "Edge from " << gE.strtNd.ToString() << " to " << gE.stopNd.ToString() << ", weight: " << gE.nWeight << std::endl;
}

// for testing/debugging
void PrintGraphEdgeList( EdgeList &vEdges ) {
    for (auto e : vEdges) {
        std::cout << "   ";
        PrintGraphEdge( e );
    }
}

// graph node record - a node has a location and 0 or more edges
typedef struct sGraphNodeStruct {
    vi2d nodeLoc;
    EdgeList vEdges;  // directed, so out going!
} GraphNode;
typedef std::vector<GraphNode> NodeStream;

typedef std::vector<vi2d> TraceType;

// for testing/debugging
void PrintGraphNode( GraphNode &gN ) {
    std::cout << "Node at " << gN.nodeLoc.ToString() << " has " << (gN.vEdges.empty() ? "NO edges." : " the following edges: ") << std::endl;
    PrintGraphEdgeList( gN.vEdges );
}

// for testing/debugging
void PrintGraphNodeStream( NodeStream &vNodes ) {
    for (auto e: vNodes) {
        PrintGraphNode( e );
    }
}

// used for calculations with directions and four connected step taking
// quite convenient in combination with vi2d type coordinates
vi2d glbDirections[4] = {
    { +1,  0 },    // East
    {  0, +1 },    // South
    { -1,  0 },    // West
    {  0, -1 }     // North
};

// a step to cell at location b is considered "physically possible" if
//    - it stays within the map boundaries
//    - it doesn't step into a forest cell
bool IsStepPhysicallyPossible( DataStream &dMap, vi2d &b ) {
    return MapInBounds( dMap, b ) &&           // 1. these coordinates are in bound
           MapValue(    dMap, b ) != '#';      // 2. this map value is not a forest
}

// returns list of physically possible steps from location a
TraceType StepsPhysicallyPossible( DataStream &dMap, vi2d &a ) {
    TraceType vResult;
    // return locations for each direction if it's possible to step there
    for (int i = 0; i < 4; i++) {
        vi2d nextPos = a + glbDirections[i];
        if (IsStepPhysicallyPossible( dMap, nextPos )) {
            vResult.push_back( nextPos );
        }
    }
    return vResult;
}

// returns nr of physically possible steps from location a
// (theoretically between 0 and 4)
int CountPhysicallyPossible( DataStream &dMap, vi2d &a ) {
    TraceType vTrace = StepsPhysicallyPossible( dMap, a );
    return int( vTrace.size());
}

// This function collects and returns all graph nodes from the input data (map). The graph nodes are all the
// locations in the input data where an actual choice is possible.
NodeStream GetGraphNodes( DataStream &dMap ) {

    // insert source and sink nodes in the graph
    NodeStream vResult;
    vi2d strtPos = GetStrtPos( dMap ); vResult.push_back( { strtPos } );
    vi2d stopPos = GetStopPos( dMap ); vResult.push_back( { stopPos } );

    // analyse input map and complete NodeStream with it
    for (int y = 0; y < MapHeight( dMap ); y++) {
        for (int x = 0; x < MapWidth( dMap ); x++) {

            vi2d curPos = { x, y };
            switch (MapValue( dMap, curPos )) {
                case '#': // ignore all these, they are not interesting for now
                case '^':
                case 'v':
                case '<':
                case '>': break;
                case '.': {
                        int nrOptions = CountPhysicallyPossible( dMap, curPos );
                        switch (nrOptions) {
                            case  0: {
                                    std::cout << "ERROR: GetGraphNodes() --> 0 options shouldn't occur" << std::endl;
                                }
                                break;
                            case  1: // is possible for start and stop locations, but we inserted them already
                            case  2: // most '.' locations will be of this type, just ignore
                                break;
                            case  3: // these are real choice locations
                            case  4: {
                                    vResult.push_back( { curPos } );
                                }
                                break;
                            default: std::cout << "ERROR: GetGraphNodes() --> unmanageble nr of options (shouldn't occur): " << nrOptions << std::endl;
                        }
                    }
                    break;
                default : std::cout << "WARNING: GetGraphNodes() --> unknown map value: " << MapValue( dMap, curPos ) << std::endl;
            }
        }
    }
    return vResult;
}

// this function is designed to crawl through corridors where there's only one option to go forward.
// So gard calling this function with checks on the number of possible steps (should be exactly 2)
// prevLoc and curLoc model the most recent step, and are used to work out direction. The next location
// is returned via ref. parameter nextLoc.
// If this step is invalid due to a steep slope, the function returns false.

// NOTE: in part 2 I added a flag, since for part 2 it suffices to disable only a small part of this code
bool NextStep( DataStream &dMap, vi2d prevLoc, vi2d curLoc, vi2d &nextLoc, bool bPart1 ) {

    bool bResult = false;
    // get a list of all steps that are possible from curLoc
    // this list should contain 2 elements, and include prevLoc
    TraceType vNextSteps = StepsPhysicallyPossible( dMap, curLoc );
    if (vNextSteps.size() != 2) {
        // size should be 2...
        std::cout << "ERROR: NextStep() --> Unexpected nr of options encountered " << std::endl;
    } else {
        // there are indeed two next steps possible, one of them must be prevLoc, get the other for nextLoc
        vi2d candidate;
        if (prevLoc == vNextSteps[0]) {
            candidate = vNextSteps[1];
        } else if (prevLoc == vNextSteps[1]) {
            candidate = vNextSteps[0];
        } else {
            std::cout << "ERROR: NextStep() --> Unexpected situation encountered " << std::endl;
        }
        // for part 1: check if a direction arrow is present and if it is aligned with this step
        vi2d nextStep = candidate - curLoc;
        if (bPart1) {
            switch (MapValue( dMap, curLoc )) {
                case '>': bResult = (nextStep == glbDirections[0]); break;  // east
                case 'v': bResult = (nextStep == glbDirections[1]); break;  // south
                case '<': bResult = (nextStep == glbDirections[2]); break;  // west
                case '^': bResult = (nextStep == glbDirections[3]); break;  // north
                default : bResult = true;
            }
        } else {
            // for part 2 these checks are cancelled
            bResult = true;
        }
        if (bResult) {
            nextLoc = candidate;
        }
    }
    return bResult;
}

// Collect all the edges in a list and return them. This function heavily uses the previous NextStep()
// function to detect end point of corridors (parts of the map that allow no alternatives in the path)
EdgeList GetGraphEdges( DataStream &dMap, NodeStream &vNodes, bool bPart1 = true ) {
    EdgeList vResult;

    for (int i = 0; i < (int)vNodes.size(); i++) {
        vi2d curPos = vNodes[i].nodeLoc;

        // create a list of next step to examine
        std::vector<vi2d> vStepList = StepsPhysicallyPossible( dMap, curPos );

        // now examine these next steps in turn
        for (int j = 0; j < (int)vStepList.size(); j++) {
            vi2d &curNextStep = vStepList[j];
            bool bSuccess = true;
            int nStepCnt = 1;
            vi2d auxCurPos = curPos;
            while (CountPhysicallyPossible( dMap, curNextStep ) == 2 && bSuccess) {
                // get the step after nextStep
                vi2d nextNextStep;
                bSuccess = NextStep( dMap, auxCurPos, curNextStep, nextNextStep, bPart1 );
                auxCurPos = curNextStep;
                curNextStep = nextNextStep;
                nStepCnt += 1;
            }
            // either bSuccess is false and trace must not be used as edge for this node
            // or bSuccess is true and nStepCnt contains the weight, and
            // curNextStep is the destination location
            if (bSuccess) {
                vResult.push_back( { curPos, curNextStep, nStepCnt } );
            }
        }
    }
    return vResult;
}

// returns index into vNodes where the node with location loc is found
// returns -1 if not found
int GetNodeIndex( NodeStream &vNodes, vi2d &loc ) {
    int nResult = -1;
    for (int i = 0; i < (int)vNodes.size() && nResult < 0; i++) {
        GraphNode &curNode = vNodes[i];
        if (curNode.nodeLoc == loc) {
            nResult = i;
        }
    }
    return nResult;
}

// returns index into vEdges where the edge is found that has locA and locB as start and end points
// returns -1 if not found
int GetEdgeIndex( EdgeList &vEdges, vi2d &locA, vi2d &locB ) {
    int nResult = -1;
    for (int i = 0; i < (int)vEdges.size() && nResult < 0; i++) {
        GraphEdge &curEdge = vEdges[i];
        if (curEdge.strtNd == locA && curEdge.stopNd == locB) {
            nResult = i;
        }
    }
    return nResult;
}

// returns the weight of the edge between locA and locB
// produces error message (and returns garbage) if that edge is not found
int GetEdgeWeight( EdgeList &vEdges, vi2d &locA, vi2d &locB ) {
    int nEdgeIx = GetEdgeIndex( vEdges, locA, locB );
    if (nEdgeIx < 0) {
        std::cout << "ERROR: GetEdgeWeight() --> couldn't find edge index for locations: " << locA.ToString() << " and " << locB.ToString() << std::endl;
    }
    return vEdges[nEdgeIx].nWeight;
}

// integrate the edges from the edge table into the nodes in vNodes
void InsertEdgesIntoNodes( NodeStream &vNodes, EdgeList &vEdges ) {
    // iterate over all nodes
    for (int i = 0; i < (int)vNodes.size(); i++) {
        // per node, look up the edges that have this node as starting point,
        // and insert them into this node
        GraphNode &curNode = vNodes[i];
        for (int j = 0; j < (int)vEdges.size(); j++) {
            GraphEdge &curEdge = vEdges[j];
            if (curNode.nodeLoc == curEdge.strtNd) {
                curNode.vEdges.push_back( curEdge );
            }
        }
    }
}

// global table of paths, filled by GeneratePaths()
std::vector<std::vector<int>> glbPathsTable;

// collects info on vPath, the number of nodes in it, and the sum of all weights along the edges between it's nodes
void GetPathStats( NodeStream &vNodes, std::vector<int> vPath, int &nodes, int &totalWeight ) {
    nodes = vPath.size();
    int nResult = 0;
    for (int i = 0; i < (int)vPath.size() - 1; i++) {
        int nCurNodeIx = vPath[i    ];
        int nNxtNodeIx = vPath[i + 1];
        nResult += GetEdgeWeight( vNodes[nCurNodeIx].vEdges, vNodes[nCurNodeIx].nodeLoc, vNodes[nNxtNodeIx].nodeLoc );
    }
    totalWeight = nResult;
}

// this function brute forces every possible path in a breadth first manner (using a queue)
void GeneratePaths( DataStream &dMap, NodeStream &vNodes, bool bPart1 = true ) {
    // create a queue for partial paths
    std::deque<std::vector<int>> pathsQueue;

    // get an index to the stop node in the graph, you need it to check if the path is complete
    vi2d stopNode = GetStopPos( dMap );
    int nStopNodeIndex = GetNodeIndex( vNodes, stopNode );

    // get an index to the start node in the graph
    vi2d strtNode = GetStrtPos( dMap );
    int nStrtNodeIndex = GetNodeIndex( vNodes, strtNode );

    // push it to an initial path, and enqueue that path
    std::vector<int> startPath = { nStrtNodeIndex };
    pathsQueue.push_back( startPath );

    int nMaxPathLen = 0;
    while (!pathsQueue.empty()) {
        // get next partial path to work on from the queue
        std::vector<int> curPath = pathsQueue.front();
        pathsQueue.pop_front();

        // grab a handle to the last node of this path, to extend on it
        int nLastNodeIx = curPath.back();

        if (nLastNodeIx == nStopNodeIndex) {
            // path completed, put it to paths table
            glbPathsTable.push_back( curPath );

            int nNrNodes, nPathLen;
            GetPathStats( vNodes, curPath, nNrNodes, nPathLen );
            // check if this path is longer than the currently longest, and adapt if it is
            nMaxPathLen = std::max( nMaxPathLen, nPathLen );

        } else {
            GraphNode &lastNode = vNodes[nLastNodeIx];
            // iterate all edges of the last node of this path
            for (int i = 0; i < (int)lastNode.vEdges.size(); i++) {
                GraphEdge &curEdge = lastNode.vEdges[i];
                int nOtherEndIndex = GetNodeIndex( vNodes, curEdge.stopNd );
                // check if the other end of this edge is already visited. If not, add
                // this index to *a copy of* the path and enqueue it
                if (std::count( curPath.begin(), curPath.end(), nOtherEndIndex ) == 0) {
                    std::vector<int> newPath( curPath );
                    newPath.push_back( nOtherEndIndex );
                    pathsQueue.push_back( newPath );
                }
            }
        }
    }
    std::cout << "GeneratePaths() --> max path len found = " << nMaxPathLen << std::endl;
}

// ----------   part 2

// there's no separate code for part 2, since part 2 turned out to be solved with a very small adaptation of part 1 code

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

    std::cout << std::endl;
    NodeStream glbNodes = GetGraphNodes( inputData );
    if (glbProgPhase != PUZZLE) PrintGraphNodeStream( glbNodes );

    EdgeList glbEdges = GetGraphEdges( inputData, glbNodes );
    if (glbProgPhase != PUZZLE) PrintGraphEdgeList( glbEdges );

    InsertEdgesIntoNodes( glbNodes, glbEdges );
    if (glbProgPhase != PUZZLE) PrintGraphNodeStream( glbNodes );

    GeneratePaths( inputData, glbNodes );    // the answer is output from this function
    std::cout << std::endl;

/* ========== */   tmr.TimeReport( "Timing 1 - solving puzzle part 1 : " );   // =========================^^^^^vvvvv

    // part 2 code here

    // need to build the graph up from scratch again, since the edges and nodes are assembled differently for part 2
    std::cout << std::endl;
    NodeStream glbNodes2 = GetGraphNodes( inputData );
    if (glbProgPhase != PUZZLE) PrintGraphNodeStream( glbNodes2 );

    EdgeList glbEdges2 = GetGraphEdges( inputData, glbNodes2, false );
    if (glbProgPhase != PUZZLE) PrintGraphEdgeList( glbEdges2 );

    InsertEdgesIntoNodes( glbNodes2, glbEdges2 );
    if (glbProgPhase != PUZZLE) PrintGraphNodeStream( glbNodes2 );

    GeneratePaths( inputData, glbNodes2, false );    // the answer is output from this function
    std::cout << std::endl;

/* ========== */   tmr.TimeReport( "Timing 2 - solving puzzle part 2 : " );   // ==============================^^^^^

    return 0;
}
