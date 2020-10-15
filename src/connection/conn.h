//
// Created by vladl on 10/15/2020.
//

#ifndef C_COM_STACK_CONN_H
#define C_COM_STACK_CONN_H

#define LOCALHOST ( (IPV4IP) {.addr = ( ( 127U << 24U ) | ( 0U << 16U ) | ( 0U << 8U ) | ( 1U << 0U ) ) } )

#include <com_stack_types.h>
#include <netinet/in.h>

ComResult initComModule (); /// TBD
ComResult stopComModule (); /// implemented

ComResult createIPV4ConnectionToServer ( connection_t *, IPV4IP, uint16 ); /// implemented
ComResult createIPV4Server ( uint16 ); /// implemented

ComResult connectToServer ( connection_t ); /// implemented
ComResult acceptClient ( connection_t * ); /// implemented
ComResult disconnect ( connection_t ); /// implemented
//ComResult disconnectClient ( connection_t );
ComResult disconnectClientIPV4 ( IPV4IP );
ComResult stopServer (); /// implemented

ComResult testConnection ( connection_t ); /// implemented

IPV4IP getConnectionIPV4IP ( connection_t );

IPV4IP strToIPV4IP ( const char *, char ** ); /// implemented
char * IPV4IPToStr ( IPV4IP, char * ); /// implemented

in_addr_t IPV4IPToInAddr ( IPV4IP ); /// implemented
IPV4IP inetAddrToIPV4IP ( in_addr_t ); /// implemented

const char * comStackErrorToString ( ComResult );

#endif //C_COM_STACK_CONN_H
