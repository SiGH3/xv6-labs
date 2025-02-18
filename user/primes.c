// user/primes.c
#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

//筛选质数的函数，接受一个管道作为参数
void sieve(int pleft[2]){
    //从左邻居读数
    int p;
    read(pleft[0], &p, sizeof(p));
    if(p == -1){
        exit(0);  //如果读取到-1，表示结束，退出进程
    }
    printf("prime %d\n", p); //此时接收到的数字一定是质数

    //创建一个新的管道
    int pright[2];
    pipe(pright);

    if(fork()==0){     //右邻居
        close(pright[1]);  //右邻居用不到此管道的写端，关闭
        close(pleft[0]);   //右邻居用不到此管道的读端，关闭
        sieve(pright);   //递归调用筛选函数
    }
    else{
        close(pright[0]);  //当前进程用不到此管道的读端，关闭
        //从左邻居接受数字
        int buf;
        while(read(pleft[0],&buf,sizeof(buf)) && buf!= -1){
            if(buf%p!=0){  //如果接收到的数字不是第一次接收到的数字的倍数
                write(pright[1],&buf,sizeof(buf));  //往管道中给右邻居写入这个数字
            }
        }

        //此时接收到了左邻居传来的-1，要给右邻居也传-1，结束右邻居进程
        buf = -1;
        write(pright[1],&buf,sizeof(buf));
        wait(0);
        exit(0);
    }
}

int main(int argc,char **argv){
    //创建初始管道
    int input_pipe[2];
    pipe(input_pipe);

    if(fork()==0){
        close(input_pipe[1]);
        sieve(input_pipe);
        exit(0);
    }
    else{
        close(input_pipe[0]);
        int i;
        for(i=2;i<=35;i++){
            write(input_pipe[1],&i,sizeof(i));
        }
        i = -1;
        write(input_pipe[1],&i,sizeof(i));
    }
    wait(0);

    exit(0);
}