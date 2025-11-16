#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

/* ==== Définitions des unités lexicales ==== */
#define program 1
#define id 2
#define pt 3
#define pv 4
#define var 5
#define dp 6
#define v 7
#define integer 8
#define chart 9
#define begin 10
#define end 11
#define iff 12
#define whilee 13
#define write 14
#define writeln 15
#define read 16
#define readln 17
#define aff 18
#define po 19
#define pf 20
#define opadd 21
#define opmul 22
#define oprel 23
#define nb 24
#define elsee 25
#define then 26
#define do 27
#define ppe 28
#define dif 29
#define ppq 30
#define pgq 31
#define pge 32
#define ega 33
#define add 34
#define sub 35
#define or 36
#define mul 37
#define divv 38
#define modv 39
#define andv 40
#define comment 41
#define strlit 1000
#define fin 100

typedef struct {
    int ul;
    int att;
} unilex;

FILE *fp;
int nb_id = 0;

int mot_cle[20] = {1, 10, 5, 8, 9, 11, 12, 13, 14, 15, 16, 17, 25, 26, 27};
char tab_mot_cle[20][20] = {
    "program", "begin", "var", "integer", "char", "end", "if", "while",
    "write", "writeln", "read", "readln", "else", "then", "do"
};

char tab_iden[100][50];
int car;
char ch[50];
int z = 0;
unilex symbole;

const char* nomSymbole(int code) {
    switch (code) {
        case program: return "program";
        case id: return "id";
        case pt: return "pt";
        case pv: return "pv";
        case var: return "var";
        case dp: return "dp";
        case v: return "v";
        case integer: return "integer";
        case chart: return "char";
        case begin: return "begin";
        case end: return "end";
        case iff: return "iff";
        case whilee: return "whilee";
        case write: return "write";
        case writeln: return "writeln";
        case read: return "read";
        case readln: return "readln";
        case aff: return ":=";
        case po: return "po";
        case pf: return "pf";
        case opadd: return "opadd";
        case opmul: return "opmul";
        case oprel: return "oprel";
        case nb: return "nb";
        case elsee: return "elsee";
        case then: return "then";
        case do: return "do";
        case ppe: return "<=";
        case dif: return "<>";
        case ppq: return "<";
        case pgq: return ">";
        case pge: return ">=";
        case ega: return "=";
        case add: return "+";
        case sub: return "-";
        case or: return "or";
        case mul: return "*";
        case divv: return "/";
        case modv: return "mod";
        case andv: return "and";
        case comment: return "comment";
        case strlit: return "strlit";
        case fin: return "fin";
        default: return "inconnu";
    }
}

void reculer(int k) { fseek(fp, -k, SEEK_CUR); }

int carsuivant() {
    return fgetc(fp);
}

int unilexid() {
    for (int i = 0; i < 15; i++)
        if (strcmp(tab_mot_cle[i], ch) == 0)
            return mot_cle[i];
    return id;
}

int rangerid(int k, int *c) {
    if (k == id) {
        for (int s = 0; s < *c; s++)
            if (strcmp(tab_iden[s], ch) == 0)
                return s;
        strcpy(tab_iden[*c], ch);
        (*c)++;
        return *c - 1;
    }
    return 0;
}

unilex analex() {
    int etat = 0;
    unilex symbole;

    while (1) {
        switch (etat) {

        case 0:
            car = carsuivant();
            if (car == EOF) { symbole.ul = fin; return symbole; }

            if (car==' ' || car=='\t' || car=='\r' || car=='\n') continue;

            else if (isalpha(car)) { 
                z=0; 
                ch[z++]=(char)car;
                etat=1; 
            }

            else if (isdigit(car)) { 
                z=0; 
                ch[z++]=(char)car;
                etat=3; 
            }

            else if (car=='<') etat=5;
            else if (car=='>' ) etat=9;

            else if (car=='=') { symbole.ul=oprel; symbole.att=ega; etat=100; }
            else if (car=='+') { symbole.ul=opadd; symbole.att=add; etat=100; }
            else if (car=='-') { symbole.ul=opadd; symbole.att=sub; etat=100; }
            else if (car=='*') { symbole.ul=opmul; symbole.att=mul; etat=100; }
            else if (car=='/') { symbole.ul=opmul; symbole.att=divv; etat=100; }
            else if (car=='(') { symbole.ul=po; etat=100; }
            else if (car==')') { symbole.ul=pf; etat=100; }
            else if (car==',') { symbole.ul=v; etat=100; }
            else if (car==':') etat=23;
            else if (car==';') { symbole.ul=pv; etat=100; }
            else if (car=='.') { symbole.ul=pt; etat=100; }
            else if (car=='\'') { z=0; etat=50; }

            break;

        case 1:
            car = carsuivant();
            if (isalpha(car) || isdigit(car))
                ch[z++] = (char)car;
            else {
                reculer(1);
                ch[z] = '\0';
                symbole.ul = unilexid();
                symbole.att = rangerid(symbole.ul, &nb_id);
                printf("TOKEN: %-10s (CODE: %d)\n", nomSymbole(symbole.ul), symbole.ul);
                return symbole;
            }
            break;

        case 3:
            car = carsuivant();
            if (isdigit(car))
                ch[z++] = (char)car;
            else {
                reculer(1);
                ch[z]='\0';
                symbole.ul = nb;
                symbole.att = atoi(ch);
                printf("TOKEN: %-10s (CODE: %d)\n", nomSymbole(symbole.ul), symbole.ul);
                return symbole;
            }
            break;

        case 5:
            car = carsuivant();
            if (car=='=') symbole.att = ppe;
            else if (car=='>') symbole.att = dif;
            else { reculer(1); symbole.att = ppq; }
            symbole.ul = oprel;
            printf("TOKEN: %-10s (att: %d)\n", nomSymbole(symbole.ul), symbole.att);
            return symbole;

        case 9:
            car = carsuivant();
            if (car=='=') symbole.att = pge;
            else { reculer(1); symbole.att = pgq; }
            symbole.ul = oprel;
            printf("TOKEN: %-10s (att: %d)\n", nomSymbole(symbole.ul), symbole.att);
            return symbole;

        case 23:
            car = carsuivant();
            if (car=='=') symbole.ul = aff;
            else { reculer(1); symbole.ul = dp; }
            printf("TOKEN: %-10s (CODE: %d)\n", nomSymbole(symbole.ul), symbole.ul);
            return symbole;

        case 50:
            car = carsuivant();
            if (car=='\'') {
                ch[z]='\0';
                symbole.ul = strlit;
                printf("TOKEN: '%s' (%s)\n", ch, nomSymbole(symbole.ul));
                return symbole;
            }
            else ch[z++] = (char)car;
            break;

        case 100:
            printf("TOKEN: %-10s (CODE: %d)\n", nomSymbole(symbole.ul), symbole.ul);
            return symbole;
        }
    }
}

