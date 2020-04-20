/*!*********************************************************************************************************************
@file antmaster.c                                                                
@brief User's tasks / applications are written here.  This description
should be replaced by something specific to the task.
----------------------------------------------------------------------------------------------------------------------
To start a new task using this antmaster as a template:
 1. Copy both antmaster.c and antmaster.h to the Application directory
 2. Rename the files yournewtaskname.c and yournewtaskname.h
 3. Add yournewtaskname.c and yournewtaskname.h to the Application Include and Source groups in the IAR project
 4. Use ctrl-h (make sure "Match Case" is checked) to find and replace all instances of "antmaster" with "yournewtaskname"
 5. Use ctrl-h to find and replace all instances of "AntMaster" with "YourNewTaskName"
 6. Use ctrl-h to find and replace all instances of "ANTMASTER" with "YOUR_NEW_TASK_NAME"
 7. Add a call to YourNewTaskNameInitialize() in the init section of main
 8. Add a call to YourNewTaskNameRunActiveState() in the Super Loop section of main
 9. Update yournewtaskname.h per the instructions at the top of yournewtaskname.h
10. Delete this text (between the dashed lines) and update the Description below to describe your task
----------------------------------------------------------------------------------------------------------------------
------------------------------------------------------------------------------------------------------------------------
GLOBALS
- NONE
CONSTANTS
- NONE
TYPES
- NONE
PUBLIC FUNCTIONS
- NONE
PROTECTED FUNCTIONS
- void AntMasterInitialize(void)
- void AntMasterRunActiveState(void)
**********************************************************************************************************************/

#include "configuration.h"



/***********************************************************************************************************************
Global variable definitions with scope across entire project.
All Global variable names shall start with "G_<type>AntMaster"
***********************************************************************************************************************/
/* New variables */
volatile u32 G_u32AntMasterFlags;                          /*!< @brief Global state flags */


/*--------------------------------------------------------------------------------------------------------------------*/
/* Existing variables (defined in other files -- should all contain the "extern" keyword) */
extern volatile u32 G_u32SystemTime1ms;                   /*!< @brief From main.c */
extern volatile u32 G_u32SystemTime1s;                    /*!< @brief From main.c */
extern volatile u32 G_u32SystemFlags;                     /*!< @brief From main.c */
extern volatile u32 G_u32ApplicationFlags;                /*!< @brief From main.c */
extern u32 G_u32AntApiCurrentMessageTimeStamp;            /*!< From ant_api.c */         
extern AntApplicationMessageType G_eAntApiCurrentMessageClass;    
extern u8 G_au8AntApiCurrentMessageBytes[ANT_APPLICATION_MESSAGE_BYTES];  
extern AntExtendedDataType G_sAntApiCurrentMessageExtData;

extern bool BTN0;
extern bool BTN1;
extern bool BTN2;
extern bool BTN3;

extern bool SigOn;

bool AlarmOn = FALSE;
/***********************************************************************************************************************
Global variable definitions with scope limited to this local application.
Variable names shall start with "AntMaster_<type>" and be declared as static.
***********************************************************************************************************************/
static fnCode_type AntMaster_pfStateMachine;               /*!< @brief The state machine function pointer */
static u32 AntMaster_u32Timeout;                           /*!< @brief Timeout counter used across states */


/**********************************************************************************************************************
Function Definitions
**********************************************************************************************************************/

/*--------------------------------------------------------------------------------------------------------------------*/
/*! @publicsection */                                                                                            
/*--------------------------------------------------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------------------------------------------------*/
/*! @protectedsection */                                                                                            
/*--------------------------------------------------------------------------------------------------------------------*/

/*!--------------------------------------------------------------------------------------------------------------------
@fn void AntMasterInitialize(void)
@brief
Initializes the State Machine and its variables.
Should only be called once in main init section.
Requires:
- NONE
Promises:
- NONE
*/
void AntMasterInitialize(void)
{ 
  AntAssignChannelInfoType AntMaster_sChannelInfo;
  
  // Configure ANT for application
  AntMaster_sChannelInfo.AntChannel = ANT_CHANNEL;
  AntMaster_sChannelInfo.AntChannelType = ANT_CHANNEL_TYPE;
  AntMaster_sChannelInfo.AntChannelPeriodLo  = ANT_CHANNEL_PERIOD_LO;
  AntMaster_sChannelInfo.AntChannelPeriodHi  = ANT_CHANNEL_PERIOD_HI;
 
  AntMaster_sChannelInfo.AntDeviceIdLo       = ANT_DEVICEID_LO;
  AntMaster_sChannelInfo.AntDeviceIdHi       = ANT_DEVICEID_HI;
  AntMaster_sChannelInfo.AntDeviceType       = ANT_DEVICE_TYPE;
  AntMaster_sChannelInfo.AntTransmissionType = ANT_TRANSMISSION_TYPE;
  AntMaster_sChannelInfo.AntFrequency        = ANT_FREQUENCY;
  AntMaster_sChannelInfo.AntTxPower          = ANT_TX_POWER;

  AntMaster_sChannelInfo.AntNetwork = ANT_NETWORK_DEFAULT;
  
  for(u8 i = 0; i < ANT_NETWORK_NUMBER_BYTES; i++)
  {
    AntMaster_sChannelInfo.AntNetworkKey[i] = ANT_DEFAULT_NETWORK_KEY;
  }
  
    /* Attempt to queue the ANT channel setup */
  if( AntAssignChannel(&AntMaster_sChannelInfo) )
  {
    AntMaster_u32Timeout = G_u32SystemTime1ms;
    AntMaster_pfStateMachine  = AntMasterSM_AntChannelAssign;
  }
  else
  {
    /* The task isn't properly initialized, so shut it down and don't run */
    DebugPrintf("Ant failed to initialize");
    AntMaster_pfStateMachine  = AntMasterSM_AntChannelAssign;
  }
  
  
} /* end AntMasterInitialize() */

  
/*!----------------------------------------------------------------------------------------------------------------------
@fn void AntMasterRunActiveState(void)
@brief Selects and runs one iteration of the current state in the state machine.
All state machines have a TOTAL of 1ms to execute, so on average n state machines
may take 1ms / n to execute.
Requires:
- State machine function pointer points at current state
Promises:
- Calls the function to pointed by the state machine function pointer
*/
void AntMasterRunActiveState(void)
{
  AntMaster_pfStateMachine();

} /* end AntMasterRunActiveState */


