
#include "mex.h"

#include "inv.h"

#define dataIn      prhs[0]
#define nwordsIn    prhs[1]

void mexFunction( int nlhs, mxArray *plhs[],
                  int nrhs, const mxArray *prhs[])
{
    int             nwordset = 0;
    invFilePyramid* invFileOut;
    mxArray*        out;
    int             nwords;         // size of dictionary
    int             i, j, l, pos;
    double          size = 0;
    
    // check input params
    if (nrhs != 2) {
        printf("missing params!\n");
        return;
    }
    
    // check input data type (dataIn must be a Cell)
    // get and check numbers of input documents
    if (mxIsCell(dataIn) && (nwordset = mxGetNumberOfElements(dataIn)) > 0)
    {
        printf("nwordset: %d\n",nwordset);
        invFileOut          = (invFilePyramid*)malloc(sizeof(invFilePyramid));
        invFileOut->inv     = (invFile*)malloc(sizeof(invFile)*nwordset);
        invFileOut->L       = find_spm_level(nwordset-1);
        invFileOut->norml1  = (float**)malloc(sizeof(float*)*(invFileOut->L+1));
        invFileOut->size    = nwordset;
        nwords = (int)mxGetScalar(nwordsIn);
    }
    else return;

    // fill data to inverted file
    pos = 0;
    for(l = 0; l <= invFileOut->L; l++)
    {
        int count,
            grids = (int)pow(2,l)*(int)pow(2,l); // (2^l)^2
        int pos1 = pos;
        for(i = 0; i < grids; i++)
        {
            const mxArray* wordsPtr = mxGetCell(dataIn, pos1);
            count = inv_fill(wordsPtr, &invFileOut->inv[pos1], nwords);
            size += inv_size(&invFileOut->inv[pos1]);
            printf("[l = %d, pos =%d]word count: %d\n",l,pos1,count);
            pos1++;
        }
        invFileOut->norml1[l] = (float*)calloc(invFileOut->inv[pos].ndocs,sizeof(float));
        for(i = 0; i < invFileOut->inv[pos].ndocs; i++)
        {
            pos1 = pos;
            for(j = 0; j < grids; j++)
                invFileOut->norml1[l][i] += invFileOut->inv[pos1++].norml1[i];
        }
        pos += grids;
    }
    
//     
//     for(i = 0; i < nwordset; i++)
//     {
//         int count;
//         const mxArray* wordsPtr = mxGetCell(dataIn, i);
//         // fill data to inverted file
//         count = inv_fill(wordsPtr, &invFileOut->inv[i], nwords);
//         // calculate size
//         size += inv_size(invFileOut[i]);
//         printf("invWordCount: %d\n", count);
//     }
    printf("Done load and fill data. Inverted file size: %.0f\n", size);

    // fill data to output pointer
    plhs[0] = mxCreateNumericMatrix(1,1,mxINDEX_CLASS,mxREAL);
    *(invFilePyramid**) mxGetPr(plhs[0]) = invFileOut;
    // plhs[0] = out;
}