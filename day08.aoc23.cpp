// AoC 2023 - Day 08 - Haunted wasteland
// =====================================

// date:  2023-12-08
// by:    Joseph21 (Joseph21-6147)

#define DAY_STRING  "day08"     //   <---  adapt this to the correct day!!

#include <iostream>
#include <fstream>
#include <algorithm>
#include <numeric>
#include <vector>
#include <map>

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

// the data consists of
// a long instruction string (modeled further on)
// triplets of node name, left neighbour name and right neighbour name (all modeled as strings)
typedef struct sDatumStruct {
    std::string sID   = "";
    std::string sLeft = "";
    std::string sRght = "";
} DatumType;
typedef std::vector<DatumType> DataStream;

// data consists of two separate parts - this part models the instruction string
std::string glbInstruction = "";
// don't forget to reset to 0 for part 2 :)
int glbInstrIndex = 0;

// function to get next instruction and wrap the index around the instruction string
// where needed
char GetNextInstruction() {
    char cInstr = glbInstruction[  glbInstrIndex ];
    glbInstrIndex = (glbInstrIndex + 1) % (int)glbInstruction.length();
    return cInstr;
}

// ==========   INPUT DATA FUNCTIONS

// hardcoded input - just to get the solution tested
void GetData_EXAMPLE( DataStream &dData ) {

    // hard code your examples here - use the same data for your test input file
    DatumType cDatum;

//    glbInstruction = "RL";    // example 1 (part 1)
//    glbInstrIndex = 0;
//
//    cDatum = { "AAA", "BBB", "CCC" }; dData.push_back( cDatum );
//    cDatum = { "BBB", "DDD", "EEE" }; dData.push_back( cDatum );
//    cDatum = { "CCC", "ZZZ", "GGG" }; dData.push_back( cDatum );
//    cDatum = { "DDD", "DDD", "DDD" }; dData.push_back( cDatum );
//    cDatum = { "EEE", "EEE", "EEE" }; dData.push_back( cDatum );
//    cDatum = { "GGG", "GGG", "GGG" }; dData.push_back( cDatum );
//    cDatum = { "ZZZ", "ZZZ", "ZZZ" }; dData.push_back( cDatum );

//    glbInstruction = "LLR";    // example 2 (part 1)
//    glbInstrIndex = 0;
//
//    cDatum = { "AAA", "BBB", "BBB" }; dData.push_back( cDatum );
//    cDatum = { "BBB", "AAA", "ZZZ" }; dData.push_back( cDatum );
//    cDatum = { "ZZZ", "ZZZ", "ZZZ" }; dData.push_back( cDatum );

    glbInstruction = "LR";     // example 3 (part 2)
    glbInstrIndex = 0;

    cDatum = { "11A", "11B", "XXX" }; dData.push_back( cDatum );
    cDatum = { "11B", "XXX", "11Z" }; dData.push_back( cDatum );
    cDatum = { "11Z", "11B", "XXX" }; dData.push_back( cDatum );
    cDatum = { "22A", "22B", "XXX" }; dData.push_back( cDatum );
    cDatum = { "22B", "22C", "22C" }; dData.push_back( cDatum );
    cDatum = { "22C", "22Z", "22Z" }; dData.push_back( cDatum );
    cDatum = { "22Z", "22B", "22B" }; dData.push_back( cDatum );
    cDatum = { "XXX", "XXX", "XXX" }; dData.push_back( cDatum );
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

    // read instruction string first
    getline( dataFileStream, glbInstruction );
    glbInstrIndex = 0;

    // read node info later
    while (getline( dataFileStream, sLine )) {
        if (sLine.length() > 0) {    // non empty line
            DatumType cDatum;
            cDatum.sID   = get_token_dlmtd( " = (", sLine );
            cDatum.sLeft = get_token_dlmtd( ", "  , sLine );
            cDatum.sRght = get_token_dlmtd( ")"   , sLine );
            vData.push_back( cDatum );
        }
    }
    dataFileStream.close();
}

void GetData_TEST(   DataStream &dData ) { std::string sInputFile = DAY_STRING; sInputFile.append( ".input.test.txt"   ); ReadInputData( sInputFile, dData ); }
void GetData_PUZZLE( DataStream &dData ) { std::string sInputFile = DAY_STRING; sInputFile.append( ".input.puzzle.txt" ); ReadInputData( sInputFile, dData ); }

// ==========   CONSOLE OUTPUT FUNCTIONS

void PrintInstruction( bool bEndl = true ) {
    std::cout << glbInstruction << " <-- index = " << glbInstrIndex;
    if (bEndl) { std::cout << std::endl; }
}

