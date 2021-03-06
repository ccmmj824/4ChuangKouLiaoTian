#include<func.h>
long pid_arr[2]={0};
//存储a1 ,a.
void  sigfunc2(int signum)
{
    //发送10号信号给 pid_arr 1 2 3 0;
    kill(pid_arr[1],10);
    kill(pid_arr[0],10);
}
void sigfunc10(int signum)
{
    kill(pid_arr[0],9);
}
int main(int argc,char **argv)
{   
    //PID
    pid_arr[0]=getpid();
    //维护消息
    typedef struct{
        long mtype;
        char buf[4088];
    }my_msg;
    //维护时间
    time_t t;
    time(&t);
    //维护消息信号量    
    int sem_arrid=semget(1000,2,IPC_CREAT|0777);
    ERROR_CHECK(sem_arrid,-1,"semget");
    struct sembuf P;
    P.sem_num=0;
    P.sem_op=-1;
    P.sem_flg=SEM_UNDO;
    //维护pid信号量
    struct sembuf pidsnd[2];
    pidsnd[0].sem_num=1;
    pidsnd[0].sem_op=-1;
    pidsnd[0].sem_flg=SEM_UNDO;
    pidsnd[1].sem_num=1;
    pidsnd[1].sem_op=1;
    pidsnd[1].sem_flg=SEM_UNDO;
    //负责接收消息并打印
    int shmid = shmget(1000,4096,IPC_CREAT|0777);
    ERROR_CHECK(shmid,-1,"shmget");
    my_msg *p=(my_msg *)shmat(shmid,NULL,0);
    //接收a.pid
    int ret=semop(sem_arrid,&pidsnd[0],1);
    ERROR_CHECK(ret,-1,"semop");
    pid_arr[1]=(long)p->mtype;
    
    signal(2,sigfunc2);
    signal(10,sigfunc10);
    while(1)
    {
        semop(sem_arrid,&P,1);
        if(p->mtype==1)//来自B的消息
        {
            printf("~b~%s",ctime(&t));
            write(STDOUT_FILENO,p,4096);
        }
        if(p->mtype==2)//来自A的消息
        {
            printf("                                  ~a~%s",ctime(&t));
            printf("                                  %s",p->buf);
        }
        
    }
    return 0;
}

