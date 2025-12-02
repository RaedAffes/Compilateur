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
    int type;  // integer=8 ou chart=9
} SymbolType;

SymbolType symbolTypeTable[100];
int symbolCount = 0;

FILE *fp;
FILE *fd;  // Fichier de sortie pour le transducteur
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

// Variables pour la gestion sémantique
int currentDataType;  // Type actuel lors de la déclaration
int idsCurrent[20];   // IDs en cours de déclaration
int count = 0;        // Nombre d'IDs en cours

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
        symbolCount++;
    } else {
        printf("Erreur : Table des symboles pleine.\n");
        exit(1);
    }
}

void semanticError(const char *message) {
    printf("Erreur sémantique : %s\n", message);
    exit(1);
}

void checkAssignmentCompatibility(int idDataType, int expDataType) {
    if (idDataType != expDataType) {
        printf("Erreur : Incompatibilité de types lors de l'affectation\n");
        exit(1);
    }
}

/* ==== CODE GENERATOR (TRANSDUCTEUR) ==== */
void emettre(const char* instruction) {
    if (fd == NULL) {
        fprintf(stderr, "Erreur : fichier émetteur non initialisé.\n");
        return;
    }
    fprintf(fd, "%s\n", instruction);
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
        emettre("DEBUT_PROGRAMME");
        accepter(program);
        accepter(id);
        accepter(pv);
        DCL();
        INST_COMP();
        accepter(pt);
        emettre("FIN_PROGRAMME");

        if (symbole.ul != fin) erreur();

        printf("\n=== Compilation réussie ! ===\n");
    }
    else erreur();
}

void DCL() { 
    emettre("DEBUT_DECLARATIONS");
    D1(); 
    emettre("FIN_DECLARATIONS");
}

void D1() {
    if (symbole.ul == var) {
        accepter(var);
        count = 0;  // Réinitialiser le compteur
        L_ID();
        accepter(dp);
        TYPE();
        accepter(pv);
        D1();
    }
}

void L_ID() { 
    if (symbole.ul == id) {
        // Vérifier si l'identificateur est déjà déclaré
        int index = findSymbol(tab_iden[symbole.att]);
        if (index != -1) {
            char error[100];
            sprintf(error, "Identificateur '%s' déjà déclaré", tab_iden[symbole.att]);
            semanticError(error);
        }
        
        // Stocker l'ID en attente
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
            // Vérifier si l'identificateur est déjà déclaré
            int index = findSymbol(tab_iden[symbole.att]);
            if (index != -1) {
                char error[100];
                sprintf(error, "Identificateur '%s' déjà déclaré", tab_iden[symbole.att]);
                semanticError(error);
            }
            
            // Stocker l'ID en attente
            idsCurrent[count] = symbole.att;
            count++;
            
            accepter(id);
            L1();
        }
    } 
}

void TYPE() {
    int type;
    char emitBuffer[100];
    
    if (symbole.ul == integer) {
        type = integer;
        accepter(integer);
    }
    else if (symbole.ul == chart) {
        type = chart;
        accepter(chart);
    }
    else erreur();
    
    // Ajouter tous les IDs en attente à la table des symboles
    for (int i = 0; i < count; i++) {
        addSymbol(tab_iden[idsCurrent[i]], type);
        sprintf(emitBuffer, "DECLARE %s : %s", 
                tab_iden[idsCurrent[i]], 
                (type == integer) ? "INTEGER" : "CHAR");
        emettre(emitBuffer);
    }
    
    count = 0;  // Réinitialiser le compteur
}