unilex symbole_suivant() { return analex(); }
void erreur() { printf("Erreur syntaxique !\n"); exit(1); }
void accepter(int t) { if (symbole.ul==t) symbole=symbole_suivant(); else erreur(); }

void P(); void DCL(); void D1(); void L_ID(); void L1(); void TYPE();
void INST_COMP(); void INST(); void L_INST(); void L_INST1();
void I(); void EXP(); void EXP_SIMPLE(); void EXP1();
void TERME(); void EXP_SIMPLE1(); void TERME1(); void FACTEUR();
void ARG_LIST();

void P() {
    if (symbole.ul == program) {
        accepter(program);
        accepter(id);
        accepter(pv);
        DCL();
        INST_COMP();
        accepter(pt);

        if (symbole.ul != fin) erreur();

        printf("Compilation réussie !\n");
    }
    else erreur();
}

void DCL() { D1(); }

void D1() {
    if (symbole.ul == var) {
        accepter(var);
        L_ID();
        accepter(dp);
        TYPE();
        accepter(pv);
        D1();
    }
}

void L_ID() { accepter(id); L1(); }
void L1() { if (symbole.ul==v) { accepter(v); accepter(id); L1(); } }

void TYPE() {
    if (symbole.ul == integer) accepter(integer);
    else if (symbole.ul == chart) accepter(chart);
    else erreur();
}

void INST_COMP() {
    if (symbole.ul == begin) {
        accepter(begin);
        INST();
        accepter(end);
    }
    else erreur();
}

void INST() { L_INST(); }

void L_INST() { I(); L_INST1(); }

void L_INST1() {
    if (symbole.ul == pv) {
        accepter(pv);
        I();
        L_INST1();
    }
}

void I() {
    if (symbole.ul == id) {
        accepter(id);
        accepter(aff);
        EXP_SIMPLE();
    }
    else if (symbole.ul == iff) {
        accepter(iff);
        EXP();
        accepter(then);
        I();
        accepter(elsee);
        I();
    }
    else if (symbole.ul == whilee) {
        accepter(whilee);
        EXP();
        accepter(do);
        I();
    }
    else if (symbole.ul == read || symbole.ul == readln) {
        accepter(symbole.ul);
        accepter(po);
        ARG_LIST();
        accepter(pf);
    }
    else if (symbole.ul == write || symbole.ul == writeln) {
        accepter(symbole.ul);
        accepter(po);
        ARG_LIST();
        accepter(pf);
    }
    else if (symbole.ul == begin) {
        INST_COMP();
    }
    else erreur();
}

void ARG_LIST() {
    if (symbole.ul == id || symbole.ul == strlit) {
        accepter(symbole.ul);
        while (symbole.ul == v) {
            accepter(v);
            if (symbole.ul == id || symbole.ul == strlit)
                accepter(symbole.ul);
            else erreur();
        }
    }
}

void EXP() { EXP_SIMPLE(); EXP1(); }
void EXP1() { if (symbole.ul == oprel) { accepter(oprel); EXP_SIMPLE(); } }

void EXP_SIMPLE() { TERME(); EXP_SIMPLE1(); }

void EXP_SIMPLE1() {
    if (symbole.ul == opadd) {
        accepter(opadd);
        TERME();
        EXP_SIMPLE1();
    }
}

void TERME() { FACTEUR(); TERME1(); }

void TERME1() {
    if (symbole.ul == opmul) {
        accepter(opmul);
        FACTEUR();
        TERME1();
    }
}

void FACTEUR() {
    if (symbole.ul == id) accepter(id);
    else if (symbole.ul == nb) accepter(nb);
    else if (symbole.ul == po) {
        accepter(po);
        EXP_SIMPLE();
        accepter(pf);
    }
    else erreur();
}

int main() {
    fp = fopen("example.txt", "r");
    if (!fp) { printf("Erreur: impossible d'ouvrir example.txt\n"); return 1; }

    symbole = symbole_suivant();
    P();

    fclose(fp);
    return 0;
}