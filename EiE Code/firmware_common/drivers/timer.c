/*!**********************************************************************************************************************
@file timer.c                                                                
@brief Provide easy access to set up and run a Timer Counter (TC) Peripheral.

------------------------------------------------------------------------------------------------------------------------
GLOBALS
- NONE

CONSTANTS
- NONE

TYPES
- TimerChannelType: TIMER0_CHANNEL0, TIMER0_CHANNEL1, TIMER0_CHANNEL2

PUBLIC FUNCTIONS
- void TimerSet(TimerChannelType eTimerChannel_, u16 u16TimerValue_)
- void TimerStart(TimerChannelType eTimerChannel_)
- void TimerStop(TimerChannelType eTimerChannel_)
- u16 TimerGetTime(TimerChannelType eTimerChannel_)
- void TimerAssignCallback(TimerChannelType eTimerChannel_, fnCode_type fpUserCallback_)
 
PROTECTED FUNCTIONS
- void TimerInitialize(void)
- void TimerRunActiveState(void)

**********************************************************************************************************************/

#include "configuration.h"

/***********************************************************************************************************************
Global variable definitions with scope across entire project.
All Global variable names shall start with "G_<type>Timer"
***********************************************************************************************************************/
/* New variables */
volatile u32 G_u32TimerFlags;                      /*!< @brief Global Timer state flags */


/*--------------------------------------------------------------------------------------------------------------------*/
/* Existing variables (defined in other files -- should all contain the "extern" keyword) */
extern volatile u32 G_u32SystemTime1ms;            /*!< @brief From main.c */
extern volatile u32 G_u32SystemTime1s;             /*!< @brief From main.c */
extern volatile u32 G_u32SystemFlags;              /*!< @brief From main.c */
extern volatile u32 G_u32ApplicationFlags;         /*!< @brief From main.c */


/***********************************************************************************************************************
Global variable definitions with scope limited to this local application.
Variable names shall start with "Timer_<type>" and be declared as static.
***********************************************************************************************************************/
static fnCode_type Timer_fpStateMachine;          /*!< @brief The state machine function pointer */
static fnCode_type Timer_fpTimer1Callback;        /*!< @brief Timer1 ISR callback function pointer */

static u32 Timer_u32Timer1IntCounter = 0;         /*!< @brief Track instances of The TC1 interrupt handler */


/**********************************************************************************************************************
Function Definitions
**********************************************************************************************************************/

/*--------------------------------------------------------------------------------------------------------------------*/
/*! @publicsection */                                                                                            
/*--------------------------------------------------------------------------------------------------------------------*/

/*!---------------------------------------------------------------------------------------------------------------------
@fn void TimerSet(TimerChannelType eTimerChannel_, u16 u16TimerValue_)

@brief Sets the timer tick period (interrupt rate).

Based on the configuration value for the timer, each tick is 2.67us.  It is expected
the user knows this and u16TimerValue_ holds a multiple of 2.67us that is
desired to be timed.

Requires:
- TimerStop should be called before, and TimerStart should be called after 
this function to reset the counter and avoid glitches.

@param eTimerChannel_ holds a valid channel
@param u16TimerValue_ is the number of ticks that will be timed

Promises:
- Updates register TC_RC value with u16TimerValue_

*/
void TimerSet(TimerChannelType eTimerChannel_, u16 u16TimerValue_)
{
  /* Build the offset to the selected channel */
  u32 u32TimerBaseAddress = (u32)AT91C_BASE_TC0;
  u32TimerBaseAddress += (u32)eTimerChannel_;

  /* Load the new timer value */
  (AT91_CAST(AT91PS_TC)u32TimerBaseAddress)->TC_RC = (u32)(u16TimerValue_) & 0x0000FFFF;

} /* end TimerSet() */


