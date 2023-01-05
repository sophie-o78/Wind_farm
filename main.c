#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "eolienne.h"
#include "simulation.h"

int main(void)
{
    srand(time(NULL));

    double tab[NUMBER_OF_STATE][NUMBER_OF_STATE] = {
        {0.2, 0.73, 0.07},
        {0.11, 0.74, 0.15},
        {0.04, 0.61, 0.35}};

    Simulation *sim = newsimulation(DEFAULT_NUMB_OF_DAY);

    initsimulation(sim);
    menu(sim, tab);

    /*
    advanceXstep(sim, 5, tab);
    printsimulation(sim);
    printf("au jour 3 la production totale est de :%f \n", prodofdayn(sim, 3));

    int *tabrank = getlstofrankofeolafterdayd(sim, 5);
    printf("[");
    for (int i = 0; i < NUMBER_OF_EOL; i++)
    {
        printf(" %d ", tabrank[i]);
    }
    printf("] \n");
    double *tabcumval = getlstofcumulatedprodforeols(sim, 5);

    printf("[");
    for (int i = 0; i < NUMBER_OF_EOL; i++)
    {
        printf(" %.2f ", tabcumval[i]);
    }
    printf("] \n");
    */

    freesimulation(sim);
    return EXIT_SUCCESS;
}