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

/* ==== SEMANTIC ANALYZER STRUCTURES ==== */
typedef struct {
    char name[50];
    int type;      // integer=8 ou chart=9
    int address;   // Adresse mémoire
    int initialized; // 0=non initialisé, 1=initialisé
} SymbolType;

SymbolType symbolTypeTable[100];
int symbolCount = 0;
int nextAddress = 0;

FILE *fp;
FILE *fd;
int nb_id = 0;
int instructionCounter = 1;

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

int currentDataType;
int idsCurrent[20];
int count = 0;

/* ==== SEMANTIC FUNCTIONS ==== */
int findSymbol(char *name) {
    for (int i = 0; i < symbolCount; i++) {
        if (strcmp(symbolTypeTable[i].name, name) == 0)
            return i;
    }
    return -1;
}

void addSymbol(char *name, int type) {
    if (symbolCount < 100) {
        strcpy(symbolTypeTable[symbolCount].name, name);
        symbolTypeTable[symbolCount].type = type;
        symbolTypeTable[symbolCount].address = nextAddress++;
        symbolTypeTable[symbolCount].initialized = 0;
        symbolCount++;
    } else {
        printf("ERREUR SEMANTIQUE : Table des symboles pleine.\n");
        exit(1);
    }
}

void semanticError(const char *message) {
    printf("ERREUR SEMANTIQUE : %s\n", message);
    exit(1);
}

void checkVariableDeclared(char *name) {
    if (findSymbol(name) == -1) {
        printf("ERREUR SEMANTIQUE : Variable '%s' non déclarée\n", name);
        exit(1);
    }
}

void checkVariableInitialized(char *name) {
    int idx = findSymbol(name);
    if (idx == -1) {
        printf("ERREUR SEMANTIQUE : Variable '%s' non déclarée\n", name);
        exit(1);
    }
    if (symbolTypeTable[idx].initialized == 0) {
        printf("AVERTISSEMENT SEMANTIQUE : Variable '%s' utilisée avant initialisation\n", name);
    }
}

void markVariableInitialized(char *name) {
    int idx = findSymbol(name);
    if (idx != -1) {
        symbolTypeTable[idx].initialized = 1;
    }
}

int getVariableType(char *name) {
    int idx = findSymbol(name);
    if (idx == -1) return -1;
    return symbolTypeTable[idx].type;
}

void checkTypeCompatibility(int type1, int type2, const char *operation) {
    if (type1 != type2) {
        printf("ERREUR SEMANTIQUE : Incompatibilité de types dans %s (%s != %s)\n", 
               operation,
               (type1 == integer) ? "integer" : "char",
               (type2 == integer) ? "integer" : "char");
        exit(1);
    }
}

/* ==== CODE GENERATOR ==== */
void emettre(const char* inst, const char* arg) {
    if (fd == NULL) return;
    if (arg != NULL)
        fprintf(fd, "(%d) %s %s\n", instructionCounter++, inst, arg);
    else
        fprintf(fd, "(%d) %s\n", instructionCounter++, inst);
}

void emettreInt(const char* inst, int val) {
    if (fd == NULL) return;
    fprintf(fd, "(%d) %s %d\n", instructionCounter++, inst, val);
}

void emettreCheck(const char* checkType) {
    if (fd == NULL) return;
    fprintf(fd, "(%d) Check_%s\n", instructionCounter++, checkType);
}

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
        default: return "inconnu";
    }
}

