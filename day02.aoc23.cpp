// AoC 2023 - Day 02 - Cube conundrum
// ==================================

// date:  2023-12-02
// by:    Joseph21 (Joseph21-6147)

#define DAY_STRING  "day02"

#include <iostream>
#include <fstream>
#include <vector>

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

// the data consists of games, with an identifier and a number of grabs in it
// each grab can hold a number of red, green or blue cubes
typedef struct sGrabStruct {
    int nRed   = 0;
    int nGreen = 0;
    int nBlue  = 0;
} GrabType;

typedef struct sDatumStruct {
    int nID = -1;
    std::vector<GrabType> vGrabs;
} DatumType;
typedef std::vector<DatumType> DataStream;

// ==========   INPUT DATA FUNCTIONS

// hardcoded input - just to get the solution tested
void GetData_EXAMPLE( DataStream &dData ) {

    // DIDN'T USE THIS FUNCTION / DEV PHASE FOR THIS PUZZLE
    // [ since it was pretty awkward to hard code the examples ]

    // hard code your examples here - use the same data for your test input file
    DatumType cDatum;
    cDatum = {  }; dData.push_back( cDatum );
}

// Cuts of and returns the front token from "input_to_be_adapted", using "delim" as delimiter.
// If delimiter is not found, the complete input string is passed as a token.
// The input string becomes shorter as a result, and may even become empty
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
            DatumType cDatum;
            std::string sIgnore = get_token_dlmtd( " ", sLine );    // get "Game " out of the way
            std::string sID     = get_token_dlmtd( ": ", sLine );   // read game id
            cDatum.nID = stoi( sID );
            // read the (var. nr of) grabs
            while (sLine.length() > 0) {
                // parse next grab
                std::string sGrab = get_token_dlmtd( "; ", sLine );
                // read the list of nrs and cubes
                GrabType auxGrab;
                while (sGrab.length() > 0) {
                    // parse next nr / colour pair
                    std::string sNbr = get_token_dlmtd( " " , sGrab );
                    std::string sCol = get_token_dlmtd( ", ", sGrab );
                    int nNbr = stoi( sNbr );
                    if (sCol == "red"  ) { auxGrab.nRed   = nNbr; }
                    if (sCol == "green") { auxGrab.nGreen = nNbr; }
                    if (sCol == "blue" ) { auxGrab.nBlue  = nNbr; }
                }
                cDatum.vGrabs.push_back( auxGrab );
            }
            vData.push_back( cDatum );
        }
    }
    dataFileStream.close();
}

void GetData_TEST(   DataStream &dData ) { std::string sInputFile = DAY_STRING; sInputFile.append( ".input.test.txt"   ); ReadInputData( sInputFile, dData ); }
void GetData_PUZZLE( DataStream &dData ) { std::string sInputFile = DAY_STRING; sInputFile.append( ".input.puzzle.txt" ); ReadInputData( sInputFile, dData ); }

// ==========   CONSOLE OUTPUT FUNCTIONS

// convenience printer function
std::string Grab2String( GrabType &g ) {
    std::string sResult =   "r: " + std::to_string( g.nRed   )
                        + ", g: " + std::to_string( g.nGreen )
                        + ", b: " + std::to_string( g.nBlue  );
    return sResult;
}

// output to console for testing
void PrintDatum( DatumType &iData ) {
    // define your datum printing code here
    std::cout << "Game: " << iData.nID << " --> ";
    for (auto e : iData.vGrabs) {
        std::cout << Grab2String( e ) << " | ";
    }
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

// returns true if the numbers in grab remain within the boundaries
// as specified by maxGrab
bool GrabIsPossible( GrabType &grab, GrabType &maxGrab ) {
    return (grab.nRed   <= maxGrab.nRed  ) &&
           (grab.nGreen <= maxGrab.nGreen) &&
           (grab.nBlue  <= maxGrab.nBlue );
}

// returns true if all of the grabs in game are possible
bool GameIsPossible( DatumType &game, GrabType &maxGrab ) {
    bool bPossible = true;
    for (int i = 0; i < (int)game.vGrabs.size() && bPossible; i++) {
        bPossible = GrabIsPossible( game.vGrabs[i], maxGrab );
    }
    return bPossible;
}

// ==========   part 2   ==========

// get the smallest set of cubes that makes this game possible
void GetSmallestCubeSet( DatumType &game, GrabType &minSet ) {
    minSet = { 0, 0, 0 };
    for (int i = 0; i < (int)game.vGrabs.size(); i++) {
        GrabType &curGrab = game.vGrabs[i];
        minSet.nRed   = std::max( minSet.nRed  , curGrab.nRed   );
        minSet.nGreen = std::max( minSet.nGreen, curGrab.nGreen );
        minSet.nBlue  = std::max( minSet.nBlue , curGrab.nBlue  );
    }
}

// power of a grab is the product of it's elements (as described in puzzle specs)
int Power( GrabType &g ) {
    return g.nRed * g.nGreen * g.nBlue;
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
    GrabType glbMaxGrab = { 12, 13, 14 };
    int nAccPossibles = 0;
    for (int i = 0; i < (int)inputData.size(); i++) {
        DatumType &curGame = inputData[i];
        bool bPossible = GameIsPossible( curGame, glbMaxGrab );
        nAccPossibles += (bPossible ? curGame.nID : 0);

        if (glbProgPhase != PUZZLE) {
            std::cout << "Game: " << curGame.nID
                      << " is " << (bPossible? "" : "NOT") << " possible"
                      << std::endl;
        }
    }

    std::cout << std::endl << "Answer to part 1: sum of possible game ID's = " << nAccPossibles << std::endl << std::endl;

/* ========== */   tmr.TimeReport( "Timing 1 - solving puzzle part 1 : " );   // =========================^^^^^vvvvv

    // part 2 code here

    int nSumPowers = 0;
    for (int i = 0; i < (int)inputData.size(); i++) {
        DatumType &curGame = inputData[i];
        GrabType curMinSet;
        GetSmallestCubeSet( curGame, curMinSet );
        int nPower = Power( curMinSet );
        nSumPowers += nPower;

        if (glbProgPhase != PUZZLE) {
            PrintDatum( curGame );
            std::cout << "smallest cube set: "             << Grab2String( curMinSet )
                      << " - power of smallest cube set: " << nPower
                      << " - cumulated powers: "           << nSumPowers
                      << std::endl;
        }
    }

    std::cout << std::endl << "Answer to part 2: sum of powers of minimal sets = " << nSumPowers << std::endl << std::endl;

/* ========== */   tmr.TimeReport( "Timing 2 - solving puzzle part 2 : " );   // ==============================^^^^^

    return 0;
}
