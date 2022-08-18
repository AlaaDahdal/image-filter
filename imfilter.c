#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <getopt.h>
#include <pthread.h>
#include <unistd.h>


#define N 50
#define n 250

typedef struct Task
{
    void* (*taskFunction)(char*, char*, char*);
    char *arg1, *arg2, *arg3;

} Task;

Task taskQueue[n]={0};

typedef struct
{
	double r;
	double g;
	double b;

} pixel_RGB;

void* filter(char *, char *, char *);
void executeTask(Task* );
void submit(Task ,void *);
void* start(void *);


/* start main */

int main (int argc, char *argv[]){
pthread_mutex_t mutexQueue;
pthread_cond_t condQueue;
int taskCount = 0;
int op;
int THREAD_NUM;
char input1[N]="";
char input2[N]="";
char input3[N]="";
char input4[N]="";

	if (argc<8){
	printf("Please enter eight parameters arguments\n");
	printf("\nfor exmple: ./image-filter  -i in_image.ppm -o out_image.ppm -p 4 -k 0,-1,0,-1,5,-1,0,-1,0\n");
	exit(EXIT_FAILURE);

	}
	
	while((op=getopt(argc,argv,"i:o:p:k:")) !=-1){

	switch(op){
	case 'i':
		strcpy(input1,optarg);
		break;
	case 'o':
		strcpy(input2,optarg);
		break;
	case 'p':
		strcpy(input4,optarg);
		THREAD_NUM=atoi(input4);
		printf("%d",THREAD_NUM);
                break;
	case 'k':
		strcpy(input3,optarg);
		break;

	default:
		printf("error input!!");
		break;

	}

	}
	
	
    pthread_t th[THREAD_NUM];
    pthread_mutex_init(&mutexQueue, NULL);
    pthread_cond_init(&condQueue, NULL);
    int i=0; 

 Task t =
    {
        .taskFunction = &filter,
        .arg1 = input1,
        .arg2=  input2,
        .arg3=  input3
    };
    submit(t,&taskCount);
    
    i=0;
    while(i < THREAD_NUM){
       if (pthread_create(&th[i], NULL, start, &taskCount) != 0)
         {   
             perror("Failed to create the thread");
         }
       i++;
    } 

    i=0;
    while(i < THREAD_NUM){
       if (pthread_join(th[i], NULL) != 0)
         {   
             perror("Failed to join the thread");
         }
       i++;
    }

    pthread_mutex_destroy(&mutexQueue);
    pthread_cond_destroy(&condQueue);

	return 0;
}


void executeTask(Task* task)
{
    task->taskFunction(task->arg1, task->arg2,task->arg3);
}

void submit(Task task,void *taskCount)
{
    pthread_mutex_t mutexQueue;
    pthread_cond_t condQueue;
    pthread_mutex_lock(&mutexQueue);
    taskQueue[(*(int*)taskCount)] = task;
    (*(int*)taskCount)++;
    
    pthread_mutex_unlock(&mutexQueue);
    pthread_cond_signal(&condQueue);

}

void* start(void *taskCount)
{
    while (1)
    {
        Task task;
        int found=0;
        if( ((int *)taskCount) !=NULL )
        {
            found = 1;
            task = taskQueue[0];
            for (int i = 0; i <  *((int*)taskCount)-1 ; i++)
            {
                taskQueue[i] = taskQueue[i + 1];
            }
           (*(int*)taskCount)--;
        }
	
        if (found ==1)
        {
            executeTask(&task);
        }
        return NULL;
    }
}


void* filter(char *path1, char *path2, char *path3){


FILE* output=NULL;
	output=fopen(path2,"wb");
	if(output==NULL){
	printf("\nerror\n");
	exit(EXIT_FAILURE);
	}

	FILE *fp=NULL;
	fp=fopen(path1,"r");
	if (fp==NULL){
       printf("error while opening the image");
  		 }
    double** kernel=NULL;
    kernel=calloc(1,(3)*sizeof(double* ));
   for (int i=0; i<(3); i++){

   kernel[i]=calloc(1,3*sizeof(double* ));

   }
   kernel[1][1]=1;
   char* end=NULL;
   char* string=path3;

   for(int i=0; i<3; i++){

   for(int k=0; k<3; k++){

   kernel[i][k]=strtod(string,&end);
   string=end+1;
   
   }

   }
   

   int x;
   int y;
   char p;
   unsigned int max;


   int format;
   fscanf(fp,"%c%d", &p,&format);
   fscanf(fp,"%d %d", &x,&y);
   fscanf(fp,"%u",&max);

   pixel_RGB** image=NULL;

   image=calloc(1,(y+2)*sizeof(pixel_RGB* ));
   for (int i=0; i<(y+2); i++){

   image[i]=calloc(1,(x+2)*sizeof(pixel_RGB));

   }

   for (int i=1; i<(y+1);i++){

   for (int k=1; k<(x+1); k++)
   {
   fscanf(fp,"%lf",&image[i][k].r);
   fscanf(fp,"%lf",&image[i][k].g);
   fscanf(fp,"%lf",&image[i][k].b);
   }

   }

   pixel_RGB** filterimage=NULL;

   filterimage=calloc(1,(y+2)*sizeof(pixel_RGB* ));
   for (int i=0; i<(y+2); i++){

   filterimage[i]=calloc(1,(x+2)*sizeof(pixel_RGB));

   }

   	for (int i=1; i<(y+1);i++){
   		for (int k=1; k<(x+1); k++){


  	 for (int v=0; v<3;v++){
   		for (int c=0; c<3; c++){
   		filterimage[i][k].r+=(image[i-1+v][k-1+c].r*kernel[c][v]);
   		filterimage[i][k].g+=(image[i-1+v][k-1+c].g*kernel[c][v]);
   		filterimage[i][k].b+=(image[i-1+v][k-1+c].b*kernel[c][v]);

	}}
		if(filterimage[i][k].r>(double)max) filterimage[i][k].r=(double )max;
		if(filterimage[i][k].g>(double)max) filterimage[i][k].g=(double )max;
		if(filterimage[i][k].b>(double)max) filterimage[i][k].b=(double )max;


		if(filterimage[i][k].r<0) filterimage[i][k].r=0;
		if(filterimage[i][k].g<0) filterimage[i][k].g=0;
		if(filterimage[i][k].b<0) filterimage[i][k].b=0;
	}}

   fprintf(output,"P%d\n%d %d\n%u\n",format,x,y,max);
   int count=0;

   for (int i=1; i<(y+1);i++){

   for (int k=1; k<(x+1); k++)
   {
   fprintf(output,"%u ",(unsigned int)filterimage[i][k].r);
   fprintf(output,"%u ",(unsigned int)filterimage[i][k].g);
   fprintf(output,"%u ",(unsigned int)filterimage[i][k].b);
   count++;
   if (count%4==0)fprintf(output,"\n");
   }

   }

   fclose(fp);
   free(image);
   free(kernel);
   fclose(output);

   return NULL;

}
