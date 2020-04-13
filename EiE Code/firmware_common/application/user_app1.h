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
    int volume; // holds the Volume value, idk if we can change the volume value
    int range;  // holds the range value
    int reference_time;   // holds the reference time, aka. (current time) - (reference_time) = time to display
    int adjust_volume_or_range; // is 0 for adjusting volume, is 1 for adjusting range
    u8 *label[2];            // holds the names of Volume and Range to help simplify display
}TheStatus;

/**********************************************************************************************************************
Function Declarations
**********************************************************************************************************************/
void control_values(int UpOrDown);  // will change the volume or range depending on Status.adjust_volume_or_range
                                                    // UpOrDown is 0 when Up, is 1 when Down

int play_note(u16 note, u16 length, u16 type);
      // not is the pitch of the not in Hz
      // length is the duration of the note in ms
      // type is the silence subtracted off the note's length in ms
      // rest is 1 if the not is not a note but a rest
        
//u8 * convert_int_to_string(int value);    // will be called by a variety of states    // gave up on this function since it gets used only 3 seperate times
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
static void Idle(void);     // waits for the user to press ON/OFF button
static void Wait(void);     // watches the ANT, aka. waiting for caller fob to be close to device
static void Connecting(void);   // will confirm that the pet is waiting for a set amount of time
static void Alarm(void);    // uses an audio cue to alert the user

static void UserApp1SM_Error(void);         



/**********************************************************************************************************************
Constants / Definitions
**********************************************************************************************************************/
#define wait_time 10000     // in milisecsods time the device waits until to sound the alarm
#define display_duration 1500     // in ms, the amount of time to display the "volume adjust mode" to user

#define max_volume 100
#define max_range 12    // CODED FOR MAX SIZE OF BELOW 99

#define display_the_time 1  // just for testing

#define REPEAT_ALARM 100000       // in miliseconds

#endif /* __USER_APP1_H */


//notes         // i think the warnings are coming cuz he jason already delcared the MACROs in another file
#define MEASURE_TIME              (u16)3600  /* Time in ms for 1 measure (1 full note) - should be divisible by 16 */

/*
#define RT                        REGULAR_NOTE_ADJUSTMENT
#define ST                        STACCATO_NOTE_TIME        
#define HT                        HOLD_NOTE_ADJUSTMENT     

#define FN                        FULL_NOTE                 
#define HN                        HALF_NOTE                 
#define QN                        QUARTER_NOTE              
#define EN                        EIGHTH_NOTE               
#define SN                        SIXTEENTH_NOTE
*/
/*--------------------------------------------------------------------------------------------------------------------*/
/* End of File                                                                                                        */
/*--------------------------------------------------------------------------------------------------------------------*/
