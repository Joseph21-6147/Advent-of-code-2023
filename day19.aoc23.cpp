// AoC 2023 - Day 19 - Aplenty
// ===========================

// date:  2024-01-02
// by:    Joseph21 (Joseph21-6147)

#define DAY_STRING  "day19"     //   <---  adapt this to the correct day!!

#include <iostream>
#include <fstream>
#include <algorithm>
#include <numeric>
#include <vector>
#include <map>
#include <deque>

#include "my_utility.h"

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

// the DatumType is a bit complicated today, since it contains quite some values:
// a "Datum" is actually a Workflow, with a name and a set of rules
// Each rule is in the form IF attribute < or > value THEN GOTO other workflow

struct sRuleStruct;   // forward declaration the break circular dependency

// the data represent workflows, and consists of
// an identifier
// a number
typedef struct sDatumStruct {
    std::string sName = "<initial>";
    std::vector<sRuleStruct> vRules;
} DatumType;
typedef std::vector<DatumType> DataStream;

typedef struct sRuleStruct {
    char cAttrib  = '?';           // Each rule is in the form IF attribute < or > value THEN GOTO other workflow
    char cCompare = '_';           // except the unconditional rules, which have the form GOTO other workflow
    int nValue    = -1;
    std::string sGoto;             // if check is true, then goto this next rule
    bool bUnconditional = false;   // if accepted/rejected, bUnconditional == true and sGoto is "A" or "R"

    sDatumStruct *pGoto = nullptr; // needed to build a graph
} RuleType;

// Besides workflows, the input data also contains a segments of parts, which have four
// xmas attributes
typedef struct sPartStruct {
    int x = -1;
    int m = -1;
    int a = -1;
    int s = -1;
} PartType;
typedef std::vector<PartType> PartStream;

// ==========   INPUT DATA FUNCTIONS

