#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define program 1
#define id 2
#define pv 3
#define var 4
#define dp 5
#define virg 6
#define integer 7
#define tchar 8
#define begin 9
#define end 10
#define affect 11
#define if_ 12
#define then_ 13
#define else_ 14
#define while_ 15
#define do_ 16
#define read_ 17
#define readln_ 18
#define write_ 19
#define writeln_ 20
#define oprel 21
#define opadd 22
#define opmul 23
#define nb 24
#define po 25
#define pf 26
#define point 27
#define fin 0

int symbole;

int symbole_suivant() {
    int s;
    printf("donner le symbole suivant:\n");
    scanf("%d", &s);
    return s;
}

void erreur()
{
	printf("symbole non accepte ! \n");
    exit(1);
}


void accepter(int T) {
    if (symbole == T)
        symbole = symbole_suivant();
    else
        erreur();
}

void Dcl();
void DclPrime();
void Inst_composee();
void Liste_id();
void Liste_idPrime();
void Type();
void Inst();
void Liste_inst();
void Liste_instPrime();
void I();
void Exp();
void ExpPrime();
void Exp_simple();
void Exp_simplePrime();
void Terme();
void TermePrime();
void Facteur();

void P() {
    accepter(program);
    accepter(id);
    accepter(pv);
    Dcl();
    Inst_composee();
    accepter(point);
    printf("Analyse syntaxique terminee avec succes.\n");
    exit(1);
   
}
void Dcl() { DclPrime(); }

void DclPrime() {
    if (symbole == var) {
        accepter(var);
        Liste_id();
        accepter(dp);
        Type();
        accepter(pv);
        DclPrime();
    }
}

void Liste_id() {
    if (symbole == id) {
        accepter(id);
    Liste_idPrime();
    }
}
void Liste_idPrime() {
    if (symbole == virg) {
        accepter(virg);
        accepter(id);
        Liste_idPrime();
    }
 
}

void Type() {
    if (symbole == integer)
        accepter(integer);
    else if (symbole == tchar)
        accepter(tchar);
    else
        erreur();
}

void Inst_composee() {
    accepter(begin);
    Inst();
    accepter(end);
}

void Inst() {
   
    if (symbole == id || symbole == if_ || symbole == while_ ||
        symbole == read_ || symbole == readln_ ||
        symbole == write_ || symbole == writeln_) {
        Liste_inst();
    }
}

void Liste_inst() {
    I();
    Liste_instPrime();
}

void Liste_instPrime() {
    if (symbole == pv) {
        accepter(pv);
        I();
        Liste_instPrime();
    }
   
}

void I() {
    switch (symbole) {
        case id:
            accepter(id);
            accepter(affect);
            Exp_simple();
            break;
        case if_:
            accepter(if_);
            Exp();
            accepter(then_);
            I();
            if (symbole == else_) {
                accepter(else_);
                I();
            }
            break;
        case while_:
            accepter(while_);
            Exp();
            accepter(do_);
            I();
            break;
        case read_:
            accepter(read_);
            accepter(po);
            accepter(id);
            accepter(pf);
            break;
        case readln_:
            accepter(readln_);
            accepter(po);
            accepter(id);
            accepter(pf);
            break;
        case write_:
            accepter(write_);
            accepter(po);
            accepter(id);
            accepter(pf);
            break;
        case writeln_:
            accepter(writeln_);
            accepter(po);
            accepter(id);
            accepter(pf);
            break;
        default:
            erreur();
    }
}

void Exp() {
    Exp_simple();
    ExpPrime();
}

void ExpPrime() {
    if (symbole == oprel) {
        accepter(oprel);
        Exp_simple();
   }
}

void Exp_simple() {
    Terme();
    Exp_simplePrime();
}

void Exp_simplePrime() {
    if (symbole == opadd) {
        accepter(opadd);
        Terme();
        Exp_simplePrime();
    }
   
}

void Terme() {
    Facteur();
    TermePrime();
}

void TermePrime() {
    if (symbole == opmul) {
        accepter(opmul);
        Facteur();
        TermePrime();
    }
}

void Facteur() {
    if (symbole == id)
        accepter(id);
    else if (symbole == nb)
        accepter(nb);
    else if (symbole == po) {
        accepter(po);
        Exp_simple();
        accepter(pf);
    } else
        erreur();
}

int main() {
    printf("donner une unite lexicale ou 0 pour terminer \n");
    scanf("%d", &symbole);
    P();
     
    return 0;
}
