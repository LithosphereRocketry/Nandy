/**
 * Proof-of-concept basic LISP implementation for extremely limited systems
*/

#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>

void error(const char* msg) {
    printf("Error! %s\nExiting\n", msg);
    exit(-1);
}

typedef enum {
    TYPE_UNRESOLVED,
    TYPE_NULL,
    TYPE_INT,
    TYPE_PAIR,
    TYPE_SYMBOL,
    TYPE_QUOTE,
    TYPE_PRIMITIVE
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
        case TYPE_PRIMITIVE: printf("<primitive:%p>", obj.data); break;
    }
}

typedef struct {
    const char* name;
    lisp_obj_t value;
} env_mapping_t;

typedef struct env {
    env_mapping_t* mappings;
    struct env* parent;
} env_t;

lisp_obj_t sum(lisp_obj_t* args) {
    long total = 0;
    while(1) {
        if(args->type == TYPE_NULL) {
            break;
        } else if(args->type == TYPE_PAIR) {
            lisp_pair_t* pairptr = args->data;
            if(pairptr->car.type == TYPE_INT) {
                total += *(long*) pairptr->car.data;
                args = &pairptr->cdr;
            } else {
                error("Invalid argument to sum");
            }
        } else {
            error("Invalid argument list to sum");
        }
    }
    lisp_obj_t out = {TYPE_INT, true, NULL};
    out.data = malloc(sizeof(long));
    *(long*) out.data = total;
    return out;
}

env_mapping_t global_mappings[] = {
    {"+", {TYPE_PRIMITIVE, false, sum}},
    {NULL, {}}
};
env_t globals = {global_mappings, NULL};

#define NAME_LEN 64

#define STACK_SZ (1<<8)
lisp_obj_t parse_stack[STACK_SZ];
size_t psp = STACK_SZ;
void ppush(lisp_obj_t obj) {
    if(psp == 0) { error("Out of stack space"); }
    parse_stack[--psp] = obj;
}
lisp_obj_t ppop() {
    if(psp == STACK_SZ) { error("Stack underflow"); }
    return parse_stack[psp++];
}

#define WSTACK_SZ (1<<7)
lisp_obj_t* work_stack[WSTACK_SZ];
size_t wsp = WSTACK_SZ;
void wpush(lisp_obj_t* obj) {
    if(wsp == 0) { error("Out of stack space"); }
    work_stack[--wsp] = obj;
}
lisp_obj_t* wpop() {
    if(wsp == STACK_SZ) { error("Stack underflow"); }
    return work_stack[wsp++];
}

// TODO: this probably also needs to hold environment information, or does it?
#define ESTACK_SZ (1<<7)
lisp_obj_t* exec_stack[ESTACK_SZ];
size_t esp = ESTACK_SZ;
void epush(lisp_obj_t* obj) {
    if(esp == 0) { error("Out of stack space"); }
    exec_stack[--esp] = obj;
}
lisp_obj_t* epop() {
    if(esp == STACK_SZ) { error("Stack underflow"); }
    return exec_stack[esp++];
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

lisp_obj_t scanInput(FILE* infile, size_t depth) {
    while(1) {
        int next = getc(infile);
        if(isspace(next)) {
            // ignore
        } else if(next == '(') {
            lisp_obj_t newobj = {TYPE_UNRESOLVED, false, NULL};
            ppush(newobj);
        } else if(next == ')') {
            lisp_obj_t tail = {TYPE_NULL, false, NULL};
            while(1) {
                lisp_obj_t nextobj = ppop();
                if(nextobj.type == TYPE_UNRESOLVED) {
                    ppush(tail);
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
            ppush(newobj);
        } else if(next == '\'') {
            lisp_obj_t* quoted = malloc(sizeof(lisp_obj_t));
            *quoted = scanInput(infile, STACK_SZ-psp);
            lisp_obj_t newobj = {TYPE_QUOTE, true, quoted};
            ppush(newobj);
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
                ppush(newobj);
            }
        }

        // If only one thing is left on the stack, and it's not an open-paren
        // placeholder, we've reached the end of the current statement and we
        // can spit out an answer
        if(psp == STACK_SZ-depth-1 && parse_stack[psp].type != TYPE_UNRESOLVED) {
            return ppop();
        }
    }
}

lisp_obj_t resolveSymbol(env_t* env, const char* symbol) {
    if(env == NULL) {
        error("Symbol not found or null environment\n");
    }
    for(env_mapping_t* e = env->mappings; e->name; e++) {
        if(strcmp(e->name, symbol) == 0) {
            lisp_obj_t result = e->value;
            result.dynamic = false;
            return result;
        }
    }
    return resolveSymbol(env->parent, symbol);
}

lisp_obj_t executeInput(lisp_obj_t tree) {
    lisp_obj_t* workptr = &tree;
    bool exec = true;
    env_t* env = &globals;
    while(1) {
        if(workptr->type == TYPE_PAIR) {
            if(exec) {
                epush(workptr);
            }
            lisp_pair_t* pairptr = workptr->data;
            wpush(&pairptr->cdr);
            workptr = &pairptr->car;
            exec = true;
        } else {
            if(workptr->type == TYPE_NULL) {
                if(esp >= ESTACK_SZ) {
                    error("Empty list in execution");
                }
                lisp_obj_t* target = epop();
                lisp_pair_t* pairptr = target->data;
                lisp_obj_t newobj;
                switch(pairptr->car.type) {
                    case TYPE_PRIMITIVE:
                        lisp_obj_t (*func)(lisp_obj_t*) = pairptr->car.data;
                        newobj = func(&pairptr->cdr);
                        break;
                    default:
                        error("Tried to execute a non-executable type");
                }
                free_obj(*target);
                *target = newobj;
            } else if(workptr->type == TYPE_SYMBOL) {
                *workptr = resolveSymbol(env, (char*) workptr->data);
            }
            
            if(wsp >= WSTACK_SZ) {
                return tree;
            }
            exec = false;
            workptr = wpop();
        }
    }
}

int main(int argc, char** argv) {
    while(1) {
        printf("> ");
        lisp_obj_t obj = scanInput(stdin, 0);
        lisp_obj_t result = executeInput(obj);
        print_obj(result);
        printf("\n");
        free_obj(result);
    }
}