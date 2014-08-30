/* FOR MATLAB ONLY !
 * Function inv_search_c(invfile,data): get candidates list from input data
 * params:
 * - invfile (invFile)          : inverted file (created by inv_create_c.c || inv_create_norm_c.c)
 * - data    (vector 1xN)       : a document contains a list of words
 * returns:
 * - candidates list (vector )  : list of candidates documents id
*/

#include "mex.h"
#include "inv.h"

#define invFileIn   prhs[0] // inverted file
#define dataIn      prhs[1] // query document
#define normIn      prhs[2] // normalize (l0, l1, l2)
#define distIn      prhs[3] // distance use (l1, l2, nhi)

void mexFunction( int nlhs, mxArray *plhs[],
                  int nrhs, const mxArray *prhs[])
{
    const invFile*  inv;        // pointer to input inverted file
    //invNodeList     qdoc;
    int*            qwords;     // pointer to input data    (document - words list)
    int*            qhist;      // query word count (histogram)
    size_t          nqwords;    // lenght of input data     (number of words)
    char            q_norm;
    char            q_dist;
    float*          dist;       // distance from query to candidates
    double          q_norm_val = 0.0f;
    int             ncands = 0; // number of output candidates
    
    int*            pOutId;             // pointer to output ids
    float*          pOutDist;           // pointer to output distance
    
    int             i = 0,
                    j = 0;
    int             pos;
    mwSize          dims[2];    // size for output
    
    // check input params
    if (nrhs < 2) {
        printf("Missing params!\n");
        return;
    }
    if (nrhs == 2)
    {
        printf("No input norm and distance, set to default: (L1, L1)!\n");
        q_norm = NORM_L1;
        q_dist = DIST_L1;
    }
    if (nrhs == 3)
    {
        char *input = (char *) mxCalloc(mxGetN(normIn)+1, sizeof(char));
        mxGetString(normIn, input, mxGetN(normIn)+1);
        q_norm = get_norm(input);
        mxFree(input);
        if (q_norm == NORM_NONE)
        {
            printf("Unknown input normalize !\n");
            return;
        }
        printf("No input distance, set to default: L1!\n");
        q_dist = DIST_L1;
    }
    if (nrhs == 4)
    {
        char* input = (char *) mxCalloc(mxGetN(normIn)+1, sizeof(char));
        mxGetString(normIn, input, mxGetN(normIn)+1);
        q_norm = get_norm(input);
        mxFree(input);
        if (q_norm == NORM_NONE)
        {
            printf("Unknown input normalize!\n");
            return;
        }
        //         printf("norm: %s(%d),", input, q_norm);
        input = (char *) mxCalloc(mxGetN(distIn)+1, sizeof(char));
        mxGetString(distIn, input, mxGetN(distIn)+1);
        q_dist = get_dist(input);
        mxFree(input);
        if (q_dist == DIST_NONE)
        {
            printf("Unknown input distance measure!\n");
            return;
        }
        //         printf("distance measure: %s(%d)!\n", input, q_dist);
    }
    
    inv     = *(invFile**) mxGetData(invFileIn);
    qwords  = (int*)mxGetPr(dataIn);
    nqwords = mxGetNumberOfElements(dataIn);
    qhist   = (int*)calloc(inv->nwords,sizeof(int));
    // allocating distance out
    dist = (float*)calloc(inv->ndocs, sizeof(float));
    
    //count words and caculate normalize
    for(i = 0; i < nqwords; i++)
    {
        int w = qwords[i] - 1;
        if (inv->flags[w] == TRUE)
        {
            int d = (qhist[w]+1)*(qhist[w]+1) - (qhist[w]*qhist[w]); // d = (n+1)^2 - n^2
            float q_tfidf = (float)1/nqwords; // tf
            q_tfidf *= log2((float)inv->ndocs / inv->words[w].size); //*idf

            switch(q_norm)
            {
                case NORM_L0:
                    if (qhist[w] == 0)
                        q_norm_val += 1; // x^0
                    break;
                case NORM_L1:
                    q_norm_val += q_tfidf; // x^1, d^0 = 1
                    break;
                case NORM_L2:
                    q_norm_val += d*q_tfidf*q_tfidf; // x^2, d^1 = d
                    break;
            }
            qhist[w]++;
        }
    }
    if (q_norm == DIST_L2) q_norm_val = sqrt(q_norm_val);

    for(i = 0; i < inv->nwords; i++)
    {
        if (qhist[i] > 0 && inv->words[i].size > 0)
        {
            float idf = (float)inv->ndocs / inv->words[i].size;
            float q_val = (float)qhist[i]/nqwords; // tf
            
            q_val = q_val*log2(idf)/q_norm_val; // *idf / norm
  
            for(j=0; j < inv->words[i].size; j++)
            {
                int x = inv->words[i].nodes[j].id;          // candidate image
                float c_val = inv->words[i].nodes[j].val;   // candidate tf
                c_val *= log2(idf);
                // normalize candidate
                c_val = norm_val(inv,x,c_val,q_norm);                
                if (dist[x] == 0.0f)
                    ncands++;
                // calculate distance
                dist[x] += dist_calc(q_val,c_val,q_dist);
            }
        }
    }
    free(qhist);

    // allocate output
    dims[0] = 1;
    dims[1] = ncands;
    
    plhs[0] = mxCreateNumericArray(2, dims, mxUINT32_CLASS, mxREAL);
    pOutId = (int*)mxGetPr(plhs[0]);
    plhs[1] = mxCreateNumericArray(2, dims, mxSINGLE_CLASS, mxREAL);
    pOutDist = (float*)mxGetPr(plhs[1]);
    
    // assign to output
    pos = 0;
    for(i = 0; i < inv->ndocs; i++)
    {   
        if (dist[i] > 0)
        {
            pOutId[pos]   = i + 1;
            pOutDist[pos] = dist[i];
            pos ++;
        }
    }
    if (q_dist == DIST_NHI)
        quicksortf_desc(pOutId, pOutDist, 0, ncands - 1); 
    else quicksortf_asc(pOutId, pOutDist, 0, ncands - 1); 
    
    // free memory
    free(dist);
}