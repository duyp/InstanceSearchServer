#ifndef INV_H
#define INV_H

#include <string.h>
#include <math.h>
#include "mex.h"
#include "sort.h"

#define MAX_COUNT   100
#define TRUE        1
#define FALSE       0

#define NORM_L0     4
#define NORM_L1     5
#define NORM_L2     6
#define NORM_NONE   22

#define DIST_L1     7
#define DIST_L2     8
#define DIST_NHI    9
#define DIST_TFIDF  10
#define DIST_NONE   23

double log2( double n )  
{  
    // log(n)/log(2) is log2.  
    return log(n) / log(2.0f);  
}

float log2f( float n )  
{  
    // log(n)/log(2) is log2.  
    return logf(n) / logf(2.0f);  
}

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
    invNodeList* docs;  // documents data   
    float* norml0;      // l0 normalization
    float* norml1;      // l1 normalization
    float* norml2;      // l2 normalization
} invFile;

// inverted file for spatial pyramid
typedef struct {
    invFile* inv;   // list inverted file
    int size;       // n inverted file
    int L;          // pyramid max level
    float** norml1; // normalize for each level
} invFilePyramid;

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
int inv_fill(const mxArray*, invFile*, int);

// clean inverted file
void inv_clean(invFile*);

// make histogram for input document
void hist(invNodeList*, const int*, int, int);

// find spm level for xth set
int find_spm_level(int x)
{
    int i;
    int lvs[4] = {1, 5, 21, 85};
    for(i = 0; i < 4; i++)
    {
        if (x + 1 <= lvs[i])
            return i;
    }
}

// get normalize by input string
char get_norm(char* inputstr)
{
    if (strcmp(inputstr,"l0") == 0)
        return NORM_L0;
    if (strcmp(inputstr,"l1") == 0)
        return NORM_L1;
    if (strcmp(inputstr,"l2") == 0)
        return NORM_L2;
    return NORM_NONE;
}

// get distanec measure by input string
char get_dist(char* inputstr)
{
    if (strcmp(inputstr,"l1") == 0)
        return DIST_L1;
    if (strcmp(inputstr,"l2") == 0)
        return DIST_L2;
    if (strcmp(inputstr,"nhi") == 0)
        return DIST_NHI;
    if (strcmp(inputstr,"tfidf") == 0)
        return DIST_TFIDF;
    return DIST_NONE;
}

double norm_val(const invFile* inv, int docId, double val, char norm)
{
    switch(norm)
    {
        case NORM_L0:
            val /= inv->norml0[docId];
            break;
        case NORM_L1:
            val /= inv->norml1[docId];
            break;
        case NORM_L2:
            val /= inv->norml2[docId];
            break;
    }
    return val;
}

float dist_calc(float x, float y, char dist)
{
    switch(dist)
    {
        case DIST_L1:
            return fabs(x - y);    // |x - y|   // undone! need furhter computation
        case DIST_L2:
            return (x - y)*(x - y);    // (x-y)^2 - undone! need furhter computation
        case DIST_NHI:
            return (x < y ? x : y);    // min(x,y) - done !
    }
    return 0;
}

void clean_nodelist(invNodeList* list)
{
    if (list->size > 0 && list->nodes != NULL)
    {
        free(list->nodes);
    }
}

void inv_clean(invFile* inv)
{
    int i;
    if (inv!=NULL)
    {
        if (inv->flags != NULL)
            free(inv->flags);
        
        if (inv->norml0 != NULL)
            free(inv->norml0);
        if (inv->norml1 != NULL)
            free(inv->norml1);
        if (inv->norml2 != NULL)
            free(inv->norml2);
        
        if (inv->words != NULL)
        {
            for (i = 0; i < inv->nwords; i++)
            {
                clean_nodelist(&inv->words[i]);
            }
            free(inv->words);
        }
        if (inv->docs != NULL)
        {
            for(i = 0; i < inv->ndocs; i++)
            {
                clean_nodelist(&inv->docs[i]);
            }
            free(inv->docs);
        }    
        free(inv);
    }
}
// get inverted file size: memory usage
double inv_size(const invFile* inv)
{
    double sum = 0;
    int i;
    sum += sizeof(inv);
    sum += sizeof(float) * inv->ndocs * 3;      // norm
    sum += sizeof(char) * inv->nwords;      // flag
    sum += sizeof(invNodeList)* inv->nwords;// data pointer
    for(i = 0; i < inv->nwords; i++)
    {
        sum += sizeof(invNode) * inv->words[i].size; // data value
    }
    for(i = 0; i < inv->ndocs; i++)
    {
        sum+= sizeof(invNode) * inv->docs[i].size;
    }
    return sum;
}