void reculer(int k) { fseek(fp, -k, SEEK_CUR); }
int carsuivant() { return fgetc(fp); }

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
            else if (isalpha(car)) { z=0; ch[z++]=(char)car; etat=1; }
            else if (isdigit(car)) { z=0; ch[z++]=(char)car; etat=3; }
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
                return symbole;
            }
            break;

        case 5:
            car = carsuivant();
            if (car=='=') symbole.att = ppe;
            else if (car=='>') symbole.att = dif;
            else { reculer(1); symbole.att = ppq; }
            symbole.ul = oprel;
            return symbole;

        case 9:
            car = carsuivant();
            if (car=='=') symbole.att = pge;
            else { reculer(1); symbole.att = pgq; }
            symbole.ul = oprel;
            return symbole;

        case 23:
            car = carsuivant();
            if (car=='=') symbole.ul = aff;
            else { reculer(1); symbole.ul = dp; }
            return symbole;

        case 50:
            car = carsuivant();
            if (car=='\'') {
                ch[z]='\0';
                symbole.ul = strlit;
                return symbole;
            }
            else ch[z++] = (char)car;
            break;

        case 100:
            return symbole;
        }
    }
}

unilex symbole_suivant() { return analex(); }
void erreur() { printf("ERREUR SYNTAXIQUE !\n"); exit(1); }
void accepter(int t) { if (symbole.ul==t) symbole=symbole_suivant(); else erreur(); }

void P(); void DCL(); void D1(); void L_ID(); void L1(); void TYPE();
void INST_COMP(); void INST(); void L_INST(); void L_INST1();
void I(); int EXP(); int EXP_SIMPLE(); void EXP1(int type1);
int TERME(); void EXP_SIMPLE1(int type1); int TERME1(int type1); int FACTEUR();

void P() {
    if (symbole.ul == program) {
        accepter(program);
        accepter(id);
        accepter(pv);
        DCL();
        INST_COMP();
        accepter(pt);
        emettre("Arret", NULL);

        if (symbole.ul != fin) erreur();

        printf("\n=== COMPILATION REUSSIE ! ===\n");
    }
    else erreur();
}

void DCL() { D1(); }

void D1() {
    if (symbole.ul == var) {
        accepter(var);
        count = 0;
        L_ID();
        accepter(dp);
        TYPE();
        accepter(pv);
        D1();
    }
}

void L_ID() { 
    if (symbole.ul == id) {
        // CONTROLE D'UNICITE
        int index = findSymbol(tab_iden[symbole.att]);
        if (index != -1) {
            printf("ERREUR SEMANTIQUE : Identificateur '%s' déjà déclaré\n", 
                   tab_iden[symbole.att]);
            exit(1);
        }
        
        idsCurrent[count] = symbole.att;
        count++;
        
        accepter(id);
        L1();
    }
    else erreur();
}

void L1() { 
    if (symbole.ul==v) { 
        accepter(v); 
        
        if (symbole.ul == id) {
            // CONTROLE D'UNICITE
            int index = findSymbol(tab_iden[symbole.att]);
            if (index != -1) {
                printf("ERREUR SEMANTIQUE : Identificateur '%s' déjà déclaré\n", 
                       tab_iden[symbole.att]);
                exit(1);
            }
            
            idsCurrent[count] = symbole.att;
            count++;
            
            accepter(id);
            L1();
        }
    } 
}

void TYPE() {
    int type;
    
    if (symbole.ul == integer) {
        type = integer;
        accepter(integer);
    }
    else if (symbole.ul == chart) {
        type = chart;
        accepter(chart);
    }
    else erreur();
    
    for (int i = 0; i < count; i++) {
        addSymbol(tab_iden[idsCurrent[i]], type);
    }
    
    count = 0;
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
        
        // Vérifier qu'on n'est pas à la fin du bloc
        if (symbole.ul != end) {
            I();
            L_INST1();
        }
    }
}

