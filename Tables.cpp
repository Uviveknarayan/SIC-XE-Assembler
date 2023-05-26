#include <bits/stdc++.h>
using namespace std;

struct optab
{
  string opcode;
  string exist;
  int frmt;
  optab()
  {
    opcode = "undefined";
    frmt = 0;
    exist = "no";
  }

  optab(string opc, int format, string doesexist)
  {
    opcode = opc;
    frmt = format;
    exist = doesexist;
  }
};

struct regtab
{
  char regnum;
  string exist;
  regtab()
  {
    regnum = 'F';
    exist = "no";
  }
  regtab(char num, string doesexist)
  {
    regnum = num;
    exist = doesexist;
  }
};

struct labeltab
{
  string address;
  string name;
  int reltve;
  int blocknum;
  string exist;
  labeltab()
  {
    name = "undefined";
    address = "0";
    blocknum = 0;
    exist = "no";
    reltve = 0;
  }
};

struct littab
{
  string value;
  string address;
  string exist;
  int blocknum = 0;
  littab()
  {
    value = "";
    address = "?";
    blocknum = 0;
    exist = "no";
  }
};

struct blocktab
{
  string startAddress;
  string name;
  string LOCCTR;
  int number;
  string exist;
  blocktab()
  {
    name = "undefined";
    startAddress = "?";
    exist = "no";
    number = -1;
    LOCCTR = "0";
  }
};


typedef map<string, labeltab> SYMBOL_TABLE;
typedef map<string, optab> OPCODE_TABLE;
typedef map<string, regtab> REG_TABLE;
typedef map<string, littab> LIT_TABLE;
typedef map<string, blocktab> BLOCK_TABLE;

SYMBOL_TABLE SYMTAB;
OPCODE_TABLE OPTAB;
REG_TABLE REGTAB;
LIT_TABLE LITTAB;
BLOCK_TABLE BLOCKS;

void loadregtable()
{
  //this function loads the registers with their traditional values A-0, X-1, L-2, B-3, S-4, T-5,F-6, PC-8, SW-9.
  regtab reginfo1('0', "yes");
  REGTAB["A"] = reginfo1;
  regtab reginfo2('1', "yes");
  REGTAB["X"] = reginfo2;
  regtab reginfo3('2', "yes");
  REGTAB["L"] = reginfo3;
  regtab reginfo4('3', "yes");
  REGTAB["B"] = reginfo4;
  regtab reginfo5('4', "yes");
  REGTAB["S"] = reginfo5;
  regtab reginfo6('5', "yes");
  REGTAB["T"] = reginfo6;
  regtab reginfo7('6', "yes");
  REGTAB["F"] = reginfo7;
  regtab reginfo8('8', "yes");
  REGTAB["PC"] = reginfo8;
  regtab reginfo9('9', "yes");
  REGTAB["SW"] = reginfo9;
}

void loadblocks()
{
  // LOADING BLOCKS
  BLOCKS["DEFAULT"].exist = "yes";
  BLOCKS["DEFAULT"].name = "DEFAULT";
  BLOCKS["DEFAULT"].startAddress = "00000";
  BLOCKS["DEFAULT"].number = 0;
  BLOCKS["DEFAULT"].LOCCTR = "0";
}