// hardcoded input - just to get the solution tested
void GetData_EXAMPLE( DataStream &dData, PartStream &iParts ) {

    // hard code your workflow examples here - use the same data for your test input file
    DatumType cD;
    cD.vRules.clear(); cD.sName = "px";  cD.vRules.push_back( { 'a', '<', 2006, "qkq", false } ); cD.vRules.push_back( { 'm', '>', 2090, "A"  , false } ); cD.vRules.push_back( { '?', '_', -1, "rfg", true } ); dData.push_back( cD );
    cD.vRules.clear(); cD.sName = "pv";  cD.vRules.push_back( { 'a', '>', 1716, "R"  , false } ); cD.vRules.push_back( { '?', '_',   -1, "A"  , true  } );                                                       dData.push_back( cD );
    cD.vRules.clear(); cD.sName = "lnx"; cD.vRules.push_back( { 'm', '>', 1548, "A"  , false } ); cD.vRules.push_back( { '?', '_',   -1, "A"  , true  } );                                                       dData.push_back( cD );
    cD.vRules.clear(); cD.sName = "rfg"; cD.vRules.push_back( { 's', '<',  537, "gd" , false } ); cD.vRules.push_back( { 'x', '>', 2440, "R"  , false } ); cD.vRules.push_back( { '?', '_', -1, "A"  , true } ); dData.push_back( cD );
    cD.vRules.clear(); cD.sName = "qs";  cD.vRules.push_back( { 's', '>', 3448, "A"  , false } ); cD.vRules.push_back( { '?', '_',   -1, "lnx", true  } );                                                       dData.push_back( cD );
    cD.vRules.clear(); cD.sName = "qkq"; cD.vRules.push_back( { 'x', '<', 1416, "A"  , false } ); cD.vRules.push_back( { '?', '_',   -1, "crn", true  } );                                                       dData.push_back( cD );
    cD.vRules.clear(); cD.sName = "crn"; cD.vRules.push_back( { 'x', '>', 2662, "A"  , false } ); cD.vRules.push_back( { '?', '_',   -1, "R"  , true  } );                                                       dData.push_back( cD );
    cD.vRules.clear(); cD.sName = "in";  cD.vRules.push_back( { 's', '<', 1351, "px" , false } ); cD.vRules.push_back( { '?', '_',   -1, "qqz", true  } );                                                       dData.push_back( cD );
    cD.vRules.clear(); cD.sName = "qqz"; cD.vRules.push_back( { 's', '>', 2770, "qs" , false } ); cD.vRules.push_back( { 'm', '<', 1801, "hdj", false } ); cD.vRules.push_back( { '?', '_', -1, "R"  , true } ); dData.push_back( cD );
    cD.vRules.clear(); cD.sName = "gd";  cD.vRules.push_back( { 'a', '>', 3333, "R"  , false } ); cD.vRules.push_back( { '?', '_',   -1, "R"  , true  } );                                                       dData.push_back( cD );
    cD.vRules.clear(); cD.sName = "hdj"; cD.vRules.push_back( { 'm', '>',  838, "A"  , false } ); cD.vRules.push_back( { '?', '_',   -1, "pv" , true  } );                                                       dData.push_back( cD );

    // hard code your part examples here - use the same data for your test input file
    PartType cPart;
    cPart = {  787, 2655, 1222, 2876 }; iParts.push_back( cPart );
    cPart = { 1679,   44, 2067,  496 }; iParts.push_back( cPart );
    cPart = { 2036,  264,   79, 2244 }; iParts.push_back( cPart );
    cPart = { 2461, 1339,  466,  291 }; iParts.push_back( cPart );
    cPart = { 2127, 1623, 2188, 1013 }; iParts.push_back( cPart );
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
void ReadInputData( const std::string sFileName, DataStream &vData, PartStream &iParts ) {

    bool bWorkFlowPhase = true;

    std::ifstream dataFileStream( sFileName );
    vData.clear();
    std::string sLine;
    while (getline( dataFileStream, sLine )) {
        if (sLine.length() > 0) {    // non empty line
            if (bWorkFlowPhase) {
                // read workflows
                DatumType cDatum;
                // name of workflow
                cDatum.sName = get_token_dlmtd( "{", sLine );
                sLine = get_token_dlmtd( "}", sLine );  // remove trailing '}'

                // read all rules, distinguish atomic and other rules
                while (sLine.length() > 0) {
                    std::string sToken = get_token_dlmtd( ",", sLine );
                    if (sToken.length() < 5) {
                        // little hack, a comparison rule contains at least 5 characters,
                        // and atomic rules contain at most three
                        RuleType cRule = { '?', '_', -1, sToken, true };
                        cDatum.vRules.push_back( cRule );
                    } else {
                        std::string sAttrComp = get_token_sized( 2, sToken );
                        std::string sValue = get_token_dlmtd( ":", sToken );
                        RuleType cRule = { sAttrComp[0], sAttrComp[1], stoi( sValue ), sToken, false };
                        cDatum.vRules.push_back( cRule );
                    }
                }
                vData.push_back( cDatum );
            } else {
                // read parts / part ratings
                std::string sIgnore = get_token_dlmtd( "{x=", sLine );
                std::string sX      = get_token_dlmtd( ",m=", sLine );
                std::string sM      = get_token_dlmtd( ",a=", sLine );
                std::string sA      = get_token_dlmtd( ",s=", sLine );
                std::string sS      = sLine;
                PartType cPart = { stoi( sX ), stoi( sM ), stoi( sA ), stoi( sS ) };
                iParts.push_back( cPart );
            }
        } else {
            // switch to reading parts
            bWorkFlowPhase = false;
        }
    }
    dataFileStream.close();
}

void GetData_TEST(   DataStream &dData, PartStream &iParts ) { std::string sInputFile = DAY_STRING; sInputFile.append( ".input.test.txt"   ); ReadInputData( sInputFile, dData, iParts ); }
void GetData_PUZZLE( DataStream &dData, PartStream &iParts ) { std::string sInputFile = DAY_STRING; sInputFile.append( ".input.puzzle.txt" ); ReadInputData( sInputFile, dData, iParts ); }

// ==========   CONSOLE OUTPUT FUNCTIONS

void PrintRule( RuleType &r ) {
    if (r.bUnconditional) {
        std::cout << "    " <<                                                                                          "GOTO " << StringAlignedR ( r.sGoto, 4 );
    } else {
        std::cout << "    " << "IF " << r.cAttrib << " " << r.cCompare << " " << StringAlignedR( r.nValue, 4 ) << " THEN GOTO " << StringAlignedR ( r.sGoto, 4 );
        if (r.pGoto == nullptr) {
            std::cout << " [nullptr]";
        } else {
            std::cout << " -> " << r.pGoto->sName;
        }
    }
    std::cout << std::endl;
}

// output to console for testing
void PrintDatum( DatumType &iData, bool bEndl = true ) {
    // define your datum printing code here
    std::cout << "Workflow: " << iData.sName << " contains rules: " << std::endl;
    for (auto e : iData.vRules) {
        PrintRule( e );
    }
    if (bEndl) { std::cout << std::endl; }
}

// output to console for testing
void PrintDatumShort( DatumType &iData ) {
    std::cout << StringAlignedR( iData.sName, 4 );
}

// output to console for testing
void PrintDataStream( DataStream &dData ) {
    for (auto &e : dData) {
        PrintDatum( e );
    }
    std::cout << std::endl;
}

void PrintPart( PartType &p, bool bEndl = true ) {
    std::cout <<  " x = " << StringAlignedR( p.x, 4 );
    std::cout << ", m = " << StringAlignedR( p.m, 4 );
    std::cout << ", a = " << StringAlignedR( p.a, 4 );
    std::cout << ", s = " << StringAlignedR( p.s, 4 );
    if (bEndl) std::cout << std::endl;
}

void PrintPartStream( PartStream &ps ) {
    for (auto e : ps ) {
        PrintPart( e );
    }
}

// ==========   PROGRAM PHASING

// populates input data, by calling the appropriate input function that is associated
// with the global program phase var
void GetInput( DataStream &dData, PartStream &iParts, bool bDisplay = false ) {

    switch( glbProgPhase ) {
        case EXAMPLE: GetData_EXAMPLE( dData, iParts ); break;
        case TEST:    GetData_TEST(    dData, iParts ); break;
        case PUZZLE:  GetData_PUZZLE(  dData, iParts ); break;
        default: std::cout << "ERROR: GetInput() --> unknown program phase: " << glbProgPhase << std::endl;
    }
    // display to console if so desired (for debugging)
    if (bDisplay) {
        PrintDataStream( dData );
        PrintPartStream( iParts );
    }
}

// ==========   PUZZLE SPECIFIC SOLUTIONS

// ----------   part 1

// type WorkflowMap is a std::map to couple work flow names to their
// corresponding index in inputData
typedef std::map<std::string, int> WorkflowMap;

// fill wfm using the data from dData
void CreateWorkflowMap( DataStream &dData, WorkflowMap &wfm ) {
    for (int i = 0; i < (int)dData.size(); i++) {
        wfm.insert( { dData[i].sName, i } );
    }
}

// use wfm to lookup the index (in DataStream) of the work flow with corresponding name
// returns -1 if not found
int GetWorkflowIndex( WorkflowMap &wfm, const std::string &sWFname ) {
    int nResult = -1;
    WorkflowMap::iterator iter = wfm.find( sWFname );
    if (iter != wfm.end()) {
        nResult = (*iter).second;
    }
    return nResult;
}

// transform the DataStream dGraph into a graph structure, by populating the pointers in each
// of the rules
void CreateGraph1( DataStream &dGraph, WorkflowMap &wfm ) {
    // add Accept and Reject nodes to the graph - these have no rules/condition attached
    dGraph.push_back( { "A" } );
    dGraph.push_back( { "R" } );

    // init workflowmap - this enables the use of GetWorkflowIndex()
    wfm.clear();
    CreateWorkflowMap( dGraph, wfm );

    // now set all pointers in the rules to their corresponding work flow nodes
    for (int i = 0; i < (int)dGraph.size(); i++) {
        DatumType &curNode = dGraph[i];
        for (int j = 0; j < (int)curNode.vRules.size(); j++) {
            RuleType &curRule = curNode.vRules[j];
            int nIndex = GetWorkflowIndex( wfm, curRule.sGoto );
            if (nIndex < 0) {
                std::cout << "ERROR: CreateGraph1() --> can't find node with label: " << curRule.sGoto << std::endl;
            }
            curRule.pGoto = &dGraph[nIndex];
        }
    }
}

// Apply the set of workflows onto part. Start at workflow named "in". Result is that part
// reaches either state Accepted or Rejected.
// If it reaches state Accepted, then sum all the attribute (xmas) values of this part and return it.
// Otherwise, 0 is returned.
int ProcessPart1( DataStream &dData, WorkflowMap &wfm, PartType &part, bool bOutput = false ) {
    // get workflow with name "in"
    int nCurIndex = GetWorkflowIndex( wfm, "in" );
    DatumType *pCurDatum = &dData[ nCurIndex ];

    if (bOutput) std::cout << pCurDatum->sName;

    // iterate / step over to other rule until accepted or rejected state found
    while (pCurDatum->sName != "A" && pCurDatum->sName != "R") {

        bool bGotoFound = false;
        std::string sGotoVal;
        DatumType *pGotoPtr = nullptr;

        for (int i = 0; i < (int)pCurDatum->vRules.size() && !bGotoFound; i++) {
            RuleType &curRule = pCurDatum->vRules[i];
            if (curRule.bUnconditional) {
                bGotoFound = true;
                sGotoVal = curRule.sGoto;
                pGotoPtr = curRule.pGoto;
            } else {
                int nAttribVal = -1;
                switch (curRule.cAttrib) {
                    case 'x': nAttribVal = part.x; break;
                    case 'm': nAttribVal = part.m; break;
                    case 'a': nAttribVal = part.a; break;
                    case 's': nAttribVal = part.s; break;
                }
                switch (curRule.cCompare) {
                    case '<': bGotoFound = nAttribVal < curRule.nValue; break;
                    case '>': bGotoFound = nAttribVal > curRule.nValue; break;
                }
                if (bGotoFound) {
                    sGotoVal = curRule.sGoto;
                    pGotoPtr = curRule.pGoto;
                }
            }
        }
        if (bGotoFound) {
            pCurDatum = pGotoPtr;    // set pointer to current node onto the "goto node"
            if (bOutput) std::cout << " --> " << pCurDatum->sName;
        } else {
            // if no next rule was found, something's going really wrong...
            std::cout << "ERROR: ProcessPart1() --> shit here" << std::endl;
        }
    }

    // Return 0 if final state is not accepted. Return sum of attrib. values otherwise
    int nResult = 0;
    if (pCurDatum->sName == "A") {
        nResult += (part.x + part.m + part.a + part.s);
    }

    if (bOutput) std::cout << std::endl;

    return nResult;
}

// ----------   part 2

// data structure for conditionals - first the different categories of conditionals

enum CondCat {
    UNKNOWN = 0,
    SINGLETON,         // format: IF <attrib> <comp> <value> GOTO <label>
    UNCONDITIONAL,     // format:                            GOTO <label>
    AND,
    OR
};

std::string Cat2String( CondCat c ) {
    switch (c) {
        case UNKNOWN      : return "UNKNOWN"      ;
        case SINGLETON    : return "SINGLETON"    ;
        case UNCONDITIONAL: return "UNCONDITIONAL";
        case AND          : return "AND"          ;
        case OR           : return "OR"           ;
    }
    return "ERROR";
}

// data structure for conditionals

class ConditionType {
public:
    ConditionType() {}

    CondCat GetCat() {
        return eCategory;
    }
    void SetCat( CondCat cat ) {
        switch (cat) {
            case UNKNOWN:
                std::cout << "WARNING: ConditionType::SetCat() --> why'd you wanna set category to UNKNOWN?" << std::endl;
                break;
            case SINGLETON:
            case UNCONDITIONAL:
                if (!vANDlist.empty() || !vORlist.empty())
                    std::cout << "WARNING: ConditionType::SetCat() --> any of these categories are not allowed when AND / OR list is not empty: " << Cat2String( cat ) << std::endl;
                break;
            case AND:
                if (!vORlist.empty())
                    std::cout << "WARNING: ConditionType::SetCat() --> category AND is not allowed when OR list is not empty" << std::endl;
                break;
            case OR:
                if (!vANDlist.empty())
                    std::cout << "WARNING: ConditionType::SetCat() --> category OR is not allowed when AND list is not empty" << std::endl;
                break;
            default:
                std::cout << "ERROR: ConditionType::SetCat() --> unknown category as argument: " << cat << std::endl;
        }
        eCategory = cat;
    }

    char GetAttribute() {
        if (eCategory != SINGLETON)
            std::cout << "WARNING: ConditionType::GetAttribute() --> wrong condition type: " << Cat2String( eCategory ) << std::endl;
        return cAttrib;
    }
    void SetAttribute( char cAttr ) {
        if (eCategory != SINGLETON)
            std::cout << "WARNING: ConditionType::SetAttribute() --> wrong condition type: " << Cat2String( eCategory ) << std::endl;
        cAttrib = cAttr;
    }

    std::string &GetComparator() {
        if (eCategory != SINGLETON)
            std::cout << "WARNING: ConditionType::GetComparator() --> wrong condition type: " << Cat2String( eCategory ) << std::endl;
        return sCompare;
    }
    void SetComparator( const std::string &sComp ) {
        if (eCategory != SINGLETON)
            std::cout << "WARNING: ConditionType::SetComparator() --> wrong condition type: " << Cat2String( eCategory )<< std::endl;
        sCompare = sComp;
    }

    int GetValue() {
        if (eCategory != SINGLETON)
            std::cout << "WARNING: ConditionType::GetValue() --> wrong condition type: " << Cat2String( eCategory ) << std::endl;
        return nValue;
    }

    void SetValue( int nVal ) {
        if (eCategory != SINGLETON)
            std::cout << "WARNING: ConditionType::SetValue() --> wrong condition type: " << Cat2String( eCategory ) << std::endl;
        nValue = nVal;
    }

    std::string GetGoto() {
        if (!(eCategory == SINGLETON || eCategory == UNCONDITIONAL))
            std::cout << "WARNING: ConditionType::GetGoto() --> wrong condition type: " << Cat2String( eCategory ) << std::endl;
        return sGoto;
    }
    void SetGoto( std::string sLabel ) {
        if (!(eCategory == SINGLETON || eCategory == UNCONDITIONAL))
            std::cout << "WARNING: ConditionType::SetGoto() --> wrong condition type: " << Cat2String( eCategory ) << std::endl;
        sGoto = sLabel;
    }

    std::string GetFrom() {
        if (!(eCategory == SINGLETON || eCategory == UNCONDITIONAL))
            std::cout << "WARNING: ConditionType::GetFrom() --> wrong condition type: " << Cat2String( eCategory ) << std::endl;
        return sFrom;
    }
    void SetFrom( std::string sLabel ) {
        if (!(eCategory == SINGLETON || eCategory == UNCONDITIONAL))
            std::cout << "WARNING: ConditionType::SetFrom() --> wrong condition type: " << Cat2String( eCategory ) << std::endl;
        sFrom = sLabel;
    }

    std::deque<ConditionType> &GetAndList() {
        if (eCategory != AND)
            std::cout << "WARNING: ConditionType::GetAndList() --> wrong condition type: " << Cat2String( eCategory ) << std::endl;
        return vANDlist;
    }
    void AddAndList( ConditionType &cond ) {
        if (eCategory != AND)
            std::cout << "WARNING: ConditionType::AddAndList() --> wrong condition type: " << Cat2String( eCategory ) << std::endl;
        vANDlist.push_back( cond );
    }

    std::deque<ConditionType> &GetOrList() {
        if (eCategory != OR)
            std::cout << "WARNING: ConditionType::GetOrList() --> wrong condition type: " << Cat2String( eCategory ) << std::endl;
        return vORlist;
    }
    void AddOrList( ConditionType &cond ) {
        if (eCategory != OR)
            std::cout << "WARNING: ConditionType::AddOrList() --> wrong condition type: " << Cat2String( eCategory ) << std::endl;
        vORlist.push_back( cond );
    }

    void Print( int nIndent = 0 ) {

        auto IndentString = [=]( int nPrintIndentCntr ) {
            std::string sResult;
            for (int i = 0; i < nPrintIndentCntr; i++) {
                for (int j = 0; j < 4; j++) {
                    sResult.append( " " );
                }
            }
            return sResult;
        };

        switch (eCategory) {
            case SINGLETON:
                std::cout << IndentString( nIndent )
                          << "IF "
                          << cAttrib  << " "
                          << sCompare << " "
                          << StringAlignedR( nValue, 4 ) << " THEN GO FROM "
                          << StringAlignedR( sFrom , 4 ) << " TO "
                          << StringAlignedR( sGoto , 4 )
                          << std::endl;
                break;
            case UNCONDITIONAL:
                std::cout << IndentString( nIndent )
                          << "GO FROM "
                          << StringAlignedR( sFrom , 4 ) << " TO "
                          << StringAlignedR( sGoto , 4 )
                          << std::endl;
                break;
            case OR:
                if (vORlist.empty()) {
                    std::cout << IndentString( nIndent )
                              << "OR list of conditions {}"
                              << std::endl;
                } else {
                    std::cout << IndentString( nIndent )
                              <<  "OR list of conditions { "
                              << std::endl;
                    for (auto orC : vORlist) {
                        orC.Print( nIndent + 1 );
                    }
                    std::cout << IndentString( nIndent )
                              << "}"
                              << std::endl;
                }
                break;
            case AND:
                if (vANDlist.empty()) {
                    std::cout << IndentString( nIndent )
                              << "AND list of conditions {}"
                              << std::endl;
                } else {
                    std::cout << IndentString( nIndent )
                              <<  "AND list of conditions { "
                              << std::endl;
                    for (auto andC : vANDlist) {
                        andC.Print( nIndent + 1 );
                    }
                    std::cout << IndentString( nIndent )
                              << "}"
                              << std::endl;
                }
                break;
            case UNKNOWN:
                std::cout << IndentString( nIndent )
                          << "Condition category type UNKNOWN"
                          << std::endl;
                break;

        }
    }

    // if this is an OR or AND condition, remove any empty OR or AND conditions from its list
    void Filter() {
        if (eCategory == OR) {
            vORlist.erase(
                std::remove_if(
                    vORlist.begin(),
                    vORlist.end(),
                    []( ConditionType &c ) {
                        return (c.GetCat() == OR  && c.GetOrList( ).empty()) ||
                               (c.GetCat() == AND && c.GetAndList().empty());
                    }
                ),
                vORlist.end()
            );
        }
        if (eCategory == AND) {
            vANDlist.erase(
                std::remove_if(
                    vANDlist.begin(),
                    vANDlist.end(),
                    []( ConditionType &c ) {
                        return (c.GetCat() == OR  && c.GetOrList( ).empty()) ||
                               (c.GetCat() == AND && c.GetAndList().empty());
                    }
                ),
                vANDlist.end()
            );
        }

    }
    // if this is an OR condition, and the OR list contains only one element, then replace the condition with that element
    // the same for AND
    void Reduce() {
        if (eCategory == OR && vORlist.size() == 1) {
            *this = vORlist[0];
            vORlist.clear();
            this->Reduce();
        } else {
            for (auto &e : vORlist) {
                e.Reduce();
            }
        }
        if (eCategory == AND && vANDlist.size() == 1) {
            *this = vANDlist[0];
            vANDlist.clear();
            this->Reduce();
        } else {
            for (auto &e : vANDlist) {
                e.Reduce();
            }
        }
    }

    void Flatten() {
        if (eCategory == AND) {
            // if the last element is unconditional, and the element before is to be determined, combine them
            int nLast = vANDlist.size() - 1;
            int nPrev = nLast - 1;
            if (nPrev >= 0 && vANDlist[nLast].eCategory == UNCONDITIONAL && vANDlist[nPrev].sGoto == "<tbd>") {
                vANDlist[nPrev].sGoto = vANDlist[nLast].sGoto;
                vANDlist.pop_back();
            }

            std::deque<ConditionType> vTmpAddList;
            for (int i = 0; i < (int)vANDlist.size(); i++) {
                if (vANDlist[i].eCategory == AND) {
                    ConditionType &curANDcondition = vANDlist[i];
                    // replace this one condition by all its elements
                    for (int j = 0; j < (int)curANDcondition.vANDlist.size(); j++) {
                        vTmpAddList.push_back(curANDcondition.vANDlist[j] );
                    }
                } else {
                    // just copy this condition
                    vTmpAddList.push_back( vANDlist[i] );
                }
            }
            vANDlist = vTmpAddList;
        }
    }

    // create and return the complementary condition of this
    // for instance: if x > 20 goto lbl becomes if x <= 20 goto <tbd>
    ConditionType GetComplementary() {
        ConditionType condResult = *this;
        std::string sCmpThis  = this->GetComparator();
        std::string sCmpOther = "xx";

        if (sCmpThis[0] == 'L') { sCmpOther[0] = 'G'; } else
        if (sCmpThis[0] == 'G') { sCmpOther[0] = 'L'; } else
        std::cout << "ERROR: ConditionType::GetComplementary() --> can't handle current condition comparator: " << this->GetComparator() << std::endl;

        if (sCmpThis[1] == 'T') { sCmpOther[1] = 'E'; } else
        if (sCmpThis[1] == 'E') { sCmpOther[1] = 'T'; } else
        std::cout << "ERROR: ConditionType::GetComplementary() --> can't handle current condition comparator: " << this->GetComparator() << std::endl;

        condResult.SetComparator( sCmpOther );
        condResult.SetGoto( "<tbd>" );
        return condResult;
    }

private:

    CondCat eCategory = UNKNOWN;

    // if category is SINGLETON, then these class variables make sense
    char cAttrib         = '?';           // Each rule is in the form IF attribute < or > value THEN GOTO other workflow
    std::string sCompare = "xx";          // except the unconditional rules, which have the form GOTO other workflow
    int  nValue          = -1;
    // if category is SINGLETON or UNCONDITIONAL, then this class variable makes sense
    std::string sGoto    = "<uninited>";     // if check is true, then goto this next rule
    std::string sFrom    = "<uninited>";     // to administer where jump is from

    // if category is AND, then only this class variable makes sense
    std::deque<ConditionType> vANDlist;
    // if category is OR, then only this class variable makes sense
    std::deque<ConditionType> vORlist;
};

struct sEdgeStruct;  // forward declaration the break circular dependency
void PrintEdge( struct sEdgeStruct &e );

typedef struct sGraphNodeStruct {
    std::string sName;
    // the conditions to get to this node are in cNodeCond
    ConditionType cNodeCond;
    std::vector<sEdgeStruct> vEdges;
} GraphNode;
typedef std::deque<GraphNode> GraphType;

void PrintGraphNode( GraphNode &gNode ) {
    std::cout << gNode.sName << std::endl << " - node condition: ";
    gNode.cNodeCond.Print();
    std::cout << " - edge list: ";
    if (gNode.vEdges.empty()) {
        std::cout << "EMPTY" << std::endl;
    } else {
        std::cout << std::endl;
        for (int i = 0; i < (int)gNode.vEdges.size(); i++) {
            std::cout << "      ";
            PrintEdge( gNode.vEdges[i] );
        }
    }
    std::cout << std::endl;
}

typedef struct sEdgeStruct {
    std::string sNodeA, sNodeB;
    ConditionType edgeCondition;               // the condition associated with this edge
//    std::deque<ConditionType> prevConditions;  // all previously "inherited" conditions
    GraphNode *pGoto = nullptr;                // pointer to graphnode with edgeCondition.sGoto label
} GraphEdge;

void PrintEdge( GraphEdge &e ) {
    std::cout << "Edge from _" << e.sNodeA << "_ to _" << e.sNodeB
              << "_ has condition: ";
    e.edgeCondition.Print();

    if (e.pGoto == nullptr) {
        std::cout << " --> [ NULL ]";
    } else {
        std::cout << " --> [ " << e.pGoto->sName << " ] ";
    }
    std::cout << std::endl;
}

void PrintGraph( GraphType &graph ) {
    for (auto e : graph) {
        PrintGraphNode( e );
    }
}

GraphNode *GetGraphNode( GraphType &vGraph, const std::string &sName ) {
    GraphNode *pResult = nullptr;
    for (int i = 0; i < (int)vGraph.size() && pResult == nullptr; i++) {
        if (vGraph[i].sName == sName) {
            pResult = &(vGraph[i]);
        }
    }
    return pResult;
}

// transform the DataStream iData into a graph structure, by populating creating nodes for it in the GraphType ds,
// and assembling coherence via the pointers in each of the edges
void CreateGraph2( DataStream &iData, GraphType &graph ) {

    // insert Accept and Reject nodes - these have no conditions attached to them
    graph.clear();

    // init internal workflowmap
    WorkflowMap wfm;
    CreateWorkflowMap( iData, wfm );

    // (create and) add all other nodes from the input data
    for (int i = 0; i < (int)iData.size(); i++) {
        DatumType &curDatum = iData[i];
        GraphNode newNode = { curDatum.sName };
        newNode.cNodeCond.SetCat( OR );
        for (int j = 0; j < (int)curDatum.vRules.size(); j++) {

            RuleType &curRule = curDatum.vRules[j];
            // create the condition that belongs to this edge
            ConditionType newEdgeCond;

            if (curRule.bUnconditional) {
                newEdgeCond.SetCat( UNCONDITIONAL );
                newEdgeCond.SetGoto( curRule.sGoto );
            } else {
                newEdgeCond.SetCat( SINGLETON );
                newEdgeCond.SetAttribute(  curRule.cAttrib  );
                newEdgeCond.SetValue(      curRule.nValue   );
                newEdgeCond.SetGoto(       curRule.sGoto    );
                newEdgeCond.SetComparator( curRule.cCompare == '>' ? "GT" : "LT" );
            }
            newEdgeCond.SetFrom( newNode.sName );

            // create and fill the edge itself, including the pointer to it's associated graph node
            GraphEdge newEdge;
            newEdge.sNodeA = newNode.sName;
            newEdge.sNodeB = newEdgeCond.GetGoto();
            newEdge.edgeCondition = newEdgeCond;
            // add edge to the current node
            newNode.vEdges.push_back( newEdge );
        }
        // add created graph node to graph datastructure
        graph.push_back( newNode );
    }

    // now that all nodes exist and the std::vector won't change in volume anymore,
    // set all pointers for all edges to the correct node in the graph
    for (int i = 0; i < (int)graph.size(); i++) {
        GraphNode &curNode = graph[i];

        for (int j = 0; j < (int)curNode.vEdges.size(); j++) {

            GraphEdge &curEdge = curNode.vEdges[j];
            // create and fill the edge itself, including the pointer to it's associated graph node
            GraphNode *nghbPtr = GetGraphNode( graph, curEdge.edgeCondition.GetGoto() );
            if (nghbPtr == nullptr) {
                std::cout << "ERROR: CreateGraph2() --> can't find index for node label: " << curEdge.edgeCondition.GetGoto() << std::endl;
            }
            curEdge.pGoto = nghbPtr;
        }
    }
}


// Start at node named "in". Then apply it's ruleset onto the node and onto it's adjacent nodes
// (by enqueueing and later processing these).
void ProcessPart2( GraphType &vGraph, bool bOutput = false ) {

    // get graphnode with name "in"
    GraphNode *strtNode = GetGraphNode( vGraph, "in" );

    // create a queue to hold nodes for future processing
    std::deque<GraphNode *> localQueue;
    // add start node to queue
    localQueue.push_back( strtNode );

    while (!localQueue.empty()) {
        // get a pointer to the next node to process
        GraphNode *curNode = localQueue.front();
        localQueue.pop_front();

        if (bOutput) {
            std::cout << std::endl << "cur. node from queue = " << curNode->sName << std::endl;
            PrintGraphNode( *curNode );
        }

        // prepare to collect complementary conditions for this graphnode
        std::deque<ConditionType> vProcCondList;

        // analyse each of the adjacent nodes by checking the edges of this graph node
        for (int i = 0; i < (int)curNode->vEdges.size(); i++) {

            // get handles to the current edge and the graph node it leads to
            GraphEdge &curEdge  = curNode->vEdges[i];
            GraphNode *nghbNode = curNode->vEdges[i].pGoto;

            if (bOutput) std::cout << "cur. edge leads to neighbournode " << nghbNode->sName << std::endl;

            // build up the condition to propagate to this neighbour node
            ConditionType nextNodeCond;
            nextNodeCond.SetCat( AND );
            nextNodeCond.AddAndList( curNode->cNodeCond );
            for (auto c : vProcCondList) {
                nextNodeCond.AddAndList( c );
            }
            nextNodeCond.AddAndList( curEdge.edgeCondition );

            nextNodeCond.Filter();
            nextNodeCond.Reduce();
            nextNodeCond.Flatten();

            // add it to the next node's condition
            nghbNode->cNodeCond.AddOrList( nextNodeCond );

            if (!nghbNode->vEdges.empty()) {
                // enqueue this neighbour for future processing
                localQueue.push_back( nghbNode );
            }

            // create complementary condition for all other edges and add it to processing Condition List
            if (curEdge.edgeCondition.GetCat() != UNCONDITIONAL) {
                // if this is not an 'A' or 'R' node
                ConditionType complCond = curEdge.edgeCondition.GetComplementary();
                vProcCondList.push_back( complCond );
            }
        }
    }
}

// assumptions:
// 1. nodeCond is an OR type condition,
// 2. this OR type condition contains only AND type conditions in itself, and
// 3. each of these AND conditions contains only SINGLETON or UNCONDITIONAL type conditions
long long CountNodeConditionOptions( ConditionType &nodeCond, bool bOutput = false ) {
    if (nodeCond.GetCat() != OR) {
        std::cout << "ERROR: CountNodeConditionOptions() --> precondition 1 violated" << std::endl;
    }
    for (int i = 0; i < (int)nodeCond.GetOrList().size(); i++) {
        ConditionType &internalCond = nodeCond.GetOrList()[i];
        if (internalCond.GetCat() != AND) {
            std::cout << "ERROR: CountNodeConditionOptions() --> precondition 2 violated on index: " << i << std::endl;
        }
    }

    if (bOutput) {
        std::cout << "This should be A node's nodecondition: " << std::endl;
        nodeCond.Print();
    }

    long long llTotalResult = 0;
    for (int i = 0; i < (int)nodeCond.GetOrList().size(); i++) {

        ConditionType &andCond = nodeCond.GetOrList()[i];

        if (bOutput) {
            std::cout << "Working out next AND condition: " << std::endl;
            andCond.Print();
        }

        long long xMin = 1, xMax = 4000;
        long long mMin = 1, mMax = 4000;
        long long aMin = 1, aMax = 4000;
        long long sMin = 1, sMax = 4000;

        long long xOptions = xMax - xMin;
        long long mOptions = mMax - mMin;
        long long aOptions = aMax - aMin;
        long long sOptions = sMax - sMin;

        if (bOutput) {
            std::cout << "Before this AND clause: "
                      << StringAlignedR( xOptions, 5 ) << ", "
                      << StringAlignedR( mOptions, 5 ) << ", "
                      << StringAlignedR( aOptions, 5 ) << ", "
                      << StringAlignedR( sOptions, 5 ) << std::endl;
        }

        // handel each AND condition in turn
        for (int j = 0; j < (int)andCond.GetAndList().size(); j++) {
            ConditionType &curCond = andCond.GetAndList()[j];
            switch (curCond.GetCat()) {
                case UNCONDITIONAL:
                        // do nothing - all options remain open on unconditional type
                    break;
                case SINGLETON: {
                        int localMin = 0, localMax = 4000;
                        bool bMaxFlag;
                        if (curCond.GetComparator() == "LT") { bMaxFlag = true ; localMax = (curCond.GetValue() - 1); } else
                        if (curCond.GetComparator() == "LE") { bMaxFlag = true ; localMax = (curCond.GetValue()    ); } else
                        if (curCond.GetComparator() == "GT") { bMaxFlag = false; localMin = (curCond.GetValue() + 1); } else
                        if (curCond.GetComparator() == "GE") { bMaxFlag = false; localMin = (curCond.GetValue()    ); } else
                        std::cout << "ERROR: CountNodeConditionOptions() --> unidentified comparator value: " << curCond.GetComparator() << std::endl;

                        switch (curCond.GetAttribute()) {
                            case 'x': if (bMaxFlag) { xMax = localMax; } else { xMin = localMin; } break;
                            case 'm': if (bMaxFlag) { mMax = localMax; } else { mMin = localMin; } break;
                            case 'a': if (bMaxFlag) { aMax = localMax; } else { aMin = localMin; } break;
                            case 's': if (bMaxFlag) { sMax = localMax; } else { sMin = localMin; } break;
                            default : std::cout << "ERROR: CountNodeConditionOptions() --> unidentified attribute value: " << curCond.GetAttribute() << std::endl;
                        }
                    }
                    break;
                default: std::cout << "ERROR: CountNodeConditionOptions() --> precondition 3 violated on indices: " << i << ", " << j << std::endl;
            }
        }

        xOptions = std::max( (long long)0, xMax - xMin + 1 );
        mOptions = std::max( (long long)0, mMax - mMin + 1 );
        aOptions = std::max( (long long)0, aMax - aMin + 1 );
        sOptions = std::max( (long long)0, sMax - sMin + 1 );

        if (bOutput) {
            std::cout << "After this AND clause: "
                      << StringAlignedR( xOptions, 5 ) << ", "
                      << StringAlignedR( mOptions, 5 ) << ", "
                      << StringAlignedR( aOptions, 5 ) << ", "
                      << StringAlignedR( sOptions, 5 ) << std::endl;
        }

        long long llLocalResult = xOptions * mOptions * aOptions * sOptions;

        llTotalResult += llLocalResult;

        if (bOutput) std::cout << "Local result = " << llLocalResult << ", total result = " << llTotalResult << std::endl;
    }
    return llTotalResult;
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
    PartStream partsData;
    GetInput( inputData, partsData, glbProgPhase != PUZZLE );
    DataStream part2Data = inputData;
    std::cout << "Data stats - size of data stream " << inputData.size() << ", size of parts stream " << partsData.size() << std::endl << std::endl;

/* ========== */   tmr.TimeReport( "Timing 0 - input data preparation: " );   // =========================^^^^^vvvvv

    // part 1 code here

    // transform the input data into a graph structure
    // this will also initialize glbWFM as lookup structure
    WorkflowMap glbWFM;                     // for ease of searching workflow nodes in the graph
    CreateGraph1( inputData, glbWFM );      // set pointers in input data to transform it into a graph

    bool bOutput = glbProgPhase != PUZZLE;
    int nCumScore1 = 0;
    for (int i = 0; i < (int)partsData.size(); i++) {
        PartType &curPart = partsData[i];
        if (bOutput) {
            PrintPart( curPart, false );
            std::cout << " - ";
        }
        int nScore = ProcessPart1( inputData, glbWFM, curPart, bOutput );
        nCumScore1 += nScore;
    }

    std::cout << std::endl << "Answer to part 1: cumulated score = " << nCumScore1 << std::endl << std::endl;

/* ========== */   tmr.TimeReport( "Timing 1 - solving puzzle part 1 : " );   // =========================^^^^^vvvvv

    // part 2 code here
    GraphType glbGraph;

    if (bOutput) std::cout << "Part 2 - creating graph..." << std::endl;
    CreateGraph2( inputData, glbGraph );
    if (bOutput) {
        std::cout << "Part 2 - printing graph..." << std::endl;
        PrintGraph( glbGraph );

        std::cout << std::endl << "Processing graph ============================ " << std::endl;
    }
    ProcessPart2( glbGraph, bOutput );

    if (bOutput) {
        std::cout << std::endl << "Again printing graph ============================ " << std::endl;
        PrintGraph( glbGraph );
    }

    GraphNode *ptrA = GetGraphNode( glbGraph, "A" );
    long long llAnswer = CountNodeConditionOptions( ptrA->cNodeCond, bOutput );

    std::cout << std::endl << "Answer to part 2: accepted options = " << llAnswer << std::endl << std::endl;

/* ========== */   tmr.TimeReport( "Timing 2 - solving puzzle part 2 : " );   // ==============================^^^^^

    return 0;
}
