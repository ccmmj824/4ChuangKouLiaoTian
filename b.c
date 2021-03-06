#include<func.h>
//维护pid
long pid_arr[2]={0};
//存储b和b1;
void sigfunc2(int signum)
{
    //发送10好信号给pid_arr1 2 3 0
}
void sigfunc10(int signum)
{
    //关闭管道读端，删除管道atob。
    //删除消息队列
    kill(pid_arr[1],10);
    kill(pid_arr[0],9);
}                              

int main(int argc,char **argv)
{
    pid_arr[0]=getpid();
    //消息信息
    typedef struct{
        long mtype;
        char buf[4096];
        //mtype: 1：a输入 2：b输入  3. pid
    }msgbuf;
    //b 和 b1 的消息队列通信
    //创建并维护消息队列
    int msqid=msgget(1000,0777|IPC_CREAT);
    //接收B1的pid
    msgbuf msg;
    printf("i will get\n");
    int ret=msgrcv(msqid,&msg,sizeof(msg),0,0);
    pid_arr[1]=msg.mtype;
    printf("B1_PID =%ld",pid_arr[1]);
    printf("i have get\n");
    //a和b之间有名管道通信"btoa.pip"
    ret= mkfifo("btoa.pip",0777);
    ERROR_CHECK(ret,-1,"mkfifo");
    int fd_rdfifo=open("atob.pip",O_RDONLY);
    ERROR_CHECK(fd_rdfifo,-1,"atob.pip");
    int fd_wrfifo=open("btoa.pip",O_WRONLY);
    ERROR_CHECK(fd_wrfifo,-1,"btoa,pip");
    //发送pid给A
    msg.mtype=pid_arr[0];
    write(fd_wrfifo,&msg,sizeof(long));
    printf("i have send pid tp A\n");
    //创建两个缓冲区,分别来自键盘和a
    msgbuf b_msg;
    msgbuf a_msg;
    //IO多路复用
    fd_set rdset;
    signal(10,sigfunc10);
    while(1)
    {
        int msg_len=0;
        FD_ZERO(&rdset);
        FD_SET(STDIN_FILENO,&rdset);
        FD_SET(fd_rdfifo,&rdset);
        select(fd_rdfifo+1,&rdset,NULL,NULL,NULL);
        //b键盘输入的信息
        if(FD_ISSET(STDIN_FILENO,&rdset))
        {
            memset(&b_msg,0,sizeof(msgbuf));
            b_msg.mtype=2;
            int len=read(STDIN_FILENO,b_msg.buf,sizeof(b_msg.buf));
            //往消息队列放
            int ret=msgsnd(msqid,&b_msg,sizeof(b_msg.buf),0);
            ERROR_CHECK(ret,-1,"msgsndb.buf");
            //往管道放
            ret=write(fd_wrfifo,b_msg.buf,len);
            ERROR_CHECK(ret,-1,"write");
            
        }
        if(FD_ISSET(fd_rdfifo,&rdset))
        {
            //b接收a的信息
            memset(&a_msg,0,sizeof(msgbuf));
            a_msg.mtype=1;
            int len=read(fd_rdfifo,a_msg.buf,sizeof(a_msg.buf));
            ERROR_CHECK(len,-1,"read");
            //往消息队列放
            int ret=msgsnd(msqid,&a_msg,sizeof(a_msg.buf),0);
            ERROR_CHECK(ret,-1,"msgsnda.buf");
        }

    }
    return 0;
}