/*
 * Check whether list contains a specific element
 * If it contains, return position of the element
 * If not, return -1
 */
int isNodeListMem(int x, const invNodeList* list)
{
    int i;
    for(i = 0; i < list->size; i++)
        if(x==list->nodes[i].id)
            return i;
    return -1;
}

int isMem(int x, const int* list, int begin, int end)
{
    int i;
    for(i = begin; i < end; i++)
        if(x==list[i])
            return i;
    return -1;
}

// initialize an inverted index file
// inv: inverted file
// nwords: size of dictionary
void inv_init(invFile* inv, int nwords)
{
    inv->nwords = nwords;
    inv->flags = (char*)calloc(nwords, sizeof(char)); // allocate and assign FLASE (0) value
    inv->words = (invNodeList*)calloc(nwords,sizeof(invNodeList));
    inv->ndocs = 0;
    inv->docs = NULL;
    inv->norml0 = NULL;
    inv->norml1 = NULL;
    inv->norml2 = NULL;
}

// fill data in inverted file
// docsPtr: pointer to input document list (from matlab, cell array)
// inv: inverted file to fill in
// return: n words in dictionary
int inv_fill(const mxArray* docsPtr, invFile* inv, int nwords)
{
    int i, j, count = 0;
    
    inv_init(inv,nwords);
    inv->ndocs = (int)mxGetNumberOfElements(docsPtr);
    inv->docs = (invNodeList*)malloc(sizeof(invNodeList)*inv->ndocs);
    inv->norml0 = (float*)malloc(sizeof(float)*inv->ndocs);
    inv->norml1 = (float*)malloc(sizeof(float)*inv->ndocs);
    inv->norml2 = (float*)malloc(sizeof(float)*inv->ndocs);
    
    //printf("Creating inverted file...");
    for(i = 0; i < inv->ndocs; ++i)      // 
    {
        const mxArray*  w_ptr = mxGetCell(docsPtr, i);  // pointer to ith document
        int* docIn = (int*)mxGetData(w_ptr);            // word set in input document
        int nw  = (int)mxGetNumberOfElements(w_ptr);    // n words in document
        
        // make histogram for input doc
        hist(&inv->docs[i], docIn, nw, inv->nwords);
        
        for(j = 0; j < inv->docs[i].size; j++)
        {
            int w = inv->docs[i].nodes[j].id;
            float c = inv->docs[i].nodes[j].val;
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
                inv->words[w].nodes[n].id = i;  // document id 
                inv->words[w].nodes[n].val = c; // tf
                inv->words[w].size ++;
            }
        }
    }
    
    // norm with tf-idf weighting
    for(i = 0; i < inv->ndocs; i++)      // 
    {
        inv->norml0[i] = 0.0f;
        inv->norml1[i] = 0.0f;
        inv->norml2[i] = 0.0f;
        for (j = 0; j < inv->docs[i].size; j++)
        {
            int w = inv->docs[i].nodes[j].id;
            float tf = inv->docs[i].nodes[j].val; // term frequency = ni/nd
            float idf = (float)inv->ndocs / inv->words[w].size; // inverse document frequency = log(N/ndi)
            float tfidf = tf*(float)log2(idf);
            
            // update normalize values
            inv->norml0[i] += 1.0f;             // l0   x^0
            inv->norml1[i] += tfidf;            // l1   x^1
            inv->norml2[i] += tfidf*tfidf;      // l2   x^2
        }
//         inv->norml0[i] = sqrtf(inv->norml0[i]); //square root
//         inv->norml1[i] = sqrtf(inv->norml1[i]);
        inv->norml2[i] = sqrtf(inv->norml2[i]);
    }
    return count;
}

void hist(invNodeList* histout, const int* doc, int docsize, int nwords)
{
    int* count = (int*)calloc(nwords,sizeof(int));
    int i, pos;
    
    // count
    for (i = 0; i < docsize; i++)
        count[doc[i]-1] += 1;
    
    histout->size = 0;
    for (i = 0; i < nwords; i++)
		if (count[i]>0) histout->size++;

    // allocating output
	histout->nodes = (invNode*)malloc(sizeof(invNode)*histout->size);
    
    // make histogram
    pos = 0;
    for(i = 0; i < nwords; i++)
    {
        if (count[i] > 0)
        {
            histout->nodes[pos].id = i;
            histout->nodes[pos].val = (float) count[i] / docsize; // tf
            pos ++;
        }
    }
    free(count);
}

#endif