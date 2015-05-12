#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "bt_setup.h"
#include "bt_lib.h"
#include "bencode.h"
//#include<inet.h>
#include <arpa/inet.h>
/**
 * usage(FILE * file) -> void
 *
 * print the usage of this program to the file stream file
 *
 **/
void usage(FILE * file){
if(file == NULL){
    file = stdout;
 }
fprintf(file,
          "bt-client [OPTIONS] file.torrent\n"
          "  -h            \t Print this help screen\n"
          "  -b ip         \t Bind to this ip for incoming connections, ports\n"
          "                \t are selected automatically\n"
          "  -s save_file  \t Save the torrent in directory save_dir (dflt: .)\n"
          "  -l log_file   \t Save logs to log_filw (dflt: bt-client.log)\n"
          "  -p ip:port    \t Instead of contacing the tracker for a peer list,\n"
          "                \t use this peer instead, ip:port (ip or hostname)\n"
          "                \t (include multiple -p for more than 1 peer)\n"
          "  -I id         \t Set the node identifier to id (dflt: random)\n"
          "  -v            \t verbose, print additional verbose info\n");
}
/**
 * __parse_peer(peer_t * peer, char peer_st) -> void
 *
 * parse a peer string, peer_st and store the parsed result in peer
 *
 * ERRORS: Will exit on various errors
 **/
void __parse_peer(peer_t * peer, char * peer_st)
{
  char * parse_str;
  char * word;
  unsigned short port;
  char * ip;
  char id[20];
  char sep[] = ":";
  int i;
printf("IN PARSE_PEER");
  //need to copy becaus strtok mangels things
  parse_str = malloc(strlen(peer_st)+1);
  strncpy(parse_str, peer_st, strlen(peer_st)+1);
 //only can have 2 tokens max, but may have less
 for(word = strtok(parse_str, sep), i=0; 
      (word && i < 3); 
      word = strtok(NULL,sep), i++)
{
    printf("%d:%s\n",i,word);
  switch(i)
{
  case 0://id
     ip = word;
    break;
    case 1://ip
   port = atoi(word);
  default:
     break;
}
  }
  if(i < 2)
{
   fprintf(stderr,"ERROR: Parsing Peer: Not enough values in '%s'\n",peer_st);
   usage(stderr);
   exit(1);
}
  if(word)
{
   fprintf(stderr, "ERROR: Parsing Peer: Too many values in '%s'\n",peer_st);
    usage(stderr);
 exit(1);
}
 //calculate the id, value placed in id
calc_id(ip,port,id);
//build the object we need
  init_peer(peer, id, ip, port);
  //free extra memory
  free(parse_str);
 return;
}
/**
 * pars_args(bt_args_t * bt_args, int argc, char * argv[]) -> void
 *
 * parse the command line arguments to bt_client using getopt and
 * store the result in bt_args.
 *
 * ERRORS: Will exit on various errors
 *
 **/
void parse_args(bt_args_t * bt_args, int argc,  char * argv[])
{
  int ch; //ch for each flag
 int n_peers = 0;
int i;
  /* set the default args */
 bt_args->verbose=0;
//no verbosity
  //null save_file, log_file and torrent_file
  memset(bt_args->save_file,0x00,FILE_NAME_MAX);
  memset(bt_args->torrent_file,0x00,FILE_NAME_MAX);
 memset(bt_args->log_file,0x00,FILE_NAME_MAX);
 //null out file pointers
bt_args->f_save = NULL;
//null bt_info pointer, should be set once torrent file is read
bt_args->bt_info = NULL;
//default lag file
strncpy(bt_args->log_file,"bt-client.log",FILE_NAME_MAX);
 for(i=0;i<MAX_CONNECTIONS;i++)
{
    bt_args->peers[i] = NULL; //initially NULL
 }
//second edit 26/10/2013 only below 1 line
bt_args->port=0;
bt_args->id = 0;
//while loop mein b add kiya 26/10
  while ((ch = getopt(argc, argv, "hp:s:l:vI:b")) != -1) 
{
switch (ch) {
    case 'h': //help
usage(stdout);
exit(0);
    break;
    case 'v': //verbose
      bt_args->verbose = 1;
      break;
case 's': //save file
strncpy(bt_args->save_file,optarg,FILE_NAME_MAX);
      break;
case 'l': //log file
strncpy(bt_args->log_file,optarg,FILE_NAME_MAX);
break;
//case b 26/10
case 'b': //port
bt_args->port=atoi(optarg);
if(bt_args->port<=0)
exit(1);
break;
case 'p': //peer
n_peers++;
//check if we are going to overflow
if(n_peers > MAX_CONNECTIONS)
{
fprintf(stderr,"ERROR: Can only support %d initial peers",MAX_CONNECTIONS);
      usage(stderr);
     exit(1);
      }
 bt_args->peers[n_peers] = malloc(sizeof(peer_t));
      //parse peer
    __parse_peer(bt_args->peers[n_peers], optarg);
 break;
    case 'I':
     bt_args->id = atoi(optarg);
     break;
  default:
 fprintf(stderr,"ERROR: Unknown option '-%c'\n",ch);
   usage(stdout);
      exit(1);
 }
 }
 argc -= optind;
 argv += optind;
 if(argc == 0)
{
   fprintf(stderr,"ERROR: Require torrent file\n");
 usage(stderr);
exit(1);
  }
 //copy torrent file over
strncpy(bt_args->torrent_file,argv[0],FILE_NAME_MAX);
return ;
}

