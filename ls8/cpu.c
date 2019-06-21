#include "cpu.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define DATA_LEN 6

/**
 * Load the binary bytes from a .ls8 source file into a RAM array
 */

// Filename should be passed in as an argument
// Implement the cpu_load() function to load an .ls8 file 
void cpu_load(struct cpu *cpu, char *filename)
{
  // TODO: Replace this with something less hard-coded
  FILE *fp = fopen(filename, "r");
  char line[1024];
  unsigned char ram_address = 0x00;

  if (fp == NULL)
  {
    fprintf(stderr, "unable to open file\n");
    exit(2);
  }

  while (fgets(line, sizeof(line), fp) != NULL)
  {
    char *end_pointer;
    unsigned char value = strtoul(line, &end_pointer, 2);

    if (end_pointer == line)
    {
      continue;
    }

    cpu_ram_write(cpu, ram_address++, value);
  }

  fclose(fp);
  
}

unsigned char cpu_ram_read(struct cpu *cpu, unsigned char mar)
{
  return cpu->ram[mar];
}

void cpu_ram_write(struct cpu *cpu, unsigned char mar, unsigned char mdr)
{
  cpu->ram[mar] = mdr;
}

/**
 * ALU
 */
void alu(struct cpu *cpu, enum alu_op op, unsigned char regA, unsigned char regB)
{
  switch (op) {
    case ALU_MUL:
      cpu->reg[regA] *= cpu->reg[regB];
      break;

    // TODO: implement more ALU ops

    case ALU_ADD: //`ADD registerA registerB`
      cpu->reg[regA] += cpu->reg[regB];
      break;
    
  }
}

/**
 * Run the CPU
 */
void cpu_run(struct cpu *cpu)
{
  int running = 1; // True until we get a HLT instruction

  unsigned char IR; 
  unsigned char operandA;
  unsigned char operandB;

  while (running) {

    // TODO
    // 1. Get the value of the current instruction (in address PC).
    IR = cpu_ram_read(cpu, cpu->PC);
    // 2. Figure out how many operands this next instruction requires
    int next_pc = (IR >> 6); 

    if (next_pc == 2) 
    { 
      operandA = cpu_ram_read(cpu, (cpu->PC+1) & 0xff);
      operandB = cpu_ram_read(cpu, (cpu->PC+2) & 0xff);
    }
    // 3. Get the appropriate value(s) of the operands following this instruction
    else if (next_pc == 1) 
    {
      operandA = cpu_ram_read(cpu, (cpu->PC+1) & 0xff);
    }
    // 4. switch() over it to decide on a course of action.
    int instruction_set_pc = (IR >> 4) & 1;

    switch(IR) 
    {
      case HLT:
			  running = 0;
				break;

      case LDI: 
        cpu->reg[operandA] = operandB; 
        break;

      case PRN:
        printf("%d\n", cpu->reg[operandA]);
        break;
      
      case MUL:  //multiply 
        alu(cpu, ALU_MUL, operandA, operandB);
        break;
      
      case ADD:
        alu(cpu, ALU_ADD, operandA, operandB);
        break;
      
      case PUSH:
        cpu->reg[SP]--; 
        cpu->ram[cpu->reg[SP]] = cpu->reg[operandA]; 
        break;
      
      case POP:
        cpu->reg[operandA] = cpu->ram[cpu->reg[SP]]; //Copy the value from the address pointed to by `SP` to the given register.
        cpu->reg[SP]++; 
        break;
      
      case CALL:
      /*Calls a subroutine (function) at the address stored in the register.*/
        cpu->reg[SP]--;
        cpu->ram[cpu->reg[SP]] = cpu->PC + 2;
        cpu->PC = cpu->reg[operandA];
        next_pc = -1;
        break;
 
      case RET:
        cpu->PC = cpu->ram[cpu->reg[SP]];//Pop the value from the top of the stack and store it in the `PC`.
        cpu->reg[SP]++;
        break;

      default:
        printf("unexpected instruction 0x%02x at 0x%02x\n", IR, cpu->PC);
        exit(1);
    }

    // 5. Do whatever the instruction should do according to the spec.
    // 6. Move the PC to the next instruction.
    if (!instruction_set_pc)
    {
      cpu->PC = cpu->PC + next_pc + 1;
    }
  }
}

/**
 * Initialize a CPU struct
 */
void cpu_init(struct cpu *cpu)
{
  // TODO: Initialize the PC and other special registers
  
  //cpu = malloc(sizeof(struct cpu));
  cpu->PC = 0;
  // The SP points at address `F4` if the stack is empty.
  cpu->reg[SP] = 0xF4;
  //cpu->FL = 4;
  memset(cpu->reg, 0, sizeof(cpu->reg));
  memset(cpu->ram, 0, sizeof(cpu->ram));
}
