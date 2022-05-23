#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAXINSTRUCTION 1024
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

struct {
  struct {
    char label[MAXINSTRUCTION];
    int addr;
  } labels[MAXINSTRUCTION];
  int numLabels;
} labelTable;

int readAndParse(FILE *, char *, char *, char *, char *, char *);
int isNumber(char *);
int findLabelAddress(const char *label);
int firstPass(FILE *inFilePtr);
int secondPass(FILE *inFilePtr, FILE *outFilePtr);

int main(int argc, char *argv[]) {
  char *inFileString, *outFileString;
  FILE *inFilePtr, *outFilePtr;
  char label[MAXLINELENGTH], opcode[MAXLINELENGTH], arg0[MAXLINELENGTH],
      arg1[MAXLINELENGTH], arg2[MAXLINELENGTH];

  if (argc != 3) {
    printf("error: usage: %s <assembly-code-file> <machine-code-file>\n",
           argv[0]);
    exit(1);
  }

  inFileString = argv[1];
  outFileString = argv[2];
  inFilePtr = fopen(inFileString, "r");

  if (inFilePtr == NULL) {
    printf("error in opening %s\n", inFileString);
    exit(1);
  }

  outFilePtr = fopen(outFileString, "w");

  if (outFilePtr == NULL) {
    printf("error in opening %s\n", outFileString);
    exit(1);
  }

  firstPass(inFilePtr);

  rewind(inFilePtr);

  secondPass(inFilePtr, outFilePtr);

  exit(0);

  return (0);
}

int readAndParse(FILE *inFilePtr, char *label, char *opcode, char *arg0,
                 char *arg1, char *arg2)
/* Read and parse a line of the assembly-language file. Fields are returned in
label, opcode, arg0, arg1, arg2 (these strings must have memory already
allocated to them). Return values: 0 if reached end of file 1 if all went well
exit(1) if line is too long. */
{
  char line[MAXLINELENGTH];
  char *ptr = line;
  /* delete prior values */
  label[0] = opcode[0] = arg0[0] = arg1[0] = arg2[0] =
      '\0'; /* read the line from the assembly-language file */
  if (fgets(line, MAXLINELENGTH, inFilePtr) == NULL) {
    /* reached end of file */
    return (0);
  }
  /* check for line too long (by looking for a \n) */
  if (strchr(line, '\n') == NULL) {
    /* line too long */
    printf("error: line too long\n");
    exit(1);
  }
  /* is there a label? */
  ptr = line;
  if (sscanf(ptr, "%[^\t\n\r ]", label)) {
    /* successfully read label; advance pointer over the label */
    ptr += strlen(label);
  }
  /*
   * Parse the rest of the line. Would be nice to have real regular
   * expressions, but scanf will suffice. */
  sscanf(ptr,
         "%*[\t\n\r ]%[^\t\n\r ]%*[\t\n\r ]%[^\t\n\r ]%*[\t\n\r ]%[^\t\n\r "
         "]%*[\t\n\r ]%[^\t\n\r ]",
         opcode, arg0, arg1, arg2);
  return (1);
}

int isNumber(char *string) {
  /* return 1 if string is a number */
  int i;
  return ((sscanf(string, "%d", &i)) == 1);
}

int findLabelAddress(const char *label) {
  for (int i = 0; i < labelTable.numLabels; i++) {
    if (!strcmp(label, labelTable.labels[i].label)) {
      return labelTable.labels[i].addr;
    }
  }
  return -1;
}

int firstPass(FILE *inFilePtr) {
  char label[MAXLINELENGTH], opcode[MAXLINELENGTH], arg0[MAXLINELENGTH],
      arg1[MAXLINELENGTH], arg2[MAXLINELENGTH];
  for (int currentAddr = 0;
       readAndParse(inFilePtr, label, opcode, arg0, arg1, arg2);
       currentAddr++) {
    if (strlen(label) > 0) {
      if (findLabelAddress(label) >= 0) {
        printf("error: duplicate labels\n");
        printf("%s\n", label);
        exit(1);
      }
      strncpy(labelTable.labels[labelTable.numLabels].label, label,
              MAXINSTRUCTION);
      labelTable.labels[labelTable.numLabels].addr = currentAddr;
      labelTable.numLabels++;
    }
  }
}

struct inst_t rTypeInstruction(int opcode, char *regA, char *regB,
                               char *destReg) {
  struct inst_t instruction = {
      0,
  };
  instruction.r.opcode = opcode;
  instruction.r.regA = atoi(regA);
  instruction.r.regB = atoi(regB);
  instruction.r.destReg = atoi(destReg);
  return instruction;
}

