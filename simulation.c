#include "eolienne.h"
#include "simulation.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

void freesimulation(Simulation *simulation)
{

    Simulationnode *temp = simulation->firstlist;
    while (simulation->taille > 0 && temp->next != NULL)
    {
        temp = simulation->firstlist;
        simulation->firstlist = simulation->firstlist->next;
        if (temp != NULL)
        {
            freelist(temp->listofeol);
            free(temp);
        }
    }
}

/* création d'une nouvelle liste */
Simulation *newsimulation(int simulationtime)
{
    Simulation *newsim = malloc(sizeof(*newsim));
    if (newsim == NULL) // on vérifie que le malloc s'est bien passé sinon on quitte le programme
    {
        exit(1);
    }
    newsim->taille = 0;
    newsim->duree = simulationtime;
    newsim->firstlist = NULL; //initialisation des pointeurs, optionnel
    newsim->lastlist = NULL;
    return newsim;
}

Simulationnode *newsimnode(List *list) // sert dans append, crée un noeud à partir d'une personne
{
    Simulationnode *newlist = malloc(sizeof(*newlist));
    if (newlist == NULL) // verif malloc
    {
        exit(1);
    }
    newlist->next = NULL;
    newlist->listofeol = list;
    return newlist;
}

void appendlist(Simulation *simulation, List *list) // Ajout d'une personne à la liste, sert dans createlistfromtxt
{
    Simulationnode *newnode = newsimnode(list);
    if (simulation->taille != 0) // l'ajout diffère si la liste est vide
    {
        simulation->lastlist->next = newnode;
        simulation->lastlist = newnode;
    }
    else
    {
        simulation->firstlist = newnode;
        simulation->lastlist = newnode;
    }
    simulation->taille += 1;
}

