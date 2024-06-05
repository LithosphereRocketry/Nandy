/**
 * Proof-of-concept basic LISP implementation for extremely limited systems
*/

#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>

typedef enum {
    TYPE_UNRESOLVED,
    TYPE_NULL,
    TYPE_INT,
    TYPE_PAIR,
    TYPE_SYMBOL,
    TYPE_QUOTE
} lisp_type_t;

typedef struct {
    lisp_type_t type;
    bool dynamic;
    void* data;
} lisp_obj_t;

typedef struct {
    lisp_obj_t car;
    lisp_obj_t cdr;
} lisp_pair_t;

#define NAME_LEN 64

#define STACK_SZ (1<<8)
lisp_obj_t stack[STACK_SZ];
size_t sp = STACK_SZ;

void error(const char* msg) {
    printf("Error! %s\nExiting\n", msg);
    exit(-1);
}

void push_obj(lisp_obj_t obj) {
    if(sp == 0) { error("Out of stack space"); }
    stack[--sp] = obj;
}
lisp_obj_t pop_obj() {
    if(sp == STACK_SZ) { error("Stack underflow"); }
    return stack[sp++];
}

long int parseInt(FILE* infile) {
    long int acc = 0;
    int chr;
    while(isdigit((chr = getc(infile)))) {
        acc *= 10;
        acc += chr - '0';
    }
    ungetc(chr, infile);
    return acc;
}

lisp_obj_t parseInput(FILE* infile, size_t depth) {
    while(1) {
        int next = getc(infile);
        if(isspace(next)) {
            // ignore
        } else if(next == '(') {
            lisp_obj_t newobj = {TYPE_UNRESOLVED, false, NULL};
            push_obj(newobj);
        } else if(next == ')') {
            lisp_obj_t tail = {TYPE_NULL, false, NULL};
            while(1) {
                lisp_obj_t nextobj = pop_obj();
                if(nextobj.type == TYPE_UNRESOLVED) {
                    push_obj(tail);
                    break;
                } else {
                    lisp_pair_t* newpair = malloc(sizeof(lisp_pair_t));
                    newpair->car = nextobj;
                    newpair->cdr = tail;
                    lisp_obj_t newtail = {TYPE_PAIR, true, newpair};
                    tail = newtail;
                }
            }
        } else if(isdigit(next)) {
            long int* i = malloc(sizeof(long int));
            ungetc(next, infile);
            *i = parseInt(infile);
            lisp_obj_t newobj = {TYPE_INT, true, i};
            push_obj(newobj);
        } else if(next == '\'') {
            lisp_obj_t* quoted = malloc(sizeof(lisp_obj_t));
            *quoted = parseInput(infile, STACK_SZ-sp);
            lisp_obj_t newobj = {TYPE_QUOTE, true, quoted};
            push_obj(newobj);
        } else {
            ungetc(next, infile);
            char namebuf[64];
            int i = 0;
            for(; i < NAME_LEN; i++) {
                char c = getc(infile);
                if(isspace(c) || c == '(' || c == ')') {
                    ungetc(c, infile);
                    break;
                }
                namebuf[i] = c;
            }
            if(i == NAME_LEN) {
                error("Name too long");
            } else {
                namebuf[i] = '\0';
                char* name = malloc(i+1);
                strcpy(name, namebuf);
                lisp_obj_t newobj = {TYPE_SYMBOL, true, name};
                push_obj(newobj);
            }
        }

        // If only one thing is left on the stack, and it's not an open-paren
        // placeholder, we've reached the end of the current statement and we
        // can spit out an answer
        if(sp == STACK_SZ-depth-1 && stack[sp].type != TYPE_UNRESOLVED) {
            return pop_obj();
        }
    }
}

void free_obj(lisp_obj_t obj) {
    if(obj.dynamic) {
        if(obj.type == TYPE_PAIR) {
            lisp_pair_t* pair = obj.data;
            free_obj(pair->car);
            free_obj(pair->cdr);
        }
        free(obj.data);
    }
}

void print_obj(lisp_obj_t obj) {
    switch(obj.type) {
        case TYPE_UNRESOLVED: printf("[UNRESOLVED]"); break;
        case TYPE_INT: printf("%li", *(long int*) obj.data); break;
        case TYPE_NULL: printf("()"); break;
        case TYPE_PAIR:
            printf("(");
            for(lisp_pair_t* pair = obj.data; pair; ) {
                print_obj(pair->car);
                switch(pair->cdr.type) {
                    case TYPE_PAIR:
                        printf(" ");
                        pair = pair->cdr.data;
                        break;
                    case TYPE_NULL:
                        printf(")");
                        pair = NULL;
                        break;
                    default:
                        printf(" . ");
                        print_obj(pair->cdr);
                        printf(")");
                        pair = NULL;
                        break;
                }
            }
            break;
        case TYPE_SYMBOL: printf("%s", (char*) obj.data); break;
        case TYPE_QUOTE:
            printf("'");
            print_obj(*(lisp_obj_t*) obj.data);
            break;
    }
}

int main(int argc, char** argv) {
    while(1) {
        printf("> ");
        lisp_obj_t obj = parseInput(stdin, 0);
        print_obj(obj);
        free_obj(obj);
        printf("\n");
    }
}