void I() {
    if (symbole.ul == id) {
        int idIndex = symbole.att;
        char *varName = tab_iden[idIndex];
        
        // CONTROLE : Variable déclarée
        checkVariableDeclared(varName);
        int varType = getVariableType(varName);
        
        emettre("Valeurg", varName);
        
        accepter(id);
        accepter(aff);
        
        // Évaluer l'expression et récupérer son type
        int expType = EXP_SIMPLE();
        
        // CONTROLE DE TYPE : Affectation
        checkTypeCompatibility(varType, expType, "affectation");
        
        // Marquer la variable comme initialisée
        markVariableInitialized(varName);
        
        emettre(":=", NULL);
    }
    else if (symbole.ul == iff) {
        static int labelCounter = 1000;
        int elseLabel = labelCounter++;
        int endLabel = labelCounter++;
        
        accepter(iff);
        int condType = EXP();
        
        // CONTROLE DE TYPE : Condition doit être booléenne (ou integer comme booléen)
        if (condType != integer) {
            printf("AVERTISSEMENT : La condition devrait être de type entier\n");
        }
        
        emettre("Not", NULL);
        emettreInt("AllerSi", elseLabel);
        
        accepter(then);
        I();
        
        emettreInt("Aller", endLabel);
        
        emettreInt("Etiq", elseLabel);
        accepter(elsee);
        I();
        
        emettreInt("Etiq", endLabel);
    }
    else if (symbole.ul == whilee) {
        static int whileLabelCounter = 2000;
        int startLabel = whileLabelCounter++;
        int endLabel = whileLabelCounter++;
        
        // DETECTION DE BOUCLE INFINIE (vérification à l'exécution)
        emettreCheck("LoopLimit");
        
        emettreInt("Etiq", startLabel);
        accepter(whilee);
        int condType = EXP();
        
        // CONTROLE DE TYPE : Condition
        if (condType != integer) {
            printf("AVERTISSEMENT : La condition devrait être de type entier\n");
        }
        
        emettre("Not", NULL);
        emettreInt("AllerSi", endLabel);
        
        accepter(do);
        I();
        
        emettreInt("Aller", startLabel);
        
        emettreInt("Etiq", endLabel);
    }
    else if (symbole.ul == read || symbole.ul == readln) {
        accepter(symbole.ul);
        accepter(po);
        
        if (symbole.ul == id) {
            char *varName = tab_iden[symbole.att];
            checkVariableDeclared(varName);
            emettre("Valeurg", varName);
            markVariableInitialized(varName);
            accepter(id);
            emettre("Lire", NULL);
            
            while (symbole.ul == v) {
                accepter(v);
                if (symbole.ul == id) {
                    varName = tab_iden[symbole.att];
                    checkVariableDeclared(varName);
                    emettre("Valeurg", varName);
                    markVariableInitialized(varName);
                    accepter(id);
                    emettre("Lire", NULL);
                }
            }
        }
        
        accepter(pf);
    }
    else if (symbole.ul == write || symbole.ul == writeln) {
        int isWriteln = (symbole.ul == writeln);
        accepter(symbole.ul);
        accepter(po);
        
        if (symbole.ul == id) {
            char *varName = tab_iden[symbole.att];
            checkVariableDeclared(varName);
            checkVariableInitialized(varName);
            emettre("Valeurd", varName);
            accepter(id);
            emettre(isWriteln ? "EcrireLn" : "Ecrire", NULL);
            
            while (symbole.ul == v) {
                accepter(v);
                if (symbole.ul == id) {
                    varName = tab_iden[symbole.att];
                    checkVariableDeclared(varName);
                    checkVariableInitialized(varName);
                    emettre("Valeurd", varName);
                    accepter(id);
                    emettre(isWriteln ? "EcrireLn" : "Ecrire", NULL);
                }
                else if (symbole.ul == strlit) {
                    accepter(strlit);
                    emettre(isWriteln ? "EcrireLn" : "Ecrire", NULL);
                }
            }
        }
        else if (symbole.ul == strlit) {
            accepter(strlit);
            emettre(isWriteln ? "EcrireLn" : "Ecrire", NULL);
        }
        
        accepter(pf);
    }
    else if (symbole.ul == begin) {
        INST_COMP();
    }
    else erreur();
}

int EXP() { 
    int type1 = EXP_SIMPLE();
    EXP1(type1);
    return type1;
}