void deletesimulationnode(Simulation *simulation, int n)
{
    /* Les listes vides sont prises en charge */
    Simulationnode *temp2 = simulation->firstlist;
    if (simulation->taille != 0) //on s'assure que n<list->taille auquel cas si list->taille !=0 on applique le modulo
        n = n % simulation->taille;
    else
        n = 0;
    if (n != 0 && simulation->taille > 0)
    {
        Simulationnode *temp = simulation->firstlist;
        for (int i = 0; i < n - 1; i++)
        {
            temp = temp->next;
        }
        if (n == simulation->taille)
        {
            simulation->lastlist = temp;
        }
        temp2 = temp->next;
        temp->next = temp2->next;
        simulation->taille--;
    }
    else if (simulation->taille > 0) //Cas ou le premier élement est à retirer
    {
        simulation->firstlist = simulation->firstlist->next;
        simulation->taille--;
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

List *getdayn(Simulation *sim, int n)
{
    Simulationnode *Point = sim->firstlist;
    if (n > sim->taille)
    {
        if (sim->taille != 0)
            n = n % sim->taille;
        else
            n = 0;
        n = n % sim->taille;
    }
    for (int i = 0; i < n; i++)
    {
        Point = Point->next;
    }
    return Point->listofeol;
}

void printsimulation(Simulation *simulation)
{
    Simulationnode *temp = simulation->firstlist;
    int i = 0;
    while (temp != NULL)
    {

        i++;
        printf("----------\n");
        printlist(temp->listofeol);
        printf("----------\n");
        temp = temp->next;
    }
}

void advance1step(Simulation *simulation, double tab[3][3])
{

    List *newlist = copyList(simulation->lastlist->listofeol);
    ListNode *temp = newlist->first;
    int i = 0;
    while (temp != NULL)
    {

        sprintf((temp->eolienne).id, "Eolienne N %d [%d]", i, simulation->taille);
        temp = temp->next;
        i++;
    }
    updatelist(newlist, tab);
    appendlist(simulation, newlist);
}

void advanceXstep(Simulation *simulation, int x, double tab[3][3])
{
    for (int i = 0; i < x && i < simulation->duree - 1; i++)
    {
        advance1step(simulation, tab);
    }
}

void initsimulation(Simulation *simulation)
{

    char commonid[ID_LEN];
    List *eoliennes = newlist();
    for (int j = 0; j < NUMBER_OF_EOL; j++)
    {

        sprintf(commonid, "Eolienne N %d [%d]", j, 0);
        Eol eol = generateeol(commonid, 0, j);
        append(eoliennes, eol);
    }
    appendlist(simulation, eoliennes);
}

double prodofdayn(Simulation *simulation, int n)
{
    Simulationnode *temp = simulation->firstlist;
    List *list = getdayn(simulation, n);
    ListNode *temp2 = list->first;
    double prod = 0;
    while (temp2 != NULL)
    {
        prod = prod + temp2->eolienne.production;
        temp2 = temp2->next;
    }
    return prod;
}

double increaseofdayn(Simulation *simulation, int n)
{
    if (n == 0)
        return prodofdayn(simulation, 0);
    else
    {
        double prec = prodofdayn(simulation, n - 1);
        if (prec > 0)
            return (prodofdayn(simulation, n) - prec) / prec;
        else
        {
            printf("! Pourcentage d'augmentation indefini ! \n");
            return 0;
        }
    }
}

double avgofdayn(Simulation *simulation, int n)
{
    return prodofdayn(simulation, n) / NUMBER_OF_EOL;
}

double sigmaofdayn(Simulation *simulation, int n)
{
    double sigma = 0; //parcours des éoliennes au jour n
    double avg = avgofdayn(simulation, n);
    List *list = getdayn(simulation, n);
    ListNode *temp2 = list->first;

    while (temp2 != NULL)
    {
        sigma = sigma + pow(temp2->eolienne.production - avg, 2);
        temp2 = temp2->next;
    }
    sigma = sqrt(sigma / NUMBER_OF_EOL);
    return sigma;
}

double totalproduction(Simulation *simulation)
{
    double total = 0;
    int i = 0;
    Simulationnode *temp = simulation->firstlist;
    while (temp != NULL)
    {
        total = total + prodofdayn(simulation, i);
        i++;
        temp = temp->next;
    }

    return total;
}

double avgday(Simulation *simulation)
{
    return totalproduction(simulation) / simulation->taille;
}

double sigmaoftotalprod(Simulation *simulation) //écart-type des prod tot de chaque éolienne
{
    double sigma = 0; //parcours des éoliennes au jour n
    double avg = totalproduction(simulation) / NUMBER_OF_EOL;

    for (int i = 0; i < NUMBER_OF_EOL; i++)
    {
        sigma = sigma + pow(totalprodofeoln(simulation, i) - avg, 2);
    }
    sigma = sqrt(sigma / NUMBER_OF_EOL);
    return sigma;
}

double totalprodofeoln(Simulation *simulation, int n)
{
    double prod = 0;
    Simulationnode *temp = simulation->firstlist;
    while (temp != NULL)
    {
        Eol eol = geteoln(temp->listofeol, n);
        prod = prod + eol.production; //ajout de la production de l'éolienne n au jour i
        temp = temp->next;
    }
    return prod;
}

double percentofpart(Simulation *simulation, int n)
{
    return (totalprodofeoln(simulation, n) / totalproduction(simulation));
}

int *getlstofrankofeol(Simulation *simulation)
{
    return getlstofrankofeolafterdayd(simulation, simulation->duree);
}

int *getlstofrankofeolafterdayd(Simulation *simulation, int d)
{
    static int tab[NUMBER_OF_EOL];
    if (d > simulation->taille)
    {
        printf("Warning, simulation not advanced to day %d", d);
        d = simulation->taille;
    }
    double *tabcumval = getlstofcumulatedprodforeols(simulation, d);
    List *temp = getdayn(simulation, d - 1);
    List *temp2 = copyList(temp);
    ListNode *temp3 = temp2->first;
    for (int i = 0; i < NUMBER_OF_EOL; i++)
    {
        temp3->eolienne.production = tabcumval[i];
        temp3 = temp3->next;
    }

    temp2 = sortList(temp2);
    for (int i = 0; i < NUMBER_OF_EOL; i++)
    {
        Eol eol = geteoln(temp2, i);
        tab[i] = eol.num;
    }
    free(temp2);
    return tab;
}

double *getlstofcumulatedprodforeols(Simulation *sim, int d)
{
    static double tabcumval[NUMBER_OF_EOL] = {0};
    List *temp = NULL;
    for (int i = 0; i < d; i++)
    {
        temp = getdayn(sim, i);
        for (int k = 0; k < NUMBER_OF_EOL; k++)
        {

            Eol currenteol = geteoln(temp, k);
            tabcumval[k] += currenteol.production;
        }
    }
    return tabcumval;
}

double totalprodafterdayn(Simulation *simulation, int n)
{
    double result = 0;
    if (n >= simulation->taille)
    {
        n = simulation->taille - 1;
    }
    for (int i = 0; i <= n; i++)
    {
        result += prodofdayn(simulation, i);
    }
    return result;
}

void menu(Simulation *simulation, double tab[NUMBER_OF_STATE][NUMBER_OF_STATE])
{
    int bool = true;
    int choix;
    double lstofprod[NUMBER_OF_STATE] = {0, 0.4, 0.65};
    char globalreport[FIC_NAME_LEN] = {};
    char dailyreport[FIC_NAME_LEN] = {};
    char name[ID_LEN];
    int numbofday = -1;
    int i = -1;
    Eol *point;
    int *tabrank;
    double *tabofvals;
    List *eoliennes;

    while (bool)
    {
        printf("Saisir le numero de la commande a effectuer (CF README) \n");
        printf("(NB : Si l'on vous redemande deux fois une saisie, c'est que la premiere est invalide) \n");
        scanf("%d", &choix);
        switch (choix)
        {
        case 0:
            printf("la simulation est arrivee a la fin du jour %d \n", simulation->taille - 1);
            break;
        case 1:
            while (numbofday < 0 || numbofday < simulation->taille)
            {
                printf("Saisir le nombre de jour de simulation \n");
                scanf("%d", &numbofday);
            }
            simulation->duree = numbofday;
            numbofday = -1;
            break;
        case 2:
            while (i < 0 || i >= NUMBER_OF_EOL)
            {
                printf("Saisissez le numero de l'eolienne a modifier : \n");
                scanf("%d", &i);
            }
            point = getpointoneoln(simulation->firstlist->listofeol, i);
            i = -1;
            while (i < 0 || i >= NUMBER_OF_STATE)
            {
                printf("Saisissez l'etat initial : \n");
                scanf("%d", &i);
            }
            point->etatvent = i;
            point->production = lstofprod[i];
            i = -1;
            break;
        case 3:
            while (i < 0 || i >= NUMBER_OF_EOL)
            {
                printf("Saisissez le numero de l'eolienne a modifier : \n");
                scanf("%d", &i);
            }
            point = getpointoneoln(simulation->firstlist->listofeol, i);
            printf("ancient name : %s \n", point->subid);
            printf("Entrez le nouveau nom : \n");
            while (read(name, ID_LEN) != 1)
            {
                printf("invalid expression, try again \n");
            }
            strcpy(point->subid, name);
            i = -1;
            break;
        case 4:
            advanceXstep(simulation, simulation->duree, tab);
            break;
        case 5:
            while (numbofday < 0)
            {
                printf("Saisir le nombre de jour a avancer \n");
                scanf("%d", &numbofday);
            }
            advanceXstep(simulation, numbofday, tab);
            numbofday = -1;
            break;
        case 6:
            printf("Entrez le nom de fichier de daily report : \n");
            while (read(dailyreport, ID_LEN) != 1)
            {
                printf("invalid expression, try again \n");
            }
            printf("Entrez le nom de fichier de Global report : \n");
            while (read(globalreport, ID_LEN) != 1)
            {
                printf("invalid expression, try again \n");
            }
            printdailyreport(simulation, dailyreport);
            printglobalreport(simulation, globalreport);
            break;
        case 7:
            while (numbofday < 0 || numbofday >= simulation->duree)
            {
                printf("Saisir le numero du jour pour la production totale correspondante \n");
                scanf("%d", &numbofday);
            }
            printf("Jour %d : %f MW \n", numbofday, prodofdayn(simulation, numbofday));
            numbofday = -1;
            break;
        case 8:
            while (numbofday <= 0 || numbofday >= simulation->duree)
            {
                printf("Saisir le numero du jour pour augmentation par rapport au jour precedent\n");
                scanf("%d", &numbofday);
            }
            printf("Jour %d : %f pourcents \n", numbofday, 100 * increaseofdayn(simulation, numbofday));
            numbofday = -1;
            break;
        case 9:
            while (numbofday < 0 || numbofday >= simulation->duree)
            {
                printf("Saisir le jour pour production moyenne par eolienne correspondante \n");
                scanf("%d", &numbofday);
            }
            printf("Jour %d : %f MW en moyenne \n", numbofday, avgofdayn(simulation, numbofday));
            numbofday = -1;
            break;
        case 10:
            while (numbofday < 0 || numbofday >= simulation->duree)
            {
                printf("Saisir le jour pour l'ecart type de production par eolienne correspondant \n");
                scanf("%d", &numbofday);
            }
            printf("Jour %d : %f MW en moyenne \n", numbofday, sigmaofdayn(simulation, numbofday));
            numbofday = -1;
            break;
        case 11:
            printf("La production totale depuis le debut au jour %d est de %f \n", simulation->taille - 1, totalproduction(simulation));
            break;
        case 12:
            printf("La moyenne de production jusqu'apres le jour %d est de %f \n", simulation->taille - 1, avgday(simulation));
            break;
        case 13:
            while (i < 0 || i >= NUMBER_OF_EOL)
            {
                printf("Saisissez le numero de l'eolienne dont vous voulez connaitre la production totale : \n");
                scanf("%d", &i);
            }
            printf("L'eolienne %d a produit : %f`\n", i, totalprodofeoln(simulation, i));
            i = -1;
            break;
        case 14:
            while (i < 0 || i >= NUMBER_OF_EOL)
            {
                printf("Saisissez le numero de l'eolienne dont vous voulez connaitre le pourcentage de participation a la production totale : \n");
                scanf("%d", &i);
            }
            printf("L'eolienne %d a produit : %f \n", i, percentofpart(simulation, i));
            i = -1;
            break;
        case 15:
            if (simulation->taille > 1)
            {
                while (numbofday <= 0 || numbofday >= simulation->taille)
                {
                    printf("Saisir le jour pour afficher le classement jusqu'au jour saisi, 0 pour le classement jour en cours \n");
                    scanf("%d", &numbofday);
                    if (numbofday == 0)
                    {
                        numbofday = simulation->taille - 1;
                    }
                }
                if (numbofday > 0)
                {
                    tabrank = getlstofrankofeolafterdayd(simulation, numbofday);
                    tabofvals = getlstofcumulatedprodforeols(simulation, numbofday);
                    printf(" last -> First, Classement après le jour %d \n", numbofday);
                    printf("[");
                    for (int i = 0; i < NUMBER_OF_EOL; i++)
                    {
                        printf(" %d ", tabrank[i]);
                    }
                    printf("] \n");
                    printf("Prod of Eol 0 -> %d \n", NUMBER_OF_EOL);
                    printf("[");
                    for (int i = 0; i < NUMBER_OF_EOL; i++)
                    {
                        printf(" %f ", tabofvals[i]);
                    }
                    printf("] \n");
                }
                else
                {
                    printf(" classement jour 0 = ininteressant \n");
                }
            }
            else
            {
                printf("Simulation non lancee, rank impossible (jour 0 = ininteressant) \n");
            }
            numbofday = -1;
            break;
        case 16:
            while (numbofday < 0 || numbofday > simulation->taille)
            {
                printf("Saisir le jour pour l'ecart type de production par eolienne correspondant \n");
                scanf("%d", &numbofday);
            }
            eoliennes = getdayn(simulation, numbofday);
            printlist(eoliennes);
            numbofday = -1;
            break;
        case 17:
            eoliennes = getdayn(simulation, simulation->taille - 1);
            printlist(eoliennes);
            break;
        case 18:
            printsimulation(simulation);
            break;
        case 19:
            printf("Entrez le nom de fichier à afficher : \n");
            while (read(dailyreport, ID_LEN) != 1)
            {
                printf("invalid expression, try again \n");
            }
            printfile(dailyreport);
            break;
        case 20:
            printf("Entrez le nom de fichier a afficher : \n");
            while (read(dailyreport, ID_LEN) != 1)
            {
                printf("invalid expression, try again \n");
            }
            while (numbofday < 0 || numbofday > simulation->taille)
            {
                printf("Saisir le numero du jour a afficher \n");
                scanf("%d", &numbofday);
            }
            printresultofdayn(dailyreport, numbofday, simulation);
            numbofday = -1;
            break;
        case 21:
            bool = false;
            break;
        default:
            printf("Selection Out of range \n");
        }
    }
}

void printdailyreport(Simulation *simulation, char *filename)
{
    FILE *fichier;
    fichier = fopen(filename, "w");
    if (fichier == NULL)
    {
        printf("fichier non ouvert correctement \n");
        perror(filename);
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < simulation->taille; i++)
    {
        fprintf(fichier, "----JOUR %d----\n", i);
        fprintf(fichier, "TotalProd : %f MW, AvgProd/Eol : %f MW, sigmaprod : %f \n", prodofdayn(simulation, i), avgofdayn(simulation, i), sigmaofdayn(simulation, i));
        if (i > 1)
            fprintf(fichier, "Pourcentage d'augmentation/JourPrec : %f pourcents \n", 100 * increaseofdayn(simulation, i));
        List *day = getdayn(simulation, i);
        ListNode *temp = day->first;
        while (temp != NULL)
        {
            char name[ID_LEN] = "undef";
            if (strcmp(name, temp->eolienne.subid) == 0)
                fprintf(fichier, "%s : %f MW \n", temp->eolienne.id, temp->eolienne.production);
            else
            {
                fprintf(fichier, "%s aka %s : %f MW \n", temp->eolienne.id, temp->eolienne.subid, temp->eolienne.production);
            }

            temp = temp->next;
        }
    }

    if (fclose(fichier) == EOF)
    {
        fprintf(stderr, "Erreur lors de la fermeture du flux/fichier\n");
        exit(EXIT_FAILURE);
    }
}
void printglobalreport(Simulation *simulation, char *filename)
{
    FILE *fichier;
    fichier = fopen(filename, "w");
    if (fichier == NULL)
    {
        printf("fichier non ouvert correctement \n");
        perror(filename);
        exit(EXIT_FAILURE);
    }
    fprintf(fichier, "Arrived at day %d of %d \n", simulation->taille - 1, simulation->duree - 1);
    fprintf(fichier, "Total Prod : %f avg/day : %f avgtotprod/eol : %f sigmatotalprodofeol : %f \n", totalproduction(simulation), avgday(simulation), totalproduction(simulation) / NUMBER_OF_EOL, sigmaoftotalprod(simulation));
    int *Ranks = getlstofrankofeolafterdayd(simulation, simulation->taille);
    fprintf(fichier, "Rankofeolnumber i worst->best \n");
    fprintf(fichier, "[");
    for (int i = 0; i < NUMBER_OF_EOL; i++)
    {
        fprintf(fichier, " %d ", Ranks[i]);
    }

    fprintf(fichier, "] \n");
    for (int k = 0; k < NUMBER_OF_EOL; k++)
    {

        fprintf(fichier, "EOL %d, Totalprod : %f, TotalParticipation : %f pourcents \n", k, totalprodofeoln(simulation, k), 100 * totalprodofeoln(simulation, k) / totalproduction(simulation));
    }
    fprintf(fichier, "Production totale jour après jour : \n");
    fprintf(fichier, "[");
    double produ = 0;
    for (int i = 0; i < simulation->taille; i++)
    {
        produ += prodofdayn(simulation, i);
        fprintf(fichier, " %f ", produ);
    }
    fprintf(fichier, "] \n");

    if (fclose(fichier) == EOF)
    {
        fprintf(stderr, "Erreur lors de la fermeture du flux/fichier\n");
        exit(EXIT_FAILURE);
    }
}

void printfile(char *filename)
{
    char tempstring[MAX_STRING_LEN] = "";
    FILE *fichier = NULL;
    fichier = fopen(filename, "r");
    if (fichier == NULL)
    {
        printf("fichier non ouvert correctement \n");
        perror(filename);
        exit(EXIT_FAILURE);
    }
    while (fgets(tempstring, MAX_STRING_LEN, fichier) != NULL)
    {
        printf("%s", tempstring);
    }
    fclose(fichier);
}
void printresultofdayn(char *filename, int n, Simulation *Simulation)
{
    if (n >= Simulation->taille)
    {
        printf("day not generated yet \n");
        return;
    }
    char comp[MAX_STRING_LEN] = "";
    sprintf(comp, "----JOUR %d----\n", n);
    char tempstring[MAX_STRING_LEN] = "";
    FILE *fichier = NULL;
    fichier = fopen(filename, "r");
    if (fichier == NULL)
    {
        printf("fichier non ouvert correctement \n");
        perror(filename);
        exit(EXIT_FAILURE);
    }
    while (fgets(tempstring, MAX_STRING_LEN, fichier) != NULL)
    {
        if (strcmp(tempstring, comp) == 0)
        {
            for (int i = 0; i < 22; i++)
            {
                printf("%s", tempstring);
                if (fgets(tempstring, MAX_STRING_LEN, fichier) == NULL)
                {
                    return;
                }
            }
        }
    }
    fclose(fichier);
}

int read(char *string, int len)
{
    char *pos = NULL;
    fflush(stdin);
    if (fgets(string, len, stdin) != NULL)
    {
        pos = strchr(string, '\n');
        if (pos != NULL)
        {
            *pos = '\0';
        }
        return 1;
    }
    else
    {
        return 0;
    }
}