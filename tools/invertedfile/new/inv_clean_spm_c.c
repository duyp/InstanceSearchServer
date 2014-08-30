
#include "mex.h"
#include "inv.h"

#define invFileIn   prhs[0] // inverted file in

void mexFunction( int nlhs, mxArray *plhs[],
                  int nrhs, const mxArray *prhs[])
{
     // pointer to input inverted file
    invFilePyramid*  invIn = *(invFilePyramid**)mxGetData(invFileIn);       
    int i, j;
    for(i = 0; i < invIn->size; i++)
    {
        inv_clean(&invIn->inv[i]);
    }
    if (invIn->norml1 != NULL)
    {
        for(i = 0; i < invIn->L; i++)
        {
            if (invIn->norml1[i] != NULL)
                free(invIn->norml1[i]);
        }
        free(invIn->norml1);
    }
    // clean data of inverted file
    
}