void EXP1(int type1) { 
    if (symbole.ul == oprel) {
        int op = symbole.att;
        accepter(oprel); 
        int type2 = EXP_SIMPLE();
        
        // CONTROLE DE TYPE : Opérations relationnelles
        checkTypeCompatibility(type1, type2, "opération relationnelle");
        
        switch(op) {
            case ega: emettre("=", NULL); break;
            case dif: emettre("<>", NULL); break;
            case ppq: emettre("<", NULL); break;
            case pgq: emettre(">", NULL); break;
            case ppe: emettre("<=", NULL); break;
            case pge: emettre(">=", NULL); break;
        }
    } 
}

int EXP_SIMPLE() { 
    int type = TERME();
    EXP_SIMPLE1(type);
    return type;
}

void EXP_SIMPLE1(int type1) {
    if (symbole.ul == opadd) {
        int op = symbole.att;
        accepter(opadd);
        int type2 = TERME();
        
        // CONTROLE DE TYPE : Opérations additives
        checkTypeCompatibility(type1, type2, "opération additive");
        
        switch(op) {
            case add: emettre("+", NULL); break;
            case sub: emettre("-", NULL); break;
            case or: emettre("Or", NULL); break;
        }
        
        EXP_SIMPLE1(type1);
    }
}

int TERME() { 
    int type = FACTEUR();
    return TERME1(type);
}

int TERME1(int type1) {
    if (symbole.ul == opmul) {
        int op = symbole.att;
        accepter(opmul);
        int type2 = FACTEUR();
        
        // CONTROLE DE TYPE : Opérations multiplicatives
        checkTypeCompatibility(type1, type2, "opération multiplicative");
        
        switch(op) {
            case mul: emettre("*", NULL); break;
            case divv: 
                // CONTROLE : Division par zéro (vérification à l'exécution)
                emettreCheck("DivByZero");
                emettre("Div", NULL); 
                break;
            case modv: 
                emettreCheck("DivByZero");
                emettre("Mod", NULL); 
                break;
            case andv: emettre("And", NULL); break;
        }
        
        return TERME1(type1);
    }
    return type1;
}

int FACTEUR() {
    if (symbole.ul == id) {
        char *varName = tab_iden[symbole.att];
        
        // CONTROLE : Variable déclarée et initialisée
        checkVariableDeclared(varName);
        checkVariableInitialized(varName);
        
        int varType = getVariableType(varName);
        
        emettre("Valeurd", varName);
        accepter(id);
        
        return varType;
    }
    else if (symbole.ul == nb) {
        emettreInt("Empiler", symbole.att);
        accepter(nb);
        return integer;
    }
    else if (symbole.ul == po) {
        accepter(po);
        int type = EXP_SIMPLE();
        accepter(pf);
        return type;
    }
    else erreur();
    return -1;
}

int main() {
    fp = fopen("example.txt", "r");
    if (!fp) { 
        printf("Erreur: impossible d'ouvrir example.txt\n"); 
        return 1; 
    }
    
    fd = fopen("emetteur.txt", "w");
    if (!fd) { 
        printf("Erreur: impossible de créer emetteur.txt\n"); 
        fclose(fp);
        return 1; 
    }

    printf("\n=== DEBUT DE LA COMPILATION ===\n\n");
    printf("=== ANALYSE SEMANTIQUE ACTIVEE ===\n");
    printf("- Controle d'unicite\n");
    printf("- Controle de type\n");
    printf("- Controle d'initialisation\n");
    printf("- Controle de flot d'execution\n");
    printf("- Verification dynamique (division par 0, boucles)\n\n");
    
    symbole = symbole_suivant();
    P();
    
    printf("\n=== TABLE DES SYMBOLES ===\n");
    for (int i = 0; i < symbolCount; i++) {
        printf("  %s : %s (adresse: %d, init: %s)\n", 
               symbolTypeTable[i].name, 
               (symbolTypeTable[i].type == integer) ? "INTEGER" : "CHAR",
               symbolTypeTable[i].address,
               symbolTypeTable[i].initialized ? "OUI" : "NON");
    }
    
    printf("\n=== Code genere dans emetteur.txt ===\n");

    fclose(fp);
    fclose(fd);
    return 0;
}