// output to console for testing
void PrintDatum( DatumType &iData, bool bEndl = true ) {
    // define your datum printing code here
    std::cout << iData.sID << " = ( " << iData.sLeft << ", " << iData.sRght << " )";
    if (bEndl) { std::cout << std::endl; }
}

// output to console for testing
void PrintDataStream( DataStream &dData ) {
    PrintInstruction();
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

// create graph nodes
// for convenience I put the datum as a whole in the graph node
typedef struct sGraphStruct {
    DatumType iDatum;
    sGraphStruct *pLeft = nullptr;
    sGraphStruct *pRght = nullptr;
} GraphNode;

// to check that all pointers are set correctly
void PrintGraphNode( GraphNode *pNode ) {
    if (pNode == nullptr) {
        std::cout << "NULL" << std::endl;
    } else {
        std::cout << "Datum = "; PrintDatum( pNode->iDatum, false );
        std::cout << ", lt ptr --> " << (pNode->pLeft != nullptr ? pNode->pLeft->iDatum.sID : "NULL");
        std::cout << ", rt ptr --> " << (pNode->pRght != nullptr ? pNode->pRght->iDatum.sID : "NULL");
        std::cout << std::endl;
    }
}

// convenience intermediate ds for easy node finding and graph building
typedef std::map<std::string, GraphNode *> GraphMap;

// creates and returns a GraphMap
GraphMap GetGraphMap( DataStream &iData ) {
    GraphMap mResult;
    for (int i = 0; i < (int)iData.size(); i++) {
        GraphNode *newNode = new GraphNode;
        newNode->iDatum = iData[i];
        mResult.insert( { iData[i].sID, newNode } );
    }
    return mResult;
}

// uses gm to quickly find node with sID in the graph
// returns a pointer to it
GraphNode *FindGraphNodePtr( GraphMap &gm, std::string sID ) {
    GraphMap::iterator iter = gm.find( sID );
    if (iter == gm.end()) {
        std::cout << "ERROR: FindGraphNodePtr() --> element: " << sID << " not found!" << std::endl;
    }
    return (*iter).second;
}

// Uses graph map gm to build the graph up. This is just setting the left and right pointer
// for each graph node
void CreateGraph( GraphMap &gm ) {
    for (auto &elt : gm) {
        elt.second->pLeft = FindGraphNodePtr( gm, elt.second->iDatum.sLeft );
        elt.second->pRght = FindGraphNodePtr( gm, elt.second->iDatum.sRght );
    }
}

// for testing/debugging
void PrintGraph( GraphMap &gm ) {
    for (auto elt : gm) {
        PrintGraphNode( elt.second );
    }
}

// to check whether sID is source or destination node
bool IsSrc( std::string &s ) { return s == "AAA"; }
bool IsDst( std::string &s ) { return s == "ZZZ"; }

int FindDest( GraphNode *src, bool bDebug = false ) {
    int nResult = 0;
    if (src == nullptr) {
        std::cout << "ERROR: FindDest() --> can't find anything on nullptr input!" << std::endl;
    } else {

        // walk the graph, following instructions L or R
        GraphNode *pCurNode = src;
        while (!IsDst( pCurNode->iDatum.sID )) {
            char cInstr = GetNextInstruction();
            if (bDebug) std::cout << "stepping from: " << pCurNode->iDatum.sID << " with instruction: " << cInstr;
            switch (cInstr) {
                case 'L': {
                        pCurNode = pCurNode->pLeft;
                        if (bDebug) std::cout << " LEFT  ";
                    }
                    break;
                case 'R': {
                        pCurNode = pCurNode->pRght;
                        if (bDebug) std::cout << " RIGHT ";
                    }
                    break;
                default : std::cout << "ERROR: FindDest() --> unexpected instruction: " << cInstr << std::endl;
            }
            if (bDebug) std::cout << " to: " << pCurNode->iDatum.sID << std::endl;
            nResult += 1;
        }
    }
    return nResult;
}

// ----------   part 2

// to check if iDatum is start or end node (cf part 2 specs)
bool IsStartNode( DatumType &iDatum ) { return iDatum.sID[2] == 'A'; }
bool IsEndNode(   DatumType &iDatum ) { return iDatum.sID[2] == 'Z'; }

// start at graph node pStart, take one step according to cInstr, and return the
// resulting graph node as a pointer
GraphNode *OneStep( GraphNode *pStart, char cInstr, bool bDebug = false ) {

    GraphNode *pResult = nullptr;
    if (pStart == nullptr) {
        std::cout << "ERROR: OneStep() --> can't find anything on nullptr input!" << std::endl;
    } else {
        if (bDebug) std::cout << "stepping from: " << pStart->iDatum.sID << " with instruction: " << cInstr;
        switch (cInstr) {
            case 'L': {
                    pResult = pStart->pLeft;
                    if (bDebug) std::cout << " LEFT  ";
                }
                break;
            case 'R': {
                    pResult = pStart->pRght;
                    if (bDebug) std::cout << " RIGHT ";
                }
                break;
            default : std::cout << "ERROR: OneStep() --> unidentified instruction: " << cInstr << std::endl;
        }
        if (bDebug) std::cout << " to: " << pResult->iDatum.sID << std::endl;
    }
    return pResult;
}

// I need an easy way to create a list of nodes
typedef std::vector<GraphNode *> NodePtrList;

// returns the list of all nodes in graph gm that are start nodes (according to specs part 2)
NodePtrList GetStartNodes( GraphMap &gm ) {
    NodePtrList vResult;
    for (auto &elt : gm) {
        if (IsStartNode( elt.second->iDatum )) {
            vResult.push_back( elt.second );
        }
    }
    return vResult;
}

// returns true if all nodes in npl are end nodes (according to specs part 2)
bool AllStopped( NodePtrList &npl ) {
    return std::all_of(
        npl.begin(),
        npl.end(),
        [=]( GraphNode *p ) {
            return IsEndNode( p->iDatum );
        }
    );
}

// LCM = least common multiple. C++ provides a std::lcm(), but it only takes two
// numbers as arguments. So I wrote this function that takes a list (vector)
// of numbers and returns the lcm of them all
long long my_lcm( std::vector<long long> &vInput ) {
    int nSize = (int)vInput.size();
    long long nResult = 1;
    if (nSize <= 0) {
        std::cout << "ERROR: my_lcm() --> invalid input" << std::endl;
        nResult = -1;
    } else {
        switch (nSize) {
            case 1:
                nResult = vInput[0];
                break;
            case 2:
                nResult = std::lcm( vInput[0], vInput[1] );
                break;
            default: {
                    // copy input without element 0. Note that size of vInput > 2 so this can be done safely
                    std::vector<long long> vSmallerInput( std::next( vInput.begin()), vInput.end());
                    // calculate lcm using recursion
                    nResult = std::lcm( vInput[0], my_lcm( vSmallerInput ));
                }
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

    // setup graph
    GraphMap glbGM = GetGraphMap( inputData );
    CreateGraph( glbGM );
    // and check it
    if (glbProgPhase != PUZZLE) {
        PrintGraph( glbGM );
    }
    // walk the graph from source to destination and collect nr of steps required
    GraphNode *glbSrc = FindGraphNodePtr( glbGM, "AAA" );
    int nrSteps = FindDest( glbSrc );

    std::cout << std::endl << "Answer to part 1: nr steps to find destination = " << nrSteps << std::endl << std::endl;

/* ========== */   tmr.TimeReport( "Timing 1 - solving puzzle part 1 : " );   // =========================^^^^^vvvvv

    // part 2 code here


    // attempt 1 - brute force approach
//    NodePtrList vNodes = GetStartNodes( glbGM );
//    int nSteps = 0;
//    bool bFinished = AllStopped( vNodes );
//    while (!bFinished) {
//        char cInstr = GetNextInstruction();
//        for (int i = 0; i < (int)vNodes.size(); i++) {
//            GraphNode *aux = OneStep( vNodes[i], cInstr, false );
//            vNodes[i] = aux;
//        }
//        nSteps += 1;
//            if (nSteps > 0 && nSteps % 1000000000 == 0) {
//                tmr.TimeReport( "Timing 1 billion steps: " );
//            }
//        bFinished = AllStopped( vNodes );
//    }


    // attempt 2 approach using least common multiple

    // collect the number of steps PER NODE to reach an end node
    // (and assume they cycle - this is true for the example)
    NodePtrList vNodes = GetStartNodes( glbGM );
    std::vector<long long> vStepsPerStartNode;

    for (int i = 0; i < (int)vNodes.size(); i++) {
        // reset instruction index
        glbInstrIndex = 0;
        // find nr of steps to get to end node
        long long nSteps = 0;
        while (!IsEndNode( vNodes[i]->iDatum )) {
            char cInstr = GetNextInstruction();
            vNodes[i] = OneStep( vNodes[i], cInstr, false );
            nSteps += 1;
        }
        // store nr of steps for this start node
        vStepsPerStartNode.push_back( nSteps );
    }

    // now get the least common multiplier for all of the collected cycle times
    long long llLCM = my_lcm( vStepsPerStartNode );
    std::cout << std::endl << "Answer to part 2: nr steps to reach all end nodes simultaneously = " << llLCM << std::endl << std::endl;

/* ========== */   tmr.TimeReport( "Timing 2 - solving puzzle part 2 : " );   // ==============================^^^^^

    return 0;
}
