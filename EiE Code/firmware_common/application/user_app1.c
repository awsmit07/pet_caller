

//version: PetCallerBase V: 0.1

/* NOTES:
we may not need two states since a lot of the code is the same

*/



#include "configuration.h"

//Global variable definitions with scope across entire project.
//All Global variable names shall start with "G_<type>UserApp1"

volatile u32 G_u32UserApp1Flags;                          /*!< @brief Global state flags */
//A variable should be declared volatile whenever its value could change unexpectedly. aka. interrupts

/* Existing variables (defined in other files -- should all contain the "extern" keyword) */
extern volatile u32 G_u32SystemTime1ms;                   /*!< @brief From main.c */ //used often
extern volatile u32 G_u32SystemTime1s;                    /*!< @brief From main.c */
extern volatile u32 G_u32SystemFlags;                     /*!< @brief From main.c */
extern volatile u32 G_u32ApplicationFlags;                /*!< @brief From main.c */

static fnCode_type UserApp1_pfStateMachine;               /*!< @brief The state machine function pointer */
//static u32 UserApp1_u32Timeout;                           /*!< @brief Timeout counter used across states */


void UserApp1Initialize(void)
{
  
/* All discrete LEDs to off */
  LedOff(WHITE);
  LedOff(PURPLE);
  LedOff(BLUE);
  LedOff(CYAN);
  LedOff(GREEN);
  LedOff(YELLOW);
  LedOff(ORANGE);
  LedOff(RED);
  
      // since we just have one set of values to display we will only need to overlap
      // the values here. 
      // Note I've had issues with odd ascii values getting put into arrays so may have to 
      // cover up the odd values by writing "SIG", "VOL", etc. again

  TheStatus.label[0] = "Volume";
  TheStatus.label[1] = " Range";  // the space just helps with formatting
  TheStatus.adjust_volume_or_range = 0;

  TheStatus.volume = 10;  // sets the start volume of the device, max value is max_value min value is 0
  TheStatus.range = 1;  // sets the start volume of the device, max value is max_range min is 1

  TheStatus.reference_time = 0;
  
  
  
  if( 1 )
  {
    UserApp1_pfStateMachine = Idle; //first state = Idle
  }
  else
  {
    /* The task isn't properly initialized, so shut it down and don't run */
    UserApp1_pfStateMachine = UserApp1SM_Error;
  }
} /* end UserApp1Initialize() */


void UserApp1RunActiveState(void) //runs the state that  UserApp1_pfStateMachine(); points to
{
  UserApp1_pfStateMachine();
} /* end UserApp1RunActiveState */

static void Idle(void)  // waits until the user presses a button, 
                          // calls control_values if BUTTON2 or BUTTON3 is pressed
                          // will switch the value of Status.adjust_volume_or_range depending on the current value
                                  // if BUTTON1 is pressed
                          // will switch to Wait state if BUTTON0 is pressed
{
  LedOn(RED); // just an indicator that this state is being run
  static int Idle_SubState = 1;
  
  if (Idle_SubState == 1)
  {
    LcdMessage(LINE1_START_ADDR, "SIG OFF   TIME 00:00");

    u8 vol[4];
    vol[0] = TheStatus.volume / 100 + 48;
    vol[1] = TheStatus.volume % 100 / 10 + 48;
    vol[2] = TheStatus.volume % 10 + 48;

    u8 rng[2];
    rng[0] = TheStatus.range / 10 + 48;
    rng[1] = TheStatus.range % 10 + 48;
    
    LcdMessage(LINE2_START_ADDR + 4, vol);
    LcdMessage(LINE2_START_ADDR + 15, rng); 
    LcdMessage(LINE2_START_ADDR, "VOL ");
    LcdMessage(LINE2_START_ADDR + 7, "   RNGE ");
    LcdMessage(LINE2_START_ADDR + 17, "   ");
    Idle_SubState = 0;
  }

  if (WasButtonPressed(BUTTON0))
  {
    UserApp1_pfStateMachine = Wait;
    ButtonAcknowledge(BUTTON0);
    LedOff(RED);
  }

  else if (WasButtonPressed(BUTTON1))
  {
    // will switch the value of Status.adjust_volume_or_range depending on the current value if BUTTON1 is pressed
    
    static u32 display_timer = 0;
    if ( display_timer == 0 )
    {
      
      TheStatus.adjust_volume_or_range = (TheStatus.adjust_volume_or_range + 1) % 2;        // switches the volume or range mode
      display_timer = G_u32SystemTime1ms;
      LcdMessage(LINE1_START_ADDR, "                    ");
      LcdMessage(LINE2_START_ADDR, "                    ");
      LcdMessage(LINE1_START_ADDR + 2, TheStatus.label[TheStatus.adjust_volume_or_range]);
      LcdMessage(LINE1_START_ADDR + 8, " Adjustmet");
      LcdMessage(LINE2_START_ADDR + 8, "Mode");
    }
    else if (G_u32SystemTime1ms >=  display_timer + display_duration)
    {
      display_timer = 0;
      Idle_SubState = 1;   // will then display the normal display
      ButtonAcknowledge(BUTTON1);
    }
  }
  else if (WasButtonPressed(BUTTON2))
  {
    // calls "control_values" and sets the argument to 0 for up
    control_values(0);
    Idle_SubState = 1;
    ButtonAcknowledge(BUTTON2);
  }
     
  else if (WasButtonPressed(BUTTON3))
  { 
    // calls "control_values" and sets the argument to 1 for down
    control_values(1);
    Idle_SubState = 1;
    ButtonAcknowledge(BUTTON3);
  }
}



