//=============================================================================
//File Name: Commands.cpp
//Description: Contains functions that can be executed as commands via Telnet
//Author: FRC Team 3512, Spartatroniks
//=============================================================================

#include "Commands.hpp"
#include "Helpers.hpp"

#include <cstdio>
#include <ioLib.h>
#include <moduleLib.h>
#include <loadLib.h>
#include <taskLib.h>
#include <rebootLib.h>
#include <sysLib.h>
#include <symLib.h>
#include <sysSymTbl.h>

void rebootRobot() {
    reboot( BOOT_NORMAL ); // reboot cRIO normally
}

void reloadRobot() {
    std::printf( "Reloading...\n" );

    // Delete robot code's task
    int frcCodeTask = taskNameToId( "FRC_RobotTask" );
    if ( frcCodeTask != ERROR ) {
        taskDelete( frcCodeTask );
    }

    // Find FRC robot code module
    MODULE_ID frcOldCode = moduleFindByName( "FRC_UserProgram.out" );

    if ( frcOldCode != NULL ) {
        unloadCode( frcOldCode );
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

void listTasks() {
    int idList[256];

    int numTasks = taskIdListGet( idList , 256 );

    std::printf( "Task ID : Name\n" );
    for ( int i = 0 ; i < numTasks ; i++ ) {
        std::printf( "%d: %s\n" , idList[i] , taskName( idList[i] ) );
    }
}
