//=============================================================================
//File Name: Commands.cpp
//Description: Contains functions that can be executed as commands via Telnet
//Author: FRC Team 3512, Spartatroniks
//=============================================================================

#include "Commands.hpp"

#include <cstdio>
#include <cstring>

// Used for opening files in VxWorks
#include <ioLib.h>

// Defines MODULE_ID typedef
#include <moduleLib.h>

// Used for loading modules
#include <loadLib.h>

// Used for unloading modules
#include <unldLib.h>

// Provides functions for manipulating tasks
#include <taskLib.h>

// Defines reboot(1) function
#include <rebootLib.h>

// Defines BOOT_NORMAL flag
#include <sysLib.h>

// Provides access to global symbol table and manipulation functions
#include <sysSymTbl.h>

void rebootRobot() {
    // reboot cRIO normally
    reboot( BOOT_NORMAL );
}

void reloadRobot() {
    std::printf( "Reloading...\n" );

    // Delete robot code's task
    char strTaskName[] = "FRC_RobotTask";
    int frcCodeTask = taskNameToId( strTaskName );
    if ( frcCodeTask != ERROR ) {
        taskDelete( frcCodeTask );
    }

    // Wait for the main robot task to exit
    while ( taskIdVerify(frcCodeTask) == OK ) {
        sleep( 1 ); // Waits one second
    }

    // Get a list of the first 256 tasks
    int idList[256];
    int numTasks = taskIdListGet( idList , 256 );

    char* name;

    // Delete any tasks with "FRC_" at the beginning of their names
    for ( int i = 0 ; i < numTasks ; i++ ) {
        // Get the next task name
        name = taskName( idList[i] );

        /* If the first four bytes of the name match "FRC_",
         * delete the task b/c the robot code created it
         */
        if ( std::strncmp( name , "FRC_" , 4 ) == 0 ) {
            taskDelete( idList[i] );
        }
    }

    // Find FRC robot code module
    char strPrgmName[] = "FRC_UserProgram.out";
    MODULE_ID frcOldCode = moduleFindByName( strPrgmName );

    // If FRC robot code was found, unload it
    if ( frcOldCode != NULL ) {
        std::printf( "Unloading robot code module..." );
        unldByModuleId( frcOldCode , UNLD_CPLUS_XTOR_AUTO );
        std::printf( "Robot code module unloaded\n" );
    }
    else {
        std::printf( "Robot code module wasn't loaded\n" );
    }

    // Load the new FRC code module
    int program = open( "/ni-rt/system/FRC_UserProgram.out" , O_RDONLY , 0 );
    MODULE_ID frcNewCode = loadModule( program , LOAD_ALL_SYMBOLS | LOAD_CPLUS_XTOR_AUTO );
    close( program );

    // If the FRC code module loaded correctly, call its entry point
    if ( frcNewCode != NULL ) {
        printf( "Robot code module loaded\n" );

        VOIDFUNCPTR frcFunctionPtr;
        uint8_t symbolType;

        char strEntryPointName[] = "FRC_UserProgram_StartupLibraryInit";
        symFindByName( sysSymTbl , strEntryPointName , (char**)&frcFunctionPtr , &symbolType );
        std::printf( "Starting robot code...\n" );
        frcFunctionPtr();
    }
    else {
        std::printf( "Robot code failed to load!\n" );
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
