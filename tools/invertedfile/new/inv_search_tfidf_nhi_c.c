/* FOR MATLAB ONLY !
 * Function inv_search_c(invfile,data): get candidates list from input data
 * params:
 * - invfile (invFile)          : inverted file (created by inv_create_c.c || inv_create_norm_c.c)
 * - data    (vector 1xN)       : a document contains a list of words
 * returns:
 * - candidates list (vector )  : list of candidates documents id
*/

#include <math.h>
#include "mex.h"
#include "inv.h"
#include "sort.h"

#define invFileIn   prhs[0] // inverted file
#define dataIn      prhs[1] // query document

void mexFunction( int nlhs, mxArray *plhs[],
                  int nrhs, const mxArray *prhs[])
{
    const invFile*  inv;        // pointer to input inverted file
    int*            qwords;     // pointer to input data    (document - words list)
    size_t          nqwords;    // lenght of input data     (number of words)
    mwSize          dims[2];
    float*          dist;               // distace from query doc to all candidates document
    float           q_magnitude = 0.0f; // magnitude value of query vector
    int             ncands = 0;         // number of output candidates
    int*            pOutId;             // pointer to output ids
    float*          pOutDist;           // pointer to output distance
    int             i = 0,
                    j = 0;
    int             pos;
    
    // check input params
    if (nrhs != 2) {
        printf("missing params!\n");
        return;
    }
    
    inv     = *(invFile**) mxGetData(invFileIn);
    qwords  = (int*)mxGetPr(dataIn);
    nqwords = mxGetNumberOfElements(dataIn);
    
    //printf("num query words: %d\n",nqwords);

    dist = (float*)malloc(sizeof(float)*inv->ndocs);
    for(i = 0; i < inv->ndocs; i++)
    {
        dist[i] = 0.0f;
    }
    
    for(i = 0; i < nqwords; i++)
    {
        int w = qwords[i] - 1;
        if(inv->words[w].size > 0)
        {
            for(j=0; j < inv->words[w].size; j++)
            {
                 int x = inv->words[w].nodes[j].id;
                 float idf  = (float)inv->ndocs / inv->words[w].size;
                 float q_tf = (float)1/nqwords;                 // query tf
                 float c_tf = inv->words[w].nodes[j].val;  // candidate tf
                 
                 float q_tfidf = q_tf*log(idf);
                 float c_tfidf = c_tf*log(idf);
                 
                 //q_magnitude += q_tfidf * q_tfidf;
                 
                 if (dist[x] == 0.0f) ncands++;
                 //dist[x] += q_tfidf * c_tfidf;
                 dist[x] += (q_tfidf < c_tfidf ? q_tfidf : c_tfidf); // nhi distance
            }
        }
    }
    //q_magnitude = sqrt(q_magnitude);

    dims[0] = 1;
    dims[1] = ncands;
    plhs[0] = mxCreateNumericArray(2, dims, mxUINT32_CLASS, mxREAL);
    pOutId = (int*)mxGetData(plhs[0]);
    plhs[1] = mxCreateNumericArray(2, dims, mxSINGLE_CLASS, mxREAL);
    pOutDist = (float*)mxGetData(plhs[1]);
    
    pos = 0;
    for(i = 0; i < inv->ndocs; i++)
    {   
        if (dist[i] > 0)
        {
            pOutId[pos]   = i + 1;
            pOutDist[pos] = dist[i]; //(q_magnitude * inv->magnitude[i]); // scalar product
            pos ++;
        }
    }
    
    quicksortf(pOutId, pOutDist, 0, ncands - 1); 
}