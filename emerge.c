/*
 * PARRALEL EXTERNAL MERGE SORT
 * Steve Bischoff
 * OS - Wolffe
 */

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <limits.h>

#define NUM_FILES 8 //Number of files in the current directory
#define MAX_INTS 20 //Maximum Number of integers in a file
#define MAX_INT_S 65534 //Maximum integer size

void Merge_Sort(int * array, int left, int right);
void Merge(int * array, int left, int mid, int right);

void Merge_Process_Generator();
void Sort_Process_Generator(FILE * file1, FILE * file2, int * merged);
int File_read_sort(FILE * file, int * array,int size);
int Comp(const int * a,const int * b); 

FILE * files [NUM_FILES];
FILE * master;


int main(void){
  char fname[16];

  for(int i = 0;i < NUM_FILES;i++){// Loop to open all files for reading with name "filen" in current dir
    sprintf(fname,"file%d",i);
    files[i]= fopen(fname, "r");
    if (files[i] == NULL)
      {
	printf("Can't open %s \n",fname);
	exit(1);
      }    
  }

 
  Merge_Process_Generator();



  return 0; 

}
/*
************* Parent Process Generator ********************
Called by main to generate n/2 file merging parent processes
*/
void Merge_Process_Generator(){


  int Number_of_Files = NUM_FILES;
  int PP_nums = Number_of_Files / 2; //Compute number of parent processes 
  pid_t ppids[PP_nums]; //Parent Process numbers 

  int fds[PP_nums][2];
  int length = NUM_FILES * MAX_INTS;// Max size of final array
  int merged_l1[MAX_INTS * 2]={0};// merge level one array to pass to parents  
  int merged_l0[NUM_FILES * MAX_INTS];//merge level 0 master process 

  
  for(int k = 0; k < length; k++){
    merged_l0[k] = MAX_INT_S;
  }

  for(int i = 0,j = 0; i < PP_nums; i++,j+=2){
    
    pipe(fds[i]); //build pipes off of parents    
    
    ppids[i] = fork();
  

    
    if(ppids[i] < 0){
    
      perror("Failed to fork parent processes");

    } 
    else if ( ppids[i] == 0){//PARENT MERGER LEVEL 1*******************************************************************************
      printf("Parent merger process %i generated\n", getpid());
      //Start parent merger process code 
      
      close(fds[i][0]);//close read end     

      Sort_Process_Generator(files[j],files[j+1],merged_l1);// generate child processes for the sort level

      write(fds[i][1] , &merged_l1 , sizeof(merged_l1));

      close(fds[i][1]);//close write end


      exit(0);
    }//****************************************************************************************************************************
   
  }


 //MASTER MERGER LEVEL 0***************************************************************************************************


  int readbuffer[length];
  int tmpbuffer[length];


    
  
  for(int i = 0;i < PP_nums;i++){
    
    sleep(1);
    close(fds[i][1]);//close write end
    
    read(fds[i][0], &readbuffer, sizeof(readbuffer));//read in buffer from parents
  
    close(fds[i][0]); //close read end

    for(int i1=0;i1<length;i1++){// clean up the buffer from the lower level
      
      if( readbuffer[i1] == MAX_INT_S){
	
	for(int s=i1;s < length;s++){

	  readbuffer[s] = MAX_INT_S;
	  
	}
      
      }
   
    } 
         
    for(int j=0;j<length;j++){//copy current merged array into a temporary array
      tmpbuffer[j]=merged_l0[j];
    }
    
           
    for(int i2=0,j2=0,k2=0;k2<length;k2++){//MERGER code for 1st level
    
      if(readbuffer[i2] < tmpbuffer[j2]){
	merged_l0[k2]=readbuffer[i2];
	i2++;
	
      }
      else {
	merged_l0[k2]=tmpbuffer[j2];
	j2++;
      }
      
    }

  }


  printf("Master Merger level 1 data recieved");
  printf(" . ");
  sleep(1);
  printf(". ");
  sleep(1); 
  printf(". merging and saving to master file\n");  
  
  
  char * name = "Master_File";

  master = fopen(name,"w");

  for(int i = 0;i < length;i++){

    if(merged_l0[i] == MAX_INT_S){
      break;
    }

    fprintf(master,"%i\n",merged_l0[i]);

  }


//******************************************************************************************************************************
    
}

/*
******************* Child process generator *************************

Called by parent process and recieves the two files for each 
child sorting process
*/
void Sort_Process_Generator(FILE * file1, FILE * file2, int * merged ){ 
 
  pid_t cpids[3];
  int i = 0;
  int fd[2],fd2[2];

  pipe(fd); //construct two pipes, one for each sorting children
  pipe(fd2);
 

  for(i = 0; i < 2; i++){
     
    cpids[i] = fork();

    if(cpids[i] < 0){
     
      perror("Failed to fork child processes");
    
    } 
    else if ( cpids[i] == 0){// CHILD SORT LEVEL **********************************************************************************************
     
      printf("Child of pid %i sorter process generated\n", getppid());

      if(i == 0){

	int numbers[MAX_INTS]; // strange compiler addon, fills entire array with 16b - 1. use it for a terminating character	
	
	for(int k = 0; k <  MAX_INTS; k++){
	  numbers[k] = MAX_INT_S;
	}
       
	
	close(fd[0]);
	

	
	File_read_sort(file1, numbers,MAX_INTS);
	
	write(fd[1], &numbers, sizeof(numbers));

	close(fd[1]);
	
	exit(0);
     
      }else if (i == 1){

	int numbers1[MAX_INTS];

	

	for(int k = 0; k <  MAX_INTS; k++){
	  numbers1[k] = MAX_INT_S;
	}


	close(fd2[0]);


	File_read_sort(file2, numbers1,MAX_INTS);

	write(fd2[1], &numbers1, sizeof(numbers1));

	close(fd2[1]);

	exit(0);
      }
      
      
      
    }//***************************************************************************************************************************************************
  }


  int readbuffer [MAX_INTS]={0};
  int readbuffer1 [MAX_INTS]={0};
  
  close(fd[1]);
  close(fd2[1]);  

  read(fd[0], &readbuffer, sizeof(readbuffer));

  read(fd2[0], &readbuffer1, sizeof(readbuffer1));

  close(fd[0]);
  close(fd2[0]);    


  int k,j;
  
  printf("Merger level 2 data recieved");
  printf(" . ");
  sleep(1);
  printf(". ");
  sleep(1); 
  printf(". merging and sending to master process\n");


  for(i=0,j=0,k=0;k<(MAX_INTS * 2);k++){//MERGER code for 1st level
    
    if(readbuffer[i] < readbuffer1[j]){
      merged[k]=readbuffer[i];
      i++;
      
    }
    else {
      merged[k]=readbuffer1[j];
      j++;
    }

  }



}
int File_read_sort(FILE * file, int * array, int size){

  int i = 0;


 while(!feof(file)){
   fscanf(file, "%d", &array[i]);
   i++;

 }
 
 printf(". ");
 sleep(1);
 printf(". ");
 sleep(1);
 printf(". file sorted...sending data upstream\n");
 
  qsort(array,size,sizeof(int),Comp);

  fclose(file);

  return i;


  //i represents the length of data to be piped up 
}

int Comp(const int * a,const int * b) 
{
  if (*a==*b)
    return 0;
  else
    if (*a < *b)
      return -1;
    else
      return 1;
}
