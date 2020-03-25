//user_app2.c will store all the functions from the 

#include "configuration.h"

/***********************************************************************************************************************
Global variable definitions with scope across entire project.
All Global variable names shall start with "G_<type>UserApp2"
***********************************************************************************************************************/
/* New variables */
volatile u32 G_u32UserApp2Flags;                          /*!< @brief Global state flags */


/*--------------------------------------------------------------------------------------------------------------------*/
/* Existing variables (defined in other files -- should all contain the "extern" keyword) */
extern volatile u32 G_u32SystemTime1ms;                   /*!< @brief From main.c */
extern volatile u32 G_u32SystemTime1s;                    /*!< @brief From main.c */
extern volatile u32 G_u32SystemFlags;                     /*!< @brief From main.c */
extern volatile u32 G_u32ApplicationFlags;                /*!< @brief From main.c */


/***********************************************************************************************************************
Global variable definitions with scope limited to this local application.
Variable names shall start with "UserApp2_<type>" and be declared as static.
***********************************************************************************************************************/
static fnCode_type UserApp2_pfStateMachine;               /*!< @brief The state machine function pointer */
//static u32 UserApp2_u32Timeout;                           /*!< @brief Timeout counter used across states */



//store the function definitions here
//--------------------------------------put--the--version--code--here----------------------------------------------------

void TestFunction2(void);// To be removed just an example of using a fuction from another file



//store the functions here
//--------------------------------------put--the--version--code--here----------------------------------------------------

void TestFunction2(void){// To be removed just an example of using a fuction from another file
  LedOn(RED);
  return;
}



//I recive errors if I comment out this below code although I don't use it
//I will fingure out the issue at the end, no rush
void UserApp2Initialize(void)   //not the feature that UserApp2 is being used for but will keep it
{
  /* If good initialization, set state to Idle */
  if( 1 )
  {
    UserApp2_pfStateMachine = UserApp2SM_Idle;
  }
  else
  {
    /* The task isn't properly initialized, so shut it down and don't run */
    UserApp2_pfStateMachine = UserApp2SM_Error;
  }

} /* end UserApp2Initialize() */

void UserApp2RunActiveState(void)
{
  UserApp2_pfStateMachine();

} /* end UserApp2RunActiveState */

static void UserApp2SM_Idle(void)
{
    
} /* end UserApp2SM_Idle() */
     

static void UserApp2SM_Error(void)          
{
  
} /* end UserApp2SM_Error() */



/*--------------------------------------------------------------------------------------------------------------------*/
/* End of File                                                                                                        */
/*--------------------------------------------------------------------------------------------------------------------*/
