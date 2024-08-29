# SIC-XE-Assembler
This was a tutorial as part of our CSN-252 course(System Software) in IIT Roorkee.
## About SIC-XE
**SIC-XE** stands for **Simplified Instructional Computer Extended Version** which is backward compatible with SIC. It was introduced in the book **System Software by LL Beck** for explaining major characteristics and  traits of assembly languages through just one language. In my assembler I have aimed to incorporate all characteristics of SIC-XE and Program blocks. The assembler is designed in the conventional two pass manner. The first pass aims to store the variables and populate the symbol table and the second pass then starts translating the assembly code to machine code.

## Implementation
My implementation provides support for literals, symbol defining statements, expressions and program blocks.

### Input
**Assembly program** in .asm file provided using SIC-XE standard set of instructions.
### Output
1. **Object Program file** containing the header, text, modification and end records.
2. **Intermediate file** containing useful information like addresses of each instruction and variable produced by pass 1 and used by pass 2.
3. **Listing File** as output from pass 2 containing listing of instructions and corresponding object codes.
4. **Error file** containing errors encountered during pass 1 and pass 2.
5. **Tables** file containing all the necessary tables constructed during the passes of assemblers such as Symbol table and Literal table.
### How to run
#### Windows
```
1. Run g++ Pass2.cpp
2. ./a.exe
3. Give input file name as input.
```
#### Ubuntu/ Linux distributions
```
1. Run g++ Pass2.cpp
2. ./a.out
3. Give input file name as input.
```
For more detailed instructions refer to this [pdf file](https://github.com/Uviveknarayan/SIC-XE-Assembler/blob/main/21114108_SIC-XE%20assembler%20Manual.pdf).
## Installation

Clone this repository:
```
git@github.com:Uviveknarayan/SIC-XE-Assembler.git
```
## Acknowledgement
I would like to thank [Prof. Manoj Misra](https://www.iitr.ac.in/~CSE/Manoj_Mishra) for providing us this wonderful opportunity to learn about intricacies of an assembler. If you like/ find my work useful, please star this repository. If there are any queries, please raise issues on this repo, I will get back to you as soon as possible.
## Footnotes
1. System Software by Leland L Beck.
2. CSN-252 System Software Course slides by [Prof. Manoj Misra](https://www.iitr.ac.in/~CSE/Manoj_Mishra).
