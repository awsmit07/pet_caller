

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
  LcdMessage(LINE1_START_ADDR, "SIG OFF   TIME 00:00"); // just for testing, 
  LcdMessage(LINE2_START_ADDR, "VOL 000   RNGE 00   "); 
      // since we just have one set of values to display we will only need to overlap
      // the values here. 
      // Note I've had issues with odd ascii values getting put into arrays so may have to 
      // cover up the odd values by writing "SIG", "VOL", etc. again
  
  
  
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
                          // will switch to Watch state if BUTTON0 is pressed
{
  LedOn(RED); // just an indicator that this state is being run
  if (WasButtonPressed(BUTTON0))
  {
    UserApp1_pfStateMachine = Watch;
    ButtonAcknowledge(BUTTON0);
    LedOff(RED);
  }
  else if (WasButtonPressed(BUTTON1))
  {
    // will switch the value of Status.adjust_volume_or_range depending on the current value if BUTTON1 is pressed
    ButtonAcknowledge(BUTTON1);
  }
  else if (WasButtonPressed(BUTTON2))
  {
    // calls "control_values" and sets the argument to 0 for up
    ButtonAcknowledge(BUTTON2);
  }
     
  else if (WasButtonPressed(BUTTON3))
  { 
    // calls "control_values" and sets the argument to 1 for down
    ButtonAcknowledge(BUTTON3);
  }
}


static void Watch(void) // waits until the user presses a button, 
                          // calls control_values if BUTTON2 or BUTTON3 is pressed
                          // will switch the value of Status.adjust_volume_or_range depending on the current value
                                  // if BUTTON1 is pressed
                          // will switch to Watch state if BUTTON0 is pressed
{
  LedOn(ORANGE); //to indicate the Watch is being run
  if (WasButtonPressed(BUTTON0))
  {
    UserApp1_pfStateMachine = Idle;
    ButtonAcknowledge(BUTTON0);
    ButtonAcknowledge(BUTTON1);
    ButtonAcknowledge(BUTTON2);
    ButtonAcknowledge(BUTTON3);
    LedOff(ORANGE);
  }


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


}

void display_time(void)
{
  // should be called every second at least
  // displays the current time a places it on the LCD

  /* should be something like this (this is mine for texas hold'em):

  u8 currentBet[4];
  currentBet[0] = num % 10000 / 1000 + 48;
  currentBet[1] = num % 1000 / 100 + 48;
  currentBet[2] = num % 100 / 10 + 48;
  currentBet[3] = num % 10 + 48;
  LcdMessage(LINE1_START_ADDR + 7, currentBet);       // 20 chars long, clears the dipslay
  LcdMessage(LINE1_START_ADDR, "Chips: ");       // 20 chars long, clears the dipslay
  LcdMessage(LINE1_START_ADDR + 11, " Bet:     ");       // 20 chars long, clears the dipslay

  */
}