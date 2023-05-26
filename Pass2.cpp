#include <bits/stdc++.h>
#include "Pass1.cpp"
#define endl '\n'
using namespace std;

ifstream intermediateFile;
ofstream errfile, objfile, ListingFile, printtab;
string op1, op2, stringadd, label, opcode, operand, comment, objectCode, writeData, currRecord, MRecord = "M^", ERecord, currentSecTitle = "DEFAULT";
int line_no, blk_no, address, startAddress, secCntr = 0, programSecLength = 0, PC, BaseRegVal, currTextRecLength;
bool isComment, baseNotUsed;

/************************************************************************************/

string rdUntilTab(string strinput, int &ind)
{
  string buff = "";

  while (ind < strinput.length() && strinput[ind] != '\t')
  {
    buff += strinput[ind];
    ind++;
  }
  ind++;
  if (buff == " ")
  {
    buff = "-1";
  }
  return buff;
}

bool rIntFile(ifstream &readFile, bool &isComment, int &line_no, int &address, int &blk_no, string &label, string &opcode, string &operand, string &comment)
{
  string fileLine = "", buff = "";
  bool tempStatus;
  int ind = 0;

  if (!getline(readFile, fileLine))
  {
    return false;
  }
  line_no = stoi(rdUntilTab(fileLine, ind));

  isComment = (fileLine[ind] == '.') ? true : false;
  if (isComment)
  {
    readFirstNonBlankSpace(fileLine, ind, tempStatus, comment, true);
    return true;
  }

  address = Hexstr_to_int(rdUntilTab(fileLine, ind));
  buff = rdUntilTab(fileLine, ind);

  if (buff == " ")
  {
    blk_no = -1;
  }
  else
  {
    blk_no = stoi(buff);
  }
  label = rdUntilTab(fileLine, ind);
  if (label == "-1")
  {
    label = " ";
  }
  opcode = rdUntilTab(fileLine, ind);
  if (opcode == "BYTE")
  {
    readByteOperand(fileLine, ind, tempStatus, operand);
  }
  else
  {
    operand = rdUntilTab(fileLine, ind);
    if (operand == "-1")
    {
      operand = " ";
    }
  }
  readFirstNonBlankSpace(fileLine, ind, tempStatus, comment, true);
  return true;
}

