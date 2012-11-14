//=============================================================================
//File Name: Main.cpp
//Description: Unloads /ni-rt/system/FRC_UserProgram.out, then reloads it and
//             exits
//Author: FRC Team 3512, Spartatroniks
//=============================================================================

#include <stdio.h>
#include <ioLib.h>
#include <loadLib.h>
#include <unldLib.h>
#include <taskLib.h>
#include <WPILib/NetworkCommunication/symModuleLink.h>

extern "C" {
INT32 FRC_UserProgram_StartupLibraryInit();
}

int ALFmain() {
    printf( "ALF.out started\n" );

    // Check for startup code already running
    INT32 oldId = taskNameToId( "FRC_RobotTask" );

    if ( oldId != ERROR ) {
        // Find the startup code module.
        char moduleName[256];
        moduleNameFindBySymbolName( "FRC_UserProgram_StartupLibraryInit" , moduleName );
        MODULE_ID startupModId = moduleFindByName( moduleName );

        if ( startupModId != NULL ) {
            // Remove the startup code.
            printf( "%d\n" , unldByModuleId( startupModId , 0 ) );
        }
    }

    int program = open( "/ni-rt/system/FRC_UserProgram.out" , O_RDONLY , 0 );
    loadModule( program , LOAD_ALL_SYMBOLS | LOAD_CPLUS_XTOR_AUTO );
    close( program );

    taskSpawn( "FRC_RobotTask" , // Task name
            101 , // Priority
            VX_FP_TASK , // Task option flag
            0xFFFF , // Stack size
            (FUNCPTR)FRC_UserProgram_StartupLibraryInit , // Entry point
            0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ); // unused task args

    return 0;
}
