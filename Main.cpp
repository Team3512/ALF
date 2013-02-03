//=============================================================================
//File Name: Main.cpp
//Description: Accepts commands that manipulate the FRC robot code module
//Author: FRC Team 3512, Spartatroniks
//=============================================================================

/* Supports the following commands via Telnet:
 * 1. reboot (reboots the cRIO)
 * 2. reload (unloads /c/ni-rt/system/FRC_UserProgram.out then reloads it)
 * 3. listTasks (lists all task IDs and their corresponding names for the tasks
 *    which are running on the system)
 */

// Port on which ALF waits for commands
const int gPortNumber = 3512;

#include <cstdio>
#include <cstring>

#include <signal.h>
#include <arch/ppc/coprocPpc.h>
#include <sockLib.h>
#include <in.h>
#define socklen_t int

#include "Commands.hpp"

// Load on startup
int alf_entrypoint();
int alf_main();
const int32_t alf_entrystatus = alf_entrypoint();

// Process input from socket
int processCommands( int sockfd ) {
    char buf[1024];
    char *cbuf;

    // Receive string from client
    for ( cbuf = buf ; *(cbuf-1) != '\n' ; cbuf++ ) {
        if( recv( sockfd , cbuf , 1 , 0 ) != 1 ) {
            return -1;
        }
    }

    // Add the null terminator
    *(cbuf-2) = '\0';

    // Match the string with valid commands
    if( std::strcmp( buf , "reboot" ) == 0 ) {
        rebootRobot();
    }
    else if( std::strcmp( buf , "reload" ) == 0 ) {
        reloadRobot();
    }
    else if ( std::strcmp( buf , "listTasks" ) == 0 ) {
        listTasks();
    }

    return 0;
}

int alf_entrypoint() {
    taskSpawn( "ALF", // Task name
            101, // Priority
            VX_FP_TASK, // Task option flag
            0xFFFF, // Stack size
            (FUNCPTR)alf_main, // Entry point
            0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ); // Unused task args

    return 0;
}

int alf_main() {
    int sockfd, nsockfd;
    int portno;
    socklen_t clilen;
    struct sockaddr_in serv_addr, cli_addr;

    // Gives user time to kill ALF if necessary
    std::printf( "ALF will start in 5 seconds...\n" );
    sleep( 5 );
    std::printf( "ALF started\n" );

    sockfd = socket( AF_INET , SOCK_STREAM , 0 );
    bzero( (char*)&serv_addr , sizeof(serv_addr));
    portno = gPortNumber;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);
    bind( sockfd , (struct sockaddr*)&serv_addr , sizeof(serv_addr) );
    listen( sockfd , 5 );
    clilen = sizeof(cli_addr);
    signal( SIGPIPE , SIG_IGN );

    while( 1 ) {
        // Accept connections from elsewhere to receive commands
        nsockfd = accept( sockfd , (struct sockaddr*)&cli_addr , &clilen );

        // Process input from socket
        while ( processCommands( nsockfd ) == 0 );
    }

    return 0;
}
