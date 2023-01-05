
#ifndef __EOLIENNE__H__
#define __EOLIENNE__H__

#define ID_LEN 30
#define NUMBER_OF_EOL 20
#define NUMBER_OF_STATE 3

/* Utilisation du type Bool sans import de bibliothèque, remplace false et true par 0 et 1 dans le code */
enum Bool
{
    false,
    true
};

/* type eolienne */
typedef struct
{
    char subid[ID_LEN];
    int etatvent;
    char id[ID_LEN];
    double production;
    int num;
} Eol;

/*Elément d'une liste chainée */
typedef struct ListElement
{
    struct ListElement *next;
    Eol eolienne;
} ListNode;

/* définition d'une Liste */
typedef struct
{
    struct ListElement *first; //first et last suffisent pour définir une liste
    struct ListElement *last;
    int taille; // Pour vérifications en cas de liste nulle
} List;

Eol generateeol(char *name, int state, int numero);
void freelist(List *list);         //libération mémoire
List *newlist(void);               // création liste chaînée
ListNode *newlistnode(Eol eol);    //création maillon liste chaînée
void append(List *list, Eol eol);  // ajout d'un maillon à partir d'une eil, utilise newlistnode
void deleteeol(List *list, int n); // pop une eolienne de la liste
void printlist(List *list);        // liste des éoliennes
List *copyList(List *list);
void updateEol(Eol *eoli, double tab[3][3]); //nouvel état de fonctionnement de l'eolienne selon V transition des vents
void updatelist(List *list, double tab[3][3]); 
List *sortList(List *list);
int min(List *list);
Eol geteoln(List *list, int n);     //accéder à l'éolienne n
Eol *getpointoneoln(List *list, int n); //pointer directement sur l'éolienne n

#endif