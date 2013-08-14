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

void rebootRobot();
void reloadRobot();
void listTasks();

#ifdef __cplusplus
}
#endif

#endif /* COMMANDS_H */
