#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NUMMEMORY 65536
#define NUMREGS 8
#define MAXLINELENGTH 1000

struct inst_t {
  union {
    unsigned int code;

    struct {
      unsigned int destReg : 3;
      unsigned int unused1 : 13;
      unsigned int regB : 3;
      unsigned int regA : 3;
      unsigned int opcode : 3;
      unsigned int unused0 : 7;
    } r;

    struct {
      int offset : 16;
      unsigned int regB : 3;
      unsigned int regA : 3;
      unsigned int opcode : 3;
      unsigned int unused : 7;
    } i;

    struct {
      unsigned int unused1 : 16;
      unsigned int regB : 3;
      unsigned int regA : 3;
      unsigned int opcode : 3;
      unsigned int unused0 : 7;
    } j;

    struct {
      unsigned int unused1 : 22;
      unsigned int opcode : 3;
      unsigned int unused0 : 7;
    } o;
  };
};

typedef struct stateStruct {
  int pc;
  int mem[NUMMEMORY];
  int reg[NUMREGS];
  int numMemory;
} stateType;

void printState(stateType *);

int main(int argc, char *argv[]) {
  char line[MAXLINELENGTH];
  stateType state = { 0, };
  struct inst_t instruction;
  FILE *filePtr;
  int numInstructions;

  if (argc != 2) {
    printf("error: usage: %s <machine-code file>\n", argv[0]);
    exit(1);
  }
  
  filePtr = fopen(argv[1], "r");
  if (filePtr == NULL) {
    printf("error: can't open file %s", argv[1]);
    perror("fopen");
    exit(1);
  }

  /* read in the entire machine-code file into memory */
  for (state.numMemory = 0; fgets(line, MAXLINELENGTH, filePtr) != NULL;
       state.numMemory++) {
    if (sscanf(line, "%d", state.mem + state.numMemory) != 1) {
      printf("error in reading address %d\n", state.numMemory);
      exit(1);
    }
    printf("memory[%d]=%d\n", state.numMemory, state.mem[state.numMemory]);
  }

  numInstructions = 0;
  for (;;) {
    numInstructions++;

    printState(&state);
    instruction.code = state.mem[state.pc++];

    if (instruction.o.opcode == 0b000)
      state.reg[instruction.r.destReg] = state.reg[instruction.r.regA] + state.reg[instruction.r.regB];
    else if (instruction.o.opcode == 0b001)
      state.reg[instruction.r.destReg] = ~(state.reg[instruction.r.regA] | state.reg[instruction.r.regB]);
    else if (instruction.o.opcode == 0b010)
      state.reg[instruction.i.regB] = state.mem[state.reg[instruction.i.regA] + instruction.i.offset];
    else if (instruction.o.opcode == 0b011)
      state.mem[state.reg[instruction.i.regA] + instruction.i.offset] = state.reg[instruction.i.regB];
    else if (instruction.o.opcode == 0b100)
      state.pc += instruction.i.offset * (state.reg[instruction.i.regA] == state.reg[instruction.i.regB]);
    else if (instruction.o.opcode == 0b101)
      state.reg[instruction.j.regB] = state.pc, state.pc = state.reg[instruction.j.regA]; 
    else if (instruction.o.opcode == 0b110)
      break;
    else if (instruction.o.opcode == 0b111)
      ;

  }

  printf("machine halted\n");
  printf("total of %d instructions executed\n", numInstructions);
  printf("final state of machine:\n");
  printState(&state);

  return (0);
}

void printState(stateType *statePtr) {
  int i;
  printf("\n@@@\nstate:\n");
  printf("\tpc %d\n", statePtr->pc);
  printf("\tmemory:\n");
  for (i = 0; i < statePtr->numMemory; i++) {
    printf("\t\tmem[ %d ] %d\n", i, statePtr->mem[i]);
  }
  printf("\tregisters:\n");
  for (i = 0; i < NUMREGS; i++) {
    printf("\t\treg[ %d ] %d\n", i, statePtr->reg[i]);
  }
  printf("end state\n");
}