//=============================================================================
//File Name: Main.cpp
//Description: Unloads /ni-rt/system/FRC_UserProgram.out, then reloads it and
//             exits
//Author: FRC Team 3512, Spartatroniks
//=============================================================================

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <hostLib.h>
#include <ioLib.h>
#include <sockLib.h>
#include <signal.h>
#include <selectLib.h>
#include <rebootLib.h>
#include <loadLib.h>
#include <unldLib.h>
#include <moduleLib.h>
#include <sysSymTbl.h>
#include <taskLib.h>
#include <sysLib.h>
#include <time.h>
#include <pthread.h>
#define socklen_t int

// Load on startup
int alf_entrypoint();
int alf_main();
const int32_t alf_entrystatus = alf_entrypoint();

struct threadargs_t {
    MODULE_ID moduleID;
};

void *
unloadRobotThread( void* arg )
{
    struct threadargs_t *args = (struct threadargs_t *)arg;
    std::printf( "unldTask: unloading...\n" );

    unldByModuleId( args->moduleID , UNLD_CPLUS_XTOR_AUTO );

    std::printf( "unldTask: done\n" );

    pthread_exit(NULL);

    return NULL;
}

void
unloadRobot(MODULE_ID module_id) {
    pthread_t thread0;
    pthread_attr_t attr;
    struct threadargs_t thread0args;

    /* set up the threadargs */
    thread0args.moduleID = module_id;

    /* thread attributes */
    pthread_attr_init(&attr);

    /* launch the first thread */
    pthread_create(&thread0, &attr, unloadRobotThread, (void *)&thread0args);

    /* wait for it to complete */
    pthread_join( thread0 , NULL );
}

void
printTasks()
{
    int idList[256];

    int numTasks = taskIdListGet( idList , 256 );

    std::printf( "Task ID : Name\n" );
    for ( int i = 0 ; i < numTasks ; i++ ) {
        std::printf( "%d: %s\n" , idList[i] , taskName( idList[i] ) );
    }
}

void
rebootRobot()
{
    reboot( BOOT_NORMAL ); // reboot cRIO normally
}

void
reloadRobot()
{
    std::printf( "Reloading...\n" );

    // Delete robot code's task
    int frcCodeTask = taskNameToId( "FRC_RobotTask" );
    if ( frcCodeTask != ERROR ) {
        taskDelete( frcCodeTask );
    }

    // Find FRC robot code module
    MODULE_ID frcOldCode = moduleFindByName( "FRC_UserProgram.out" );

    if ( frcOldCode != NULL ) {
        unloadRobot( frcOldCode );
    }

    int program = open( "/ni-rt/system/FRC_UserProgram.out" , O_RDONLY , 0 );
    MODULE_ID frcNewCode = loadModule( program , LOAD_ALL_SYMBOLS | LOAD_CPLUS_XTOR_AUTO );
    close( program );

    if ( frcNewCode != NULL ) {
        printf( "FRC module loaded\n" );
        VOIDFUNCPTR frcFunctionPtr;
        uint8_t symbolType;
        symFindByName( sysSymTbl , "FRC_UserProgram_StartupLibraryInit" , (char**)&frcFunctionPtr , &symbolType );
        printf( "Starting robot code\n" );
        frcFunctionPtr();
    }
    else {
        std::printf( "...not really!\n" );
    }
}

int
procCommands(int sockfd)
{
    char buf[1024];
    char *cbuf;

    for ( cbuf = buf ; *(cbuf-1) != '\n' ; cbuf++ ) {
        if( recv( sockfd , cbuf , 1 , 0 ) != 1 ) {
            return -1;
        }
    }

    // add the null terminator
    *(cbuf-2) = '\0';

    if( std::strcmp( buf , "reboot" ) == 0 ) {
        rebootRobot();
    }
    else if( std::strcmp( buf , "reload" ) == 0 ) {
        reloadRobot();
    }

    return 0;
}

int
alf_entrypoint()
{
    taskSpawn( "ALF", // Task name
            101, // Priority
            VX_FP_TASK, // Task option flag
            0xFFFF, // Stack size
            (FUNCPTR)alf_main, // Entry point
            0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ); // Unused task args

    return 0;
}

int
alf_main()
{
    int sockfd, nsockfd;
    int portno;
    socklen_t clilen;
    struct sockaddr_in serv_addr, cli_addr;

    // Gives user time to kill ALF if necessary
    std::printf( "Delaying ALF startup 5 seconds\n" );
    sleep( 5 );
    std::printf( "Starting ALF...\n" );

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    bzero((char *) &serv_addr, sizeof(serv_addr));
    portno = 3512;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);
    bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr));
    listen(sockfd,5);
    clilen = sizeof(cli_addr);
    signal(SIGPIPE, SIG_IGN);


    while( 1 ) {
        // Accept commands from elsewhere as string and process them
        nsockfd = accept(sockfd, (struct sockaddr *) &cli_addr,
            &clilen);
        while( procCommands(nsockfd) == 0 );
    }

    return 0;
}
