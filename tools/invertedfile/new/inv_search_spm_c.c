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

// seach query word set in inverted file
//@param:
// dist: output distance from query to all candidates (NHI distance)
// ncands: output n candidates
// inv: input inverted file
// qhist: input query word set histogram
// nqwords: input size of query word set
// q_norm: query normalize  (sum of normalize value of all query word set (cell) in each level (resolution))
// c-norm: candidate docs normalize (get from first inverted file)
// spmweight: spm weight value for this level
void search(float* dist, int* ncands, // out put
        const invFile* inv, const int* qhist, int nqwords,   // input data
        float q_norm, const float* c_norm , float spmweight);

void mexFunction( int nlhs, mxArray *plhs[],
                  int nrhs, const mxArray *prhs[])
{
    const invFilePyramid* inv = *(invFilePyramid**) mxGetData(invFileIn);   // inverted files in
    int             nqset = (int)mxGetNumberOfElements(dataIn);     // n query word set
    float*          dist;       // distance from query to candidates
    int             ncands = 0; // number of output candidates
    float           q_norm = 0.0f;
   
    int*            pOutId;             // pointer to output ids
    float*          pOutDist;           // pointer to output distance
    
    int             i, j, pos,
                    l, L;          // pyramid level
    mwSize          dims[2];    // size for output
    
    // check input params
    if (nrhs < 2) {
        printf("Missing params!\n");
        return;
    }
    
    // allocating distance out
    dist = (float*)calloc(inv->inv[0].ndocs, sizeof(float));
    
    L = find_spm_level(nqset-1);
    pos = 0;
    for(l = 0; l <= L; l++)
    {
        int grids = (int)pow(2,l)*(int)pow(2,l); // (2^l)^2
        float weight = (float)1/( l==0 ? pow(2,L) : pow(2,L-l+1) );
        //printf("weight: %lf\n",weight);
        int** qhist = (int**)malloc(sizeof(int*)*grids);
        int* nqwords = (int*)malloc(sizeof(int)*grids);
        int pos1 = pos;
        q_norm = 0.0f;
        for(i = 0; i < grids; i++)
        {
            // get input data
            mxArray* wptr   = mxGetCell(dataIn,pos1);            // pointer to each word set
            int* qwords     = (int*)mxGetData(wptr);            // query word set
            nqwords[i]      = (int)mxGetNumberOfElements(wptr); // query word set size
            
            // hist
            qhist[i]      = (int*)calloc(inv->inv[pos1].nwords,sizeof(int));
            for(j = 0; j < nqwords[i]; j++)
            {
                int w = qwords[j] - 1;
                if (inv->inv[pos1].flags[w] == TRUE)
                {
                    float q_tfidf = (float)1/nqwords[i];                               // tf
                    q_tfidf *= log2f((float)inv->inv[pos1].ndocs/inv->inv[pos1].words[w].size);//*idf
                    //if (l==0) q_norm += q_tfidf;  // L1 norm
                    q_norm += q_tfidf;  // L1 norm
                    qhist[i][w]++;
                }
            }
            pos1++;
        }
       // printf("norm: %f\n",q_norm);
        for( i = 0; i < grids; i++)
        {
            // search
            search(dist, &ncands, &inv->inv[pos], qhist[i], nqwords[i], q_norm, inv->norml1[l], weight);
            
            // free temp memory
            free(qhist[i]);
            // increase position
            pos ++;
        }
        free(qhist);
    }        

    // allocate output
    dims[0] = 1;
    dims[1] = ncands;
    //printf("ncands: %d\n", ncands);
    
    if (ncands > 0)
    {
        plhs[0] = mxCreateNumericArray(2, dims, mxUINT32_CLASS, mxREAL);
        pOutId = (int*)mxGetPr(plhs[0]);
        plhs[1] = mxCreateNumericArray(2, dims, mxSINGLE_CLASS, mxREAL);
        pOutDist = (float*)mxGetPr(plhs[1]);

        // assign to output
        pos = 0;
        for(i = 0; i < inv->inv[0].ndocs; i++)
        {   
            if (dist[i] > 0)
            {
                pOutId[pos]   = i + 1;
                pOutDist[pos++] = dist[i];
            }
        }
        quicksortf_desc(pOutId, pOutDist, 0, dims[1] - 1); 
    }
    // free memory
    free(dist);
}

void search(float* dist, int* ncands, // out put
        const invFile* inv, const int* qhist, int nqwords,   // input data
        float q_norm, const float* c_norm , float spmweight)                            // input norm + weight
{
    int i,j;
    //search 
    for(i = 0; i < inv->nwords; i++)
    {
        if (qhist[i] > 0 && inv->flags[i] == TRUE)
        {
            float idf   = (float)inv->ndocs/inv->words[i].size;
            float q_val = (float)qhist[i]/nqwords;          // tf = qhist[i]/nqwords
                  q_val = q_val*log2f(idf)/q_norm;             // query word value with tf-idf weighting and L1 normalization
                  
            for(j=0; j < inv->words[i].size; j++)
            {
                int x = inv->words[i].nodes[j].id;          // candidate image
                float c_val = inv->words[i].nodes[j].val;   // candidate tf
                c_val *= log2f(idf);
                c_val /= c_norm[x]; // candidate value with tf-idf weighting and L1 normalization
                
                // found new candidate
                if (dist[x] == 0)
                    ncands[0]++;
                // NHI distance
                dist[x] += dist_calc(q_val,c_val,DIST_NHI)*spmweight;
            }
        }
    }
}