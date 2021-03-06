#include<func.h>
//维护pid
long pid_arr[2]={0};
//存储a,和b:
void sigfunc2(int signum)
{
    //发送10好信号给pid_arr1 2 3 0
}
void sigfunc10(int signum)
{
    //关闭管道读端，删除管道btoa。
    //删除共享内存
    
    //删除信号量
    printf("i ha ve ge t signal\n");
    kill(pid_arr[1],10);
    unlink("atob.pip");
    unlink("btoa.pip");
    kill(pid_arr[0],9);
}
int main(int argc,char **argv)
{
    pid_arr[0]=getpid();
    //消息信息
    typedef struct{
        long mtype;
        char buf[4088];
        //flag: 1：b输入 2：a输入
    }my_msg;
    //a 和 a1 的共享内存通信
    //1.创建维护信号量
    int sem_arrid=semget(1000,2,IPC_CREAT|0777);
    ERROR_CHECK(sem_arrid,-1,"semget");
    unsigned short res[2]={0};
    int ret=semctl(sem_arrid,0,SETALL,res);
    ERROR_CHECK(ret,-1,"semctl");
    //维护消息信号量
    struct sembuf V;
    V.sem_num=0;
    V.sem_op=1;
    V.sem_flg=SEM_UNDO;
    //维护pid信号量
    struct sembuf pidsnd[2];
    pidsnd[0].sem_num=1;
    pidsnd[0].sem_op=-1;
    pidsnd[0].sem_flg=SEM_UNDO;
    pidsnd[1].sem_num=1;
    pidsnd[1].sem_op=1; 
    pidsnd[1].sem_flg=SEM_UNDO;
    //2.创建并维护共享内存
    int shmid=shmget(1000,4096,IPC_CREAT|0777);
    ERROR_CHECK(shmid,-1,"shmget");
    my_msg *p =(my_msg *)shmat(shmid,NULL,0);
    memset(p,0,4096);
    //发送pid给a1.c
    my_msg pid_buf={0,{0}};
    pid_buf.mtype=getpid();
    memset(p,0,4096);
    memcpy(p,&pid_buf,sizeof(my_msg));
    ret=semop(sem_arrid,&pidsnd[1],1);
    ERROR_CHECK(ret,-1,"semop");
    
    //a和b之间有名管道通信"atobpip"
    ret= mkfifo("atob.pip",0777);
    ERROR_CHECK(ret,-1,"mkfifo");
    int fd_wrfifo=open("atob.pip",O_WRONLY);
    ERROR_CHECK(fd_wrfifo,-1,"open,atob");
    int fd_rdfifo=open("btoa.pip",O_RDONLY);
    ERROR_CHECK(fd_rdfifo,-1,"open,btoa");
    read(fd_rdfifo,&pid_arr[1],sizeof(long));
    printf("B's pid =%ld\n",pid_arr[1]);
    //创建两个缓冲区,分别来自键盘和B
    my_msg a_msg;
    my_msg b_msg;

    fd_set rdset;
    signal(10,sigfunc10);
    while(1)
    {
        int msg_len=0;
        FD_ZERO(&rdset);
        FD_SET(STDIN_FILENO,&rdset);
        FD_SET(fd_rdfifo,&rdset);
        select(fd_rdfifo+1,&rdset,NULL,NULL,NULL);
        //a键盘输入的信息
        if(FD_ISSET(STDIN_FILENO,&rdset))
        {
            memset(&a_msg,0,sizeof(a_msg));
            a_msg.mtype=2;
            int len=read(STDIN_FILENO,a_msg.buf,sizeof(a_msg.buf));
            //往管道放
            write(fd_wrfifo,&a_msg,len+8);
            //往共享内存放
            memset(p,0,4096);
            memcpy(p,&a_msg,len+8);
            ret=semop(sem_arrid,&V,1);
            ERROR_CHECK(ret,-1,"semop");
        }
        if(FD_ISSET(fd_rdfifo,&rdset))
        {
            memset(&b_msg,0,sizeof(b_msg));
            b_msg.mtype=1;
            int len=read(fd_rdfifo,b_msg.buf,sizeof(b_msg.buf));
            //往共享内存放
            memset(p,0,4096);
            memcpy(p,&b_msg,len+8);
            ret=semop(sem_arrid,&V,1);
            ERROR_CHECK(ret,-1,"semop");
        }
    }
    return 0;
}

