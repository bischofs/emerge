#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>

void Merge_Sort(int * array, int left, int right);
void Merge(int * array, int left, int mid, int right);

void Merge_Process_Generator();
void Sort_Process_Generator();


int main(void){

  
  


  Merge_Process_Generator();









  return 0; 

}

void Merge_Process_Generator(){// Parent Process generator for merging

  int Number_of_Files = 8;
  int PP_nums = Number_of_Files / 2; //Compute number of parent processes 
  pid_t ppids[PP_nums]; //Parent Process numbers 


  for(int i = 0; i < PP_nums; i++){
    
    ppids[i] = fork();
  
    if(ppids[i] < 0){
      perror("Failed to fork parent processes");
    } else if ( ppids[i] == 0){
      printf("Parent merger process %i generated\n", getpid());
      //Start parent merger process code 
      Sort_Process_Generator();// generate child processes for the sort level
      exit(0);
    }

  }

  



}
void Sort_Process_Generator(){//Child parent 










}
void Merge_Sort(int * array, int left, int right)
{
  int mid = (left+right)/2;
  /* We have to sort only when left<right because when left=right it is anyhow sorted*/
  if(left<right)
    {
      /* Sort the left part */
      Merge_Sort(array,left,mid);
      /* Sort the right part */
      Merge_Sort(array,mid+1,right);
      /* Merge the two sorted parts */
      Merge(array,left,mid,right);
    }
}

void Merge(int * array, int left, int mid, int right)
{
  /*We need a Temporary array to store the new sorted part*/
  int tempArray[right-left+1];
  int pos=0,lpos = left,rpos = mid + 1;
  while(lpos <= mid && rpos <= right)
    {
      if(array[lpos] < array[rpos])
	{
	  tempArray[pos++] = array[lpos++];
	}
      else
	{
	  tempArray[pos++] = array[rpos++];
	}
    }
  while(lpos <= mid)  tempArray[pos++] = array[lpos++];
  while(rpos <= right)tempArray[pos++] = array[rpos++];
  int iter;
  /* Copy back the sorted array to the original array */
  for(iter = 0;iter < pos; iter++)
    {
      array[iter+left] = tempArray[iter];
    }
  return;
}
