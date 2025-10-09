#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define program 1
#define id 2
#define pv 3
#define var 4
#define dp 5
#define integer 6
#define car 7
#define begin_ 8
#define end_ 9
#define affect 10
#define if_ 11
#define then 12
#define else_ 13
#define while_ 14
#define do_ 15
#define read_ 16
#define readln 17
#define write 18
#define writeln 19
#define oprel 20
#define opadd 21
#define opmul 22
#define nb 23
#define po 24
#define pf 25
#define point 26
#define virg 27
#define opsub 28
int symbole;
int tokens[] = {
    program, id, pv,                   // program complexe;
    var, id, virg, id, dp, integer, pv,  // var n, s : integer;
    begin_,                            // begin
        id, affect, nb, pv,           // s := 0;
        read_, po, id, pf, pv,        // read(n);
        while_, id, oprel, nb, do_,   // while n > 0 do
            begin_,                   // begin
                id, affect, id, opadd, id, pv,  // s := s + n;
                id, affect, id, opsub, nb, pv,  // n := n - 1  ← CORRIGÉ ICI
            end_                      // end
    end_,                             // end
    point                             // .
};
int pos = 0;

int symbole_suivant() {
    if (pos >= sizeof(tokens)/sizeof(tokens[0])) {
        return -1;
    }
    return tokens[pos++];
}

void erreur() {
    printf("Erreur syntaxique ! Symbole inattendu %d à la position %d\n", symbole, pos-1);
    exit(1);
}

void accepter(int T) {
    if (symbole == T) {
        symbole = symbole_suivant();
    } else {
        erreur();
    }
}

// Déclarations CORRECTES selon la grammaire
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

// P → program id ; Dcl Inst_composée .
void P() {
    accepter(program);
    accepter(id);
    accepter(pv);
    Dcl();
    Inst_composee();
    accepter(point);
    printf("Analyse syntaxique réussie\n");
}

// Dcl → ε Dcl‘
void Dcl() {
    DclPrime();  // Commence directement par Dcl'
}

// Dcl‘ → var Liste_id : Type ; Dcl‘ | ε
void DclPrime() {
    if (symbole == var) {
        accepter(var);
        Liste_id();
        accepter(dp);
        Type();
        accepter(pv);
        DclPrime();
    }
    // ε
}

// Liste_id → id Liste_id‘
void Liste_id() {
    accepter(id);
    Liste_idPrime();
}

// Liste_id‘ → , id Liste_id‘ | ε
void Liste_idPrime() {
    if (symbole == virg) {
        accepter(virg);
        accepter(id);
        Liste_idPrime();
    }
    // ε
}

void Type() {
    if (symbole == integer) {
        accepter(integer);
    } else if (symbole == car) {
        accepter(car);
    } else {
        erreur();
    }
}

// Inst_composée → begin Inst end
void Inst_composee() {
    accepter(begin_);
    Inst();
    accepter(end_);
}

// Inst → Liste_inst | ε
void Inst() {
    if (symbole == id || symbole == if_ || symbole == while_ ||
        symbole == read_ || symbole == readln || symbole == write || symbole == writeln) {
        Liste_inst();
    }
    // ε
}

// Liste_inst → I Liste_inst‘
void Liste_inst() {
    I();
    Liste_instPrime();
}

// Liste_inst‘ → ; I Liste_inst‘ | ε
void Liste_instPrime() {
    if (symbole == pv) {
        accepter(pv);
        I();
        Liste_instPrime();
    }
    // ε
}

void I() {
    switch(symbole) {
        case id:
            accepter(id);
            accepter(affect);
            Exp_simple();
            break;
        case if_:
            accepter(if_);
            Exp();
            accepter(then);
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
        case read_: case readln:
            accepter(symbole);
            accepter(po);
            accepter(id);
            accepter(pf);
            break;
        case write: case writeln:
            accepter(symbole);
            accepter(po);
            accepter(id);
            accepter(pf);
            break;
        default:
            erreur();
    }
}

// Exp → Exp_simple Exp‘
void Exp() {
    Exp_simple();
    ExpPrime();
}

// Exp‘ → oprel Exp_simple | ε
void ExpPrime() {
    if (symbole == oprel) {
        accepter(oprel);
        Exp_simple();
    }
    // ε
}

// Exp_simple → Terme Exp_simple‘
void Exp_simple() {
    Terme();
    Exp_simplePrime();
}

// Exp_simple‘ → opadd Terme Exp_simple‘ | ε
void Exp_simplePrime() {
    if (symbole == opadd) {
        accepter(opadd);
        Terme();
        Exp_simplePrime();
    }
    // ε
}

// Terme → Facteur Terme‘
void Terme() {
    Facteur();
    TermePrime();
}

// Terme‘ → opmul Facteur Terme‘ | ε
void TermePrime() {
    if (symbole == opmul) {
        accepter(opmul);
        Facteur();
        TermePrime();
    }
    // ε
}

void Facteur() {
    if (symbole == id) {
        accepter(id);
    } else if (symbole == nb) {
        accepter(nb);
    } else if (symbole == po) {
        accepter(po);
        Exp_simple();
        accepter(pf);
    } else {
        erreur();
    }
}

int main() {
    symbole = symbole_suivant();
    P();
    return 0;
}
