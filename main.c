#include <stdio.h>
#include <stdlib.h>

#define ADD 0
#define SUB 1
#define LSF 2
#define RSF 3
#define AND 4
#define OR  5
#define XOR 6
#define LHI 7
#define LD 8
#define ST 9
#define JLT 16
#define JLE 17
#define JEQ 18
#define JNE 19
#define JIN 20
#define HLT 24

#define MEM_SIZE_BITS	(16)
#define MEM_SIZE	(1 << MEM_SIZE_BITS)
#define MEM_MASK	(MEM_SIZE - 1)

int mem[MEM_SIZE] = {0};
int reg[8] = {0};
int PC = 0;
int counter = 0;
FILE *input, *output, *memDump;
char *filename;

typedef struct Instruction {
	int opcode;
	int dst, src0, src1;
	int immediate;
	char* name;
} Instruction;

void setInstName(Instruction *inst);

Instruction parseInst() {
	int inst = mem[PC];

	Instruction iinst;

	iinst.immediate = 0xffff & inst;

	inst >>= 16;
	iinst.src1 = inst & 0x7;

	inst >>= 3;
	iinst.src0 = inst & 0x7;

	inst >>= 3;
	iinst.dst = inst & 0x7;

	inst >>= 3;
	iinst.opcode = inst & 0x1f;

	setInstName(&iinst);

	return iinst;
}

void execute(Instruction inst) {
	int op1, op2;
	op1 = (inst.src0 == 1 ? inst.immediate : reg[inst.src0]);
	op2 = (inst.src1 == 1 ? inst.immediate : reg[inst.src1]);


	switch (inst.opcode) {

	case ADD:
		reg[inst.dst] = op1 + op2;
		fprintf(output, ">>>> EXEC: R[%d] = %d ADD %d <<<<\n\n", inst.dst, op1, op2);
		break;

	case SUB:
		reg[inst.dst] = op1 - op2;
		fprintf(output, ">>>> EXEC: R[%d] = %d SUB %d <<<<\n\n", inst.dst, op1, op2);
		break;

	case LSF:
		reg[inst.dst] = op1 << op2;
		fprintf(output, ">>>> EXEC: R[%d] = %d LSF %d <<<<\n\n", inst.dst, op1, op2);
		break;

	case RSF:
		reg[inst.dst] = op1 >> op2;
		fprintf(output, ">>>> EXEC: R[%d] = %d RSF %d <<<<\n\n", inst.dst, op1, op2);
		break;

	case AND:
		reg[inst.dst] = op1 & op2;
		fprintf(output, ">>>> EXEC: R[%d] = %d AND %d <<<<\n\n", inst.dst, op1, op2);
		break;

	case OR:
		reg[inst.dst] = op1 | op2;
		fprintf(output, ">>>> EXEC: R[%d] = %d OR %d <<<<\n\n", inst.dst, op1, op2);
		break;

	case XOR:
		reg[inst.dst] = op1 ^ op2;
		fprintf(output, ">>>> EXEC: R[%d] = %d XOR %d <<<<\n\n", inst.dst, op1, op2);
		break;

	case LHI:
		reg[inst.dst] = (inst.immediate << 16) | (reg[inst.dst] & 0xffff);
		fprintf(output, ">>>> EXEC: R[%d] = %d <<<<\n\n", inst.dst, reg[inst.dst]);
		break;

	case LD:
		reg[inst.dst] = mem[op2 & 0xffff];
		fprintf(output, ">>>> EXEC: R[%d] = MEM[%d] = %08x <<<<\n\n", inst.dst, op2 & 0xffff, mem[op2 & 0xffff]);
		break;

	case ST:
		mem[op2 & 0xffff] = op1;
		fprintf(output, ">>>> EXEC: MEM[%d] = R[%d] = %08x <<<<\n\n", op2 & 0xffff, inst.src0, op1);
		break;

	case JLT:
		if (op1 < op2) {
			reg[7] = (PC == 0 ? 0xffff : PC - 1);
			PC = inst.immediate;
		}
		fprintf(output , ">>>> EXEC: JLT %d, %d, %d <<<<\n\n", op1, op2, PC);
		break;

	case JLE:
		if (op1 <= op2) {
			reg[7] = (PC == 0 ? 0xffff : PC - 1);
			PC = inst.immediate;
		}
		fprintf(output, ">>>> EXEC: JLE %d, %d, %d <<<<\n\n", op1, op2, PC);
		break;

	case JEQ:
		if (op1 == op2) {
			reg[7] = (PC == 0 ? 0xffff : PC - 1);
			PC = inst.immediate;
		}
		fprintf(output, ">>>> EXEC: JEQ %d, %d, %d <<<<\n\n", op1, op2, PC);
		break;

	case JNE:
		if (op1 != op2) {
			reg[7] = (PC == 0 ? 0xffff : PC - 1);
			PC = inst.immediate;
		}
		fprintf(output, ">>>> EXEC: JNE %d, %d, %d <<<<\n\n", op1, op2, PC);
		break;

	case JIN:
		reg[7] = (PC == 0 ? 0xffff : PC - 1);
		PC = op1;
		fprintf(output, ">>>> EXEC: JIN %d <<<<\n\n", PC);
		break;

	}
}