string ObjcodeFrmt34()
{
  string objcode;
  int halfBytes;
  halfBytes = (getFlagFrmt(opcode) == '+') ? 5 : 3;

  if (getFlagFrmt(operand) == '#')
  { // ITS AN IMMEDIATE
    if (operand.substr(operand.length() - 2, 2) == ",X")
    { 
      writeData = "Line: " + to_string(line_no) + " INDEX BASED ADDRESSING IS NOT SUPPORTED WITH INDIRECT ADDRESSING";
      write_to_file(errfile, writeData);
      objcode = Int_to_hex_str(Hexstr_to_int(OPTAB[getFinalOp(opcode)].opcode) + 1, 2);
      objcode += (halfBytes == 5) ? "100000" : "0000";
      return objcode;
    }

    string tempOpnd = operand.substr(1, operand.length() - 1);

    if (if_all_numbers(tempOpnd) || ((SYMTAB[tempOpnd].exist == "yes") && (SYMTAB[tempOpnd].reltve == 0)))
    { 
      int immediateValue;

      if (if_all_numbers(tempOpnd))
      {
        immediateValue = stoi(tempOpnd);
      }
      else
      {
        immediateValue = Hexstr_to_int(SYMTAB[tempOpnd].address);
      }

      if (immediateValue >= (1 << 4 * halfBytes))
      {
        writeData = "Line: " + to_string(line_no) + " IMMEDIATE VALUE EXCEDES FORMAT LIMIT";
        write_to_file(errfile, writeData);
        objcode = Int_to_hex_str(Hexstr_to_int(OPTAB[getFinalOp(opcode)].opcode) + 1, 2);
        objcode += (halfBytes == 5) ? "100000" : "0000";
      }
      else
      {
        objcode = Int_to_hex_str(Hexstr_to_int(OPTAB[getFinalOp(opcode)].opcode) + 1, 2);
        objcode += (halfBytes == 5) ? '1' : '0';
        objcode += Int_to_hex_str(immediateValue, halfBytes);
      }
      return objcode;
    }
    else if (SYMTAB[tempOpnd].exist == "no")
    {

      if (halfBytes == 3)
      {
        writeData = "Line " + to_string(line_no);
        if (halfBytes == 3)
        {
          writeData += " : INVALID FORMAT FOR EXTERNAL REFERENCE " + tempOpnd;
        }
        else
        {
          writeData += " : SYMBOL NOT FOUND. INSTEAD, FOUND " + tempOpnd;
        }
        write_to_file(errfile, writeData);
        objcode = Int_to_hex_str(Hexstr_to_int(OPTAB[getFinalOp(opcode)].opcode) + 1, 2);
        objcode += (halfBytes == 5) ? "100000" : "0000";
        return objcode;
      }
    }
    else
    {
      int operandAddress = Hexstr_to_int(SYMTAB[tempOpnd].address) + Hexstr_to_int(BLOCKS[blksnumtoname[SYMTAB[tempOpnd].blocknum]].startAddress);

      /*PROCESS IMMEDIATE SINGLE VALUE*/
      if (halfBytes == 5)
      { /*IF FORMAT = 4*/
        objcode = Int_to_hex_str(Hexstr_to_int(OPTAB[getFinalOp(opcode)].opcode) + 1, 2);
        objcode += '1';
        objcode += Int_to_hex_str(operandAddress, halfBytes);

        /*ADDING THE MODIFICATION RECORD*/
        MRecord += "M^" + Int_to_hex_str(address + 1, 6) + '^';
        MRecord += (halfBytes == 5) ? "05" : "03";
        MRecord += '\n';

        return objcode;
      }

      /*FORMAT = 3*/
      PC = address + Hexstr_to_int(BLOCKS[blksnumtoname[blk_no]].startAddress);
      PC += (halfBytes == 5) ? 4 : 3;
      int reltveAddress = operandAddress - PC;

      /*PC ADDRESSING*/
      if (reltveAddress >= (-2048) && reltveAddress <= 2047)
      {
        objcode = Int_to_hex_str(Hexstr_to_int(OPTAB[getFinalOp(opcode)].opcode) + 1, 2);
        objcode += '2';
        objcode += Int_to_hex_str(reltveAddress, halfBytes);
        return objcode;
      }

      /*BASE ADDRESSING*/
      if (!baseNotUsed)
      {
        reltveAddress = operandAddress - BaseRegVal;
        if (reltveAddress >= 0 && reltveAddress <= 4095)
        {
          objcode = Int_to_hex_str(Hexstr_to_int(OPTAB[getFinalOp(opcode)].opcode) + 1, 2);
          objcode += '4';
          objcode += Int_to_hex_str(reltveAddress, halfBytes);
          return objcode;
        }
      }

      /*DIRECT ADDRESSING(WITHOUT PC OR BASE reltve)*/
      if (operandAddress <= 4095)
      {
        objcode = Int_to_hex_str(Hexstr_to_int(OPTAB[getFinalOp(opcode)].opcode) + 1, 2);
        objcode += '0';
        objcode += Int_to_hex_str(operandAddress, halfBytes);

        //ADD MODIFICATION RECORD HERE
        MRecord += "M^" + Int_to_hex_str(address + 1 + Hexstr_to_int(BLOCKS[blksnumtoname[blk_no]].startAddress), 6) + '^';
        MRecord += (halfBytes == 5) ? "05" : "03";
        MRecord += '\n';

        return objcode;
      }
    }
  }
  else if (getFlagFrmt(operand) == '@')
  {
    string tempOpnd = operand.substr(1, operand.length() - 1);
    if (tempOpnd.substr(tempOpnd.length() - 2, 2) == ",X" || SYMTAB[tempOpnd].exist == "no")
    { // ERROR HANDLING FOR INDIRECT ADDRESSING.

      if (halfBytes == 3)
      {
        writeData = "Line " + to_string(line_no);

        writeData += " : SYMBOL NOT FOUND. INDIRECT ADDRESSING DOESN'T SUPPORT INDEX ADDRESSING ";

        write_to_file(errfile, writeData);
        objcode = Int_to_hex_str(Hexstr_to_int(OPTAB[getFinalOp(opcode)].opcode) + 2, 2);
        objcode += (halfBytes == 5) ? "100000" : "0000";
        return objcode;
      }
    }
    int operandAddress = Hexstr_to_int(SYMTAB[tempOpnd].address) + Hexstr_to_int(BLOCKS[blksnumtoname[SYMTAB[tempOpnd].blocknum]].startAddress);
    PC = address + Hexstr_to_int(BLOCKS[blksnumtoname[blk_no]].startAddress);
    PC += (halfBytes == 5) ? 4 : 3;

    if (halfBytes == 3)
    {
      int reltveAddress = operandAddress - PC;
      if (reltveAddress >= (-2048) && reltveAddress <= 2047)
      {
        objcode = Int_to_hex_str(Hexstr_to_int(OPTAB[getFinalOp(opcode)].opcode) + 2, 2);
        objcode += '2';
        objcode += Int_to_hex_str(reltveAddress, halfBytes);
        return objcode;
      }

      if (!baseNotUsed)
      {
        reltveAddress = operandAddress - BaseRegVal;
        if (reltveAddress >= 0 && reltveAddress <= 4095)
        {
          objcode = Int_to_hex_str(Hexstr_to_int(OPTAB[getFinalOp(opcode)].opcode) + 2, 2);
          objcode += '4';
          objcode += Int_to_hex_str(reltveAddress, halfBytes);
          return objcode;
        }
      }

      if (operandAddress <= 4095)
      {
        objcode = Int_to_hex_str(Hexstr_to_int(OPTAB[getFinalOp(opcode)].opcode) + 2, 2);
        objcode += '0';
        objcode += Int_to_hex_str(operandAddress, halfBytes);

        //ADD MODIFICATION RECORD
        MRecord += "M^" + Int_to_hex_str(address + 1 + Hexstr_to_int(BLOCKS[blksnumtoname[blk_no]].startAddress), 6) + '^';
        MRecord += (halfBytes == 5) ? "05" : "03";
        MRecord += '\n';

        return objcode;
      }
    }
    else
    { 
      objcode = Int_to_hex_str(Hexstr_to_int(OPTAB[getFinalOp(opcode)].opcode) + 2, 2);
      objcode += '1';
      objcode += Int_to_hex_str(operandAddress, halfBytes);

      //ADD MODIFICATION RECORD
      MRecord += "M^" + Int_to_hex_str(address + 1 + Hexstr_to_int(BLOCKS[blksnumtoname[blk_no]].startAddress), 6) + '^';
      MRecord += (halfBytes == 5) ? "05" : "03";
      MRecord += '\n';

      return objcode;
    }

    writeData = "Line: " + to_string(line_no);
    writeData += " CAN'T FIT INTO PC OR BASE REGISTER BASED ADDRESSING.";
    write_to_file(errfile, writeData);
    objcode = Int_to_hex_str(Hexstr_to_int(OPTAB[getFinalOp(opcode)].opcode) + 2, 2);
    objcode += (halfBytes == 5) ? "100000" : "0000";

    return objcode;
  }
  else if (getFlagFrmt(operand) == '=')
  { //LITERALS FOUND
    string tempOpnd = operand.substr(1, operand.length() - 1);

    if (tempOpnd == "*")
    {
      tempOpnd = "X'" + Int_to_hex_str(address, 6) + "'";
      //ADD MODIFICATION RECORD
      MRecord += "M^" + Int_to_hex_str(Hexstr_to_int(LITTAB[tempOpnd].address) + Hexstr_to_int(BLOCKS[blksnumtoname[LITTAB[tempOpnd].blocknum]].startAddress), 6) + '^';
      MRecord += Int_to_hex_str(6, 2);
      MRecord += '\n';
    }

    if (LITTAB[tempOpnd].exist == "no")
    {
      writeData = "Line " + to_string(line_no) + " : SYMBOL NOT FOUND. INSTEAD, FOUND " + tempOpnd;
      write_to_file(errfile, writeData);

      objcode = Int_to_hex_str(Hexstr_to_int(OPTAB[getFinalOp(opcode)].opcode) + 3, 2);
      objcode += (halfBytes == 5) ? "000" : "0";
      objcode += "000";
      return objcode;
    }

    int operandAddress = Hexstr_to_int(LITTAB[tempOpnd].address) + Hexstr_to_int(BLOCKS[blksnumtoname[LITTAB[tempOpnd].blocknum]].startAddress);
    PC = address + Hexstr_to_int(BLOCKS[blksnumtoname[blk_no]].startAddress);
    PC += (halfBytes == 5) ? 4 : 3;

    if (halfBytes == 3)
    {
      int reltveAddress = operandAddress - PC;
      if (reltveAddress >= (-2048) && reltveAddress <= 2047)
      {
        objcode = Int_to_hex_str(Hexstr_to_int(OPTAB[getFinalOp(opcode)].opcode) + 3, 2);
        objcode += '2';
        objcode += Int_to_hex_str(reltveAddress, halfBytes);
        return objcode;
      }

      if (!baseNotUsed)
      {
        reltveAddress = operandAddress - BaseRegVal;
        if (reltveAddress >= 0 && reltveAddress <= 4095)
        {
          objcode = Int_to_hex_str(Hexstr_to_int(OPTAB[getFinalOp(opcode)].opcode) + 3, 2);
          objcode += '4';
          objcode += Int_to_hex_str(reltveAddress, halfBytes);
          return objcode;
        }
      }

      if (operandAddress <= 4095)
      {
        objcode = Int_to_hex_str(Hexstr_to_int(OPTAB[getFinalOp(opcode)].opcode) + 3, 2);
        objcode += '0';
        objcode += Int_to_hex_str(operandAddress, halfBytes);

        //ADD MODIFICATION RECORD
        MRecord += "M^" + Int_to_hex_str(address + 1 + Hexstr_to_int(BLOCKS[blksnumtoname[blk_no]].startAddress), 6) + '^';
        MRecord += (halfBytes == 5) ? "05" : "03";
        MRecord += '\n';

        return objcode;
      }
    }
    else
    { 
      objcode = Int_to_hex_str(Hexstr_to_int(OPTAB[getFinalOp(opcode)].opcode) + 3, 2);
      objcode += '1';
      objcode += Int_to_hex_str(operandAddress, halfBytes);

      //ADD MODIFICATION RECORD
      MRecord += "M^" + Int_to_hex_str(address + 1 + Hexstr_to_int(BLOCKS[blksnumtoname[blk_no]].startAddress), 6) + '^';
      MRecord += (halfBytes == 5) ? "05" : "03";
      MRecord += '\n';

      return objcode;
    }

    writeData = "Line: " + to_string(line_no);
    writeData += " CAN'T FIT INTO PC OR BASE REGISTER BASED ADDRESSING.";
    write_to_file(errfile, writeData);
    objcode = Int_to_hex_str(Hexstr_to_int(OPTAB[getFinalOp(opcode)].opcode) + 3, 2);
    objcode += (halfBytes == 5) ? "100" : "0";
    objcode += "000";

    return objcode;
  }
  else
  { 
    int xbpe = 0;
    string tempOpnd = operand;
    if (operand.substr(operand.length() - 2, 2) == ",X")
    {
      tempOpnd = operand.substr(0, operand.length() - 2);
      xbpe = 8;
    }

    if (SYMTAB[tempOpnd].exist == "no")
    {
      
      if (halfBytes == 3)
      { 

        writeData = "Line " + to_string(line_no);

        writeData += " : SYMBOL NOT FOUND. INSTEAD, FOUND " + tempOpnd;

        write_to_file(errfile, writeData);
        objcode = Int_to_hex_str(Hexstr_to_int(OPTAB[getFinalOp(opcode)].opcode) + 3, 2);
        objcode += (halfBytes == 5) ? (Int_to_hex_str(xbpe + 1, 1) + "00") : Int_to_hex_str(xbpe, 1);
        objcode += "000";
        return objcode;
      }
    }
    else
    {

      int operandAddress = Hexstr_to_int(SYMTAB[tempOpnd].address) + Hexstr_to_int(BLOCKS[blksnumtoname[SYMTAB[tempOpnd].blocknum]].startAddress);
      PC = address + Hexstr_to_int(BLOCKS[blksnumtoname[blk_no]].startAddress);
      PC += (halfBytes == 5) ? 4 : 3;

      if (halfBytes == 3)
      {
        int reltveAddress = operandAddress - PC;
        if (reltveAddress >= (-2048) && reltveAddress <= 2047)
        {
          objcode = Int_to_hex_str(Hexstr_to_int(OPTAB[getFinalOp(opcode)].opcode) + 3, 2);
          objcode += Int_to_hex_str(xbpe + 2, 1);
          objcode += Int_to_hex_str(reltveAddress, halfBytes);
          return objcode;
        }

        if (!baseNotUsed)
        {
          reltveAddress = operandAddress - BaseRegVal;
          if (reltveAddress >= 0 && reltveAddress <= 4095)
          {
            objcode = Int_to_hex_str(Hexstr_to_int(OPTAB[getFinalOp(opcode)].opcode) + 3, 2);
            objcode += Int_to_hex_str(xbpe + 4, 1);
            objcode += Int_to_hex_str(reltveAddress, halfBytes);
            return objcode;
          }
        }

        if (operandAddress <= 4095)
        {
          objcode = Int_to_hex_str(Hexstr_to_int(OPTAB[getFinalOp(opcode)].opcode) + 3, 2);
          objcode += Int_to_hex_str(xbpe, 1);
          objcode += Int_to_hex_str(operandAddress, halfBytes);

          //ADD MODIFICATION RECORD HERE
          MRecord += "M^" + Int_to_hex_str(address + 1 + Hexstr_to_int(BLOCKS[blksnumtoname[blk_no]].startAddress), 6) + '^';
          MRecord += (halfBytes == 5) ? "05" : "03";
          MRecord += '\n';

          return objcode;
        }
      }
      else
      { 
        objcode = Int_to_hex_str(Hexstr_to_int(OPTAB[getFinalOp(opcode)].opcode) + 3, 2);
        objcode += Int_to_hex_str(xbpe + 1, 1);
        objcode += Int_to_hex_str(operandAddress, halfBytes);

        //ADD MODIFICATION RECORD HERE
        MRecord += "M^" + Int_to_hex_str(address + 1 + Hexstr_to_int(BLOCKS[blksnumtoname[blk_no]].startAddress), 6) + '^';
        MRecord += (halfBytes == 5) ? "05" : "03";
        MRecord += '\n';

        return objcode;
      }

      writeData = "Line: " + to_string(line_no);
      writeData += " CAN'T FIT INTO PC OR BASE REGISTER BASED ADDRESSING.";
      write_to_file(errfile, writeData);
      objcode = Int_to_hex_str(Hexstr_to_int(OPTAB[getFinalOp(opcode)].opcode) + 3, 2);
      objcode += (halfBytes == 5) ? (Int_to_hex_str(xbpe + 1, 1) + "00") : Int_to_hex_str(xbpe, 1);
      objcode += "000";

      return objcode;
    }
  }
  return "AB";
}

