#include "Utils.cpp"
#include "Tables.cpp"
#include <bits/stdc++.h>
#define endl '\n'
using namespace std;

string fileName, *blksnumtoname, firstExecSec;
bool Errflag = false;
int proglen;

void determine_E(string expr, bool &reltve, string &tempOpnd, int lineNum, ofstream &errorFile, bool &Errflag)
{
  string opnd = "?", singleOptr = "?", stringval = "", tmp = "", wrtdata = "";
  int lastOperand = 0, lastOperator = 0, numPairs = 0;
  char prevByte = ' ';
  bool illegal_expr = false;

  for (int i = 0; i < expr.length();)
  {
    opnd = "";

    prevByte = expr[i];
    while ((prevByte != '+' && prevByte != '-' && prevByte != '/' && prevByte != '*') && i < expr.length())
    {
      opnd += prevByte;
      prevByte = expr[++i];
    }

    if (SYMTAB[opnd].exist == "yes")
    { 
      // Checks if operand exists or not.
      lastOperand = SYMTAB[opnd].reltve;
      tmp = to_string(Hexstr_to_int(SYMTAB[opnd].address));
    }
    else if ((opnd != "" || opnd != "?") && if_all_numbers(opnd))
    {
      lastOperand = 0;
      tmp = opnd;
    }
    else
    {
      wrtdata = "Line: " + to_string(lineNum) + " : Can't find symbol. Instead found " + opnd;
      write_to_file(errorFile, wrtdata);
      illegal_expr = true;
      break;
    }

    if (lastOperand * lastOperator == 1)
    { // Check exprs legallity
      wrtdata = "Line: " + to_string(lineNum) + " : illegal expression";
      write_to_file(errorFile, wrtdata);
      Errflag = true;
      illegal_expr = true;
      break;
    }
    else if ((singleOptr == "-" || singleOptr == "+" || singleOptr == "?") && lastOperand == 1)
    {
      if (singleOptr == "-")
      {
        numPairs--;
      }
      else
      {
        numPairs++;
      }
    }

    stringval += tmp;

    singleOptr = "";
    while (i < expr.length() && (prevByte == '+' || prevByte == '-' || prevByte == '/' || prevByte == '*'))
    {
      singleOptr += prevByte;
      prevByte = expr[++i];
    }

    if (singleOptr.length() > 1)
    {
      wrtdata = "Line: " + to_string(lineNum) + " : ILLEGAL OPERATOR IN EXPRESSION. FOUND " + singleOptr;
      write_to_file(errorFile, wrtdata);
      Errflag = true;
      illegal_expr = true;
      break;
    }

    if (singleOptr == "*" || singleOptr == "/")
    {
      lastOperator = 1;
    }
    else
    {
      lastOperator = 0;
    }

    stringval += singleOptr;
  }

  if (!illegal_expr)
  {
    if (numPairs == 1)
    { // RELATIVE
      reltve = 1;
      EvaluateString tempOBJ(stringval);
      tempOpnd = Int_to_hex_str(tempOBJ.getRes());
    }
    else if (numPairs == 0)
    { // ABSOLUTE
      reltve = 0;
      cout << stringval << endl;
      EvaluateString tempOBJ(stringval);
      tempOpnd = Int_to_hex_str(tempOBJ.getRes());
    }
    else
    {
      wrtdata = "Line: " + to_string(lineNum) + " : ILLEGAL EXPRESSION";
      write_to_file(errorFile, wrtdata);
      Errflag = true;
      tempOpnd = "00000";
      reltve = 0;
    }
  }
  else
  {
    tempOpnd = "00000";
    Errflag = true;
    reltve = 0;
  }
}
void LTORG_put(string &front_lit, int &lineNumDelta, int lineNum, int &LOCCTR, int &lastDeltaLOCCTR, int currBlockNum)
{
  string addr_lit, val_lit;
  front_lit = "";
  for (auto const &it : LITTAB)
  {
    addr_lit = it.second.address;
    val_lit = it.second.value;
    if (addr_lit != "?")
    {
      // PASS
    }
    else
    {
      lineNum += 5;
      lineNumDelta += 5;
      LITTAB[it.first].address = Int_to_hex_str(LOCCTR);
      LITTAB[it.first].blocknum = currBlockNum;
      front_lit += "\n" + to_string(lineNum) + "\t" + Int_to_hex_str(LOCCTR) + "\t" + to_string(currBlockNum) + "\t" + "*" + "\t" + "=" + val_lit + "\t" + " " + "\t" + " ";

      if (val_lit[0] == 'X')
      {
        LOCCTR += (val_lit.length() - 3) / 2;
        lastDeltaLOCCTR += (val_lit.length() - 3) / 2;
      }
      else if (val_lit[0] == 'C')
      {
        LOCCTR += val_lit.length() - 3;
        lastDeltaLOCCTR += val_lit.length() - 3;
      }
    }
  }
}

