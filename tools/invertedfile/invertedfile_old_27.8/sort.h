void quicksort(int* id,int* val, int first,int last)
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

         quicksort(id, val,first,j-1);
         quicksort(id, val,j+1,last);

    }
}

void quicksortf(int* id,float* val, int first,int last)
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

         quicksortf(id, val,first,j-1);
         quicksortf(id, val,j+1,last);

    }
}

void quicksortf_dist(int* id,float* val, int first,int last)
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

         quicksortf(id, val,first,j-1);
         quicksortf(id, val,j+1,last);

    }
}