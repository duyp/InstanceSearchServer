
#include "mex.h"
#include "inv.h"

#define invFileIn   prhs[0] // inverted file in

void mexFunction( int nlhs, mxArray *plhs[],
                  int nrhs, const mxArray *prhs[])
{
    const invFile*  inv;        // pointer to input inverted file
    int             i = 0,
                    j = 0;
    
    inv     = *(invFile**) mxGetData(invFileIn);
    
    // clean data of inverted file
    inv_clean(inv);
}