void writeTrace1(FILE *output, Instruction *inst) {


	fprintf(output, "--- instruction %d (%04x) @ PC %d (%04x) -----------------------------------------------------------\n"
	"pc = %04x, inst = %08x, opcode = %d (%s), dst = %d, src0 = %d, src1 = %d, immediate = %08x\n"
	"r[0] = %08x r[1] = %08x r[2] = %08x r[3] = %08x \n"
	"r[4] = %08x r[5] = %08x r[6] = %08x r[7] = %08x \n\n",
		counter, counter,
		PC, PC, PC,
		mem[PC],
		inst->opcode, inst->name, inst->dst, inst->src0, inst->src1, inst->immediate,
		reg[0], inst->immediate, reg[2], reg[3], reg[4], reg[5], reg[6], reg[7]);


}

void setInstName(Instruction *inst){

	switch (inst->opcode) {

		case ADD:
			inst->name = "ADD";
			break;

		case SUB:
			inst->name = "SUB";
			break;

		case LSF:
			inst->name = "LSF";
			break;

		case RSF:
			inst->name = "RSF";
			break;

		case AND:
			inst->name = "AND";
			break;

		case OR:
			inst->name = "OR";
			break;

		case XOR:
			inst->name = "XOR";
			break;

		case LHI:
			inst->name = "LHI";
			break;

		case LD:
			inst->name = "LD";
			break;

		case ST:
			inst->name = "ST";
			break;

		case JLT:
			inst->name = "JLT";
			break;

		case JLE:
			inst->name = "JLE";
			break;

		case JEQ:
			inst->name = "JEQ";
			break;

		case JNE:
			inst->name = "JNE";
			break;

		case JIN:
			inst->name = "JIN";
			break;

		case HLT:
			inst->name = "HLT";
			break;

		}
}

void writeMemDump() {
	int i;

	for (i = 0; i < MEM_SIZE; i++) {
		fprintf(memDump, "%08x\n", mem[i]);
	}
}

void run() {
	Instruction inst;

	while (1) {
		inst = parseInst();

		writeTrace1(output, &inst);	// before


		if (inst.opcode == HLT) {
			fprintf(output, ">>>> EXEC: HALT at PC %04x<<<<\n", PC);
			fprintf(output, "sim finished at pc %d, %d instructions\n", PC, counter + 1);
			break;
		}


		if (PC == 0xffff) {
			PC = 0;
		}
		else {
			PC++;
		}

		execute(inst); // after

		//fprintf(output, ">>>> EXEC: %s <<<<\n\n", inst.toString);

		counter++;

	}
}



void readMem() {
	input = fopen(filename, "r");
	if (input == NULL) {
		printf("couldn't open file %s\n", filename);
		exit(1);
	}

	char line[9] = {0};

	int addr = 0;
	while (fgets(line, 10, input) != NULL) {
		mem[addr] = strtol(line, NULL, 16);
		addr++;

	}

	fprintf(output, "program %s loaded, %d lines\n\n", filename, addr);
	fclose(input);
}

int main(int argc, char *argv[]){

	output = fopen("trace.txt", "w+");
	if (output == NULL) {
		printf("couldn't open file trace.txt\n");
		exit(1);
	}
	memDump = fopen("sram_out.txt", "w+");
	if (memDump == NULL) {
		printf("couldn't open file sram_out.txt\n");
		exit(1);
	}

	reg[0] = 0;
	reg[1] = 0xf;
	filename = argv[1];

	readMem();
	run();

	writeMemDump();

	fclose(output);
	fclose(memDump);
	return 0;
}