/*------------------------------------------------------------------------------------------------------------------*/
/*! @privatesection */                                                                                            
/*--------------------------------------------------------------------------------------------------------------------*/


/**********************************************************************************************************************
State Machine Function Definitions
**********************************************************************************************************************/
/*-------------------------------------------------------------------------------------------------------------------*/

static void Alarm(void)
{
  static u32 u32counter = 0;
  if(WasButtonPressed(BUTTON0) || WasButtonPressed(BUTTON1) || WasButtonPressed(BUTTON2) || WasButtonPressed(BUTTON3) || BTN0 || BTN1 || BTN2 || BTN3)
  {
    if(WasButtonPressed(BUTTON0))
    {
      BTN0 = TRUE;
    }
    else if(WasButtonPressed(BUTTON1))
    {
      BTN1 = TRUE;
    }
    else if(WasButtonPressed(BUTTON2))
    {
      BTN2 = TRUE;
    }
    else if(WasButtonPressed(BUTTON3))
    {
      BTN3 = TRUE;
    }
    ButtonAcknowledge(BUTTON0);
    ButtonAcknowledge(BUTTON1);
    ButtonAcknowledge(BUTTON2);
    ButtonAcknowledge(BUTTON3);
    PWMAudioOff(BUZZER1);
    u32counter = 0;
    AlarmOn = FALSE;
    LedOff(GREEN);
    LedOn(RED);
    AntMaster_pfStateMachine  = AntMasterSM_Idle;
  }
  else if(u32counter > 500 && u32counter < 1000)
  {
    //PWMAudioSetFrequency(BUZZER1, 700);
    PWMAudioOn(BUZZER1);
    u32counter++;
  }
  else if( u32counter == 1000)
  {
    //PWMAudioSetFrequency(BUZZER1, 500);
    PWMAudioOff(BUZZER1);
    u32counter = 0;
  }
  else
  {
    u32counter++;
  }
}

static void AntMasterSM_Idle(void)
{
  // Message to send
  static u8 au8TestMessage[] = {0, 0, 0, 0, 0xA5, 0, 0, 0};
  
  u8 au8DataContent[] = "xxxxxxxx";
  
  if( AntReadAppMessageBuffer() )
  {
     /* New message from ANT task: check what it is */
    if(G_eAntApiCurrentMessageClass == ANT_DATA)
    {
      /* We got some data: parse it into au8DataContent */
      for(u8 i = 0; i < ANT_DATA_BYTES; i++)
      {
          au8DataContent[i] = (char) G_au8AntApiCurrentMessageBytes[i];
      }
      /* If our Acknowledge message received from the Slave is 0x54-0x52-0x55-0x45-0xXX-0xXX-0xXX-0xXX than the Alarm is triggered */
      /* How this would work, is after the set amount of time has passed where the Master and the Slave are communicating
         this exact acknowldgement signal would be sent triggering the alarm state. */
      if(((au8DataContent[0] == 'T') && (au8DataContent[1] == 'R') && (au8DataContent[2] == 'U') && (au8DataContent[3] == 'E')) && SigOn)
      {
        PWMAudioSetFrequency(BUZZER1, 500);
        LedBlink(GREEN, LED_4HZ);
        AlarmOn = TRUE;
        AntMaster_pfStateMachine = Alarm;
      }
    }
    else if(G_eAntApiCurrentMessageClass == ANT_TICK)
    {
      /* A channel period has gone by: typically this is when new data should be queued to be sent */
      
      //Set message to be sent
        /* Check all the buttons and update au8TestMessage according to the button state */ 
      au8TestMessage[0] = 0xff;
      au8TestMessage[1] = 0xff;
      au8TestMessage[2] = 0xff;
      au8TestMessage[3] = 0xff;

      au8TestMessage[7]++;
      if(au8TestMessage[7] == 0)
      {
        au8TestMessage[6]++;
        if(au8TestMessage[6] == 0)
        {
          au8TestMessage[5]++;
        }
      }
      // Send Message
      AntQueueBroadcastMessage(ANT_CHANNEL, au8TestMessage);
    }
  }
} /* end AntMasterSM_Idle() */
     
static void AntMasterSM_AntChannelAssign(void)
{
  if(AntRadioStatusChannel(ANT_CHANNEL) == ANT_CONFIGURED)
  {
    //DebugPrintf("Channel is opened");
    // Channel assignment is successful
    AntOpenChannelNumber(ANT_CHANNEL);
    AntMaster_pfStateMachine = AntMasterSM_Idle;
  }
  if(IsTimeUp(&AntMaster_u32Timeout, 3000))
  {
     DebugPrintf("Ant Error occured");
     AntMaster_pfStateMachine = AntMasterSM_Error;
  }
}


/*-------------------------------------------------------------------------------------------------------------------*/
/* Handle an error */
static void AntMasterSM_Error(void)          
{
  
} /* end AntMasterSM_Error() */




/*--------------------------------------------------------------------------------------------------------------------*/
/* End of File                                                                                                        */
/*--------------------------------------------------------------------------------------------------------------------*/