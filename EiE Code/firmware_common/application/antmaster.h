/*!*********************************************************************************************************************
@file antmaster.h                                                                
@brief Header file for antmaster

----------------------------------------------------------------------------------------------------------------------
To start a new task using this antmaster as a template:
1. Follow the instructions at the top of antmaster.c
2. Use ctrl-h to find and replace all instances of "antmaster" with "yournewtaskname"
3. Use ctrl-h to find and replace all instances of "AntMaster" with "YourNewTaskName"
4. Use ctrl-h to find and replace all instances of "ANTMASTER" with "YOUR_NEW_TASK_NAME"
5. Add #include yournewtaskname.h" to configuration.h
6. Add/update any special configurations required in configuration.h (e.g. peripheral assignment and setup values)
7. Delete this text (between the dashed lines)
----------------------------------------------------------------------------------------------------------------------

**********************************************************************************************************************/

#ifndef __ANTMASTER_H
#define __ANTMASTER_H

/**********************************************************************************************************************
Type Definitions
**********************************************************************************************************************/


/**********************************************************************************************************************
Function Declarations
**********************************************************************************************************************/

/*------------------------------------------------------------------------------------------------------------------*/
/*! @publicsection */                                                                                            
/*--------------------------------------------------------------------------------------------------------------------*/


/*------------------------------------------------------------------------------------------------------------------*/
/*! @protectedsection */                                                                                            
/*--------------------------------------------------------------------------------------------------------------------*/
void AntMasterInitialize(void);
void AntMasterRunActiveState(void);


/*------------------------------------------------------------------------------------------------------------------*/
/*! @privatesection */                                                                                            
/*--------------------------------------------------------------------------------------------------------------------*/


/***********************************************************************************************************************
State Machine Declarations
***********************************************************************************************************************/
static void AntMasterSM_Idle(void);    
static void AntMasterSM_Error(void);         
static void AntMasterSM_AntChannelAssign(void);


/**********************************************************************************************************************
Constants / Definitions
**********************************************************************************************************************/
/* Required constants for ANT channel configuration */
#define ANT_CHANNEL_TYPE        CHANNEL_TYPE_MASTER   // Device is a MASTER
#define ANT_CHANNEL             ANT_CHANNEL_0         // Channel 0 - 7
#define ANT_CHANNEL_PERIOD_LO   (u8)0x00              // LO; 0x0001 - 0x7fff
#define ANT_CHANNEL_PERIOD_HI   (u8)0x20              // HI; 0x0001 - 0x7fff
#define ANT_DEVICEID_LO         (u8)0x34              // Device # Low byte
#define ANT_DEVICEID_HI         (u8)0x12              // Device # High byte
#define ANT_DEVICE_TYPE         (u8)1                 // 1 - 255
#define ANT_TRANSMISSION_TYPE   (u8)1                 // 1-127; MSB is pairing
#define ANT_FREQUENCY           (u8)50                // 2400MHz + 0 - 99 MHz
#define ANT_TX_POWER            RADIO_TX_POWER_4DBM   // Max tx power


#endif /* __ANTMASTER_H */
/*--------------------------------------------------------------------------------------------------------------------*/
/* End of File                                                                                                        */
/*--------------------------------------------------------------------------------------------------------------------*/