void INST_COMP() {
    if (symbole.ul == begin) {
        emettre("DEBUT_BLOC");
        accepter(begin);
        INST();
        accepter(end);
        emettre("FIN_BLOC");
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
    char emitBuffer[100];
    
    if (symbole.ul == id) {
        int idIndex = symbole.att;
        
        // Vérifier que la variable est déclarée
        int symIndex = findSymbol(tab_iden[idIndex]);
        if (symIndex == -1) {
            char error[100];
            sprintf(error, "Variable '%s' non déclarée", tab_iden[idIndex]);
            semanticError(error);
        }
        
        sprintf(emitBuffer, "AFFECTATION %s", tab_iden[idIndex]);
        emettre(emitBuffer);
        
        accepter(id);
        accepter(aff);
        EXP_SIMPLE();
        
        // Note: La vérification complète de type nécessiterait 
        // de propager le type de l'expression
    }
    else if (symbole.ul == iff) {
        emettre("DEBUT_IF");
        accepter(iff);
        EXP();
        emettre("IF_THEN");
        accepter(then);
        I();
        emettre("IF_ELSE");
        accepter(elsee);
        I();
        emettre("FIN_IF");
    }
    else if (symbole.ul == whilee) {
        emettre("DEBUT_WHILE");
        accepter(whilee);
        EXP();
        emettre("WHILE_DO");
        accepter(do);
        I();
        emettre("FIN_WHILE");
    }
    else if (symbole.ul == read || symbole.ul == readln) {
        sprintf(emitBuffer, "%s", (symbole.ul == read) ? "READ" : "READLN");
        emettre(emitBuffer);
        accepter(symbole.ul);
        accepter(po);
        ARG_LIST();
        accepter(pf);
    }
    else if (symbole.ul == write || symbole.ul == writeln) {
        sprintf(emitBuffer, "%s", (symbole.ul == write) ? "WRITE" : "WRITELN");
        emettre(emitBuffer);
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
        if (symbole.ul == id) {
            // Vérifier que la variable est déclarée
            int symIndex = findSymbol(tab_iden[symbole.att]);
            if (symIndex == -1) {
                char error[100];
                sprintf(error, "Variable '%s' non déclarée", tab_iden[symbole.att]);
                semanticError(error);
            }
        }
        accepter(symbole.ul);
        while (symbole.ul == v) {
            accepter(v);
            if (symbole.ul == id || symbole.ul == strlit) {
                if (symbole.ul == id) {
                    int symIndex = findSymbol(tab_iden[symbole.att]);
                    if (symIndex == -1) {
                        char error[100];
                        sprintf(error, "Variable '%s' non déclarée", tab_iden[symbole.att]);
                        semanticError(error);
                    }
                }
                accepter(symbole.ul);
            }
            else erreur();
        }
    }
}

void EXP() { 
    emettre("DEBUT_EXPRESSION");
    EXP_SIMPLE(); 
    EXP1(); 
    emettre("FIN_EXPRESSION");
}

void EXP1() { 
    if (symbole.ul == oprel) { 
        char emitBuffer[50];
        sprintf(emitBuffer, "OPERATEUR_RELATIONNEL");
        emettre(emitBuffer);
        accepter(oprel); 
        EXP_SIMPLE(); 
    } 
}

void EXP_SIMPLE() { TERME(); EXP_SIMPLE1(); }

void EXP_SIMPLE1() {
    if (symbole.ul == opadd) {
        emettre("OPERATEUR_ADDITIF");
        accepter(opadd);
        TERME();
        EXP_SIMPLE1();
    }
}

void TERME() { FACTEUR(); TERME1(); }

void TERME1() {
    if (symbole.ul == opmul) {
        emettre("OPERATEUR_MULTIPLICATIF");
        accepter(opmul);
        FACTEUR();
        TERME1();
    }
}

void FACTEUR() {
    char emitBuffer[100];
    
    if (symbole.ul == id) {
        // Vérifier que la variable est déclarée
        int symIndex = findSymbol(tab_iden[symbole.att]);
        if (symIndex == -1) {
            char error[100];
            sprintf(error, "Variable '%s' non déclarée", tab_iden[symbole.att]);
            semanticError(error);
        }
        sprintf(emitBuffer, "LOAD_VAR %s", tab_iden[symbole.att]);
        emettre(emitBuffer);
        accepter(id);
    }
    else if (symbole.ul == nb) {
        sprintf(emitBuffer, "LOAD_CONST %d", symbole.att);
        emettre(emitBuffer);
        accepter(nb);
    }
    else if (symbole.ul == po) {
        emettre("DEBUT_PARENTHESE");
        accepter(po);
        EXP_SIMPLE();
        accepter(pf);
        emettre("FIN_PARENTHESE");
    }
    else erreur();
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

    printf("\n=== DÉBUT DE LA COMPILATION ===\n\n");
    
    symbole = symbole_suivant();
    P();
    
    printf("\n=== TABLE DES SYMBOLES ===\n");
    for (int i = 0; i < symbolCount; i++) {
        printf("  %s : %s\n", 
               symbolTypeTable[i].name, 
               (symbolTypeTable[i].type == integer) ? "INTEGER" : "CHAR");
    }
    
    printf("\n=== Code généré dans emetteur.txt ===\n");

    fclose(fp);
    fclose(fd);
    return 0;
}
