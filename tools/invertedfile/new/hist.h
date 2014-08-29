#ifndef HIST_H
#define HIST_H

#include "inv.h"

void hist(invNodeList* histout, const int* doc, int docsize, int num_words)
{
    int* count = (int*)calloc(num_words,sizeof(int));
    int i, pos;
    
    // count
    for (i = 0; i < nwords; i++)
        count[doc[i]-1] += 1;
    
    histout.size = 0;
    for (i = 0; i < num_words; i++)
		if (count[i]>0) histout.size++;
    
    // allocating output
	histout.nodes = (invNode*)malloc(sizeof(invNode)*histout.size);
    
    // make histogram
    pos = 0;
    for(i = 0; i < num_words; i++)
    {
        if (temp[i] > 0)
        {
            histout.nodes[pos].id = i + 1;
            histout.nodes[pos].val = count[i];
            pos ++;
        }
    }
}

#endif