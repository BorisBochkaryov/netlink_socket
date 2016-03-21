#include <sys/socket.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <linux/netlink.h>
#include <linux/rtnetlink.h>

int main(int argc, char *argv[]){
  int sock;
  // создаем сокет
  if((sock = socket(AF_NETLINK, SOCK_DGRAM, NETLINK_ROUTE)) == -1){
    perror("Error socket()");
    return -1;
  }
  
  // структура адреса
  struct sockaddr_nl addr; 
  addr.nl_family = AF_NETLINK;
  addr.nl_pid = getpid();
  addr.nl_groups = RTMGRP_IPV4_ROUTE;

  // привязываем сокет
  if (bind(sock,(struct sockaddr *)&addr,sizeof(addr))==-1){
    perror("Error bind()");
    return -1;
  }

  while (1){ 
    struct msghdr msg; // сообщение
    struct iovec iov[1]; // вектор данных
    struct nlmsghdr nlh; // сообщение от NetLink socket
    
    iov[0].iov_base = (void*)&nlh;
    iov[0].iov_len = sizeof(nlh);

    // заполняем структуру сообщения
    msg.msg_name = (void*)&addr;
    msg.msg_namelen = sizeof(addr);
    msg.msg_iov = iov;
    msg.msg_iovlen = sizeof(iov)/sizeof(struct iovec);

    // принимаем сообщения от kernal
    if(recvmsg(sock, &msg, 0)){
      if(nlh.nlmsg_type == RTM_NEWROUTE){ // если было подключение интерфейса
        printf ("Новое подключение!\n");
      } else if(nlh.nlmsg_type == RTM_DELROUTE){ // если был отключен интерфейс
        printf ("Удалено подключение!\n");
      }
    }
  }
  close(sock);
  return 0;
}
