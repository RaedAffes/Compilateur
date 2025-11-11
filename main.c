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
int *tokens = NULL;
int token_count = 0;
int pos = 0;
int erreurs[1000];
int nb_erreurs = 0;

int symbole_suivant() {
    if (pos >= token_count) return -1;
    return tokens[pos++];
}

void erreur() {
    erreurs[nb_erreurs++] = pos - 1;
    symbole = symbole_suivant();
}

void accepter(int T) {
    if (symbole == T) symbole = symbole_suivant();
    else erreur();
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
    accepter(id);
    Liste_idPrime();
}

void Liste_idPrime() {
    if (symbole == virg) {
        accepter(virg);
        accepter(id);
        Liste_idPrime();
    }
}

void Type() {
    if (symbole == integer) accepter(integer);
    else if (symbole == car) accepter(car);
    else erreur();
}

void Inst_composee() {
    accepter(begin_);
    Inst();
    accepter(end_);
}

void Inst() {
    if (symbole == id || symbole == if_ || symbole == while_ || symbole == read_ ||
        symbole == readln || symbole == write || symbole == writeln)
        Liste_inst();
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
        case read_:
        case readln:
            accepter(symbole);
            accepter(po);
            accepter(id);
            accepter(pf);
            break;
        case write:
        case writeln:
            accepter(symbole);
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
    if (symbole == id) accepter(id);
    else if (symbole == nb) accepter(nb);
    else if (symbole == po) {
        accepter(po);
        Exp_simple();
        accepter(pf);
    } else erreur();
}

void lire_tokens_fichier(const char *nom) {
    FILE *f = fopen(nom, "r");
    if (!f) {
        perror("Erreur fichier");
        exit(1);
    }
    int val;
    while (fscanf(f, "%d", &val) == 1) {
        tokens = realloc(tokens, sizeof(int) * (token_count + 1));
        tokens[token_count++] = val;
    }
    fclose(f);
}

int main() {
    lire_tokens_fichier("C:\\Users\\Hp\\Desktop\\Compilation\\Analyse_syntaxique\\code.txt");
    symbole = symbole_suivant();
    P();
    if (nb_erreurs == 0) printf("Analyse syntaxique réussie\n");
    else {
        printf("Analyse terminée avec %d erreur(s):\n", nb_erreurs);
        for (int i = 0; i < nb_erreurs; i++)
            printf(" - Erreur à la position %d\n", erreurs[i]);
    }
    free(tokens);
    return 0;
}
