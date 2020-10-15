//
// Created by vladl on 10/16/2020.
//

#include <conn.h>
#include <stdio.h>

int main(){
    printf( "%s\n", comStackErrorToString( initComModule() ) );
    printf( "%s\n", comStackErrorToString( createIPV4Server( 6000 ) ) );

    connection_t clientConnection;

    printf( "%s\n", comStackErrorToString( acceptClient( & clientConnection ) ) );

    printf( "%s\n", comStackErrorToString( testConnection( clientConnection ) ) );

    printf( "%s\n", comStackErrorToString( disconnect( clientConnection ) ) );
    printf( "%s\n", comStackErrorToString( stopServer() ) );

    printf( "%s\n", comStackErrorToString( stopComModule() ) );
}