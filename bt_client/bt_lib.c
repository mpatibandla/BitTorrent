#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h> //internet address library
#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <arpa/inet.h>
#include <openssl/sha.h> //hashing pieces
#include "bencode.h"
#include "bt_lib.h"
#include "bt_setup.h"
void calc_id(char * ip, unsigned short port, char *id)
{
char data[256];
int len;
  
  //format print
len = snprintf(data,256,"%s%u",ip,port);
//id is just the SHA1 of the ip and port string
//SHA1((unsigned char *) data, len, (unsigned char *) id); 
return;
}

int log_write(log_info * log){
  float ms;
  char time[10];
  int time_len;
  gettimeofday(&(log->cur_tv),NULL);
  ms = (log->cur_tv.tv_sec - log->start_tv.tv_sec)*1000;
  ms += (log->cur_tv.tv_usec - log->start_tv.tv_usec)/1000;
  time_len = snprintf(time,10,"%.2f ",ms);
  int fw = fwrite(time,time_len,1,log->log_file);
  fw = fwrite(log->logmsg,log->len,1,log->log_file);
  fflush(log->log_file);
  return fw;
}
/**
 * init_peer(peer_t * peer, int id, char * ip, unsigned short port) -> int
 *
 *
 * initialize the peer_t structure peer with an id, ip address, and a
 * port. Further, it will set up the sockaddr such that a socket
 * connection can be more easily established.
 *
 * Return: 0 on success, negative values on failure. Will exit on bad
 * ip address.
 *   
 **/
int init_peer(peer_t *peer, char * id, char * ip, unsigned short port)
{
struct hostent * hostinfo;
//set the host id and port for referece
memcpy(peer->id, id, ID_SIZE);
peer->port = port;
//get the host by name
if((hostinfo = gethostbyname(ip)) ==  NULL)
{
perror("gethostbyname failure, no such host?");
herror("gethostbyname");
exit(1);
}
//zero out the sock address
bzero(&(peer->sockaddr), sizeof(peer->sockaddr));
//set the family to AF_INET, i.e., Iternet Addressing
peer->sockaddr.sin_family = AF_INET;
//copy the address to the right place
bcopy((char *) (hostinfo->h_addr), 
        (char *) &(peer->sockaddr.sin_addr.s_addr),
        hostinfo->h_length);
//encode the port
peer->sockaddr.sin_port = htons(port);
return 0;
}
/**
 * print_peer(peer_t *peer) -> void
 *
 * print out debug info of a peer
 *
 **/
int acceptpeer(int sockfd,char *sha1, char * msg, char *rmsg,int * new, log_info * log, peer_t * peer)
{



       //waitingg...
             int client;//accepting new peer
             struct sockaddr_in client_addr;
             socklen_t client_addr_len;
             printf("waiting for conn:");
             client=accept(sockfd,(struct sockaddr *)&client_addr,&client_addr_len);
       
             fprintf(stderr,"conn established");
if(client==-1){
perror("Accept New Peer Failed");
    log->len = snprintf(log->logmsg,100,"HANDSHAKE FAILED accept failed\n");
    log_write(log);

    return 1;
  }
              //   char *sha1;
              //sha1=track.announce;
              /*parintf("sha1:\n");
              int j;i
              for(j=0;j<20;j++)
              printf("%c",sha1[j]);
              gethandshake(peer,sha1,msg);
              printf("handshake\n");
              for(j=0;j<68;j++)
              printf("%c",msg[j]);*/
              //gethandshake(peer,sha1,msg);
              //char shapieces[20];
              //SHA1(&(track.name),FILE_NAME_MAX,shapieces[0]);
              char self_id[]="1440";
              bzero(msg,MSG_LEN);
              msg[0]=19;
              strcpy(&(msg[1]),"BitTorrent Protocol");
              /*memset(msg+20,0,8);
              memcpy(sha1,msg+28,20);
              memcpy(sha1,msg+48,20);*/
              memset(&(msg[20]),0,8);
              memcpy(&(msg[28]),sha1,20);
              memcpy(&(msg[48]),self_id,20);
int rh_ret = read_handshake(client,rmsg,msg);
  char * ip;
  int port;
  char id[20];
  ip = inet_ntoa(client_addr.sin_addr);
  port = htons(client_addr.sin_port);
  calc_id(ip,port,id);
  id[20] = '\0';

  if(rh_ret){ 
printf("read handshake could not be done");
log->len = snprintf(log->logmsg,100,"HANDSHAKE FAILED peer:%s port:%d id:i%20s\n",
        ip,port,id);
    log_write(log);

    return 1;
  }
   /*           int hread=read(client,rmsg,68);
              if(hread){
              printf("incorrect handshake");
              //continue;
              }*/
              int sent=send(client,msg,68,0);
              //for(j=0;j<68;j++)
              //printf("%c",rmsg[j]);
              if(sent!=MSG_LEN){
              fprintf(stderr,"handshake sending error");
log->len = snprintf(log->logmsg,100,"HANDSHAKE SEND FAILED peer:%s port:%d id:%20s\n",
        ip,port,id);
    log_write(log);
    return 1;
  }   
//peer_t * peer;
peer=malloc(sizeof(peer_t *));
//char id[20];
//char *ip=inet_ntoa(client_addr.sin_addr);
//int port=htons(client_addr.sin_port);
//calc_id(ip,port,id);
init_peer(peer,id,ip,port);
*new=client;
log->len = snprintf(log->logmsg,100,"HANDSHAKE SUCCESS peer:%s port:%d id:%20s\n" ,inet_ntoa(peer->sockaddr.sin_addr),peer->port,id);
  log_write(log);
  return 0;
}
//return client;
// return client;   

void print_peer(peer_t * peer)
{
int i;
if(peer)
{
printf("peer: %s:%u ",
           inet_ntoa(peer->sockaddr.sin_addr),
           peer->port);
printf("id: ");
for(i=0;i<ID_SIZE;i++)
{
printf("%02x",peer->id[i]);
}
printf("\n");
}
}
//second edit 26/10/2013
void gethandshake(peer_t * peer,char * sha1, char * message)
{

message[0]=19;
//&(message[1])="BitTorrent Protocol";
strcpy(&(message[1]),"BitTorrent Protocol");
memset(&(message[20]),0,8);
memcpy(sha1,&(message[28]),20);
memcpy(peer->id,&(message[48]),20);
return;
}
int addpeer(peer_t *peer, bt_args_t *bt_Args,char *hname,int port)
{
return 0;
}