struct inst_t iTypeInstruction(int opcode, char *regA, char *regB,
                               char *offset, int currentAddr) {
  struct inst_t instruction = {
      0,
  };
  if (atoi(offset) < -0x00008000 || atoi(offset) >= 0x00008000) {
    printf("error: offsetFields that don't fit in 16 bits\n");
    printf("%s\n", offset);
    exit(1);
  }
  instruction.i.opcode = opcode;
  instruction.i.regA = atoi(regA);
  instruction.i.regB = atoi(regB);
  if (isNumber(offset)) {
    instruction.i.offset = atoi(offset);
  } else {
    if (findLabelAddress(offset) < 0) {
      printf("error: undefined labels\n");
      printf("%s\n", offset);
      exit(1);
    }
    if (opcode == 0b100) {
      instruction.i.offset = findLabelAddress(offset) - (currentAddr+1);
    } else {
      instruction.i.offset = findLabelAddress(offset);
    }
  }
  return instruction; 
}

struct inst_t jTypeInstruction(int opcode, char *regA, char *regB) {
  struct inst_t instruction = {
      0,
  };
  instruction.j.opcode = opcode;
  if (isNumber(regA)) {
    instruction.j.regA = atoi(regA);
  } else {
    if (findLabelAddress(regA) < 0) {
      printf("error: undefined labels\n");
      printf("%s\n", regA);
      exit(1);
    }
    instruction.j.regA = findLabelAddress(regA);
  }
  if (isNumber(regB)) {
    instruction.j.regB = atoi(regB);
  } else {
    if (findLabelAddress(regB) < 0) {
      printf("error: undefined labels\n");
      printf("%s\n", regB);
      exit(1);
    }
    instruction.j.regB = findLabelAddress(regB);
  }
  return instruction;
}

struct inst_t oTypeInstruction(int opcode) {
  struct inst_t instruction = {
      0,
  };
  instruction.o.opcode = opcode;
  return instruction;
}

int fillValue(char *field) {
  int value;
  if (isNumber(field)) {
    value = atoi(field);
  } else {
    if (findLabelAddress(field) < 0) {
      printf("error: undefined labels\n");
      printf("%s\n", field);
      exit(1);
    }
    value = findLabelAddress(field);
  }
  return value;
}

int secondPass(FILE *inFilePtr, FILE *outFilePtr) {
  char label[MAXLINELENGTH], opcode[MAXLINELENGTH], arg0[MAXLINELENGTH],
      arg1[MAXLINELENGTH], arg2[MAXLINELENGTH];
  char line[MAXLINELENGTH];
  char *ptr = line;
  struct inst_t instruction;

  label[0] = opcode[0] = arg0[0] = arg1[0] = arg2[0] = '\0';

  for (int currentAddr = 0;
       readAndParse(inFilePtr, label, opcode, arg0, arg1, arg2);
       currentAddr++) {
    if (currentAddr) {
      fputs("\n", outFilePtr);
    }

    if (!strcmp(opcode, ".fill")) {
      fprintf(outFilePtr, "%d", fillValue(arg0));
    } else {
      if (!strcmp(opcode, "add"))
        instruction = rTypeInstruction(0b000, arg0, arg1, arg2);
      else if (!strcmp(opcode, "nor"))
        instruction = rTypeInstruction(0b001, arg0, arg1, arg2);
      else if (!strcmp(opcode, "lw"))
        instruction = iTypeInstruction(0b010, arg0, arg1, arg2, currentAddr);
      else if (!strcmp(opcode, "sw"))
        instruction = iTypeInstruction(0b011, arg0, arg1, arg2, currentAddr);
      else if (!strcmp(opcode, "beq"))
        instruction = iTypeInstruction(0b100, arg0, arg1, arg2, currentAddr);
      else if (!strcmp(opcode, "jalr"))
        instruction = jTypeInstruction(0b101, arg0, arg1);
      else if (!strcmp(opcode, "halt"))
        instruction = oTypeInstruction(0b110);
      else if (!strcmp(opcode, "noop"))
        instruction = oTypeInstruction(0b111);
      else {
        printf("error: unrecognized opcodes\n");
        printf("%s\n", opcode);
        exit(1);
      }
      fprintf(outFilePtr, "%d", instruction.code);
    }
  }
}