void writeTextRecord(bool lastRecord = false)
{
  if (lastRecord)
  {
    if (currRecord.length() > 0)
    { //LAST TEXT RECORD
      writeData = Int_to_hex_str(currRecord.length() / 2, 2) + '^' + currRecord;
      write_to_file(objfile, writeData);
      currRecord = "";
    }
    return;
  }
  if (objectCode != "")
  {
    if (currRecord.length() == 0)
    {
      writeData = "T^" + Int_to_hex_str(address + Hexstr_to_int(BLOCKS[blksnumtoname[blk_no]].startAddress), 6) + '^';
      write_to_file(objfile, writeData, false);
    }
    // ONJCODE LENGTH > 60
    if ((currRecord + objectCode).length() > 60)
    {
      // CURR RECORD
      writeData = Int_to_hex_str(currRecord.length() / 2, 2) + '^' + currRecord;
      write_to_file(objfile, writeData);

      // NEW TEXT RECORD
      currRecord = "";
      writeData = "T^" + Int_to_hex_str(address + Hexstr_to_int(BLOCKS[blksnumtoname[blk_no]].startAddress), 6) + '^';
      write_to_file(objfile, writeData, false);
    }

    currRecord += objectCode;
  }
  else
  {
    // ASSEMBLY DIRECTIVES DOESN'T NEED MEMORY ALLOCATION/ADDRESS ALLOCATION
    if (opcode == "START" || opcode == "END" || opcode == "BASE" || opcode == "baseNotUsed" || opcode == "LTORG" || opcode == "ORG" || opcode == "EQU")
    {
//do nothing
    }
    else
    {
      // WRITE CURRENT RECORD, IF CURRENT RECORD EXISTS
      if (currRecord.length() > 0)
      {
        writeData = Int_to_hex_str(currRecord.length() / 2, 2) + '^' + currRecord;
        write_to_file(objfile, writeData);
      }
      currRecord = "";
    }
  }
}

