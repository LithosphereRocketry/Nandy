#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "argparse.h"
#include "shuntingyard.h"
#include "nandy_emu_tools.h"
#include "nandy_instr_defs.h"
#include "nandy_parse_tools.h"
#include "micros.h"

argument_t arg_debug = { .abbr = 'g', .name = "debug", .hasval = false };
argument_t arg_forcedebug = { .abbr = 'G', .name = "force-debug", .hasval = false };
argument_t arg_out = { .abbr = 'o', .name = "out", .hasval = true };
argument_t arg_timeacc = { .abbr = 't', .name = "time-accurate", .hasval = false };

argument_t* args[] = {
    &arg_debug,
    &arg_forcedebug,
    &arg_out,
    &arg_timeacc
};
const size_t n_args = sizeof(args) / sizeof(argument_t*);

cpu_state_t state;

const instruction_t* disasm_cache[65536];

void scanDisasm(cpu_state_t* state, addr_t start) {
    // If the disassembly is already done, don't bother
    if(disasm_cache[start]) return;
    int len = nbytes(peek(state, start));
    const instruction_t* instr = ilookup(peek(state, start));
    if(instr) {
        disasm_cache[start] = instr;
        // Delete any disassemblies of instructions that are now part of the
        // data field of this instructions
        // This loop is super unnecessary but it supports longer instructions
        for(int i = 1; i < len; i++) { disasm_cache[start + i] = NULL; }
        // These instructions are completely intractable to the disassembler
        // because their jump target is runtime-variable
        if(instr != &i_ja && instr != &i_jar && instr != &i_jri) {
            // TODO: how do we scan targets of relative jumps without creating
            // infinite loops of misaligned instructions fighting with each
            // other
            if(instr == &i_jcz) {
                scanDisasm(state, start + len);
                // TODO: scan jump target
            } else if(instr == &i_j) {
                // TODO: scan jump target
            } else {
                scanDisasm(state, start + len);
            }
        }
    }
}

void printDebug(cpu_state_t* state) {
    /** Layout should look like this:
PC  0x123F  CARRY 1         FF      ...
ACC 0x3F    SP  0x3F        FF      rd io
DX  0x3F    DY  0x3F        FF      sw dy
IRX 0x3F    IRY 0x3F        FF      add dx
IN  0x3F    OUT 0x3F        FF  PC> addi 6
IE  1       INT 1           FF      jri
                            FF      nop
Cycles 1234567890       SP> FF      nop 
    */
    static char linebuf[8][40];
    for(int i = 0; i < 8; i++) {
    }
    addr_t pcdis = state->pc;
    int pos = 0;
    for(int i = 0; i < 4; i++) {
        if(disasm_cache[pcdis - 2] && nbytes(peek(state, pcdis - 2)) == 2) {
            pos --;
            pcdis -= 2;
        } else if(disasm_cache[pcdis - 1]) {
            pos --;
            pcdis --;
        } else {
            break;
        }
    }
    for(int i = -4; i < 4; i++) {
        if(i >= pos && disasm_cache[pcdis]) {
            disasm_cache[pcdis]->disassemble(disasm_cache[pcdis],
                    state, pcdis, linebuf[i + 4], 40);
            pcdis += nbytes(peek(state, pcdis));
        } else {
            addr_t fallback_ind;
            if(i < pos) {
                fallback_ind = pcdis + i;
            } else {
                fallback_ind = pcdis;
                pcdis ++;
            }
            snprintf(linebuf[i + 4], 40, "??? (0x%.2hhx)", peek(state, fallback_ind));
        }
    }

    printf("\nPC  0x%04hx  CARRY %c         %02hhx      %-40s\n",
            state->pc, state->carry ? '1' : '0', peek(state, 0xFF00 + (uint8_t) state->sp + 7), linebuf[0]);
    printf("ACC 0x%02hhx    SP  0x%02hhx        %02hhx      %-40s\n",
            state->acc, state->sp, peek(state, 0xFF00 + (uint8_t) state->sp + 6), linebuf[1]);
    printf("DX  0x%02hhx    DY  0x%02hhx        %02hhx      %-40s\n",
            state->dx, state->dy, peek(state, 0xFF00 + (uint8_t) state->sp + 5), linebuf[2]);
    printf("IRX 0x%02hhx    IRY 0x%02hhx        %02hhx      %-40s\n",
            state->irx, state->iry, peek(state, 0xFF00 + (uint8_t) state->sp + 4), linebuf[3]);
    printf("IN  0x%02hhx    OUT 0x%02hhx        %02hhx  PC> %-40s\n",
            state->ioin, state->ioout, peek(state, 0xFF00 + (uint8_t) state->sp + 3), linebuf[4]);
    printf("IE  %c       INT %c           %02hhx      %-40s\n",
            state->int_en ? '1' : '0', state->int_active ? '1' : '0', peek(state, 0xFF00 + (uint8_t) state->sp + 2), linebuf[5]);
    printf("IOA 0x%02hhx    IOS %c%c          %02hhx      %-40s\n",
            state->ioaddr, state->io_rd ? 'R' : '-', state->io_wr ? 'W' : '-',
            peek(state, 0xFF00 + (uint8_t) state->sp + 1), linebuf[6]);
    char cyclesbuf[17];
    snprintf(cyclesbuf, 17, "%lu", state->elapsed);
    printf("Cycles %-16s SP> %02hhx      %-40s\n",
            cyclesbuf, peek(state, 0xFF00 | state->sp), linebuf[7]);
}

