#include "eolienne.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

Eol generateeol(char *name, int state, int numero)
{
    Eol eol;
    strcpy(eol.id, name);
    strcpy(eol.subid, "undef");
    eol.etatvent = state;
    eol.num = numero;
    switch (state)
    {
    case 0:
        eol.production = 0;
        break;
    case 1:
        eol.production = 0.4;
        break;
    case 2:
        eol.production = 0.65;
        break;
    default:
        break;
    }

    return eol;
}

void freelist(List *list)
{

    ListNode *temp = list->first;
    while (list->taille > 0 && temp->next != NULL)
    {
        temp = list->first;
        list->first = list->first->next;
        if (temp != NULL)
        {
            free(temp);
        }
    }
}

/* création d'une nouvelle liste */
List *newlist(void)
{
    List *newlist = malloc(sizeof(List));
    if (newlist == NULL) // on vérifie que le malloc s'est bien passé sinon on quitte le programme
    {
        exit(1);
    }
    newlist->taille = 0;
    newlist->first = NULL; //initialisation des pointeurs, optionnel
    newlist->last = NULL;
    return newlist;
}

ListNode *newlistnode(Eol eol) // sert dans append, crée un noeud à partir d'une personne
{
    ListNode *newlist = malloc(sizeof(ListNode));
    if (newlist == NULL) // verif malloc
    {
        exit(1);
    }
    newlist->next = NULL;
    newlist->eolienne = eol;
    return newlist;
}

void append(List *list, Eol eol) // Ajout d'une personne à la liste, sert dans createlistfromtxt
{
    ListNode *newnode = newlistnode(eol);
    if (list->taille != 0) // l'ajout diffère si la liste est vide
    {
        list->last->next = newnode;
        list->last = newnode;
    }
    else
    {
        list->first = newnode;
        list->last = newnode;
    }
    list->taille += 1;
}

void deleteeol(List *list, int n)
{
    /* Les listes vides sont prises en charge */
    ListNode *temp2 = list->first;
    if (list->taille != 0) //on s'assure que n<list->taille auquel cas si list->taille !=0 on applique le modulo
        n = n % list->taille;
    else
        n = 0;
    if (n != 0 && list->taille > 0)
    {
        ListNode *temp = list->first;
        for (int i = 0; i < n - 1; i++)
        {
            temp = temp->next;
        }
        if (n == list->taille)
        {
            list->last = temp;
        }
        temp2 = temp->next;
        temp->next = temp2->next;
        list->taille--;
    }
    else if (list->taille > 0) //Cas ou le premier élement est à retirer
    {
        list->first = list->first->next;
        list->taille--;
    }
    else
    {
        fprintf(stderr, "List already empty \n");
    }

    if (temp2 != NULL)
    {
        free(temp2);
    }
    temp2 = NULL;
}

void printlist(List *list)
{
    ListNode *temp = list->first;
    while (temp != NULL)
    {
        char name[ID_LEN] = "undef";
        if (strcmp(name, temp->eolienne.subid) == 0)
            printf("%s : %.2f MW \n", temp->eolienne.id, temp->eolienne.production);
        else
        {
            printf("%s aka %s : %.2f MW \n", temp->eolienne.id, temp->eolienne.subid, temp->eolienne.production);
        }

        temp = temp->next;
    }
}

List *copyList(List *list)
{
    List *copylist = newlist();
    ListNode *Point = list->first;
    int i = 0;
    while (Point != NULL)
    {
        Eol neweol = generateeol(Point->eolienne.id, Point->eolienne.etatvent, i);
        strcpy(neweol.subid, Point->eolienne.subid);
        i++;
        append(copylist, Point->eolienne);
        Point = Point->next;
    }
    return copylist;
}

void updateEol(Eol *eoli, double tab[3][3])
{
    int i = eoli->etatvent;
    double random = (double)(rand() % 1000) / 1000;
    if (random < tab[i][0])
    {
        eoli->etatvent = 0;
        eoli->production = 0;
    }
    else if (random > tab[i][0] && random < tab[i][1] + tab[i][0])
    {
        eoli->etatvent = 1;
        eoli->production = 0.4;
    }
    else
    {
        eoli->etatvent = 2;
        eoli->production = 0.65;
    }
}

void updatelist(List *list, double tab[3][3])
{
    ListNode *temp = list->first;
    while (temp != NULL)
    {
        updateEol(&(temp->eolienne), tab);
        temp = temp->next;
    }
}

List *sortList(List *list) //Rmq : renvoit une List * pour éviter de travailler avec des pointeurs de pointeurs
{
    List *copiedlist = copyList(list);
    freelist(list);
    list = newlist();
    while (copiedlist->taille > 0)
    {
        int n = min(copiedlist);
        Eol eol = geteoln(copiedlist, n);
        append(list, eol);
        deleteeol(copiedlist, n);
    }
    return list;
}

int min(List *list)
{
    int i = 0;
    Eol eoli = geteoln(list, i);
    int k = 1;
    ListNode *Point = list->first->next;
    while (Point != NULL)
    {
        Eol eolk = geteoln(list, k);

        if (eolk.production < eoli.production)
        {
            i = k;
            eoli = geteoln(list, i);
        }

        k++;
        Point = Point->next;
    }
    return i;
}

Eol geteoln(List *list, int n)
{
    ListNode *Point = list->first;
    if (n > list->taille)
    {
        if (list->taille != 0)
            n = n % list->taille;
        else
            n = 0;
        n = n % list->taille;
    }
    for (int i = 0; i < n; i++)
    {
        Point = Point->next;
    }
    return Point->eolienne;
}

Eol *getpointoneoln(List *list, int n)
{
    ListNode *Point = list->first;
    if (n > list->taille)
    {
        if (list->taille != 0)
            n = n % list->taille;
        else
            n = 0;
        n = n % list->taille;
    }
    for (int i = 0; i < n; i++)
    {
        Point = Point->next;
    }
    return &(Point->eolienne);
}