// AoC 2023 - Day 15 - Lens library
// ================================

// date:  2023-12-15
// by:    Joseph21 (Joseph21-6147)

#define DAY_STRING  "day15"     //   <---  adapt this to the correct day!!

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

// the data consists of operations (modeled as a string), separated by commas
typedef std::string DatumType;
typedef std::vector<DatumType> DataStream;

// ==========   INPUT DATA FUNCTIONS

// hardcoded input - just to get the solution tested
void GetData_EXAMPLE( DataStream &dData ) {

    // hard code your examples here - use the same data for your test input file
    DatumType cDatum;
//    cDatum = "HASH"; dData.push_back( cDatum );    // part 1 - example 1

    cDatum = "rn=1"; dData.push_back( cDatum );    // part 1 - example 2
    cDatum = "cm-" ; dData.push_back( cDatum );
    cDatum = "qp=3"; dData.push_back( cDatum );
    cDatum = "cm=2"; dData.push_back( cDatum );
    cDatum = "qp-" ; dData.push_back( cDatum );
    cDatum = "pc=4"; dData.push_back( cDatum );
    cDatum = "ot=9"; dData.push_back( cDatum );
    cDatum = "ab=5"; dData.push_back( cDatum );
    cDatum = "pc-" ; dData.push_back( cDatum );
    cDatum = "pc=6"; dData.push_back( cDatum );
    cDatum = "ot=7"; dData.push_back( cDatum );
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
        while (sLine.length() > 0) {    // non empty line
            DatumType cDatum = get_token_dlmtd( ",", sLine );
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
    std::cout << StringAlignedL( iData, 10 );
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

// calculate and return hash value cf puzzle specs
int GetHashValue( std::string &s ) {
    int nCurVal = 0;
    for (int i = 0; i < (int)s.length(); i++) {
        int nTmp = nCurVal + int( s[i] );
        nTmp *= 17;
        nCurVal = nTmp % 256;
    }
    return nCurVal;
}

// ----------   part 2

// a lens has a label (which has a hash value) and a focal length
typedef struct sLensStruct {
    std::string sLabel = "";
    int nLabelHash = -1;
    int nFocalLength = -1;
} LensType;

// for testing / debugging
void PrintLens( LensType &l ) {
    std::cout << "[" << l.sLabel << " " << l.nFocalLength << "]";
}

// a box has an id (which is the same as it's index) and a (possibly empty) list of lenses
typedef struct sBoxStruct {
    int nID = -1;
    std::vector<LensType> vLenses;
} BoxType;

// there are 256 boxes in total
BoxType glbBoxes[256];

// sets the id of each box equal to its index
void InitAllBoxes() {
    for (int i = 0; i < 256; i++) {
        glbBoxes[i].nID = i;
    }
}

// for testing / debugging
void PrintBox( BoxType &b ) {
    if (!b.vLenses.empty()) {
        std::cout << "Box " << b.nID << ": ";
        for (auto e : b.vLenses) {
            PrintLens( e );
            std::cout << " ";
        }
        std::cout << std::endl;
    }
}

// for testing / debugging
void PrintAllBoxes() {
    for (int i = 0; i < 256; i++) {
        PrintBox( glbBoxes[i] );
    }
}

// auxiliary function to PerformOneOperation() - this function removes a lens
// with label sLensID from the box with index (and ID) nBoxIx
// If the lens is not there, nothing is done
void RemoveLensFromBox( int nBoxIx, std::string &sLensID ) {
    BoxType &curBox = glbBoxes[nBoxIx];
    // remove the lens if it is present in the box
    curBox.vLenses.erase(
        std::remove_if(
            curBox.vLenses.begin(),
            curBox.vLenses.end(),
            [sLensID]( LensType &l ) {
                return l.sLabel == sLensID;
            }
        ),
        curBox.vLenses.end()
    );
}

// auxiliary function to PerformOneOperation() - this function inserts (at the back) a lens
// with label sLensID and focal length nFocalLen into the box with index (and ID) nBoxIx
// If a lens with the same label is already present, it's focal length is replaced
void InsertOrReplaceInBox( int nBoxIx, std::string &sLensID, int nFocalLen ) {
    BoxType &curBox = glbBoxes[nBoxIx];
    // see if a lens with this label is present
    std::vector<LensType>::iterator iter = std::find_if(
        curBox.vLenses.begin(),
        curBox.vLenses.end(),
        [sLensID]( LensType &l ) {
            return l.sLabel == sLensID;
        }
    );
    if (iter != curBox.vLenses.end()) {
        // a lens with this label already exists in the box - replace it's focal length
        (*iter).nFocalLength = nFocalLen;
    } else {
        // there's no lens with this label - insert it
        LensType newLens = { sLensID, nBoxIx, nFocalLen };
        curBox.vLenses.push_back( newLens );
    }
}

// Performs the operation that is in sOper on one of the boxes in glbBoxes. That string
// contains the lens label, which indicates the box index, so all info is in that string.
void PerformOneOperation( std::string &sOper ) {

    if( std::count( sOper.begin(), sOper.end(), '=' ) > 0) {     // operation is of insertion or replacement type
        // parse operation string
        std::string sCurLabel = get_token_dlmtd( "=", sOper );
        int nFocLen = std::stoi( sOper );
        int nCurBox = GetHashValue( sCurLabel );
        // insert this lens into the box (or replace the focal length
        // if a lens with this label is alreay there)
        InsertOrReplaceInBox( nCurBox, sCurLabel, nFocLen );
    } else
    if( std::count( sOper.begin(), sOper.end(), '-' ) > 0) {     // operation is of removal type
        // parse operation string
        std::string sCurLabel = get_token_dlmtd( "-", sOper );
        int nCurBox = GetHashValue( sCurLabel );
        // remove this lens from the box (if it is present)
        RemoveLensFromBox( nCurBox, sCurLabel );
    } else {
        std::cout << "ERROR: PerformOneOperation() --> couldn't match type of operation: " << sOper << std::endl;
    }
}

// calculate the power of a single lens using puzzle specs
// note that the puzzle text counts boxes and lenses from 1, hence the '+ 1'
int GetLensPower( int nBoxIx, int nLensIx ) {
    return (nBoxIx + 1) * (nLensIx + 1) * glbBoxes[nBoxIx].vLenses[nLensIx].nFocalLength;
}

// work out the added power of all lenses in a single box
int GetBoxPower( int nBoxIx ) {
    int nResult = 0;
    BoxType &curBox = glbBoxes[nBoxIx];
    for (int i = 0; i < (int)curBox.vLenses.size(); i++) {
        nResult += GetLensPower( nBoxIx, i );
    }
    return nResult;
}

// work out the cumulated power of all lenses in all boxes
int GetFocussingPower() {
    int nResult = 0;
    for (int i = 0; i < 256; i++) {
        nResult += GetBoxPower( i );
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

    // get hash value for each operation (string) and accumulate them into a total value
    int nHashTotal = 0;
    for (int i = 0; i < (int)inputData.size(); i++) {
        DatumType &curDatum = inputData[i];
        int nCurHashVal = GetHashValue( curDatum );
        nHashTotal += nCurHashVal;

        if (glbProgPhase != PUZZLE) {
            PrintDatum( curDatum, false );
            std::cout << " yields HASH value = " << nCurHashVal << ", cumulated values = " << nHashTotal << std::endl;
        }
    }
    std::cout << std::endl << "Answer to part 1: total HASH values = " << nHashTotal << std::endl << std::endl;

/* ========== */   tmr.TimeReport( "Timing 1 - solving puzzle part 1 : " );   // =========================^^^^^vvvvv

    // part 2 code here

    // initialize the glbBoxes[] array
    InitAllBoxes();
    // perform the operations from the input data one by one on the glbBoxes[] array
    for (int i = 0; i < (int)inputData.size(); i++) {
        DatumType &curDatum = inputData[i];
        PerformOneOperation( curDatum );

        if (glbProgPhase != PUZZLE) {
            std::cout << "After: ";
            // curDatum is affected by the parsing process, so use copy for printing
            PrintDatum( part2Data[i] );
            PrintAllBoxes();
            std::cout << std::endl;
        }
    }
    // work out the accumulated focussing power of all the lenses in all the boxes
    int nTotalFocPower = GetFocussingPower();
    std::cout << std::endl << "Answer to part 2: total focussing power = " << nTotalFocPower << std::endl << std::endl;

/* ========== */   tmr.TimeReport( "Timing 2 - solving puzzle part 2 : " );   // ==============================^^^^^

    return 0;
}