static void Wait(void) // waits until the user presses a button, 
                          // calls control_values if BUTTON2 or BUTTON3 is pressed
                          // will switch the value of Status.adjust_volume_or_range depending on the current value
                                  // if BUTTON1 is pressed
                          // will switch to Wait state if BUTTON0 is pressed
{
  LedOn(ORANGE); //to indicate the Wait is being run
  static int W_SubState = 1;
  if (W_SubState == 1)
  {
    TheStatus.reference_time = G_u32SystemTime1s;
    W_SubState = 0;
  }

  
  if (G_u32SystemTime1ms % 200 == 0)   // updates the displayed time every 0.2 seconds
    {
      u8 min[2];
      min[0] = (G_u32SystemTime1s - TheStatus.reference_time) / 60 / 10 + 48;
      min[1] = (G_u32SystemTime1s - TheStatus.reference_time) / 60 % 10 + 48;

      u8 sec[2];
      sec[0] = (G_u32SystemTime1s - TheStatus.reference_time) % 60 / 10 + 48;
      sec[1] = (G_u32SystemTime1s - TheStatus.reference_time) % 60 % 10 + 48;
      LcdMessage(LINE1_START_ADDR + 15, min);
      LcdMessage(LINE1_START_ADDR + 18, sec); 
      LcdMessage(LINE1_START_ADDR, "SIG ON    TIME ");
      LcdMessage(LINE1_START_ADDR + 17, ":");
    }
  
  
  if (WasButtonPressed(BUTTON0))
  {
    UserApp1_pfStateMachine = Idle;
    ButtonAcknowledge(BUTTON0);
    ButtonAcknowledge(BUTTON1);
    ButtonAcknowledge(BUTTON2);
    ButtonAcknowledge(BUTTON3);
    
    W_SubState = 1;
    LedOff(ORANGE);
  }
  
  if (WasButtonPressed(BUTTON3))  // simulates the connection of the device
  {
  }

}





static void Connecting(void)  // confirm that the pet is waiting for a set amount of time
{
  // has a local varile that takes in the current time of the program and waits 10 seconds from that time
}




static void Alarm(void)    // uses an audio cue to alert the user
{

}





static void UserApp1SM_Error(void)          
{
  //i think this happens if it takes longer then 1ms, unsure if it is
  LcdMessage(LINE1_START_ADDR, "     ERROR     ");
  LedOff(LCD_RED);
  LedOff(LCD_BLUE);
  LedOff(LCD_GREEN);
  LedOn(LCD_RED);
} /* end UserApp1SM_Error() */

//Functions down here

void control_values(int UpOrDown)
{
  // changes the volume or the range up or down depending on the Status.adjust_volume_or_range and the argument 
  if (TheStatus.adjust_volume_or_range == 0)
  {
    if (UpOrDown == 0)
    {
      if (TheStatus.volume < 100)
      {
        TheStatus.volume += 5;
      }
    }
    else
    {
      if (TheStatus.volume > 0)
      {
        TheStatus.volume -= 5; 
      }
    }
  }
  else if (TheStatus.adjust_volume_or_range == 1)
  {
    if (UpOrDown == 0)
    {
      if (TheStatus.range < 12)
      {
        TheStatus.range += 1;
      }
    }
    else
    {
      if (TheStatus.range > 1)
      {
        TheStatus.range -= 1;
      }
    }

  }
}




/*      //DECIDED TO NOT USE THIS FUNCTION
u8 *convert_int_to_string(int value)   //before sending time to this function should convert to mins and seconds seperately
{
  // should be called every second at least
  // displays the current time a places it on the LCD

  should be something like this (this is mine for texas hold'em):

  u8 currentBet[4];
  currentBet[0] = num % 10000 / 1000 + 48;
  currentBet[1] = num % 1000 / 100 + 48;
  currentBet[2] = num % 100 / 10 + 48;
  currentBet[3] = num % 10 + 48;
  LcdMessage(LINE1_START_ADDR + 7, currentBet);       // 20 chars long, clears the dipslay
  LcdMessage(LINE1_START_ADDR, "Chips: ");       // 20 chars long, clears the dipslay
  LcdMessage(LINE1_START_ADDR + 11, " Bet:     ");       // 20 chars long, clears the dipslay

  
  u8 array[4];
  array[0] = value % 10000 / 1000 + 48;
  array[1] = value % 1000 / 100 + 48;
  array[2] = value % 100 / 10 + 48;
  array[3] = value % 10 + 48;
  return array;
}
*/