void writeERecord(bool write = true)
{
  if (write)
  {
    if (ERecord.length() > 0)
    {
      write_to_file(objfile, ERecord);
    }
    else
    {
      writeERecord(false);
    }
  }
  if ((operand == "" || operand == " ") && currentSecTitle == "DEFAULT")
  { 
    ERecord = "E^" + Int_to_hex_str(startAddress, 6);
  }
  else if (currentSecTitle != "DEFAULT")
  {
    ERecord = "E";
  }
  else
  { 
    int firstExecutableAddress;

    firstExecutableAddress = Hexstr_to_int(SYMTAB[firstExecSec].address);

    ERecord = "E^" + Int_to_hex_str(firstExecutableAddress, 6) + "\n";
  }
}

void pass2()
{
  string buff;
  intermediateFile.open("IntermediateFile_of_" + fileName); 
  if (!intermediateFile)
  {
    cout << "Unable to open file: IntermediateFile_of_" << fileName << endl;
    exit(1);
  }
  getline(intermediateFile, buff); 

  objfile.open("objfile_of_" + fileName);
  if (!objfile)
  {
    cout << "Unable to open file: objfile_of_" << fileName << endl;
    exit(1);
  }

  ListingFile.open("listingfile_of_" + fileName);
  if (!ListingFile)
  {
    cout << "Unable to open file: listingfile_of_" << fileName << endl;
    exit(1);
  }
  write_to_file(ListingFile, "Line\tAddress\tLabel\tOPCODE\tOPERAND\tObjectCode\tComment");

  errfile.open("errfile_of_" + fileName, fstream::app);
  if (!errfile)
  {
    cout << "Unable to open file: errfile_of_" << fileName << endl;
    exit(1);
  }
  write_to_file(errfile, "\n\nERRORS OF PASS2:");
  
  objectCode = "";
  currTextRecLength = 0;
  currRecord = "";
  MRecord = "";
  blk_no = 0;
  baseNotUsed = true;

  rIntFile(intermediateFile, isComment, line_no, address, blk_no, label, opcode, operand, comment);
  while (isComment)
  { 
    writeData = to_string(line_no) + "\t" + comment;
    write_to_file(ListingFile, writeData);
    rIntFile(intermediateFile, isComment, line_no, address, blk_no, label, opcode, operand, comment);
  }

  if (opcode == "START")
  {
    startAddress = address;
    writeData = to_string(line_no) + "\t" + Int_to_hex_str(address) + "\t" + to_string(blk_no) + "\t" + label + "\t" + opcode + "\t" + operand + "\t" + objectCode + "\t" + comment;
    write_to_file(ListingFile, writeData);
  }
  else
  {
    label = "";
    startAddress = 0;
    address = 0;
  }
  programSecLength =proglen;

  writeData = "H^" + expandString(label, 6, ' ', true) + '^' + Int_to_hex_str(address, 6) + '^' + Int_to_hex_str(programSecLength, 6);
  write_to_file(objfile, writeData);

  rIntFile(intermediateFile, isComment, line_no, address, blk_no, label, opcode, operand, comment);
  currTextRecLength = 0;

  while (opcode != "END")
  {
    while (opcode != "END")
    {
      if (!isComment)
      {
        if (OPTAB[getFinalOp(opcode)].exist == "yes")
        {
          if (OPTAB[getFinalOp(opcode)].frmt == 1)
          {
            objectCode = OPTAB[getFinalOp(opcode)].opcode;
          }
          else if (OPTAB[getFinalOp(opcode)].frmt == 2)
          {
            op1 = operand.substr(0, operand.find(','));
            op2 = operand.substr(operand.find(',') + 1, operand.length() - operand.find(',') - 1);

            if (op2 == operand)
            { // If not two operand i.e. a
              if (getFinalOp(opcode) == "SVC")
              {
                objectCode = OPTAB[getFinalOp(opcode)].opcode + Int_to_hex_str(stoi(op1), 1) + '0';
              }
              else if (REGTAB[op1].exist == "yes")
              {
                objectCode = OPTAB[getFinalOp(opcode)].opcode + REGTAB[op1].regnum + '0';
              }
              else
              {
                objectCode = getFinalOp(opcode) + '0' + '0';
                writeData = "Line: " + to_string(line_no) + " INVALID REGISTER NAME";
                write_to_file(errfile, writeData);
              }
            }
            else
            { // Two operands i.e. a,b
              if (REGTAB[op1].exist == "no")
              {
                objectCode = OPTAB[getFinalOp(opcode)].opcode + "00";
                writeData = "Line: " + to_string(line_no) + " INVALID REGISTER NAME";
                write_to_file(errfile, writeData);
              }
              else if (getFinalOp(opcode) == "SHIFTR" || getFinalOp(opcode) == "SHIFTL")
              {
                objectCode = OPTAB[getFinalOp(opcode)].opcode + REGTAB[op1].regnum + Int_to_hex_str(stoi(op2), 1);
              }
              else if (REGTAB[op2].exist == "no")
              {
                objectCode = OPTAB[getFinalOp(opcode)].opcode + "00";
                writeData = "Line: " + to_string(line_no) + " INVALID REGISTER NAME";
                write_to_file(errfile, writeData);
              }
              else
              {
                objectCode = OPTAB[getFinalOp(opcode)].opcode + REGTAB[op1].regnum + REGTAB[op2].regnum;
              }
            }
          }
          else if (OPTAB[getFinalOp(opcode)].frmt == 3)
          {
            if (getFinalOp(opcode) == "RSUB")
            {
              objectCode = OPTAB[getFinalOp(opcode)].opcode;
              objectCode += (getFlagFrmt(opcode) == '+') ? "000000" : "0000";
            }
            else
            {
              objectCode = ObjcodeFrmt34();
            }
          }
        } // If opcode in optab
        else if (opcode == "BYTE")
        {
          if (operand[0] == 'X')
          {
            objectCode = operand.substr(2, operand.length() - 3);
          }
          else if (operand[0] == 'C')
          {
            objectCode = stringToHexString(operand.substr(2, operand.length() - 3));
          }
        }
        else if (label == "*")
        {
          if (opcode[1] == 'C')
          {
            objectCode = stringToHexString(opcode.substr(3, opcode.length() - 4));
          }
          else if (opcode[1] == 'X')
          {
            objectCode = opcode.substr(3, opcode.length() - 4);
          }
        }
        else if (opcode == "WORD")
        {
          objectCode = Int_to_hex_str(stoi(operand), 6);
        }
        else if (opcode == "BASE")
        {
          if (SYMTAB[operand].exist == "yes")
          {
            BaseRegVal = Hexstr_to_int(SYMTAB[operand].address) + Hexstr_to_int(BLOCKS[blksnumtoname[SYMTAB[operand].blocknum]].startAddress);
            baseNotUsed = false;
          }
          else
          {
            writeData = "Line " + to_string(line_no) + " : SYMBOL NOT FOUND. INSTEAD, FOUND " + operand;
            write_to_file(errfile, writeData);
          }
          objectCode = "";
        }
        else if (opcode == "baseNotUsed")
        {
          if (baseNotUsed)
          { // check if assembler was using base addressing
            writeData = "Line " + to_string(line_no) + ": ASSEMBLER NOT USING BASE ADDRESSING";
            write_to_file(errfile, writeData);
          }
          else
          {
            baseNotUsed = true;
          }
          objectCode = "";
        }
        else
        {
          objectCode = "";
        }
        
        writeTextRecord();

        if (blk_no == -1 && address != -1)
        {
          writeData = to_string(line_no) + "\t" + Int_to_hex_str(address) + "\t" + " " + "\t" + label + "\t" + opcode + "\t" + operand + "\t" + objectCode + "\t" + comment;
        }
        else if (address == -1)
        {

          writeData = to_string(line_no) + "\t" + " " + "\t" + " " + "\t" + label + "\t" + opcode + "\t" + operand + "\t" + objectCode + "\t" + comment;
        }

        else
        {
          writeData = to_string(line_no) + "\t" + Int_to_hex_str(address) + "\t" + to_string(blk_no) + "\t" + label + "\t" + opcode + "\t" + operand + "\t" + objectCode + "\t" + comment;
        }
      } 
      else
      {
        writeData = to_string(line_no) + "\t" + comment;
      }
      write_to_file(ListingFile, writeData);                                                           
      rIntFile(intermediateFile, isComment, line_no, address, blk_no, label, opcode, operand, comment); 
      objectCode = "";
    } 
  }   
    writeTextRecord();
    writeERecord(true);
    writeData =  writeData = to_string(line_no) + "\t" + Int_to_hex_str(address) + "\t" + " " + label + "\t" + "END" + "    " + "FIRST";
    write_to_file(ListingFile,writeData);
    
      while (rIntFile(intermediateFile, isComment, line_no, address, blk_no, label, opcode, operand, comment))
      {
        if (label == "*")
        {
          if (opcode[1] == 'C')
          {
            objectCode = stringToHexString(opcode.substr(3, opcode.length() - 4));
          }
          else if (opcode[1] == 'X')
          {
            objectCode = opcode.substr(3, opcode.length() - 4);
          }
          writeTextRecord();
        }
        writeData = to_string(line_no) + "\t" + Int_to_hex_str(address) + "\t" + to_string(blk_no) + label + "\t" + opcode + "\t" + operand + "\t" + objectCode + "\t" + comment;
        write_to_file(ListingFile, writeData);
      }
    

    writeTextRecord(true);
    if (!isComment)
    {

      write_to_file(objfile, MRecord, false); // Write modification record
      writeERecord(true);                                    // Write end record
      currentSecTitle = label;
      MRecord = "";
    }
    
  }
