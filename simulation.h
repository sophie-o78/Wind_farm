#ifndef __SIMULATION__H__
#define __SIMULATION__H__

#include "eolienne.h"

#define FIC_NAME_LEN 30
#define DEFAULT_NUMB_OF_DAY 30
#define MAX_STRING_LEN 100

typedef struct nodeSimulation
{
    List *listofeol;
    struct nodeSimulation *next;
} Simulationnode;

typedef struct
{
    int duree; //en jours
    int taille;
    Simulationnode *firstlist;
    Simulationnode *lastlist;

} Simulation;

void freesimulation(Simulation *simulation);              //libération mémoire
Simulation *newsimulation(int simulationtime);            // création liste chaînée
Simulationnode *newsimnode(List *list);                   //création maillon liste chaînée
void appendlist(Simulation *simulation, List *list);      // ajout d'un maillon à partir d'une eol, utilise newlistnode
void deletesimulationnode(Simulation *simulation, int n); // pop une simulation de la liste
void printsimulation(Simulation *simulation);
void advance1step(Simulation *simulation, double tab[3][3]); //avancer la simulation d'un jour
void advanceXstep(Simulation *simulation, int x, double tab[3][3]);// avancer la simulation de X jours
void initsimulation(Simulation *simulation);

double prodofdayn(Simulation *simulation, int n); // production du jour n par  l'ensemble des éoliennes
double avgofdayn(Simulation *simulation, int n);  // moyenne de production d'une éolienne au jour n
double sigmaofdayn(Simulation *simulation, int n);// écart-type au jour n
double totalproduction(Simulation *simulation);   // production totale du parc éolien lors de la simulation
double avgday(Simulation *simulation);            // production moyenne du parc éolien par jour
double sigmaoftotalprod(Simulation *simulation);  // écart-type de la production totale
double totalprodofeoln(Simulation *simulation, int n);// production de l'eol n sur l'ensemble de la simulation
double percentofpart(Simulation *simulation, int n);
int *getlstofrankofeol(Simulation *simulation);     // liste des rangs des éoliennes à la fin de la simulation, ordre croissant
int *getlstofrankofeolafterdayd(Simulation *simulation, int d);//liste des rangs des éoliennes après le jour d
double *getlstofcumulatedprodforeols(Simulation *sim, int d);// liste des productions cumulées de l'ensemble du parc au jour d
int read(char *string, int len);

void menu(Simulation *simulation, double tab[NUMBER_OF_STATE][NUMBER_OF_STATE]);
void printdailyreport(Simulation *simulation, char *filename); //rapport quotidien du parc éolien
void printglobalreport(Simulation *simulation, char *filename);//rapport global du parc éolien
void printfile(char *filename);
void printresultofdayn(char *filename, int n, Simulation *Simulation); //accéder à la ligne du jour n du rapport quotidien

#endif