/*!---------------------------------------------------------------------------------------------------------------------
@fn void TimerStart(TimerChannelType eTimerChannel_)

@brief Starts the designated Timer.

Requires:
@param eTimerChannel_ is the timer to start

Promises:
- Specified channel on Timer is set to run; if already running it remains running
- Does NOT reset the timer value

*/
void TimerStart(TimerChannelType eTimerChannel_)
{
  /* Build the offset to the selected peripheral */
  u32 u32TimerBaseAddress = (u32)AT91C_BASE_TC0;
  u32TimerBaseAddress += (u32)eTimerChannel_;

  /* Ensure clock is enabled and triggered */
  (AT91_CAST(AT91PS_TC)u32TimerBaseAddress)->TC_CCR |= (AT91C_TC_CLKEN | AT91C_TC_SWTRG);
  
} /* end TimerStart() */


/*!---------------------------------------------------------------------------------------------------------------------
@fn void TimerStop(TimerChannelType eTimerChannel_)

@brief Stops the designated Timer.

Requires:
@param eTimerChannel_ is the timer to stop

Promises:
- Specified timer is stopped; if already stopped it remains stopped
- Does NOT reset the timer value

*/
void TimerStop(TimerChannelType eTimerChannel_)
{
  /* Build the offset to the selected peripheral */
  u32 u32TimerBaseAddress = (u32)AT91C_BASE_TC0;
  u32TimerBaseAddress += (u32)eTimerChannel_;
  
  /* Ensure clock is disabled */
  (AT91_CAST(AT91PS_TC)u32TimerBaseAddress)->TC_CCR |= AT91C_TC_CLKDIS;
  
} /* end TimerStop */


/*!---------------------------------------------------------------------------------------------------------------------
@fn u16 TimerGetTime(TimerChannelType eTimerChannel_)

@brief Returns the current count.

Requires:
@param eTimerChannel_ is the timer to query

Promises:
- Current 16 bit timer value is returned

*/
u16 TimerGetTime(TimerChannelType eTimerChannel_)
{
  /* Build the offset to the selected peripheral */
  u32 u32TimerBaseAddress = (u32)AT91C_BASE_TC0;
  u32TimerBaseAddress += (u32)eTimerChannel_;
  
  /* Read and format the timer count */
  return ((u16)( (AT91_CAST(AT91PS_TC)u32TimerBaseAddress)->TC_CV & 0x0000FFFF));
  
} /* end TimerGetTime */


/*!---------------------------------------------------------------------------------------------------------------------
@fn void TimerAssignCallback(TimerChannelType eTimerChannel_, fnCode_type fpUserCallback_)

@brief Allows user to specify a custom callback function for when the Timer interrupt occurs.

Requires:
@param eTimerChannel_ is the channel to which the callback will be assigned
@param fpUserCallback_ is the function address (name) for the user's callback

Promises:
- fpTimerxCallback loaded with fpUserCallback_

*/
void TimerAssignCallback(TimerChannelType eTimerChannel_, fnCode_type fpUserCallback_)
{
  switch(eTimerChannel_)
  {
    case TIMER0_CHANNEL0:
    {
      break;
    }
    case TIMER0_CHANNEL1:
    {
      Timer_fpTimer1Callback = fpUserCallback_;
      break;
    }
    case TIMER0_CHANNEL2:
    {
      break;
    }
    default:
    {
      break;
    }
  } /* end switch(eTimerChannel_) */
  
} /* end TimerAssignCallback */


/*--------------------------------------------------------------------------------------------------------------------*/
/*! @protectedsection */                                                                                            
/*--------------------------------------------------------------------------------------------------------------------*/