int main()
{
  cout << "File name to take input from:";
  cin >> fileName;

  cout << "\nLoading OPCODE TAB" << endl;
  load_all_tables();

  cout << "\nPass1 is running" << endl;
  cout << "Writing intermediate values to 'IntermediateFile_of_" << fileName << "'" << endl;
  cout << "Writing errors to 'errfile_of_" << fileName << "'" << endl;
  pass1();

  cout << "Writing Symbol Table into tablesfile_of_" + fileName << endl;
  printtab.open("tablesfile_of_" + fileName);
  write_to_file(printtab, "Symbol Table:\n");
  for (auto const &it : SYMTAB)
  {
    stringadd += it.first + ":-\t" + "name:" + it.second.name + "\t|" + "address:" + it.second.address + "\t|" + "reltve:" + Int_to_hex_str(it.second.reltve) + " \n";
  }
  write_to_file(printtab, stringadd);

  stringadd = "";
  cout << "Writing Literal Table to tablesfile_of_" + fileName << endl;

  write_to_file(printtab, "LITERAL TABLE:\n");
  for (auto const &it : LITTAB)
  {
    stringadd += it.first + ":-\t" + "value:" + it.second.value + "\t|" + "address:" + it.second.address + " \n";
  }
  write_to_file(printtab, stringadd);

  cout << "\nPass-2 is running" << endl;
  cout << "Writing objectcode to 'objfile_of_" << fileName << "'" << endl;
  cout << "Listing to 'listingfile_of_" << fileName << "'\n.\n.\n." << endl;
  pass2();
  cout << "Object Program, Listing File, Errors file, Table file are ready." << endl;
}
