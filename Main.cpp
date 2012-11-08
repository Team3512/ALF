//=============================================================================
//File Name: Main.cpp
//Description: Unloads ni-rt/system/FRC_UserProgram.out, then reloads it and
//             exits
//Author: FRC Team 3512, Spartatroniks
//=============================================================================

#include <stdio.h>
#include <ioLib.h>
#include <loadLib.h>
#include <unldLib.h>
#include <taskLib.h>
#include <fcntl.h>

INT32 FRC_UserProgram_StartupLibraryInit();

int ALFmain() {
    printf( "ALF.out started\n" );

    unldByNameAndPath( "FRC_UserProgram.out" , "/ni-rt/system" , UNLD_CPLUS_XTOR_AUTO );

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