/*!--------------------------------------------------------------------------------------------------------------------
@fn void TimerInitialize(void)

@brief Initializes the State Machine and its variables.

Requires:
- NONE

Promises:
- Timer 1 is configured per timer.h INIT settings

*/
void TimerInitialize(void)
{
  /* Load the block configuration registers */
  AT91C_BASE_TCB0->TCB_BMR = TCB_BMR_INIT;
 
  /* Channel 0 and 2 settings not configured at this time */

  /* Load Channel 1 settings and set the default callback */
  AT91C_BASE_TC1->TC_CMR = TC1_CMR_INIT;
  AT91C_BASE_TC1->TC_RC  = TC1_RC_INIT;
  AT91C_BASE_TC1->TC_IER = TC1_IER_INIT;
  AT91C_BASE_TC1->TC_IDR = TC1_IDR_INIT;
  AT91C_BASE_TC1->TC_CCR = TC1_CCR_INIT;

  Timer_fpTimer1Callback = TimerDefaultCallback;

  /* If good initialization, set state to Idle */
  if( 1 )
  {
    /* Enable required interrupts */
    NVIC_ClearPendingIRQ(IRQn_TC1);
    NVIC_EnableIRQ(IRQn_TC1);
    Timer_fpStateMachine = TimerSM_Idle;
    DebugPrintf("Timer1 initialized\n\r");
    
    /* Flag that the Timer task is ready */
    G_u32ApplicationFlags |= _APPLICATION_FLAGS_TIMER;
  }
  else
  {
    /* The task isn't properly initialized, so shut it down and don't run */
    Timer_fpStateMachine = TimerSM_Error;
  }

} /* end TimerInitialize() */

  
/*!----------------------------------------------------------------------------------------------------------------------
@fn void TimerRunActiveState(void)

@brief Selects and runs one iteration of the current state in the state machine.

All state machines have a TOTAL of 1ms to execute, so on average n state machines
may take 1ms / n to execute.

Requires:
- State machine function pointer points at current state

Promises:
- Calls the function to pointed by the state machine function pointer

*/
void TimerRunActiveState(void)
{
  Timer_fpStateMachine();

} /* end TimerRunActiveState */


/*!----------------------------------------------------------------------------------------------------------------------
@fn void TC1_IrqHandler(void)

@brief Parses the TC1 interrupts and handles them appropriately.  

Note that all enabled TC1 interrupts are ORed and will trigger this handler, 
therefore any expected interrupt that is enabled must be parsed out 
and handled.

Requires:
- NONE

Promises:

If Channel1 RC interrupt:
- Timer Channel 1 is reset and automatically restarts counting
- AT91C_TC_CPCS is cleared
- Associated callback function is invoked
- IRQn_TC1 interrupt flag is cleared

*/
void TC1_IrqHandler(void)
{
  /* Check for RC compare interrupt - reading TC_SR clears the bit if set */
  if(AT91C_BASE_TC1->TC_SR & AT91C_TC_CPCS)
  {
    Timer_u32Timer1IntCounter++;
    Timer_fpTimer1Callback();
  }

  /* Clear the TC pending flag and exit */
  NVIC_ClearPendingIRQ(IRQn_TC1);
  
} /* end TC1_IrqHandler() */


/*------------------------------------------------------------------------------------------------------------------*/
/*! @privatesection */                                                                                            
/*--------------------------------------------------------------------------------------------------------------------*/
  
/*!----------------------------------------------------------------------------------------------------------------------
@fn static void TimerDefaultCallback(void)

@brief An empty function that the Timer Callback points to.  Expected that the 
user will set their own.

Requires:
- NONE 

Promises:
- NONE 

*/
static void TimerDefaultCallback(void)
{
  
} /* end TimerDefaultCallback() */


/**********************************************************************************************************************
State Machine Function Definitions
**********************************************************************************************************************/

/*!-------------------------------------------------------------------------------------------------------------------
@fn static void TimerSM_Idle(void)

@brief Wait for a message to be queued 
*/static void TimerSM_Idle(void)
{
   
} /* end TimerSM_Idle() */
     

/*-------------------------------------------------------------------------------------------------------------------*/
/* Handle an error */
static void TimerSM_Error(void)          
{
  
} /* end TimerSM_Error() */




/*--------------------------------------------------------------------------------------------------------------------*/
/* End of File                                                                                                        */
/*--------------------------------------------------------------------------------------------------------------------*/