void loadoptable()
{
  // filling the opcode table with predefined opcodes according to SIC-XE instruction sheet.
  optab opinfo1("18", 3, "yes");
  OPTAB["ADD"] = opinfo1;
  optab opinfo2("58", 3, "yes");
  OPTAB["ADDF"] = opinfo2;
  optab opinfo3("90", 2, "yes");
  OPTAB["ADDR"] = opinfo3;
  optab opinfo4("40", 3, "yes");
  OPTAB["AND"] = opinfo4;
  optab opinfo5("B4", 2, "yes");
  OPTAB["CLEAR"] = opinfo5;
  optab opinfo6("28", 3, "yes");
  OPTAB["COMP"] = opinfo6;
  optab opinfo7("88", 3, "yes");
  OPTAB["COMPF"] = opinfo7;
  optab opinfo8("A0", 2, "yes");
  OPTAB["COMPR"] = opinfo8;
  optab opinfo9("24", 3, "yes");
  OPTAB["DIV"] = opinfo9;
  optab opinfo10("64", 3, "yes");
  OPTAB["DIVF"] = opinfo10;
  optab opinfo11("9C", 2, "yes");
  OPTAB["DIVR"] = opinfo11;
  optab opinfo12("C4", 1, "yes");
  OPTAB["FIX"] = opinfo12;
  optab opinfo13("C0", 1, "yes");
  OPTAB["FLOAT"] = opinfo13;
  optab opinfo14("F4", 1, "yes");
  OPTAB["HIO"] = opinfo14;
  optab opinfo15("3C", 3, "yes");
  OPTAB["J"] = opinfo15;
  optab opinfo16("30", 3, "yes");
  OPTAB["JEQ"] = opinfo16;
  optab opinfo17("34", 3, "yes");
  OPTAB["JGT"] = opinfo17;
  optab opinfo18("38", 3, "yes");
  OPTAB["JLT"] = opinfo18;
  optab opinfo19("48", 3, "yes");
  OPTAB["JSUB"] = opinfo19;
  optab opinfo20("00", 3, "yes");
  OPTAB["LDA"] = opinfo20;
  optab opinfo21("68", 3, "yes");
  OPTAB["LDB"] = opinfo21;
  optab opinfo22("50", 3, "yes");
  OPTAB["LDCH"] = opinfo22;
  optab opinfo23("70", 3, "yes");
  OPTAB["LDF"] = opinfo23;
  optab opinfo24("08", 3, "yes");
  OPTAB["LDL"] = opinfo24;
  optab opinfo25("6C", 3, "yes");
  OPTAB["LDS"] = opinfo25;
  optab opinfo26("74", 3, "yes");
  OPTAB["LDT"] = opinfo26;
  optab opinfo27("04", 3, "yes");
  OPTAB["LDX"] = opinfo27;
  optab opinfo28("D0", 3, "yes");
  OPTAB["LPS"] = opinfo28;
  optab opinfo29("20", 3, "yes");
  OPTAB["MUL"] = opinfo29;
  optab opinfo30("60", 3, "yes");
  OPTAB["MULF"] = opinfo30;
  optab opinfo31("98", 2, "yes");
  OPTAB["MULR"] = opinfo31;
  optab opinfo32("C8", 1, "yes");
  OPTAB["NORM"] = opinfo32;
  optab opinfo33("44", 3, "yes");
  OPTAB["OR"] = opinfo33;
  optab opinfo34("D8", 3, "yes");
  OPTAB["RD"] = opinfo34;
  optab opinfo35("AC", 2, "yes");
  OPTAB["RMO"] = opinfo35;
  optab opinfo36("4F", 3, "yes");
  OPTAB["RSUB"] = opinfo36;
  optab opinfo37("A4", 2, "yes");
  OPTAB["SHIFTL"] = opinfo37;
  optab opinfo38("A8", 2, "yes");
  OPTAB["SHIFTR"] = opinfo38;
  optab opinfo39("F0", 1, "yes");
  OPTAB["SIO"] = opinfo39;
  optab opinfo40("EC", 3, "yes");
  OPTAB["SSK"] = opinfo40;
  optab opinfo41("0C", 3, "yes");
  OPTAB["STA"] = opinfo41;
  optab opinfo42("78", 3, "yes");
  OPTAB["STB"] = opinfo42;
  optab opinfo43("54", 3, "yes");
  OPTAB["STCH"] = opinfo43;
  optab opinfo44("80", 3, "yes");
  OPTAB["STF"] = opinfo44;
  optab opinfo45("D4", 3, "yes");
  OPTAB["STI"] = opinfo45;
  optab opinfo46("14", 3, "yes");
  OPTAB["STL"] = opinfo46;
  optab opinfo47("7C", 3, "yes");
  OPTAB["STS"] = opinfo47;
  optab opinfo48("E8", 3, "yes");
  OPTAB["STSW"] = opinfo48;
  optab opinfo49("84", 3, "yes");
  OPTAB["STT"] = opinfo49;
  optab opinfo50("10", 3, "yes");
  OPTAB["STX"] = opinfo50;
  optab opinfo51("1C", 3, "yes");
  OPTAB["SUB"] = opinfo51;
  optab opinfo52("5C", 3, "yes");
  OPTAB["SUBF"] = opinfo52;
  optab opinfo53("94", 2, "yes");
  OPTAB["SUBR"] = opinfo53;
  optab opinfo54("B0", 2, "yes");
  OPTAB["SVC"] = opinfo54;
  optab opinfo55("E0", 3, "yes");
  OPTAB["TD"] = opinfo55;
  optab opinfo56("F8", 1, "yes");
  OPTAB["TIO"] = opinfo56;
  optab opinfo57("2C", 3, "yes");
  OPTAB["TIX"] = opinfo57;
  optab opinfo58("B8", 2, "yes");
  OPTAB["TIXR"] = opinfo58;
  optab opinfo59("DC", 3, "yes");
  OPTAB["WD"] = opinfo59;
}

void load_all_tables()
{
  // LOADING ALL TABLES.
  loadregtable();
  loadblocks();
  loadoptable();
}
