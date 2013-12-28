/*=============================================================================
File Name: Commands.h
Description: Contains functions that can be executed as commands via Telnet
Author: FRC Team 3512, Spartatroniks
=============================================================================*/

#ifndef COMMANDS_H
#define COMMANDS_H

#ifdef __cplusplus
extern "C" {
#endif

void alf_reboot();
void alf_reload( const char* newModName );
void alf_listTasks();

void alf_copy( const char* fromName , const char* toName );

#ifdef __cplusplus
}
#endif

#endif /* COMMANDS_H */
