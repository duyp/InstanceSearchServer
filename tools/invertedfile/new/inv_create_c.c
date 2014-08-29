
#include "mex.h"

#include "inv.h"

#define dataIn      prhs[0]
#define nwordsIn    prhs[1]

void mexFunction( int nlhs, mxArray *plhs[],
                  int nrhs, const mxArray *prhs[])
{
    int         ndocs;          // number of input documents
    int         nwords;         // size of dictionary
    mxArray*    out;            // output pointer
    invFile*    invFileOut;     // output inverted file index
    int         invWordCount;   // word count of inverted file
    
    int         i       = 0,
                j       = 0,
                count   = 0;
    
    // check input params
    if (nrhs != 2) {
        printf("missing params!\n");
        return;
    }
            
    // check input data type (dataIn must be a Cell)
    // get and check numbers of input documents
    if (mxIsCell(dataIn) && (ndocs = (int)mxGetNumberOfElements(dataIn)) > 0)
    {
        printf("numbers of input docs: %d\n",ndocs);
        // allocate output inverted file
        invFileOut = (invFile*)malloc(sizeof(invFile));
        nwords = (int)mxGetScalar(nwordsIn);
    }
    else return;

    // fill data to inverted file
    invWordCount = inv_fill(dataIn, invFileOut, nwords);
    
    printf("Done load and fill data to inverted file: %d words!\nInverted file size: %.0f\n", invWordCount, inv_size(invFileOut));

    // fill data to output pointer
     plhs[0] = mxCreateNumericMatrix(1,1,mxINDEX_CLASS,mxREAL);
     *(invFile**) mxGetPr(plhs[0]) = invFileOut;
}