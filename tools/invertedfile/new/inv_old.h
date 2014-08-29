
#include "mex.h"
#include <math.h>

#define MAX_COUNT   100
#define TRUE        1
#define FALSE       0

typedef struct {
    int id;         // id of document (image)
    float val;       // normalized word frequency value
} invNode;

typedef struct {
    int size;       // size of list
    invNode* nodes;  // list of documents contains this word
} invNodeList;

typedef struct {
    int ndocs;          // number of documents
    int nwords;         // size of dictionary
    char* flags;        // value at ith = TRUE if ith word existen
    invNodeList* words; // inverted index data
    float* magnitude;   // magnitude of all document vector after tf-idf weighting
    invNodeList* docs;  // documents data
} invFile;

// get inverted file size: memory usage
double inv_size(const invFile*);

/*
 * Check whether list contains a specific element
 * If it contains, return position of the element
 * If not, return -1
 */
int isNodeListMem(int, const invNodeList*);

// initialize an inverted index file
// inv: inverted file
// nwords: size of dictionary
void inv_init(invFile*, int);

// fill data in inverted file
// docsPtr: pointer to input document list (from matlab, cell array)
// inv: inverted file to fill
// return: nwords
int inv_fill(const mxArray*, invFile*);

// clean inverted file
void inv_clean(invFile*);

void inv_clean(invFile* inv)
{
    int i;
    free(inv->flags);
    free(inv->magnitude);
    for (i = 0; i < inv->nwords; i++)
    {
        if (inv->words[i].size > 0)
        {
            free(inv->words[i].nodes);
        }
    }
//     for(i = 0; i < inv->ndocs; i++)
//     {
//         if(inv->docs[i].size > 0)
//         {
//             free(inv->docs[i].nodes);
//         }
//     }
    free(inv->words);
//     free(inv->docs);
    free(inv);
}

// get inverted file size: memory usage
double inv_size(const invFile* inv)
{
    double sum = 0;
    int i;
    sum += sizeof(inv);
    sum += sizeof(float) * inv->ndocs;      // magnitude
    sum += sizeof(char) * inv->nwords;      // flag
    sum += sizeof(invNodeList)* inv->nwords;// data pointer
    for(i = 0; i < inv->nwords; i++)
    {
        sum += sizeof(invNode) * inv->words[i].size; // data value
    }
    return sum;
}

/*
 * Check whether list contains a specific element
 * If it contains, return position of the element
 * If not, return -1
 */
int isNodeListMem( int x, const invNodeList* v)
{
    int i;
    for(i = 0; i < v->size; i++)
        if(x==v->nodes[i].id)
            return i;
    return -1;
}

// initialize an inverted index file
// inv: inverted file
// nwords: size of dictionary
void inv_init(invFile* inv, int nwords)
{
    int i;
    
    inv->nwords = nwords;
    
    inv->flags = (char*)malloc(sizeof(char)*nwords);
    inv->words = (invNodeList*)malloc(sizeof(invNodeList)*nwords);
    
    for (i = 0; i < nwords; i++)
    {
        inv->flags[i] = FALSE;
    }
}