bool debug(cpu_state_t* state) {
    scanDisasm(state, state->pc);
    printDebug(state);
    while(1) {

        printf("DEBUG> ");
        char input[256];
        fgets(input, 255, stdin);
        if(input[0] == '\n') {
            // On empty line, reprint the display
            return debug(state);
        }
        char cmd[64];
        int scanlen;
        sscanf(input, "%63s%n", cmd, &scanlen);
        if(!strcmp(cmd, "quit") || !strcmp(cmd, "q")) {
            return false;
        } else if(!strcmp(cmd, "step") || !strcmp(cmd, "s")) {
            state->idbg = true;
            return true;
        } else if(!strcmp(cmd, "continue") || !strcmp(cmd, "c")) {
            state->idbg = false;
            return true;
        } else if(!strcmp(cmd, "io") || !strcmp(cmd, "i")) {
            int64_t io;
            if(parseExp(NULL, input+scanlen, &io, stdout) != SHUNT_DONE) {
                printf("Failed to parse value\n");
            } else if(!isBounded(io, 8, BOUND_EITHER)) {
                printf("Value does not fit in word\n");
            } else {
                state->ioin = (word_t) io;
                return debug(state);
            }
        } else if(!strcmp(cmd, "peek") || !strcmp(cmd, "e")) {
            int64_t addr;
            if(parseExp(NULL, input+scanlen, &addr, stdout) != SHUNT_DONE) {
                printf("Failed to parse address\n");
            } else if(!isBounded(addr, 16, BOUND_UNSIGNED)) {
                printf("Address is out of bounds\n");
            } else {
                printf("0x%hhx\n", peek(state, (addr_t) addr));
            }
        } else if(!strcmp(cmd, "goto") || !strcmp(cmd, "g")) {
            int64_t addr;
            if(parseExp(NULL, input+scanlen, &addr, stdout) != SHUNT_DONE) {
                printf("Failed to parse address\n");
            } else if(!isBounded(addr, 16, BOUND_UNSIGNED)) {
                printf("Address is out of bounds\n");
            } else {
                state->pc = addr;
                scanDisasm(state, state->pc);
            }
        } else if(!strcmp(cmd, "poke") || !strcmp(cmd, "o")) {
            char* div = strchr(input+scanlen, ',');
            if(div == NULL) {
                printf("Format: poke <addr>, <value>\n");
            } else {
                *div = '\0';
                int64_t addr;
                if(parseExp(NULL, input+scanlen, &addr, stdout) != SHUNT_DONE) {
                    printf("Failed to parse address\n");
                } else if(!isBounded(addr, 16, BOUND_UNSIGNED)) {
                    printf("Address is out of bounds\n");
                } else {
                    int64_t val;
                    if(parseExp(NULL, div+1, &val, stdout) != SHUNT_DONE) {
                        printf("Failed to parse value\n");
                    } else if(!isBounded(val, 8, BOUND_EITHER)) {
                        printf("Value does not fit in word\n");
                    } else {
                        poke(state, (addr_t) addr, (word_t) val);
                    }
                }
            }
        } else {
            printf("Unrecognized command \"%s\"\n", cmd);
        }
    }
}

// Weird little block here for Unix signal support
#ifdef __unix__
#include <signal.h>
void onInterrupt() {
    state.idbg = true;
}
struct sigaction act = { .sa_handler = onInterrupt };
#endif

int main(int argc, char** argv) {
    argc = argparse(args, n_args, argc, argv);
    if(argc <= 0) return -1;
    if(argc != 2) {
        printf("Wrong number of args\n");
        return -1;
    }
    
    FILE* f = fopen(argv[1], "rb");
    if(!f) {
        printf("File %s not found\n", argv[1]);
        return -1;
    }

    FILE* fout = stdout;
    if(arg_out.result.value) {
        fout = fopen(arg_out.result.value, "wb");
        if(!fout) {
            printf("File %s not found\n", arg_out.result.value);
            return -1;
        }
    }

    state = INIT_STATE;
    fread(state.rom, sizeof(word_t), ADDR_RAM_MASK, f);

#ifdef __unix__
    sigaction(SIGINT, &act, NULL);
#endif

    setbuf(stdin, NULL);
    micros_init();
    long lastMicros = 0;
    do {
        if(arg_forcedebug.result.present) {
            if(!debug(&state)) { break; }
        }
        lastMicros = micros(); // Don't count the time spent in the debugger as
                               // execution time
        do {
            scanDisasm(&state, state.pc);

            // Stall for realistic execution times
            if(arg_timeacc.result.present && state.elapsed % 1000 == 0) {
                while(micros() - lastMicros < 1000);
                // really this should use a system delay call probably
                lastMicros += 1000;
            }
        } while(!emu_step(&state, fout));
        if(arg_debug.result.present) {
            if(!debug(&state)) { break; }
        }
    } while(arg_debug.result.present || arg_forcedebug.result.present);

    printf("Complete, executed %li cycles\n", state.elapsed);

    fclose(f);
    if(arg_out.result.value) { // technically it's probably harmless to close
            // stdout here but it makes me sad
        fclose(fout);
    }
}