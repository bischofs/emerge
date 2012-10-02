#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>

#define num_files 8

void Merge_Sort(int * array, int left, int right);
void Merge(int * array, int left, int mid, int right);

void Merge_Process_Generator();
void Sort_Process_Generator(FILE * file1, FILE * file2);
void File_read_sort(FILE * file);

FILE * files [num_files];

int main(void){
  char fname[16];

  for(int i = 0;i < num_files;i++){// Loop to open all files for reading with name "filen" in current dir
    sprintf(fname,"file%d",i);
    files[i]= fopen(fname, "r");
    if (files[i] == NULL)
      {
	printf("Can't open %s \n",fname);
	exit(1);
      }    
  }



  Merge_Process_Generator();

  sleep(15);

  return 0; 

}
/*
Parent Process Generator
Called by main to generate n/2 file merging parent processes
*/
void Merge_Process_Generator(){

  int Number_of_Files = num_files;
  int PP_nums = Number_of_Files / 2; //Compute number of parent processes 
  pid_t ppids[PP_nums]; //Parent Process numbers 


  for(int i = 0,j = 0; i < PP_nums; i++,j+=2){
    
    ppids[i] = fork();
  
    if(ppids[i] < 0){
      perror("Failed to fork parent processes");
    } else if ( ppids[i] == 0){
      //printf("Parent merger process %i generated\n", getpid());
      //Start parent merger process code 
      Sort_Process_Generator(files[j],files[j+1]);// generate child processes for the sort level
      exit(0);
    }

  }


}
/*
**Child process generator**

Called by parent process and recieves the two files for each 
child sorting process
*/
void Sort_Process_Generator(FILE * file1, FILE * file2 ){ 
 
  pid_t cpids[3];
  int i = 0;
  //set pipe to write
  //open file


  for(i = 0; i < 2; i++){
     
    cpids[i] = fork();

    if(cpids[i] < 0){
      perror("Failed to fork parent processes");
    } else if ( cpids[i] == 0){
      //printf("Child of pid %i sorter process generated\n", getppid());
      if(i == 0){

	File_read_sort(file1);

      }else if (i == 1){
	
	File_read_sort(file2);

      }

      //send individual file to read_sort
        exit(0);
    }
    
  }


}
void File_read_sort(FILE * file){

  char name[16];

  fgets(name, 16, file);

  printf("%s",name);



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