// fill data in inverted file
// IMPOTANT: must call inv_init first
// docsPtr: pointer to input document list (from matlab, cell array)
// inv: inverted file to fill
// return: n words
int inv_fill(const mxArray* docsPtr, invFile* inv)
{
    int i, j, k, count = 0;
    
//     int* dflag;// = (int*)malloc(inv->nwords*sizeof(int)); // flag for fill data in docs
    
    inv->ndocs = (int)mxGetNumberOfElements(docsPtr);
//     inv->docs = (invNodeList*)malloc(sizeof(invNodeList)*inv->ndocs);
    inv->magnitude = (float*)malloc(sizeof(float)*inv->ndocs);
    
    printf("Creating inverted file...");
    for(i = 0; i < inv->ndocs; ++i)      // 
    {
        const mxArray*  w_ptr = mxGetCell(docsPtr, i);  // pointer to ith document
        int* wordset = (int*)mxGetData(w_ptr);          // word set in document
        int nw  = (int)mxGetNumberOfElements(w_ptr);    // n words in document
        
//         printf("i = %d\n", i);
        for(j = 0; j < nw; ++j)
        {
            int w = wordset[j] - 1; // jth word
            if (inv->flags[w] == FALSE)
            {
                inv->words[w].nodes = (invNode*)malloc(sizeof(invNode)*MAX_COUNT);
                inv->words[w].size = 0;
                inv->flags[w] = TRUE;
                count++;
            }
            if (inv->flags[w] == TRUE)
            {
                int n = inv->words[w].size;
                int pos;
                if ( (pos = isNodeListMem(i, &inv->words[w])) == -1)
                {
                    inv->words[w].nodes[n].id = i;            // document id 
                    inv->words[w].nodes[n].val = (float)1/nw; // normalize word frequency (tf)
                    inv->words[w].size ++;                   // increase size of current word
                }
                // if ith document is already in current word data
                else 
                {   
                    inv->words[w].nodes[pos].val += (float)1/nw;
                }
            }
        }
    }
    
    
    for(i = 0; i < inv->ndocs; ++i)  
    {
        const mxArray*  w_ptr = mxGetCell(docsPtr, i);  // pointer to ith document
        int* wordset = (int*)mxGetData(w_ptr);          // word set in document
        int nw  = (int)mxGetNumberOfElements(w_ptr);    // n words in document
        int* wcount = (int*)calloc(inv->nwords,sizeof(int));
        
        inv->magnitude[i] = 0.0f;
//         printf("i = %d\n", i);
        for(j = 0; j < nw; ++j)
        {
            int w = wordset[j] - 1; // jth word
            float tf = (float)1/nw;
            float idf = (float)inv->ndocs / inv->words[w].size;
            float tfidf = tf*log(idf)/log(2);
            
            if (wcount[w] == 0)
            {
                inv->magnitude[i] += tfidf * tfidf;
                wcount[w] = 1;
            }
            else
            {
                int d = (wcount[w]+1)*(wcount[w]+1) - (wcount[w]*wcount[w]);
                inv->magnitude[i] += d*tfidf*tfidf;
                wcount[w] ++;
            }
        }
        inv->magnitude[i] = sqrt(inv->magnitude[i]);
        free(wcount);
    }
    
//     fill data to docs with tf-idf weighting
//     dflag = (int*)calloc(inv->nwords,sizeof(int));
//     for(i = 0; i < inv->ndocs; ++i)      // 
//     {
//         
//         int pos = 0;
//         const mxArray*  w_ptr = mxGetCell(docsPtr, i);  // pointer to ith document
//         int* wordset = (int*)mxGetData(w_ptr);          // word set in document
//         int nw  = (int)mxGetNumberOfElements(w_ptr);    // n words in document
//         
//         inv->docs[i].size = 0;
//         inv->docs[i].nodes = (invNode*)malloc(sizeof(invNode)*nw);
//         
//         int* wcount = (int*)calloc(inv->nwords,sizeof(int));
//         
//         for(j = 0; j < inv->nwords; i++) dflag[j] = -1;  // reset flags
//         
//         inv->docs[i].nodes = (invNode*)malloc(sizeof(invNode)*inv->docs[i].size); // allocate memory
//         inv->magnitude[i] = 0.0f; // init magnitude value for ith document
//         for(j = 0; j < nw; ++j)
//         {
//             int w = wordset[j] - 1; // jth word
//             if (dflag[w] == -1)
//             {
//                 float tf = (float)1/nw; // normalize (tf value)
//                 float idf = (float)inv->ndocs/inv->words[w].size; // idf
//                 float tfidf = tf*log(idf); //tf-idf
//                 
//                 inv->magnitude[i] += tfidf*tfidf;
//                 wcount[w] = 1; // count for magnitude computing
//                 
//                 inv->docs[i].nodes[pos].id = w;
//                 inv->docs[i].nodes[pos].val = tfidf;
//                 dflag[w] = pos; // save position
//                 pos++;
//             }
//             else
//             {
//                 float tf = (float)1/nw; // normalize (tf value)
//                 float idf = (float)inv->ndocs/inv->words[w].size; // idf
//                 float tfidf = tf*log(idf); //tf-idf
//                 
//                 int d = (wcount[w]+1)*(wcount[w]+1) - (wcount[w]*wcount[w]);
//                 inv->magnitude[i] += d*tfidf*tfidf;
//                 wcount[w] ++;
//                         
//                 inv->docs[i].nodes[dflag[w]].val += tf*log(idf);
//             }
//         }
//         inv->magnitude[i] = sqrt(inv->magnitude[i]);
//         free(wcount);
//     }
    
//     free(dflag);
    
    return count;
}