/*!*********************************************************************************************************************
@file user_app1.h                                                                
@brief Header file for user_app1

**********************************************************************************************************************/

#ifndef __USER_APP1_H
#define __USER_APP1_H

/**********************************************************************************************************************
Structure Definitions
**********************************************************************************************************************/
static struct TheStatus{  // global structure call values by:     Status.volume
    int volume; // holds the Volume value
    int range;  // holds the range value
    int reference_time;   // holds the reference time, aka. (current time) - (reference_time) = time to display
    int adjust_volume_or_range; // is 0 for adjusting volume, is 1 for adjusting range

}ThStatus;

/**********************************************************************************************************************
Function Declarations
**********************************************************************************************************************/
void control_values(int UpOrDown);    // will change the volume or range depending on Status.adjust_volume_or_range
                                                    // UpOrDown is 0 when Up, is 1 when Down

void display_time(void);    // will be called every second
/*------------------------------------------------------------------------------------------------------------------*/
/*! @publicsection */                                                                                            
/*--------------------------------------------------------------------------------------------------------------------*/


/*------------------------------------------------------------------------------------------------------------------*/
/*! @protectedsection */                                                                                            
/*--------------------------------------------------------------------------------------------------------------------*/
void UserApp1Initialize(void);
void UserApp1RunActiveState(void);


/*------------------------------------------------------------------------------------------------------------------*/
/*! @privatesection */                                                                                            
/*--------------------------------------------------------------------------------------------------------------------*/


/***********************************************************************************************************************
State Machine Declarations
***********************************************************************************************************************/
static void Idle(void); // waits for the user to press ON/OFF button
static void Watch(void);    // watches the ANT, aka. waiting for caller fob to be close to device

static void UserApp1SM_Error(void);         



/**********************************************************************************************************************
Constants / Definitions
**********************************************************************************************************************/


#endif /* __USER_APP1_H */

/*--------------------------------------------------------------------------------------------------------------------*/
/* End of File                                                                                                        */
/*--------------------------------------------------------------------------------------------------------------------*/