void pass1()
{
  ifstream sourceFile;
  ofstream intermediateFile, errorFile;

  sourceFile.open(fileName);
  if (!sourceFile)
  {
    cout << "Can't find file : " << fileName << endl;
    exit(1);
  }

  intermediateFile.open("IntermediateFile_of_" + fileName);
  if (!intermediateFile)
  {
    cout << "Unable to open file: IntermediateFile_of_" << fileName << endl;
    exit(1);
  }
  write_to_file(intermediateFile, "Line\tAddress\tLabel\tOPCODE\tOPERAND\tComment");
  errorFile.open("errfile_of_" + fileName);
  if (!errorFile)
  {
    cout << "Unable to open file: errfile_of_" << fileName << endl;
    exit(1);
  }
  write_to_file(errorFile, "ERRORS OF PASS1:");

  string fileLine;
  string wrtdata, wrtdataSuffix = "", wrtdataPrefix = "";
  int ind = 0;

  string currblkname = "DEFAULT";
  int currBlockNum = 0;
  int totalBlocks = 1;

  bool statusCode;
  string label, opcode, operand, comment;
  string tempOpnd;

  int startAddress, LOCCTR, saveLOCCTR, lineNum, lastDeltaLOCCTR, lineNumDelta = 0;
  lineNum = 0;
  lastDeltaLOCCTR = 0;

  getline(sourceFile, fileLine);
  lineNum += 5;
  while (check_comment(fileLine))
  {
    wrtdata = to_string(lineNum) + "\t" + fileLine;
    write_to_file(intermediateFile, wrtdata);
    getline(sourceFile, fileLine); // read first input line
    lineNum += 5;
    ind = 0;
  }

  readFirstNonBlankSpace(fileLine, ind, statusCode, label);
  readFirstNonBlankSpace(fileLine, ind, statusCode, opcode);

  if (opcode == "START")
  {
    readFirstNonBlankSpace(fileLine, ind, statusCode, operand);
    readFirstNonBlankSpace(fileLine, ind, statusCode, comment, true);
    startAddress = Hexstr_to_int(operand);
    // cout<<startAddress<<endl;
    // exit(0);
    LOCCTR = startAddress;
    wrtdata = to_string(lineNum) + "\t" + Int_to_hex_str(LOCCTR - lastDeltaLOCCTR) + "\t" + to_string(currBlockNum) + "\t" + label + "\t" + opcode + "\t" + operand + "\t" + comment;
    write_to_file(intermediateFile, wrtdata); // Write file to intermediate file

    getline(sourceFile, fileLine); // Read next line
    lineNum += 5;
    ind = 0;
    readFirstNonBlankSpace(fileLine, ind, statusCode, label);  // Parse label
    readFirstNonBlankSpace(fileLine, ind, statusCode, opcode); // Parse OPCODE
  }
  else
  {
    startAddress = 0;
    LOCCTR = 0;
  }
  //*************************************************************
  string currentSecTitle = "DEFAULT";
  int secCntr = 0;
  //**********************************************************************
  while (opcode != "END")
  {
    //****************************************************************

    while (opcode != "END")
    {

      //****************************************************************
      if (!check_comment(fileLine))
      {
        if (label != "")
        { // Label found
          if (SYMTAB[label].exist == "no")
          {
            SYMTAB[label].name = label;
            SYMTAB[label].address = Int_to_hex_str(LOCCTR);
            SYMTAB[label].reltve = 1;
            SYMTAB[label].exist = "yes";
            SYMTAB[label].blocknum = currBlockNum;
          }
          else
          {
            wrtdata = "Line: " + to_string(lineNum) + " : Duplicate symbol for '" + label + "'. Previously defined at " + SYMTAB[label].address;
            write_to_file(errorFile, wrtdata);
            Errflag = true;
          }
        }
        if (OPTAB[getFinalOp(opcode)].exist == "yes")
        { //SEARCHING OPCODE IN OPTAB
          if (OPTAB[getFinalOp(opcode)].frmt == 3)
          {
            LOCCTR += 3;
            lastDeltaLOCCTR += 3;
            if (getFlagFrmt(opcode) == '+')
            {
              LOCCTR += 1;
              lastDeltaLOCCTR += 1;
            }
            if (getFinalOp(opcode) == "RSUB")
            {
              operand = " ";
            }
            else
            {
              readFirstNonBlankSpace(fileLine, ind, statusCode, operand);
              if (operand[operand.length() - 1] == ',')
              {
                readFirstNonBlankSpace(fileLine, ind, statusCode, tempOpnd);
                operand += tempOpnd;
              }
            }

            if (getFlagFrmt(operand) == '=')
            {
              tempOpnd = operand.substr(1, operand.length() - 1);
              if (tempOpnd == "*")
              {
                tempOpnd = "X'" + Int_to_hex_str(LOCCTR - lastDeltaLOCCTR, 6) + "'";
              }
              if (LITTAB[tempOpnd].exist == "no")
              {
                LITTAB[tempOpnd].value = tempOpnd;
                LITTAB[tempOpnd].exist = "yes";
                LITTAB[tempOpnd].address = "?";
                LITTAB[tempOpnd].blocknum = -1;
              }
            }
          }
          else if (OPTAB[getFinalOp(opcode)].frmt == 1)
          {
            operand = " ";
            LOCCTR += OPTAB[getFinalOp(opcode)].frmt;
            lastDeltaLOCCTR += OPTAB[getFinalOp(opcode)].frmt;
          }
          else
          {
            LOCCTR += OPTAB[getFinalOp(opcode)].frmt;
            lastDeltaLOCCTR += OPTAB[getFinalOp(opcode)].frmt;
            readFirstNonBlankSpace(fileLine, ind, statusCode, operand);
            if (operand[operand.length() - 1] == ',')
            {
              readFirstNonBlankSpace(fileLine, ind, statusCode, tempOpnd);
              operand += tempOpnd;
            }
          }
        }

        else if (opcode == "WORD")
        {
          readFirstNonBlankSpace(fileLine, ind, statusCode, operand);
          LOCCTR += 3;
          lastDeltaLOCCTR += 3;
        }
        else if (opcode == "RESW")
        {
          readFirstNonBlankSpace(fileLine, ind, statusCode, operand);
          LOCCTR += 3 * stoi(operand);
          lastDeltaLOCCTR += 3 * stoi(operand);
        }
        else if (opcode == "RESB")
        {
          readFirstNonBlankSpace(fileLine, ind, statusCode, operand);
          LOCCTR += stoi(operand);
          lastDeltaLOCCTR += stoi(operand);
        }
        else if (opcode == "BYTE")
        {
          readByteOperand(fileLine, ind, statusCode, operand);
          if (operand[0] == 'X')
          {
            LOCCTR += (operand.length() - 3) / 2;
            lastDeltaLOCCTR += (operand.length() - 3) / 2;
          }
          else if (operand[0] == 'C')
          {
            LOCCTR += operand.length() - 3;
            lastDeltaLOCCTR += operand.length() - 3;
          }
        }
        else if (opcode == "BASE")
        {
          readFirstNonBlankSpace(fileLine, ind, statusCode, operand);
        }
        else if (opcode == "LTORG")
        {
          operand = " ";
          LTORG_put(wrtdataSuffix, lineNumDelta, lineNum, LOCCTR, lastDeltaLOCCTR, currBlockNum);
        }
        else if (opcode == "ORG")
        {
          readFirstNonBlankSpace(fileLine, ind, statusCode, operand);

          char prevByte = operand[operand.length() - 1];
          while (prevByte == '+' || prevByte == '-' || prevByte == '/' || prevByte == '*')
          {
            readFirstNonBlankSpace(fileLine, ind, statusCode, tempOpnd);
            operand += tempOpnd;
            prevByte = operand[operand.length() - 1];
          }

          int tempVariable;
          tempVariable = saveLOCCTR;
          saveLOCCTR = LOCCTR;
          LOCCTR = tempVariable;

          if (SYMTAB[operand].exist == "yes")
          {
            LOCCTR = Hexstr_to_int(SYMTAB[operand].address);
          }
          else
          {
            bool reltve;
            Errflag = false;
            determine_E(operand, reltve, tempOpnd, lineNum, errorFile, Errflag);
            if (!Errflag)
            {
              LOCCTR = Hexstr_to_int(tempOpnd);
            }
            Errflag = false;
          }
        }
        else if (opcode == "USE")
        {

          readFirstNonBlankSpace(fileLine, ind, statusCode, operand);
           if(statusCode==false){
            operand="DEFAULT";
          }
          BLOCKS[currblkname].LOCCTR = Int_to_hex_str(LOCCTR);

          if (BLOCKS[operand].exist == "no")
          {
            BLOCKS[operand].exist = "yes";
            BLOCKS[operand].name = operand;
            BLOCKS[operand].number = totalBlocks++;
            BLOCKS[operand].LOCCTR = "0";
          }

          currBlockNum = BLOCKS[operand].number;
          currblkname = BLOCKS[operand].name;
          LOCCTR = Hexstr_to_int(BLOCKS[operand].LOCCTR);
        }
        else if (opcode == "EQU")
        {
          readFirstNonBlankSpace(fileLine, ind, statusCode, operand);
          tempOpnd = "";
          bool reltve;

          if (operand == "*")
          {
            tempOpnd = Int_to_hex_str(LOCCTR - lastDeltaLOCCTR, 6);
            reltve = 1;
          }
          else if (if_all_numbers(operand))
          {
            tempOpnd = Int_to_hex_str(stoi(operand), 6);
            reltve = 0;
          }
          else
          {
            char prevByte = operand[operand.length() - 1];

            while (prevByte == '+' || prevByte == '-' || prevByte == '/' || prevByte == '*')
            {
              readFirstNonBlankSpace(fileLine, ind, statusCode, tempOpnd);
              operand += tempOpnd;
              prevByte = operand[operand.length() - 1];
            }

           
            determine_E(operand, reltve, tempOpnd, lineNum, errorFile, Errflag);
          }

          SYMTAB[label].name = label;
          SYMTAB[label].address = tempOpnd;
          SYMTAB[label].reltve = reltve;
          SYMTAB[label].blocknum = currBlockNum;
          lastDeltaLOCCTR = LOCCTR - Hexstr_to_int(tempOpnd);
        }
        else
        {
          readFirstNonBlankSpace(fileLine, ind, statusCode, operand);
          wrtdata = "Line: " + to_string(lineNum) + " : Invalid OPCODE. Found " + opcode;
          write_to_file(errorFile, wrtdata);
          Errflag = true;
        }
        readFirstNonBlankSpace(fileLine, ind, statusCode, comment, true);
        if (opcode == "EQU" && SYMTAB[label].reltve == 0)
        {
          wrtdata = wrtdataPrefix + to_string(lineNum) + "\t" + Int_to_hex_str(LOCCTR - lastDeltaLOCCTR) + "\t" + " " + "\t" + label + "\t" + opcode + "\t" + operand + "\t" + comment + wrtdataSuffix;
        }
        else
        {
          wrtdata = wrtdataPrefix + to_string(lineNum) + "\t" + Int_to_hex_str(LOCCTR - lastDeltaLOCCTR) + "\t" + to_string(currBlockNum) + "\t" + label + "\t" + opcode + "\t" + operand + "\t" + comment + wrtdataSuffix;
        }
        wrtdataPrefix = "";
        wrtdataSuffix = "";
      }
      else
      {
        wrtdata = to_string(lineNum) + "\t" + fileLine;
      }
      write_to_file(intermediateFile, wrtdata);

      BLOCKS[currblkname].LOCCTR = Int_to_hex_str(LOCCTR); // UPDATE LCCNTR OF BLOCK AFTER EVERY INSTRUCTN.
      getline(sourceFile, fileLine);                            // READ NEXT LINE
      lineNum += 5 + lineNumDelta;
      lineNumDelta = 0;
      ind = 0;
      lastDeltaLOCCTR = 0;
      readFirstNonBlankSpace(fileLine, ind, statusCode, label);  
      readFirstNonBlankSpace(fileLine, ind, statusCode, opcode); 
    }
    //*****************************************************************************************

    if (opcode != "END")
    {

      if (SYMTAB[label].exist == "no")
      {
        SYMTAB[label].name = label;
        SYMTAB[label].address = Int_to_hex_str(LOCCTR);
        SYMTAB[label].reltve = 1;
        SYMTAB[label].exist = "yes";
        SYMTAB[label].blocknum = currBlockNum;
      }

      write_to_file(intermediateFile, wrtdataPrefix + to_string(lineNum) + "\t" + Int_to_hex_str(LOCCTR - lastDeltaLOCCTR) + "\t" + to_string(currBlockNum) + "\t" + label + "\t" + opcode);

      getline(sourceFile, fileLine); // Read next line
      lineNum += 5;

      readFirstNonBlankSpace(fileLine, ind, statusCode, label);  // Parse label
      readFirstNonBlankSpace(fileLine, ind, statusCode, opcode); // Parse OPCODE
    }
  }

  if (opcode == "END")
  {
    firstExecSec = SYMTAB[label].address;
    SYMTAB[firstExecSec].name = label;
    SYMTAB[firstExecSec].address = firstExecSec;
  }

  readFirstNonBlankSpace(fileLine, ind, statusCode, operand);
  readFirstNonBlankSpace(fileLine, ind, statusCode, comment, true);

  /*Change to deafult before dumping literals*/
  currblkname = "DEFAULT";
  currBlockNum = 0;
  LOCCTR = Hexstr_to_int(BLOCKS[currblkname].LOCCTR);

  LTORG_put(wrtdataSuffix, lineNumDelta, lineNum, LOCCTR, lastDeltaLOCCTR, currBlockNum);

  wrtdata = to_string(lineNum) + "\t" + Int_to_hex_str(LOCCTR - lastDeltaLOCCTR) + "\t" + " " + "\t" + label + "\t" + opcode + "\t" + operand + "\t" + comment + wrtdataSuffix;
  write_to_file(intermediateFile, wrtdata);

  int LocctrArr[totalBlocks];
  blksnumtoname = new string[totalBlocks];
  for (auto const &it : BLOCKS)
  {
    LocctrArr[it.second.number] = Hexstr_to_int(it.second.LOCCTR);
    blksnumtoname[it.second.number] = it.first;
  }

  for (int i = 1; i < totalBlocks; i++)
  {
    LocctrArr[i] += LocctrArr[i - 1];
  }

  for (auto const &it : BLOCKS)
  {
    if (it.second.startAddress == "?")
    {
      BLOCKS[it.first].startAddress = Int_to_hex_str(LocctrArr[it.second.number - 1]);
    }
  }

  proglen = LocctrArr[totalBlocks - 1] - startAddress;

  sourceFile.close();
  intermediateFile.close();
  errorFile.close();
}
