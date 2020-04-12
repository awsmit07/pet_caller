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

#define REPEAT_ALARM 5000       // in miliseconds

#endif /* __USER_APP1_H */


//notes
#define MESURE_TIME               (u16)2400         // is very important in making timing

#define FULL_NOTE                 (u16)(MEASURE_TIME)
#define HALF_NOTE                 (u16)(MEASURE_TIME / 2)
#define QUARTER_NOTE              (u16)(MEASURE_TIME / 4)
#define EIGHTH_NOTE               (u16)(MEASURE_TIME / 8)
#define SIXTEENTH_NOTE            (u16)(MEASURE_TIME / 16)

/* Note length adjustments */
#define REGULAR_NOTE_ADJUSTMENT   (u16)50
#define STACCATO_NOTE_TIME        (u16)75
#define HOLD_NOTE_ADJUSTMENT      (u16)0

#define NOTE_C3                   (u16)131
#define NOTE_C3_SHARP             (u16)139
#define NOTE_D3_FLAT              (u16)139
#define NOTE_D3                   (u16)147
#define NOTE_D3_SHARP             (u16)156
#define NOTE_E3_FLAT              (u16)156
#define NOTE_E3                   (u16)165
#define NOTE_F3                   (u16)175
#define NOTE_F3_SHARP             (u16)185
#define NOTE_G3_FLAT              (u16)185
#define NOTE_G3                   (u16)196
#define NOTE_G3_SHARP             (u16)208
#define NOTE_A3                   (u16)220
#define NOTE_A3_SHARP             (u16)233
#define NOTE_B3                   (u16)245
#define NOTE_C4                   (u16)262  /* Middle C */
#define NOTE_C4_SHARP             (u16)277
#define NOTE_D4_FLAT              (u16)277
#define NOTE_D4                   (u16)294
#define NOTE_D4_SHARP             (u16)311
#define NOTE_E4_FLAT              (u16)311
#define NOTE_E4                   (u16)330
#define NOTE_F4                   (u16)349
#define NOTE_F4_SHARP             (u16)370
#define NOTE_G4_FLAT              (u16)370
#define NOTE_G4                   (u16)392
#define NOTE_G4_SHARP             (u16)415
#define NOTE_A4                   (u16)440
#define NOTE_A4_SHARP             (u16)466
#define NOTE_B4                   (u16)494
/*--------------------------------------------------------------------------------------------------------------------*/
/* End of File                                                                                                        */
/*--------------------------------------------------------------------------------------------------------------------*/
