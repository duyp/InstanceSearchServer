#ifndef SORT_H
#define SORT_H

void quicksort_desc(int* x,int first,int last){
    int pivot,j,temp,i;

     if(first<last){
         pivot=first;
         i=first;
         j=last;

         while(i<j){
             while(x[i]>=x[pivot]&&i<last)
                 i++;
             while(x[j]<x[pivot])
                 j--;
             if(i<j){
                 temp=x[i];
                  x[i]=x[j];
                  x[j]=temp;
             }
         }

         temp=x[pivot];
         x[pivot]=x[j];
         x[j]=temp;
         quicksort_desc(x,first,j-1);
         quicksort_desc(x,j+1,last);

    }
}

void quicksorti_desc(int* id,int* val, int first,int last)
{
    int pivot,j,temp,i, temp_i;

     if(first<last){
         pivot=first;
         i=first;
         j=last;

         while(i<j){
             while(val[i]>=val[pivot]&&i<last)
                 i++;
             while(val[j]<val[pivot])
                 j--;
             if(i<j){
                 temp=val[i];
                  val[i]=val[j];
                  val[j]=temp;

				  temp_i = id[i];
				  id[i] = id[j];
				  id[j] = temp_i;
             }
         }

         temp=val[pivot];
         val[pivot]=val[j];
         val[j]=temp;

		 temp_i = id[pivot];
		 id[pivot] = id[j];
		 id[j] = temp_i;

         quicksorti_desc(id, val,first,j-1);
         quicksorti_desc(id, val,j+1,last);

    }
}

void quicksorti_asc(int* id,int* val, int first,int last)
{
    int pivot,j,temp,i, temp_i;

     if(first<last){
         pivot=first;
         i=first;
         j=last;

         while(i<j){
             while(val[i]<=val[pivot]&&i<last)
                 i++;
             while(val[j]>val[pivot])
                 j--;
             if(i<j){
                 temp=val[i];
                  val[i]=val[j];
                  val[j]=temp;

				  temp_i = id[i];
				  id[i] = id[j];
				  id[j] = temp_i;
             }
         }

         temp=val[pivot];
         val[pivot]=val[j];
         val[j]=temp;

		 temp_i = id[pivot];
		 id[pivot] = id[j];
		 id[j] = temp_i;

         quicksorti_asc(id, val,first,j-1);
         quicksorti_asc(id, val,j+1,last);

    }
}

void quicksortf_desc(int* id,float* val, int first,int last)
{
    int     pivot,j,i;
    int     temp_i;
    float   temp;
    
     if(first<last){
         pivot=first;
         i=first;
         j=last;

         while(i<j){
             while(val[i]>=val[pivot]&&i<last)
                 i++;
             while(val[j]<val[pivot])
                 j--;
             if(i<j){
                 temp=val[i];
                  val[i]=val[j];
                  val[j]=temp;

				  temp_i = id[i];
				  id[i] = id[j];
				  id[j] = temp_i;
             }
         }

         temp       = val[pivot];
         val[pivot] = val[j];
         val[j]     = temp;

		 temp_i     = id[pivot];
		 id[pivot]  = id[j];
		 id[j]      = temp_i;

         quicksortf_desc(id, val,first,j-1);
         quicksortf_desc(id, val,j+1,last);

    }
}

void quicksortf_asc(int* id,float* val, int first,int last)
{
    int     pivot,j,i;
    int     temp_i;
    float   temp;
    
     if(first<last){
         pivot=first;
         i=first;
         j=last;

         while(i<j){
             while(val[i]<=val[pivot]&&i<last)
                 i++;
             while(val[j]>val[pivot])
                 j--;
             if(i<j){
                 temp=val[i];
                  val[i]=val[j];
                  val[j]=temp;

				  temp_i = id[i];
				  id[i] = id[j];
				  id[j] = temp_i;
             }
         }

         temp       = val[pivot];
         val[pivot] = val[j];
         val[j]     = temp;

		 temp_i     = id[pivot];
		 id[pivot]  = id[j];
		 id[j]      = temp_i;

         quicksortf_asc(id, val,first,j-1);
         quicksortf_asc(id, val,j+1,last);

    }
}

int binarySearch(const int* sortedArray, int toFind, int len) 
{
    // Returns index of toFind in sortedArray, or -1 if not found
    int low = 0;
    int high = len - 1;
    int mid;

    int l = sortedArray[low];
    int h = sortedArray[high];

    while (l <= toFind && h >= toFind) {
        int m;

        mid = (low + high)/2;
        m = sortedArray[mid];
        
        if (m < toFind) {
            l = sortedArray[low = mid + 1];
        } else if (m > toFind) {
            h = sortedArray[high = mid - 1];
        } else {
            return mid;
        }
    }

    if (sortedArray[low] == toFind)
        return low;
    else
        return -1; // Not found
}

// int nodeSearch(const invNodeList* sortedArray, int toFind, int len) 
// {
//     // Returns index of toFind in sortedArray, or -1 if not found
//     int low = 0;
//     int high = len - 1;
//     int mid;
// 
//     int l = sortedArray->nodes[low].id;
//     int h = sortedArray->nodes[high].id;
// 
//     while (l <= toFind && h >= toFind) {
//         int m;
//         mid = (low + high)/2;
// 
//         m = sortedArray->nodes[mid].id;
// 
//         if (m < toFind) {
//             l = sortedArray->nodes[low = mid + 1].id;
//         } else if (m > toFind) {
//             h = sortedArray.nodes[high = mid - 1].id;
//         } else {
//             return mid;
//         }
//     }
// 
//     if (sortedArray->nodes[low].id == toFind)
//         return low;
//     else
//         return -1; // Not found
// }

#endif