#include <stdio.h>

#include <stdlib.h>

#include <unistd.h>

#include <string.h>

#include <netinet/in.h>

#include <netinet/ip.h> 
//ip hdeader library (must come before ip_icmp.h)

#include <netinet/ip_icmp.h> //icmp header

#include <arpa/inet.h> //internet address library

#include <netdb.h>

#include <sys/socket.h>

#include <sys/types.h>

#include <signal.h>

#include<errno.h>

#include "bencode.h"

#include "bt_lib.h"

#include "bt_setup.h"
#include<openssl/sha.h>
#define BUF_LEN 1024
int send_bitfield(int new_client_sockfd,bt_bitfield_t bfield){
  // TODO send bitfield
  bt_msg_t bitfield_msg;
  bitfield_msg.length = 1+bfield.size;
  bitfield_msg.bt_type = BT_BITFILED;
  bitfield_msg.payload.bitfiled = bfield;
 int sent = 
 send(new_client_sockfd,&bitfield_msg,bitfield_msg.length,0);
  printf("Bitfield sent!\n");
  return sent;
}
void parsetorrent(bt_info_t *track,be_node *node);




int initialize_socket(int);

int main (int argc, char * argv[])
{

bt_info_t track;  
bt_args_t bt_args;
struct timeval tv;
be_node * node; // top node in the bencoding
fd_set readset,temp;  
int i,maxfd,result,read_size;
char *sha1;
char msg[MSG_LEN], rmsg[MSG_LEN];  
parse_args(&bt_args, argc, argv);
char buf[BUF_LEN];
int peers=0;
unsigned char bhave;
//logging
char message[25];
char msg_info[50];
log_info log;
gettimeofday(&(log.start_tv),NULL);
if(bt_args.verbose)
{
    
printf("Args:\n");
    
printf("verbose: %d\n",bt_args.verbose);
    
printf("save_file: %s\n",bt_args.save_file);
    
printf("log_file: %s\n",bt_args.log_file);
    
printf("torrent_file: %s\n", bt_args.torrent_file);

    
for(i=0;i<MAX_CONNECTIONS;i++)
{
      
if(bt_args.peers[i] != NULL)
        
print_peer(bt_args.peers[i]);
    
}
log.log_file=fopen(bt_args.log_file,"w");
    
  
}//intialising socket
int in_socket;
in_socket=initialize_socket(bt_args.port);
//read and parse the torrent file
 FD_ZERO(&readset);
FD_SET(in_socket,&readset);
maxfd=in_socket;
node = load_be_node(bt_args.torrent_file);

  
if(bt_args.verbose)
{
  
be_dump(node);
  
}
sha1=track.name;
//second edit 26/10
//bt_info_t track;
node=load_be_node(bt_args.torrent_file);
parsetorrent(&track,node);
printf("tracker announce :\t%s",track.announce);

//second edit
//parsetorrent(&track,node);
//be_free(node);
bt_bitfield_t bit;
bit.size=(track.num_pieces)/8 + 1;
bit.bitfield=malloc(bit.size);
bzero(&bit.bitfield,bit.size);
peer_t *peer;
for(i=0;i<MAX_CONNECTIONS;i++) //connect
{
if(bt_args.peers[i]!=NULL){
peer=bt_args.peers[i];
peer->btfield=malloc(bit.size);
bzero(peer->btfield,bit.size);
log.len=snprintf(log.logmsg,100,"HANDSHAKE INIT peer:%s port %d id%20s",inet_ntoa(peer->sockaddr.sin_addr),peer->port,peer->id);
int logw=log_write(&log);
int *sfd=&(bt_args.sockets[i]);
//Connection
if(connection(peer,sha1,msg,rmsg,sfd)){
log.len = snprintf(log.logmsg,100,"HANDSHAKE FAILED peer:%s port:%d id:%20s\n",
            inet_ntoa(peer->sockaddr.sin_addr),peer->port,peer->id);
        logw = log_write(&log);

        free(bt_args.peers[i]);
      }else{
        log.len = snprintf(log.logmsg,100,"HANDSHAKE SUCCESS peer:%s port:%d id:%20s\n",
            inet_ntoa(peer->sockaddr.sin_addr),peer->port,peer->id);
        logw = log_write(&log);
FD_SET(bt_args.sockets[i],&readset);
 if (bt_args.sockets[i] > maxfd) { // keep track of the max
          maxfd = bt_args.sockets[i];
        }
        send_bitfield(bt_args.sockets[i],bit);
      }

}
}

//main client loop
  
printf("Starting Main Loop\n");
  
while(1){
int peerpos=-1,j;
memcpy(&temp,&readset,sizeof(temp));
tv.tv_sec=30;
tv.tv_usec=0;
result=select(maxfd+1,&temp,NULL,NULL,&tv);
if(result==0)
printf("timeout");
else if(result<0 && errno!=EINTR)
printf("error %s",strerror(errno));
else if(result>0)
{
for(i=0;i<=maxfd;i++)
{
if(FD_ISSET(i,&temp))
{
if(i==in_socket)
{
   int newclientsockfd;
peerpos=-1;
for(j=0;j<MAX_CONNECTIONS;j++)
{if(bt_args.peers[j]==NULL){
peerpos=j;
break;}}
if(peerpos==-1)
printf("New connection cannot be accepted");
else
{
           if( acceptpeer(in_socket, sha1,msg, rmsg,&newclientsockfd,&log,bt_args.peers[peerpos])){}
else{
            FD_SET(newclientsockfd, &readset); // add to master set
            if (newclientsockfd > maxfd) { // keep track of the max
              maxfd = newclientsockfd;
            }
            // TODO send bitfield
  bt_msg_t bitfield_msg;
 bitfield_msg.length = 1+bit.size;
   bitfield_msg.bt_type = BT_BITFILED;
  bitfield_msg.payload.bitfiled = bit;
  int sent = send(newclientsockfd,&bitfield_msg,bitfield_msg.length,0);
 printf("Bitfield seant!\n  Msg len: %3d, Sent Size %3d\n",bitfield_msg.length,sent);
  }  } }                                                                                           else { 
                   // otherwise someone else is sending us something
                                                                                                 int message_len;
  read(i,&message_len,sizeof(int));
//    unsigned char bt_type;
  //     read(i,&bt_type,sizeof(bt_type));
                                                                                                     // switch on type of bt_message and handle accordingly
                                                                                                       // TODO change the rest of these to #define vals
if(!message_len)
continue;    
peerpos=-1;
for(j=0;j<MAX_CONNECTIONS;j++){
if(bt_args.sockets[j] == i && bt_args.peers[j] != NULL){
                peerpos=j;
                break;
              }
            }
            if(peerpos==-1){
              fprintf(stderr,"Couldn't find connected peer in peers\n");
              exit(1);
            }

            unsigned char bt_type;
            read(i,&bt_type,sizeof(bt_type));
int have,charpos;
unsigned char bithave;
  switch(bt_type){
         case BT_CHOKE: //choke
bt_args.peers[peerpos]->imchoked=1;
strcpy(message,"MESSAGE CHOKE FROM");
strcpy(msg_info,"");
         break;
       case BT_UNCHOKE:
 //unchoke
 bt_args.peers[peerpos]->imchoked=0;
                strcpy(message,"MESSAGE UNCHOKE FROM");
                strcpy(msg_info,"");
             break;
     case BT_INTERSTED: //interested
bt_args.peers[peerpos]->interested=1;
                strcpy(message,"MESSAGE INTERESTED FROM");
                strcpy(msg_info,"");
        break;
 case BT_NOT_INTERESTED:
 //not interested
  bt_args.peers[peerpos]->interested=0;
                strcpy(message,"MESSAGE NOT INTERESTED FROM");
                strcpy(msg_info,"");
        break;
        case BT_HAVE: //have
read(i,&have,message_len-1);
                bhave = 1;
                charpos = have%8;
                charpos = 7-charpos;
                bhave<<charpos;
                bt_args.peers[peerpos]->btfield[have/8] |= bhave;
                strcpy(message,"MESSAGE HAVE FROM");
                snprintf(msg_info,50,"have:%d",have);       



break;
       case BT_BITFILED: //bitfield
           printf("bitfield received\n");
       do{
    read_size = read(i,&buf,BUF_LEN);
    printf("buf contains: %s\n",buf);
        }while(read_size == BUF_LEN);

      send_bitfield(i,bit);                                                                                             //read(i,&buf,sizeof(message_len));
strcpy(message,"MESSAGE REQUEST FROM");     // reply with bitfield
                break;
   case BT_REQUEST:  
strcpy(message,"MESSAGE REQUEST FROM"); //request

  break; 
   case BT_PIECE: //piece
strcpy(message,"MESSAGE PIECE FROM");
      break;
    case BT_CANCEL: //cancel
strcpy(message,"MESSAGE CANCEL FROM");
       break;                                                                                                                                                                     
//try to aaccept incoming connection from new peer
       //waitingg...
/*      int client;//accepting new peer
struct sockaddr client_addr;
socklen_t client_addr_len;
printf("waiting for conn:");
client=accept(sock_desc,&client_addr,&client_addr_len);

fprintf(stderr,"conn established");
 //   char *sha1;
//sha1=track.announce;
parintf("sha1:\n");
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
    
//poll current peers for incoming traffic
    
//   write pieces to files
    
//   udpdate peers choke or unchoke status
    
//   responses to have/havenots/interested etc.
    
   
//for peers that are not choked
    //   request pieaces from outcoming traffic

    //check livelenss of peers and replace dead (or useless) peers
    //with new potentially useful peers
    
    //update peers, 
}}}}}
}


 return 0;

}









