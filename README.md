# SIC-XE-Assembler
This repository contains the code for SIC-XE Assembler which works on the SIC-XE assembly language instructions and outputs machine language code in binary. I have included support for program blocks in my assembler and it is written in C++ based on principles we learnt in the course CSN-252 (System Software).

![image](https://github.com/user-attachments/assets/08c20eb8-c411-48db-b7d7-1cdb67f93a78)
## About SIC-XE
**SIC-XE** stands for **Simplified Instructional Computer Extended Version** which is backward compatible with SIC. It was introduced in the book **System Software by LL Beck** for explaining major characteristics and  traits of assembly languages through an exploratory assembly language.
This assembler incorporates the key features of SIC-XE, including program blocks, and is implemented in a traditional two-pass manner:

* Pass 1: Parses the source code, processes variables, and generates the symbol table.
* Pass 2: Translates the parsed assembly instructions into machine code.

### Features
* **Support for literals**: Handles literal values efficiently.
* **Symbol-defining statements**: Processes symbol definitions and expressions.
* **Program blocks**: Allows modular code handling via program blocks.
* **Error handling**: Identifies and logs errors from both passes.

### Input
**Assembly program** in .asm file provided using SIC-XE standard set of instructions.
### Output
1. **Object Program file** containing the header, text, modification and end records.
2. **Intermediate file** containing useful information like addresses of each instruction and variable produced by pass 1 and used by pass 2.
3. **Listing File** as output from pass 2 containing listing of instructions and corresponding object codes.
4. **Error file** containing errors encountered during pass 1 and pass 2.
5. **Tables** file containing all the necessary tables constructed during the passes of assemblers such as Symbol table and Literal table.
### How to run
#### Prerequisites
* A working C++ compiler such as g++.
* Input assembly code in a .asm file.
I have briefly listed how to run the assembler on Windows and Linux/Ubuntu below. The complete instructions to run the assembler and the input and output explanation along with the methodolgy used can be found in this [manual](https://github.com/Uviveknarayan/SIC-XE-Assembler/blob/main/21114108_SIC-XE%20assembler%20Manual.pdf).
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