//first edit
int parsetorrent(bt_info_t * par ,be_node * node)
{
size_t i,j;
  be_node * currnode;
  be_node * infonode;
  //bt_info_t out;
  // announce
  // parse dict with info
  for (i = 0; node->val.d[i].val; ++i) {
  currnode = node -> val.d[i].val;
  if(strcmp(node->val.d[i].key,"announce") == 0)
  strcpy(par->announce,currnode->val.s);
  else if(strcmp(node->val.d[i].key,"info") == 0){
  for (j = 0; currnode->val.d[j].val; ++j) {
  infonode = currnode -> val.d[j].val;
  if(strcmp(currnode->val.d[j].key,"name") == 0)
  strcpy(par->name,infonode->val.s);
  else if(strcmp(currnode->val.d[j].key,"length") == 0)
  par->length = infonode->val.i;
  else if(strcmp(currnode->val.d[j].key,"piece length") == 0)        {
  par->piece_length = infonode->val.i;
// once we have the total length and piece length
   // we can find the number of pieces req'd
  par->num_pieces = (par->length)/(par-> piece_length);
 // handle partial pieces
  if (par->length%par->piece_length > 0)
  par -> num_pieces++; 
  }
  else if(strcmp(currnode->val.d[j].key,"pieces") == 0)
    {
 // here we malloc the pieces_hash buffer based
 // on the number of pieces we have
 par -> piece_hashes = (char **)malloc(par->num_pieces*sizeof(char *));
  int k;
  for(k=0;k<par->num_pieces;k++){//malloc a sha1 per piece
      par->piece_hashes[k] = (char *)malloc(20);
      memcpy(par->piece_hashes[k],infonode->val.s + 20*k,20);
 }   }
  }                                                                               }
 }
      return 1;
      }
//first edit

int initialize_socket(int port)
{

struct addrinfo info,*cur;
int sock_desc;
memset(&info,0,sizeof(info));
info.ai_family=AF_UNSPEC;
info.ai_socktype=SOCK_STREAM;
info.ai_flags=AI_PASSIVE;
char port_s[5];
sprintf(port_s,"%d",port);
getaddrinfo(NULL,port_s,&info,&cur);
sock_desc=socket(cur->ai_family,cur->ai_socktype,cur->ai_protocol);
bind(sock_desc,cur->ai_addr,cur->ai_addrlen);
fprintf(stderr,"server socket %d\n",sock_desc);
if(listen(sock_desc,10)==-1)
 {
perror("error in socket initialising");
}
return sock_desc;
}
int read_handshake(int peer_sock_fd,char * rh_message,char * h_message){
  int read_size = read(peer_sock_fd,rh_message,68);
  if(read_size == -1)
    perror("Read Handshake Failed");
    return 1;
}

int connection(peer_t * peer, char *sha1, char * msg, char * rmsg,int *sfd)
{
int sock;
int hread;
sock=socket(AF_INET,SOCK_STREAM,0);
 printf("Attempting connection with peer %s on port %d\n",inet_ntoa(peer->sockaddr.sin_addr),peer->port);
        // Connect to socket A Priori
         if(connect(sock, (const struct sockaddr*) &(peer -> sockaddr),sizeof(peer -> sockaddr))< 0 ){
         perror("Connection failed");
         exit(1);
         }
         /*bt_args.sockets[i]=sock;
         //char * msg;
         if((msg=(char *)malloc(68))==NULL)
         exit(1);
         //char *rmsg;
         if((rmsg=(char *)malloc(68))==NULL)
         exit(1);
         */
         //char * sha1;
         //sha1=track.announce;
         gethandshake(peer,sha1,msg);
         //send handshake
         int sent=send(sock,msg,68,0);
         if(sent!=68)
         fprintf(stderr,"handshake error, return value is %d]n",sent);
         //read handshake
         hread=read(sock,rmsg,68);
         if(hread!=MSG_LEN){
         printf("incorrect handshake");
         }
        
         if(memcmp(msg,rmsg,48)){
         printf("handshake attempted");
         close(sock);}
         else
         printf("handshake");
        // bt_args.sockets[i]=sock;
         
return sock;
}
