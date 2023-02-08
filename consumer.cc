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

using namespace std;

typedef struct
{
   int index;
   double price;
   double AvgPrice;
    
}commodity;

union sem{
	int val; // value for setvalue command for semctl
}sem_attr;

void print_dashboard(std::vector<std::string>commodities)
{
    printf("+");
    for(int i=0; i<35;i++)
    {
        printf("-");
    }
    printf("+\n| Currency   |  Price   | AvgPrice  |\n+");
    for(int i=0; i<35;i++)
    {
        printf("-");
    }
    printf("+\n");
    for(int i=0;i<11;i++)
    {
        printf("| %-10s |\033[;34m%7.2lf  \033[0m |\033[;34m%7.2lf  \033[0m  |\n",commodities[i].c_str(),0.00,0.00);
    }
    printf("+");
    for(int i=0; i<35;i++)
    {
        printf("-");
    }
    printf("+\n");
}
void modify(commodity c,vector<double> price_list,vector<double> Avg)
{
    std::vector<std::string>commodities{"ALUMINIUM","COPPER","COTTON","CRUDEOIL","GOLD","LEAD","MENTHAOIL","NATURALGAS","NICKEL","SILVER","ZINC"};
    int n=price_list.size();
    int m=Avg.size();
    if(n>=2 && m>=2)
    {
        if(price_list[n-1]<price_list[n-2] && Avg[m-1]>Avg[m-2])
        {
            printf("\033[1;31m \033[%d;15H%7.2lf\u2193 \033[0m |\033[;32m%7.2lf\u2191 \033[0m \n",c.index,c.price,c.AvgPrice);
        }
        if(price_list[n-1]<price_list[n-2] && Avg[m-1]<Avg[m-2])
        {
            printf("\033[1;31m \033[%d;15H%7.2lf\u2193 \033[0m |\033[1;31m%7.2lf\u2193 \033[0m \n",c.index,c.price,c.AvgPrice);
        }
        if(price_list[n-1]>price_list[n-2] && Avg[m-1]>Avg[m-2])
        {
            printf("\033[;32m \033[%d;15H%7.2lf\u2191 \033[0m |\033[;32m%7.2lf\u2191 \33[0m \n",c.index,c.price,c.AvgPrice);
        }
        if(price_list[n-1]>price_list[n-2] && Avg[m-1]<Avg[m-2])
        {
            printf("\033[;32m \033[%d;15H%7.2lf\u2191 \033[0m |\033[1;31m%7.2lf\u2193 \033[0m \n",c.index,c.price,c.AvgPrice);
        }
    }
    else
    {
        printf("\033[;32m \033[%d;15H%7.2lf\u2191 \033[0m |\033[;32m%7.2lf\u2191 \033[0m \n",c.index,c.price,c.AvgPrice);
    }
    printf("\033[16;15H \n");

}
int consumer(int argc, char** argv)
{
    int i=1;
    int count= atoi(argv[1]);
    commodity test;
    commodity initialize;
    initialize.index=1;
    int shmid;
    key_t key = ftok("shmfile.txt",5);
    if ( (shmid = shmget (key, 0, 0)) < 0 )
    {
        if ( (shmid = shmget (key, (count+1)*sizeof(commodity), IPC_CREAT | 0666)) < 0 )
        {
            perror("shmget");
            return -1;
        }
    }
    commodity *commodities = (commodity *)shmat(shmid,(void*)0,0);
    vector<double> price_list[11];
    vector<double> Avg[11];
    if (commodities == (void *) -1) {
        perror("Shared memory attach: ");
        return -1;
    }
    commodities[0]=initialize;
    key_t key1 = ftok("shmfile.txt",7);
    int m_semid=semget(key1, 1,0666|IPC_CREAT);	//semget(key , nsems, semflg)
	if(m_semid==-1)
	{
		perror("semget error");
		exit(1);
	}
	sem_attr.val = 1;
	semctl(m_semid,0,SETVAL,sem_attr);
    key_t key2 = ftok("shmfile.txt",8);
	if(key2==-1)
	{
		perror("ftok error");
		exit(1);
	}
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
        sem.sem_num=0;
		sem.sem_op=-1;
		sem.sem_flg=SEM_UNDO;
		semop(p_semid,&sem,1);
		semop(m_semid,&sem,1);
        if(i<(count+1))
        {
            test=commodities[i];
        }
        else
        {
            i=1;
            test=commodities[i];
        }
        if(price_list[commodities[i].index-4].size()<5)
        {
            price_list[commodities[i].index-4].push_back(commodities[i].price);
        }
        else
        {
            price_list[commodities[i].index-4].erase(price_list[commodities[i].index-4].begin());
            price_list[commodities[i].index-4].push_back(commodities[i].price);
        }
        double sum=0.0;
        for(int k=0;k<price_list[commodities[i].index-4].size();k++)
        {
            sum=sum+price_list[commodities[i].index-4][k];
        }
        test.AvgPrice=sum/(price_list[commodities[i].index-4].size());
        if(Avg[commodities[i].index-4].size()<2)
        {
            Avg[commodities[i].index-4].push_back(test.AvgPrice);
        }
        else
        {
            Avg[commodities[i].index-4].erase(Avg[commodities[i].index-4].begin());
            Avg[commodities[i].index-4].push_back(test.AvgPrice);
        }
        sem.sem_op=1;
		semop(m_semid,&sem,1);
		semop(c_semid,&sem,1);
        modify(test,price_list[commodities[i].index-4],Avg[commodities[i].index-4]);
        i++;
    }

    shmdt(commodities); //detach from shared memory
    shmctl(shmid,IPC_RMID,NULL); // destroy the shared memory
    return 0;
}

int main(int argc, char** argv)
{
    printf("\e[1;1H\e[2J");
    std::vector<std::string>commodities{"ALUMINIUM","COPPER","COTTON","CRUDEOIL","GOLD","LEAD","MENTHAOIL","NATURALGAS","NICKEL","SILVER","ZINC"};
    print_dashboard(commodities);
    consumer(argc,argv);
    return 0;
}
