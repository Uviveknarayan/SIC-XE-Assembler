#include <bits/stdc++.h>
using namespace std;
#define endl '\n'

string expandString(string strinput, int len, char appendchar, bool atend = false)
{
  //returns a string with appendchar's appended at end or front (depending on the value of bool atend.) if the strinput is <length.
  if (atend)
  {
    if (len <= strinput.length())
    {
      return strinput.substr(0, len);
    }
    else
    {
      for (int i = len - strinput.length(); i > 0; i--)
      {
        strinput += appendchar;
      }
    }
  }
  else
  {
    if (len <= strinput.length())
    {
      return strinput.substr(strinput.length() - len, len);
    }
    else
    {
      for (int i = len - strinput.length(); i > 0; i--)
      {
        strinput = appendchar + strinput;
      }
    }
  }
  return strinput;
}
int Hexstr_to_int(string x)
{
  //function to convert a string in hex to integer.
  return stoul(x, nullptr, 16);
}
string Int_to_hex_str(int y, int x = 5)
{
  // exact reverse of Hexstr_to_int function converting an integer to a string in hex.
  stringstream s;
  s << setfill('0') << setw(x) << hex << y;
  string st = s.str();
  st = st.substr(st.length() - x, x);
  transform(st.begin(), st.end(), st.begin(), ::toupper);
  return st;
}
string stringToHexString(const string &input)
{
  //an input string in int is converted to a string in hexadecimal.
  static const char *const lut = "0123456789ABCDEF";
  size_t len = input.length();

  string output;
  output.reserve(2 * len);
  for (size_t i = 0; i < len; ++i)
  {
    const unsigned char c = input[i];
    output.push_back(lut[c >> 4]);
    output.push_back(lut[c & 15]);
  }
  return output;
}
bool check_for_blank(char x)
{
  if (x == ' ' || x == '\t')
  {
    return true;
  }
  return false;
}
bool check_comment(string line)
{
  if (line[0] == '.')
  {
    return true;
  }
  return false;
}

bool if_all_numbers(string x)
{
  bool all_num = true;
  int i = 0;
  while (all_num && (i < x.length()))
  {
    all_num &= isdigit(x[i++]);
  }
  return all_num;
}

string getFinalOp(string opcode)
{
  //function to fetch the opcodes
  if (opcode[0] == '+' || opcode[0] == '@')
  {
    return opcode.substr(1, opcode.length() - 1);
  }
  return opcode;
}
void readFirstNonBlankSpace(string line, int &ind, bool &status, string &strinput, bool readTillTheEnd = false)
{
  strinput = "";
  status = true;
  if (readTillTheEnd)
  {
    strinput = line.substr(ind, line.length() - ind);
    if (strinput == "")
    {
      status = false;
    }
    return;
  }
  while (ind < line.length() && !check_for_blank(line[ind]))
  {
     // If no whitespace is prsent then it is some kind of data
    strinput += line[ind];
    ind++;
  }

  if (strinput == "")
  {
    status = false;
  }

  while (ind < line.length() && check_for_blank(line[ind]))
  { 
    // Increase ind until we skip all whitespace
    ind++;
  }
}
char getFlagFrmt(string input)
{
  if (input[0] == '#' || input[0] == '+' || input[0] == '@' || input[0] == '=')
  {
    return input[0];
  }
  return ' ';
}
void readByteOperand(string line, int &ind, bool &status, string &strinput)
{
  //method to read the byte operand.
  strinput = "";
  status = true;
  if (line[ind] == 'C')
  {
    strinput += line[ind++];
    char identifier = line[ind++];
    strinput += identifier;
    while (ind < line.length() && line[ind] != identifier)
    {
      strinput += line[ind];
      ind++;
    }
    strinput += identifier;
  }
  else
  {
    while (ind < line.length() && !check_for_blank(line[ind]))
    {
      strinput += line[ind];
      ind++;
    }
  }

  if (strinput == "")
  {
    status = false;
  }

  while (ind < line.length() && check_for_blank(line[ind]))
  { 
    // Increase ind to pass all whitespace
    ind++;
  }
}

void write_to_file(ofstream &fileused, string strinput, bool newline = true)
{
  if (newline)
  {
    fileused << strinput << endl;
  }
  else
  {
    fileused << strinput;
  }
}
class EvaluateString
{
public:
  int getRes();
  EvaluateString(string strinput);

private:
  string dataStored;
  int ind;
  char peek();
  int factor();
  int num();
  char get();
  int term();
};

EvaluateString::EvaluateString(string strinput)
{
  dataStored = strinput;
  ind = 0;
}

int EvaluateString::getRes()
{
  int result = term();
  while (peek() == '+' || peek() == '-')
  {
    if (get() == '+')
    {
      result += term();
    }
    else
    {
      result -= term();
    }
  }
  return result;
}

int EvaluateString::term()
{
  int result = factor();
  while (peek() == '*' || peek() == '/')
  {
    if (get() == '*')
    {
      result *= factor();
    }
    else
    {
      result /= factor();
    }
  }
  return result;
}

int EvaluateString::factor()
{
  if (peek() >= '0' && peek() <= '9')
  {
    return num();
  }
  else if (peek() == '(')
  {
    get();
    int result = getRes();
    get();
    return result;
  }
  else if (peek() == '-')
  {
    get();
    return -factor();
  }
  return 0;
}

int EvaluateString::num()
{
  int ans = get() - '0';
  while (peek() >= '0' && peek() <= '9')
  {
    ans = 10 * ans + get() - '0';
  }
  return ans;
}

char EvaluateString::get()
{
  return dataStored[ind++];
}

char EvaluateString::peek()
{
  return dataStored[ind];
}
