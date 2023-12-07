// AoC 2023 - Day 07 - Camel cards
// ===============================

// date:  2023-12-07
// by:    Joseph21 (Joseph21-6147)

#define DAY_STRING  "day07"     //   <---  adapt this to the correct day!!

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
//   * a hand (5 cards, modeled by a string of length 5)
//   * a bid value
//   * a rank (to be filled in after data input)
typedef struct sDatumStruct {
    std::string sHand = "     ";
    int nBid = 0;
    int nRnk = -1;
} DatumType;
typedef std::vector<DatumType> DataStream;

// ==========   INPUT DATA FUNCTIONS

// hardcoded input - just to get the solution tested
void GetData_EXAMPLE( DataStream &dData ) {

    // hard code your examples here - use the same data for your test input file
    DatumType cDatum;
    cDatum = { "32T3K", 765, -1 }; dData.push_back( cDatum );
    cDatum = { "T55J5", 684, -1 }; dData.push_back( cDatum );
    cDatum = { "KK677",  28, -1 }; dData.push_back( cDatum );
    cDatum = { "KTJJT", 220, -1 }; dData.push_back( cDatum );
    cDatum = { "QQQJA", 483, -1 }; dData.push_back( cDatum );
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
            DatumType cDatum = {
                      get_token_dlmtd( " ", sLine ),    // hand of cards
                stoi( get_token_dlmtd( " ", sLine ) )   // bid value
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
    std::cout << iData.sHand << ", bid: " << iData.nBid << ", rank: " << iData.nRnk;
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

// alternative representation of a hand of cards: which card in what amounts
typedef std::map<char, int> CardAnalysis;

// convenience function for debugging
void PrintCardAnalysis( CardAnalysis &ca, bool bEndl = true ) {
    for (auto e : ca) {
        std::cout << e.first << ": " << e.second << ", ";
    }
     if (bEndl) { std::cout << std::endl; }
}

// determine what type of hand you have
CardAnalysis AnalyseHand( std::string &sHand ) {
    CardAnalysis mResult;
    for (int i = 0; i < 5; i++) {
        char curCard = sHand[i];
        CardAnalysis::iterator iter = mResult.find( curCard );
        if (iter == mResult.end()) {    // map elt with this card type does not exist - insert count
            mResult.insert( { curCard, 1 } );
        } else {                        // map elt with this card type already exists - increment count
            (*iter).second += 1;
        }
    }
    return mResult;
}

// precondition: AnalyseHand() has been called and the result is passed as parameter
// counts number of cards that occur x times in hand
int CountX( CardAnalysis &mCardAn, int x ) {
    int nResult = 0;
    for (CardAnalysis::iterator iter = mCardAn.begin(); iter != mCardAn.end(); iter++) {
        if ((*iter).second == x) {
            nResult += 1;
        }
    }
    return nResult;
}

// convenience functions to determine the type of hand using the counts per card
bool IsFiveOfAKind(  CardAnalysis &mCardAn) { return CountX( mCardAn, 5 ) == 1; }
bool IsFourOfAKind(  CardAnalysis &mCardAn) { return CountX( mCardAn, 4 ) == 1; }   // remainder is 1 unique card
bool IsTwoPair(      CardAnalysis &mCardAn) { return CountX( mCardAn, 2 ) == 2; }   // remainder is 1 unique card
bool IsHighCard(     CardAnalysis &mCardAn) { return CountX( mCardAn, 1 ) == 5; }
bool IsFullHouse(    CardAnalysis &mCardAn) { return CountX( mCardAn, 3 ) == 1 && CountX( mCardAn, 2 ) == 1; }
bool IsThreeOfAKind( CardAnalysis &mCardAn) { return CountX( mCardAn, 3 ) == 1 && CountX( mCardAn, 1 ) == 2; }
bool IsOnePair(      CardAnalysis &mCardAn) { return CountX( mCardAn, 2 ) == 1 && CountX( mCardAn, 1 ) == 3; }

// convenience function for printing
std::string HandType2String( std::string sHand ) {
    CardAnalysis ca = AnalyseHand( sHand );

    if (IsFiveOfAKind(  ca )) return "Five of Kind ";
    if (IsFourOfAKind(  ca )) return "Four of Kind ";
    if (IsFullHouse(    ca )) return "Full House   ";
    if (IsThreeOfAKind( ca )) return "Three of Kind";
    if (IsTwoPair(      ca )) return "Two pair     ";
    if (IsOnePair(      ca )) return "One pair     ";
    if (IsHighCard(     ca )) return "High card    ";
    return                           "type ERROR !!";
}

// returns the ranking value of a hand type?
// used in HandStronger() to determine order of hand types
int GetHandRank( std::string sHand ) {
    CardAnalysis ca = AnalyseHand( sHand );

    if (IsFiveOfAKind(  ca )) return 7;
    if (IsFourOfAKind(  ca )) return 6;
    if (IsFullHouse(    ca )) return 5;
    if (IsThreeOfAKind( ca )) return 4;
    if (IsTwoPair(      ca )) return 3;
    if (IsOnePair(      ca )) return 2;
    if (IsHighCard(     ca )) return 1;
    return 0;
}

// used in HandStronger() to determine order of hand types
// returns whether card a is stronger than card b
bool CardStronger1( char a, char b ) {
    std::string sOrder = "AKQJT98765432";
    return sOrder.find( a ) < sOrder.find( b );
}

// returns whether hand a is stronger than hand b?
bool HandStronger1( DatumType &a, DatumType &b ) {
    bool bResult = false;
    if (GetHandRank( a.sHand ) == GetHandRank( b.sHand )) {
        // if hand types are the same, determine which wins on strongest first card
        bool bFound  = false;
        for (int i = 0; i < 5 && !bFound; i++) {
            if (a.sHand[i] != b.sHand[i]) {
                bResult = CardStronger1( a.sHand[i], b.sHand[i] );
                bFound = true;
            }
        }
    } else {
        // if hand types are not the same, determine if a is stronger than b
        bResult = GetHandRank( a.sHand ) > GetHandRank( b.sHand );
    }
    return bResult;
}

// ----------   part 2

// returns # 'J' cards in ca
int GetNrJokers( CardAnalysis &ca ) {
    int nResult = 0;
    CardAnalysis::iterator iter = ca.find( 'J' );
    if (iter != ca.end()) {
        nResult = (*iter).second;
    }
    return nResult;
}

// the addition of jokers in the game creates possibility for "promoting" hand types.
// this variation on GetHandRank() implements these cases
int GetHandRank2( std::string sHand ) {
    CardAnalysis ca = AnalyseHand( sHand );
    int nrJokers = GetNrJokers( ca );
    int nResult = 0;

    if (IsFiveOfAKind( ca )) {    // 5 J's or 5 others, either way Five of a Kind
        nResult = 7;
    }
    if (IsFourOfAKind( ca )) {    // promotes to Five of a kind if either the four cards are J's or the 5th card is
        nResult = 6;
        switch (nrJokers) {
            case 4 : nResult += 1; break;
            case 1 : nResult += 1; break;
        }
    }
    if (IsFullHouse( ca )) {     // promotes to Five of a Kind if either of it's two group consists of jokers
        nResult = 5;
        switch (nrJokers) {
            case 2 : nResult += 2; break;
            case 3 : nResult += 2; break;
        }
    }
    if (IsThreeOfAKind( ca )) {  // promotes to Four of a kind if either the three, or one of the remaining cards is J
        nResult = 4;
        switch (nrJokers) {
            case 3 : nResult += 2; break;
            case 1 : nResult += 2; break;
        }
    }
    if (IsTwoPair( ca )) {       // promote to Full House if the remaining card is J, or to Four of a kind if either of the pairs are J's
        nResult = 3;
        switch (nrJokers) {
            case 2 : nResult += 3; break;
            case 1 : nResult += 2; break;
        }
    }
    if (IsOnePair( ca )) {       // One pair promotes to three of a kind if either the pair, or one of the remaining cards is a J
        nResult = 2;
        switch (nrJokers) {
            case 1 : nResult += 2; break;
            case 2 : nResult += 2; break;
        }
    }
    if (IsHighCard( ca )) {      // All cards are different - if one of them is a Joker (more than 1 is not possible) it promotes to one pair
        nResult = 1;
        switch (nrJokers) {
            case 1: nResult += 1;
        }
    }

    return nResult;
}

// for part two the ranking of the individual cards is different:
// the J card is lower than the 2
bool CardStronger2( char a, char b ) {
    std::string sOrder = "AKQT98765432J";
    return sOrder.find( a ) < sOrder.find( b );
}

// variant of HandStronger1(), using GetHandRank2() and CardStronger2() to get the correct ordering
bool HandStronger2( DatumType &a, DatumType &b ) {
    bool bResult = false;
    if (GetHandRank2( a.sHand ) == GetHandRank2( b.sHand )) {
        bool bFound  = false;
        for (int i = 0; i < 5 && !bFound; i++) {
            if (a.sHand[i] != b.sHand[i]) {
                bResult = CardStronger2( a.sHand[i], b.sHand[i] );
                bFound = true;
            }
        }
    } else {
        bResult = GetHandRank2( a.sHand ) > GetHandRank2( b.sHand );
    }
    return bResult;
}

// convenience function for printing hand type
// for part 2 the rank is determined using the part 2 variant GetHandRank2(), and
// the hand type string is returned as a result of that rank value
std::string HandType2String2( std::string sHand ) {
    int nRank = GetHandRank2( sHand );
    switch (nRank) {
        case 7: return "Five of Kind ";
        case 6: return "Four of Kind ";
        case 5: return "Full House   ";
        case 4: return "Three of Kind";
        case 3: return "Two pair     ";
        case 2: return "One pair     ";
        case 1: return "High card    ";
    }
    return "card type ERROR";
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

    // use HandStronger1() to sort the input data
    std::sort( inputData.begin(), inputData.end(), HandStronger1 );
    // fill in the rank value as a result of the sorting
    for (int i = 0; i < (int)inputData.size(); i++) {
        DatumType &curHand = inputData[i];
        curHand.nRnk = (int)inputData.size() - i;
    }
    // cumulate the total winnings value
    int nTotalWinnings1 = 0;
    for (int i = 0; i < (int)inputData.size(); i++) {
        DatumType curHand = inputData[i];
        nTotalWinnings1 += curHand.nBid * curHand.nRnk;

        if (glbProgPhase != PUZZLE) {
            PrintDatum( curHand, false );
            std::cout << ", type = " << HandType2String( curHand.sHand ) << " intermediate total winnings: " << nTotalWinnings1 << std::endl;
        }
    }
    // report final puzzle answer
    std::cout << std::endl << "Answer to part 1: total winnings = " << nTotalWinnings1 << std::endl << std::endl;

/* ========== */   tmr.TimeReport( "Timing 1 - solving puzzle part 1 : " );   // =========================^^^^^vvvvv


    // part 2 code here

    // get fresh copy of inputData
    inputData = part2Data;
    // use HandStronger2() to sort the input data
    std::sort( inputData.begin(), inputData.end(), HandStronger2 );
    // fill in the rank value as a result of the sorting
    for (int i = 0; i < (int)inputData.size(); i++) {
        DatumType &curHand = inputData[i];
        curHand.nRnk = (int)inputData.size() - i;
    }
    // cumulate the total winnings value
    int nTotalWinnings2 = 0;
    for (int i = 0; i < (int)inputData.size(); i++) {
        DatumType curHand = inputData[i];
        nTotalWinnings2 += curHand.nBid * curHand.nRnk;

        if (glbProgPhase != PUZZLE) {
            PrintDatum( curHand, false );
            std::cout << ", type = " << HandType2String2( curHand.sHand ) << " intermediate total winnings: " << nTotalWinnings2 << std::endl;
        }
    }
    // report final puzzle answer
    std::cout << std::endl << "Answer to part 2: total winnings = " << nTotalWinnings2 << std::endl << std::endl;


/* ========== */   tmr.TimeReport( "Timing 2 - solving puzzle part 2 : " );   // ==============================^^^^^

    return 0;
}
