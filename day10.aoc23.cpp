// AoC 2023 - Day 10 - Pipe maze
// =============================

// date:  2023-12-10
// by:    Joseph21 (Joseph21-6147)

#define DAY_STRING  "day10"     //   <---  adapt this to the correct day!!

#include <iostream>
#include <fstream>
#include <algorithm>
#include <numeric>
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

// the data consists of ...
typedef std::string DatumType;
typedef std::vector<DatumType> DataStream;

// ==========   INPUT DATA FUNCTIONS

// hardcoded input - just to get the solution tested
void GetData_EXAMPLE( DataStream &dData ) {

    // hard code your examples here - use the same data for your test input file
    DatumType cDatum;
//    cDatum = "....."; dData.push_back( cDatum );                   // example 1 (part 1)
//    cDatum = ".S-7."; dData.push_back( cDatum );
//    cDatum = ".|.|."; dData.push_back( cDatum );
//    cDatum = ".L-J."; dData.push_back( cDatum );
//    cDatum = "....."; dData.push_back( cDatum );

//    cDatum = "..F7."; dData.push_back( cDatum );                   // example 2 (part 1)
//    cDatum = ".FJ|."; dData.push_back( cDatum );
//    cDatum = "SJ.L7"; dData.push_back( cDatum );
//    cDatum = "|F--J"; dData.push_back( cDatum );
//    cDatum = "LJ..."; dData.push_back( cDatum );


//    cDatum = "..........."; dData.push_back( cDatum );             // example 3 (part 2)
//    cDatum = ".S-------7."; dData.push_back( cDatum );
//    cDatum = ".|F-----7|."; dData.push_back( cDatum );
//    cDatum = ".||.....||."; dData.push_back( cDatum );
//    cDatum = ".||.....||."; dData.push_back( cDatum );
//    cDatum = ".|L-7.F-J|."; dData.push_back( cDatum );
//    cDatum = ".|..|.|..|."; dData.push_back( cDatum );
//    cDatum = ".L--J.L--J."; dData.push_back( cDatum );
//    cDatum = "..........."; dData.push_back( cDatum );

//    cDatum = ".........."; dData.push_back( cDatum );              // example 4 (part 2)
//    cDatum = ".S------7."; dData.push_back( cDatum );
//    cDatum = ".|F----7|."; dData.push_back( cDatum );
//    cDatum = ".||....||."; dData.push_back( cDatum );
//    cDatum = ".||....||."; dData.push_back( cDatum );
//    cDatum = ".|L-7F-J|."; dData.push_back( cDatum );
//    cDatum = ".|..||..|."; dData.push_back( cDatum );
//    cDatum = ".L--JL--J."; dData.push_back( cDatum );
//    cDatum = ".........."; dData.push_back( cDatum );

//    cDatum = ".F----7F7F7F7F-7...."; dData.push_back( cDatum );    // example 5 (part 2)
//    cDatum = ".|F--7||||||||FJ...."; dData.push_back( cDatum );
//    cDatum = ".||.FJ||||||||L7...."; dData.push_back( cDatum );
//    cDatum = "FJL7L7LJLJ||LJ.L-7.."; dData.push_back( cDatum );
//    cDatum = "L--J.L7...LJS7F-7L7."; dData.push_back( cDatum );
//    cDatum = "....F-J..F7FJ|L7L7L7"; dData.push_back( cDatum );
//    cDatum = "....L7.F7||L7|.L7L7|"; dData.push_back( cDatum );
//    cDatum = ".....|FJLJ|FJ|F7|.LJ"; dData.push_back( cDatum );
//    cDatum = "....FJL-7.||.||||..."; dData.push_back( cDatum );
//    cDatum = "....L---J.LJ.LJLJ..."; dData.push_back( cDatum );

    cDatum = "FF7FSF7F7F7F7F7F---7"; dData.push_back( cDatum );    // example 6 (part 2)
    cDatum = "L|LJ||||||||||||F--J"; dData.push_back( cDatum );
    cDatum = "FL-7LJLJ||||||LJL-77"; dData.push_back( cDatum );
    cDatum = "F--JF--7||LJLJ7F7FJ-"; dData.push_back( cDatum );
    cDatum = "L---JF-JLJ.||-FJLJJ7"; dData.push_back( cDatum );
    cDatum = "|F|F-JF---7F7-L7L|7|"; dData.push_back( cDatum );
    cDatum = "|FFJF7L7F-JF7|JL---7"; dData.push_back( cDatum );
    cDatum = "7-L-JL7||F7|L7F-7F7|"; dData.push_back( cDatum );
    cDatum = "L.L7LFJ|||||FJL7||LJ"; dData.push_back( cDatum );
    cDatum = "L7JLJL-JLJLJL--JLJ.L"; dData.push_back( cDatum );
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

// this enum defines the directions and positions of tiles w.r.t. other tiles
enum eDirection {
    EAST = 0,
    SOUTH,
    WEST,
    NORTH
};

// convenience function to put direction in a std::string
std::string Dir2String( eDirection dir ) {
    switch (dir) {
        case EAST : return "EAST";
        case SOUTH: return "SOUTH";
        case WEST : return "WEST";
        case NORTH: return "NORTH";
    }
    return "dirER";
}

// the map consists of tiles with a location, a type and a connection array
typedef struct sTileStruct {
    int x = -1, y = -1;
    char cType = 'x';
    bool bConnected[4] = { false };
} TileType;
typedef std::vector<std::vector<TileType>> MapType;

// converts the input data into a map with TileType elements
MapType InputToMap( DataStream &iData ) {
    MapType vvResult;

    for (int y = 0; y < (int)iData.size(); y++) {
        std::vector<TileType> vRow;

        for (int x = 0; x < (int)iData[0].length(); x++) {
            TileType T;
            T.cType = iData[y][x];
            T.x = x;
            T.y = y;

  // | is a vertical pipe connecting north and south.
  // - is a horizontal pipe connecting east and west.
  // L is a 90-degree bend connecting north and east.
  // J is a 90-degree bend connecting north and west.
  // 7 is a 90-degree bend connecting south and west.
  // F is a 90-degree bend connecting south and east.

            switch (T.cType) {
                case '|': T.bConnected[EAST] = false; T.bConnected[SOUTH] = true ; T.bConnected[WEST] = false; T.bConnected[NORTH] = true ; break;
                case '-': T.bConnected[EAST] = true ; T.bConnected[SOUTH] = false; T.bConnected[WEST] = true ; T.bConnected[NORTH] = false; break;
                case 'L': T.bConnected[EAST] = true ; T.bConnected[SOUTH] = false; T.bConnected[WEST] = false; T.bConnected[NORTH] = true ; break;
                case 'J': T.bConnected[EAST] = false; T.bConnected[SOUTH] = false; T.bConnected[WEST] = true ; T.bConnected[NORTH] = true ; break;
                case '7': T.bConnected[EAST] = false; T.bConnected[SOUTH] = true ; T.bConnected[WEST] = true ; T.bConnected[NORTH] = false; break;
                case 'F': T.bConnected[EAST] = true ; T.bConnected[SOUTH] = true ; T.bConnected[WEST] = false; T.bConnected[NORTH] = false; break;
                case '.': T.bConnected[EAST] = false; T.bConnected[SOUTH] = false; T.bConnected[WEST] = false; T.bConnected[NORTH] = false; break;
                case 'S':  /* skip for now */                                  break;
                default: std::cout << "ERROR: InputToMap() --> unknown tile type: " << T.cType << std::endl;
            }
            vRow.push_back( T );
        }
        vvResult.push_back( vRow );
    }
    return vvResult;
}

// for testing/debugging
void PrintTile( TileType &t, bool bVerbose = false ) {
    if (bVerbose) {
        std::cout << "(" << t.x << ", " << t.y << ") = " << t.cType;
        std::cout << " - conny: ";
        std::cout << (t.bConnected[EAST ] ? "O" : "X");
        std::cout << (t.bConnected[SOUTH] ? "O" : "X");
        std::cout << (t.bConnected[WEST ] ? "O" : "X");
        std::cout << (t.bConnected[NORTH] ? "O" : "X");
        std::cout << std::endl;
    } else {
        std::cout << t.cType;
    }
}

// convenience getters on map dimensions
int MapWidth(  MapType &m ) { return (int)m[0].size(); }
int MapHeight( MapType &m ) { return (int)m.size(); }

// convenience checker if coordinates are out of bounds of m
bool IsOutOfBounds( MapType &m, int x, int y ) {
    return x < 0 || x >= MapWidth(  m ) ||
           y < 0 || y >= MapHeight( m );
}


// for testing/debugging
void PrintMap( MapType &m, bool bVerbose = false ) {
    for (int y = 0; y < MapHeight( m ); y++) {
        for (int x = 0; x < MapWidth( m ); x++) {
            PrintTile( m[y][x], bVerbose );
        }
        if (!bVerbose) std::cout << std::endl;
    }
}

// returns whether the tile at (tX, tY) in m is connected in direction dir
bool IsConnected( MapType &m, int tX, int tY, eDirection dir ) {
    return m[tY][tX].bConnected[dir];
}

// sets connectivity for start ('S') tile
void SetStartConnectivity( MapType &m, int xStart, int yStart ) {
    if (m[yStart][xStart].cType != 'S') {
        std::cout << "ERROR: SetStartConnectivity() --> wrong start tile: " << m[yStart][xStart].cType << std::endl;
    }

    TileType &strTile = m[yStart][xStart];
    strTile.bConnected[EAST ] = xStart < MapWidth(  m ) - 1 && IsConnected( m, xStart + 1, yStart    , WEST  );
    strTile.bConnected[WEST ] = xStart >                  0 && IsConnected( m, xStart - 1, yStart    , EAST  );
    strTile.bConnected[SOUTH] = yStart < MapHeight( m ) - 1 && IsConnected( m, xStart    , yStart + 1, NORTH );
    strTile.bConnected[NORTH] = yStart >                  0 && IsConnected( m, xStart    , yStart - 1, SOUTH );
}

// searches map m for the location that is labeled 'S' and returns that
// location via the reference parameters
void GetStartLocation( MapType &m, int &xStart, int &yStart ) {
    bool bFound = false;
    for (int y = 0; y < MapHeight( m ) && !bFound; y++) {
        for (int x = 0; x < MapWidth( m ) && !bFound; x++) {
            if (m[y][x].cType == 'S') {
                xStart = x;
                yStart = y;
                bFound = true;
            }
        }
    }
}

// Assembles a walk along the pipe cycle in vTilePtr. It does so in a number of steps:
// 1. find start tile
// 2. choose first dir in connection list of start tile
// 3. follow trail until you arrive at start node again
void WalkPipeCycle( MapType &m, std::vector<TileType *> &vTilePtr, bool bOutput = false ) {
    // 1. find start tile
    int nStrtX, nStrtY;
    GetStartLocation( m, nStrtX, nStrtY );
    TileType &strtTile = m[nStrtY][nStrtX];
    vTilePtr.clear();

    // 2. choose first direction in connection list of start tile
    eDirection stepDir;
    bool bFound = false;
    for (int i = 0; i < 4 && !bFound; i++) {
        if (strtTile.bConnected[i]) {
            bFound = true;
            stepDir = eDirection( i );
        }
    }
    // 3. follow pipe cycle
    int newX = nStrtX;
    int newY = nStrtY;
    bool bFinished = false;
    do {
        int curX = newX;
        int curY = newY;
        vTilePtr.push_back( &m[curY][curX] );

        switch (stepDir) {
            case EAST : newX = curX + 1; newY = curY    ; break;
            case WEST : newX = curX - 1; newY = curY    ; break;
            case SOUTH: newX = curX    ; newY = curY + 1; break;
            case NORTH: newX = curX    ; newY = curY - 1; break;
        }

        bFinished = (newX == nStrtX && newY == nStrtY);
        if (!bFinished) {
            // determine direction for next step
            int nDirForbidden = (int(stepDir) + 2) % 4;        // make sure you don't go back
            TileType &nextTile = m[newY][newX];

            // find next direction that is not the forbidden direction
            bool bDirFound = false;
            for (int dir = 0; dir < 4 && !bDirFound; dir++) {
                if (dir != nDirForbidden && nextTile.bConnected[dir]) {
                    stepDir = eDirection( dir );
                    bDirFound = true;
                }
            }
        }
    } while (!bFinished);
}

// ----------   part 2

/* determine the direction of the step
    if north south/ south north or west east/east/west
        if the tile to the right of the directed step is empty
            floodfill it with I
        if the tile left to the directed step is empty
            floodfill it with O
 */

// performs flood fill on the cell in m at (nX, nY):
//   if that cell is labeled empty, then it's set to cVal, and
//   all it's four connected neighbours are recursively called for flood fill as well
void FloodFill( MapType &m, int nX, int nY, char cVal ) {
    if (!IsOutOfBounds( m, nX, nY )) {
        if (m[nY][nX].cType == '.') {
            m[nY][nX].cType = cVal;
            FloodFill( m, nX - 1, nY    , cVal );
            FloodFill( m, nX + 1, nY    , cVal );
            FloodFill( m, nX    , nY - 1, cVal );
            FloodFill( m, nX    , nY + 1, cVal );
        }
    }
}

// Performs the flood fill for one tile of the pipe cycle. outDir is the direction where the trail leaves this tile.
void ProcessTile( MapType &m, TileType &t, eDirection outDir, bool bLeftOutside ) {
    char localType = t.cType;
    // if tile t is a start tile, then determine what it's connectivity is in the pipe cycle
    if (t.cType == 'S') {
        if (!t.bConnected[EAST] &&  t.bConnected[SOUTH] && !t.bConnected[WEST] &&  t.bConnected[NORTH]) localType = '|';
        if ( t.bConnected[EAST] && !t.bConnected[SOUTH] &&  t.bConnected[WEST] && !t.bConnected[NORTH]) localType = '-';
        if ( t.bConnected[EAST] &&  t.bConnected[SOUTH] && !t.bConnected[WEST] && !t.bConnected[NORTH]) localType = 'F';
        if (!t.bConnected[EAST] &&  t.bConnected[SOUTH] &&  t.bConnected[WEST] && !t.bConnected[NORTH]) localType = '7';
        if ( t.bConnected[EAST] && !t.bConnected[SOUTH] && !t.bConnected[WEST] &&  t.bConnected[NORTH]) localType = 'L';
        if (!t.bConnected[EAST] && !t.bConnected[SOUTH] &&  t.bConnected[WEST] &&  t.bConnected[NORTH]) localType = 'J';
    }
    // determine whether left must be regarded outside ('o') and right inside ('i') or the other way around
    char cLt, cRt;
    if (bLeftOutside) {
        cLt = 'o';
        cRt = 'i';
    } else {
        cLt = 'i';
        cRt = 'o';
    }
    // depending on the type of segment, consider it's left and right sides and flood fill their neigbours with appropriate values
    switch (localType) {
        case '|': {
                if (outDir == NORTH) {
                    FloodFill( m, t.x + 1, t.y, cRt );
                    FloodFill( m, t.x - 1, t.y, cLt );
                } else {
                    FloodFill( m, t.x + 1, t.y, cLt );
                    FloodFill( m, t.x - 1, t.y, cRt );
                }
            }
            break;
        case '-': {
                if (outDir == EAST) {
                    FloodFill( m, t.x, t.y + 1, cRt );
                    FloodFill( m, t.x, t.y - 1, cLt );
                } else {
                    FloodFill( m, t.x, t.y + 1, cLt );
                    FloodFill( m, t.x, t.y - 1, cRt );
                }
            }
            break;
        case 'F': {
                if (outDir == EAST) {
                    FloodFill( m, t.x - 1, t.y    , cLt );
                    FloodFill( m, t.x    , t.y - 1, cLt );
                } else {
                    FloodFill( m, t.x - 1, t.y    , cRt );
                    FloodFill( m, t.x    , t.y - 1, cRt );
                }
            }
            break;
        case '7': {
                if (outDir == WEST) {
                    FloodFill( m, t.x + 1, t.y    , cRt );
                    FloodFill( m, t.x    , t.y - 1, cRt );
                } else {
                    FloodFill( m, t.x + 1, t.y    , cLt );
                    FloodFill( m, t.x    , t.y - 1, cLt );
                }
            }
            break;
        case 'L': {
                if (outDir == EAST) {
                    FloodFill( m, t.x - 1, t.y    , cRt );
                    FloodFill( m, t.x    , t.y + 1, cRt );
                } else {
                    FloodFill( m, t.x - 1, t.y    , cLt );
                    FloodFill( m, t.x    , t.y + 1, cLt );
                }
            }
            break;
        case 'J': {
                if (outDir == WEST) {
                    FloodFill( m, t.x + 1, t.y    , cLt );
                    FloodFill( m, t.x    , t.y - 1, cLt );
                } else {
                    FloodFill( m, t.x + 1, t.y    , cRt );
                    FloodFill( m, t.x    , t.y - 1, cRt );
                }
            }
            break;
    }
}

// looks at the relative position of pNext w.r.t. pCur, and returns a direction as a result
// example: returns NORTH if pNext is to the north of pCur
eDirection WhereIsNextTile( TileType *pCur, TileType *pNext ) {
    eDirection eResult;
    if (pCur->x == pNext->x) {
        if (pCur->y < pNext->y) {
            eResult = SOUTH;
        } else {
            eResult = NORTH;
        }
    } else if (pCur->y == pNext->y) {
        if (pCur->x < pNext->x) {
            eResult = EAST;
        } else {
            eResult = WEST;
        }
    }
    return eResult;
}

// reuse the pipe cycle in vTilePtrs for walking the pipe cycle, and flood filling all neigbouring tiles where
// possible with the right value (inside or outside)
void FloodFillMap( MapType &m, std::vector<TileType *> vTilePtrs, bool bLeftIsOutside, bool bOutput = false ) {

    for (int i = 0; i < (int)vTilePtrs.size(); i++) {
        eDirection eNextPos;
        if (i == (int)vTilePtrs.size() - 1) {
            eNextPos = WhereIsNextTile( vTilePtrs[i], vTilePtrs[0] );
        } else {
            eNextPos = WhereIsNextTile( vTilePtrs[i], vTilePtrs[i + 1] );
        }
        ProcessTile( m, *(vTilePtrs[i]), eNextPos, bLeftIsOutside );
    }
}

// get stats of m
void AnalyseMap( MapType &m, int &nrInside, int &nrOutside, int &nrOccupied, int &nrEmpty ) {
    nrInside   = 0;
    nrOutside  = 0;
    nrOccupied = 0;
    nrEmpty    = 0;

    for (int y = 0; y < MapHeight( m ); y++) {
        for (int x = 0; x < MapWidth( m ); x++) {
            switch (m[y][x].cType) {
                case 'i': nrInside   += 1; break;
                case 'o': nrOutside  += 1; break;
                case '.': nrEmpty    += 1; break;
                default : nrOccupied += 1;
            }
        }
    }
}

// returns true if curTile is part of vMainLoop
bool IsInMainLoop( TileType &curTile, std::vector<TileType *> &vMainLoop ) {
    bool bFound = false;
    for (int i = 0; i < (int)vMainLoop.size() && !bFound; i++) {
        TileType &loopTile = *(vMainLoop[i]);
        if (curTile.x == loopTile.x && curTile.y == loopTile.y) {
            bFound = true;
        }
    }
    return bFound;
}

// cleans junk pipe segments that are not part of the main pipe cycle
void CleanJunkTiles( MapType &m, std::vector<TileType *> &vMainLoop ) {
    for (int y = 0; y < MapHeight( m ); y++) {
        for (int x = 0; x < MapWidth( m ); x++) {
            if (m[y][x].cType != '.') {
                if (!IsInMainLoop( m[y][x], vMainLoop )) {
                    m[y][x].cType = '.';
                }
            }
        }
    }
}

// convenience function to flood fill all empty tiles at map boundaries with label outside
// this helps in finding the direction orientation
void FillBordersOutside( MapType &m ) {
    for (int x = 0; x < MapWidth( m ); x++) {
        if (m[                 0][x].cType == '.') { FloodFill( m, x,                  0, 'o' ); }
        if (m[MapHeight( m ) - 1][x].cType == '.') { FloodFill( m, x, MapHeight( m ) - 1, 'o' ); }
    }
    for (int y = 0; y < MapHeight( m ); y++) {
        if (m[y][                0].cType == '.') { FloodFill( m,                 0, y, 'o' ); }
        if (m[y][MapWidth( m ) - 1].cType == '.') { FloodFill( m, MapWidth( m ) - 1, y, 'o' ); }
    }
}

// tries to work out from curTile and nextPos info whether or net bLeftIsOutside can be set to true or false.
// If that can be done with certainty, bAnswerFound is also set to true
// NOTE: I'm only using segment types '|' and '-' here, assuming that somewhere in the cycle the answer can be found
void EvaluateTile( MapType &m, TileType &curTile, eDirection nextPos, bool &bAnswerFound, bool &bLeftIsOutside ) {
    bAnswerFound = false;
    switch (curTile.cType) {
        case '|': {
                if (nextPos == NORTH) {
                    if (IsOutOfBounds( m, curTile.x - 1, curTile.y ) || m[curTile.y][curTile.x - 1].cType == 'o') {
                        bAnswerFound = true;
                        bLeftIsOutside = true;
                    }
                    if (IsOutOfBounds( m, curTile.x + 1, curTile.y ) || m[curTile.y][curTile.x + 1].cType == 'o') {
                        bAnswerFound = true;
                        bLeftIsOutside = false;
                    }
                } else if (nextPos == SOUTH) {
                    if (IsOutOfBounds( m, curTile.x - 1, curTile.y ) || m[curTile.y][curTile.x - 1].cType == 'o') {
                        bAnswerFound = true;
                        bLeftIsOutside = false;
                    }
                    if (IsOutOfBounds( m, curTile.x + 1, curTile.y ) || m[curTile.y][curTile.x + 1].cType == 'o') {
                        bAnswerFound = true;
                        bLeftIsOutside = true;
                    }
                }
            }
            break;
        case '-': {
                if (nextPos == EAST) {
                    if (IsOutOfBounds( m, curTile.x, curTile.y - 1 ) || m[curTile.y - 1][curTile.x].cType == 'o') {
                        bAnswerFound = true;
                        bLeftIsOutside = true;
                    }
                    if (IsOutOfBounds( m, curTile.x, curTile.y + 1 ) || m[curTile.y + 1][curTile.x].cType == 'o') {
                        bAnswerFound = true;
                        bLeftIsOutside = false;
                    }
                } else if (nextPos == WEST) {
                    if (IsOutOfBounds( m, curTile.x, curTile.y - 1 ) || m[curTile.y - 1][curTile.x].cType == 'o') {
                        bAnswerFound = true;
                        bLeftIsOutside = false;
                    }
                    if (IsOutOfBounds( m, curTile.x, curTile.y + 1 ) || m[curTile.y + 1][curTile.x].cType == 'o') {
                        bAnswerFound = true;
                        bLeftIsOutside = true;
                    }
                }
            }
            break;
    }
}

// analyses the pipe cycle to find out whether the left (w.r.t. direction of the pipe cycle)
// must be considered outside (in which case true is returned) or inside (in which case false is returned)
bool IsLeftOutside( MapType &m, std::vector<TileType *> &vTilePtrs ) {
    bool bResult = false;
    bool bFound  = false;
    for (int i = 0; i < (int)vTilePtrs.size() - 1 && !bFound; i++) {
        TileType *pCur  = vTilePtrs[i    ];
        TileType *pNext = vTilePtrs[i + 1];
        eDirection ePositionOfNextTile = WhereIsNextTile( pCur, pNext );
        EvaluateTile( m, *pCur, ePositionOfNextTile, bFound, bResult );
//        if (bFound) {
//            std::cout << "INFO: IsLeftOutside() --> answer found at index: " << i << std::endl;
//        }
    }
    return bResult;
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

    // convert input data to map of tiles
    MapType myMap = InputToMap( inputData );
    // find out where the start location is
    int nStartX, nStartY;
    GetStartLocation( myMap, nStartX, nStartY );
    // set connectivity info for start tile
    SetStartConnectivity( myMap, nStartX, nStartY );
    // get the pipe cycle into a vector of pointers to tiles
    std::vector<TileType *> vTilePointers;
    WalkPipeCycle( myMap, vTilePointers, false );
    // get length of pipe cycle to calculate the answer from
    int nCycleDistance = vTilePointers.size();

    std::cout << std::endl << "Answer to part 1: max distance = " << nCycleDistance / 2 << std::endl << std::endl;

/* ========== */   tmr.TimeReport( "Timing 1 - solving puzzle part 1 : " );   // =========================^^^^^vvvvv

    // part 2 code here

    if (glbProgPhase != PUZZLE) PrintMap( myMap, false );
    // remove any junk, set it to empty tiles
    CleanJunkTiles( myMap, vTilePointers );
    if (glbProgPhase != PUZZLE) {
        std::cout << std::endl;
        PrintMap( myMap );
    }
    // flood fill empty tiles against borders with label "outside"
    FillBordersOutside( myMap );
    if (glbProgPhase != PUZZLE) {
        std::cout << std::endl;
        PrintMap( myMap );
    }
    // find out whether left is outside of the cycle, or the inside
    bool bLeftOutside = IsLeftOutside( myMap, vTilePointers );

    if (glbProgPhase != PUZZLE) std::cout << "Direction orientation: Left side is " << (bLeftOutside ? "OUT side" : "IN side") << std::endl;

    // flood fill along the pipe cycle with the info on how the left side of segments is to be interpreted
    FloodFillMap( myMap, vTilePointers, bLeftOutside, true );

    if (glbProgPhase != PUZZLE) PrintMap( myMap, false );

    // get counts for inside, outside, empty and other labels
    int nrIn, nrOut, nrOther, nrEmpty;
    AnalyseMap( myMap, nrIn, nrOut, nrOther, nrEmpty );
    if (glbProgPhase != PUZZLE) {
        std::cout << " in: "    << nrIn
                  << " out: "   << nrOut
                  << " empty: " << nrEmpty
                  << " other: " << nrOther
                  << " total: " << nrIn + nrOut + nrEmpty + nrOther
                  << std::endl;

        std::cout << "Map width = " << MapWidth( myMap ) << ", map height = " << MapHeight( myMap ) << " --> map area = " << MapWidth( myMap ) * MapHeight( myMap ) << std::endl;
    }

    std::cout << std::endl << "Answer to part 2: nr of enclosed cells = " << nrIn << std::endl << std::endl;

/* ========== */   tmr.TimeReport( "Timing 2 - solving puzzle part 2 : " );   // ==============================^^^^^

    return 0;
}
