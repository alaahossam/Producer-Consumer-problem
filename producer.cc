#include <iostream>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/sem.h>
#include <semaphore.h>
#include <string>
#include <random>
#include <unistd.h>
#include <vector>
#include <time.h>
#include <sys/time.h>
using namespace std;

typedef struct
{
   int index;
   double price;
   double AvgPrice;
    
}commodity;

union sem
{
    int val; // value for setvalue command for semctl
 }sem_attr;


int producer(int argc, char** argv)

{
    commodity variable;
    if(strcmp(argv[1],"ALUMINIUM")==0)
    {
        variable.index=4;
    }
    else if(strcmp(argv[1],"COPPER")==0)
    {
        variable.index=5;
    }
    else if(strcmp(argv[1],"COTTON")==0)
    {
        variable.index=6;
    }
    else if(strcmp(argv[1],"CRUDEOIL")==0)
    {
        variable.index=7;
    }
    else if(strcmp(argv[1],"GOLD")==0)
    {
        variable.index=8;
    }
    else if(strcmp(argv[1],"LEAD")==0)
    {
        variable.index=9;
    }
    else if(strcmp(argv[1],"METHANOIL")==0)
    {
        variable.index=10;
    }
    else if(strcmp(argv[1],"NATURALGAS")==0)
    {
        variable.index=11;
    }
    else if(strcmp(argv[1],"NICKEL")==0)
    {
        variable.index=12;
    }
    else if(strcmp(argv[1],"SILVER")==0)
    {
        variable.index=13;
    }
    else if(strcmp(argv[1],"ZINC")==0)
    {
        variable.index=14;
    }
    int count=atoi(argv[5]);
    default_random_engine generator;
    normal_distribution<double> distribution(atoi(argv[2]),atoi(argv[3]));
    double number;
    commodity *commodities;
    int i=0;
    int shmid;
    int semid;
    int getsem;
    key_t key = ftok("shmfile.txt",5);
    if ( (shmid = shmget (key, 0, 0)) < 0 )
    {
        if ( (shmid = shmget (key, count*sizeof(commodity), IPC_CREAT | 0666)) < 0 )
        {
            perror("shmget");
            return -1;
        }
    }
//    printf( "shmid = %d\n", shmid );
    commodities = (commodity *)shmat(shmid,(void*)0,0);
    if (commodities == (void *) -1) {
        perror("Shared memory attach: ");
        return -1;
    }
    key_t key1 = ftok("shmfile.txt",7);
    int m_semid=semget(key1, 1,0666|IPC_CREAT);	//semget(key , nsems, semflg)
	if(m_semid==-1)
	{
		perror("semget error");
		exit(1);
	}
//	printf("semget = %d\n",m_semid);
	
	sem_attr.val = 1;
	semctl(m_semid,0,SETVAL,sem_attr);
//	printf("semctl of mutex semaphore = %d\n",semctl(m_semid,0,GETVAL,sem_attr));
    key_t key2 = ftok("shmfile.txt",8);
	if(key2==-1)
	{
		perror("ftok error");
		exit(1);
	}
//	printf("ftok= %d\n",key2);
	int p_semid=semget(key2 ,1,0666|IPC_CREAT);	//semget(key , nsems, semflg)
	if(p_semid==-1)
	{
		perror("semget error");
		exit(1);
	}
	
	sem_attr.val = 0;
	semctl(p_semid,0,SETVAL,sem_attr);

    key_t key3 = ftok("shmfile.txt",9);
	if(key3 == -1)
	{
		perror("ftok error");
		exit(1);
	}

	int c_semid=semget(key3 , 1,0666|IPC_CREAT);	//semget(key , nsems, semflg)
	if(c_semid==-1)
	{
		perror("semget error");
		exit(1);
	}
	
	sem_attr.val = count;
	semctl(c_semid,0,SETVAL,sem_attr);

    struct sembuf sem;
    
    while(1)
    {
    
    	time_t now = time(NULL) ;
	struct tm tm_now ;
	struct timespec tv;
	char buff[100] ;
	
        number=abs(distribution(generator));
        variable.price=number;
        clock_gettime(CLOCK_REALTIME, &tv);
	    localtime_r(&now, &tm_now) ;
        strftime(buff, sizeof(buff), "%Y-%m-%d %H:%M:%S", &tm_now) ;
        printf("[%s.", buff) ;
	    printf("%ld] ",tv.tv_nsec);
        printf("%s: generating a new value %7.2lf \n",argv[1],variable.price);
        variable.AvgPrice=0.00;
        sem.sem_num=0;
	    sem.sem_op=-1;
	    sem.sem_flg=SEM_UNDO;
        semop(c_semid,&sem,1);
        clock_gettime(CLOCK_REALTIME, &tv);
        localtime_r(&now, &tm_now) ;
        strftime(buff, sizeof(buff), "%Y-%m-%d %H:%M:%S", &tm_now) ;
        printf("[%s.", buff) ;
        printf("%ld] ",tv.tv_nsec);
        printf("%s: trying to get mutex on shared buffer \n",argv[1]);
       // printf("empty value after wait= %d\n",semctl(c_semid,0,GETVAL,sem_attr));
        semop(m_semid,&sem,1);
      //  printf("mutex value after wait= %d\n",semctl(m_semid,0,GETVAL,sem_attr));
      	clock_gettime(CLOCK_REALTIME, &tv);
	    localtime_r(&now, &tm_now) ;
      	strftime(buff, sizeof(buff), "%Y-%m-%d %H:%M:%S", &tm_now) ;
        printf("[%s.", buff) ;
        printf("%ld] ",tv.tv_nsec);
        printf("%s: placing %7.2lf  on shared buffer\n",argv[1],variable.price);
        if(i<count)
        {
            commodities[commodities[0].index]=variable;
            commodities[0].index=commodities[0].index+1;
        }
        else
        {
            commodities[0].index=1;
            commodities[commodities[0].index]=variable;
            commodities[0].index=commodities[0].index+1;
        }
   /*     printf("Data written in memory: %d %f \n",commodities[i].index,commodities[i].price);*/
        sem.sem_num=0;
        sem.sem_op=1;
        sem.sem_flg=SEM_UNDO;
        semop(m_semid,&sem,1);
    //	printf("mutex value after sig= %d\n",semctl(m_semid,0,GETVAL,sem_attr));
        semop(p_semid,&sem,1);
    //	printf("full value after sig= %d\n",semctl(p_semid,0,GETVAL,sem_attr));
        clock_gettime(CLOCK_REALTIME, &tv);
        localtime_r(&now, &tm_now) ;
        strftime(buff, sizeof(buff), "%Y-%m-%d %H:%M:%S", &tm_now) ;
        printf("[%s.", buff) ;
        printf("%ld] ",tv.tv_nsec);
        printf("%s: sleeping for %s ms\n",argv[1], argv[4]);
        sleep(atoi(argv[4])/1000);
        i++;
    }
    shmdt(commodities); //detach from shared memory
    return 0;

}

int main(int argc, char** argv)
{
    producer(argc,argv);
    return 0;
}
