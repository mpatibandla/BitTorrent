#ifndef _BT_SETUP_H
#define _BT_SETUP_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "bt_setup.h"
#include "bt_lib.h"
#include "bencode.h"


/**
 * __parse_peer(peer_t * peer, char peer_st) -> void
 *
 * parse a peer string, peer_st and store the parsed result in peer
 *
 * ERRORS: Will exit on various errors
 **/
void usage(FILE * file);


/**
 * pars_args(bt_args_t * bt_args, int argc, char * argv[]) -> void
 *
 * parse the command line arguments to bt_client using getopt and
 * store the result in bt_args.
 *
 * ERRORS: Will exit on various errors
 *
 **/

void parse_args(bt_args_t * bt_args, int argc,  char ** argv);


int initialize_sock(int port);

int readhandshake(int peer_sock_fd,char * rmsg, char *msg);

//int connection(peer_t * peer, char *sha1, char * msg, char * rmsg);
int connect_to_peer(peer_t * peer, char * sha1, char * h_message, char * rh_message,int * func);
int read_handshake(int peer_sock_fd,char * rh_message,char * h_message);
#endif
