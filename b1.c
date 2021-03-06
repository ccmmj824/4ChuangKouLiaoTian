#include<func.h>
void sigfunc10(int signum)
{
    kill(getpid(),9);
}
int main(int argc,char **argv)
{
    //维护消息队列
    int msqid=msgget(1000,0777|IPC_CREAT);
    ERROR_CHECK(msqid,-1,"msgget");
    //维护消息
    typedef struct{
        long mtype;
        char buf[4088];
    }my_msg;
    my_msg msg;
    memset(&msg,0,sizeof(my_msg));
    //给B发PID
    msg.mtype=getpid();
    printf("i will send\n");
    msgsnd(msqid,&msg,sizeof(msg),0);
    printf("i have send\n");
    //维护时间
    time_t t;
    time(&t);
    signal(10,sigfunc10);
    while(1)
    {
        int ret=msgrcv(msqid,&msg,sizeof(msg),0,0);
        ERROR_CHECK(ret,-1,"msgrcv");
        if(msg.mtype==1)//来自A的消息
        {
            printf("~a~%s",ctime(&t));
            write(STDOUT_FILENO,msg.buf,ret);
            //printf("%s\n",msg.buf);
        }
        if(msg.mtype==2)//来自B的消息
        {
            printf("                                  ~b~%s",ctime(&t));
            printf("                                  %s",msg.buf);
        }
    }
    return 0;

}



