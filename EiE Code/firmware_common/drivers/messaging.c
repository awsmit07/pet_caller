/*!**********************************************************************************************************************
@file messaging.c                                                                
@brief This file contains utilities to create and monitor outgoing messages.  It essentially provides a 
smart queue system for outgoing data.

Any message being transferred by a peripheral is kept here.  As the peripheral sends the message,
it talks to messaging.c to get data and update the message status.  

All interaction between the peripheral and this task is through unique message tokens that are assigned
to every message queued to messaging.c

The API is simplified and provides just a single function for high-level client tasks to check the
status of their message based on a provided unique token.  If the status of a message is such that
it will no longer change (i.e. COMPLETE, TIMOEOUT, ABANDONDED) then that message's status in the loop
will be cleared automatically.  There is ample room in the message status array to keep sufficient message 
status history given the processing time of the firmware.  However, the status array is circular so 
message statuses can be lost eventually.  If a task waits too long and received a "NOT FOUND" status 
because they have waited too long, the task should increase the frequency at which it queries the 
message status.

------------------------------------------------------------------------------------------------------------------------
GLOBALS
- NONE

CONSTANTS
- NONE

TYPES
- MessageStateType {EMPTY, WAITING, SENDING, COMPLETE, 
                    TIMEOUT, ABANDONED, NOT_FOUND}

PUBLIC FUNCTIONS
- MessageStateType QueryMessageStatus(u32 u32Token_)

PROTECTED FUNCTIONS
- void MessagingInitialize(void)
- u32 QueueMessage(MessageType** ppsTargetTxBuffer_, u32 u32MessageSize_, u8* pu8MessageData_)
- void DeQueueMessage(MessageType** pTargetQueue_)
- void UpdateMessageStatus(u32 u32Token_, MessageStateType eNewState_)


**********************************************************************************************************************/

#include "configuration.h"

/***********************************************************************************************************************
Global variable definitions with scope across entire project.
All Global variable names shall start with "G_<type>Messaging"
***********************************************************************************************************************/
/* New variables */
u32 G_u32MessagingFlags;                               /*!< @brief Global state flags */


/*--------------------------------------------------------------------------------------------------------------------*/
/* Existing variables (defined in other files -- should all contain the "extern" keyword) */
extern volatile u32 G_u32SystemTime1ms;                /*!< @brief From main.c */
extern volatile u32 G_u32SystemTime1s;                 /*!< @brief From main.c */
extern volatile u32 G_u32SystemFlags;                  /*!< @brief From main.c */
extern volatile u32 G_u32ApplicationFlags;             /*!< @brief From main.c */


/***********************************************************************************************************************
Global variable definitions with scope limited to this local application.
Variable names shall start with "Msg_<type>" and be declared as static.
***********************************************************************************************************************/
static fnCode_type Messaging_pfnStateMachine;          /*!< @brief The state machine function pointer */

static u32 Msg_u32Token;                               /*!< @brief Incrementing message token used for all external communications */

static MessageSlotType Msg_asPool[U8_TX_QUEUE_SIZE];   /*!< @brief Array of MessageSlotType used for the transmit queue */
static u8 Msg_u8QueuedMessageCount;                    /*!< @brief Number of messages slots currently occupied */

/* A separate status queue needs to be maintained since the message information in Msg_asPool will be lost when the message
has been dequeued.  Applications must be able to query to determine the status of their message, particularly if
it has been sent. */
static MessageStatusType  Msg_asStatusQueue[U8_STATUS_QUEUE_SIZE]; /*!< @brief Array of MessageStatusType used to monitor message status */
static MessageStatusType* Msg_psNextStatus;                        /*!< @brief Pointer to next available message status */



/**********************************************************************************************************************
Function Definitions
**********************************************************************************************************************/

/*--------------------------------------------------------------------------------------------------------------------*/
/*! @publicsection */                                                                                            
/*--------------------------------------------------------------------------------------------------------------------*/

/*!---------------------------------------------------------------------------------------------------------------------
@fn MessageStateType QueryMessageStatus(u32 u32Token_)

@brief Checks the state of a message and returns the current MessageStateType

If the state is COMPLETE, TIMEOUT or ABANDONED, calling this function
forces the associated status to be cleared from the message queue.
Since the queue is quite short, most of the time it will hold very little entries.  
New entries are always filled at the front, using a simple linear search starting at index 0.

Requires:
@param u32Token_ is the token (ID) of the message of interest

Promises:
- Returns MessageStateType indicating the status of the message
- if the message is found in COMPLETE, TIMEOUT, or ABANDONED state, the status is removed from
the queue and time-stamped for debugging purposes.

*/
MessageStateType QueryMessageStatus(u32 u32Token_)
{
  MessageStateType eStatus = NOT_FOUND;
  MessageStatusType* pListParser = &Msg_asStatusQueue[0];
  
  /* Brute force search for the token - the queue will never be large enough on this system to require a more
  intelligent search algorithm */
  while( (pListParser->u32Token != u32Token_) && 
         (pListParser != &Msg_asStatusQueue[U8_STATUS_QUEUE_SIZE]) )
  {
    pListParser++;
  }

  /* If the token was found pListParser is pointing at it, take appropriate action */
  if(pListParser != &Msg_asStatusQueue[U8_STATUS_QUEUE_SIZE])
  {
    /* Save the status */
    eStatus = pListParser->eState;

    /* Release the slot if the message state is final (the client must deal with it now) */
    if( (eStatus == COMPLETE) || (eStatus == TIMEOUT) || (eStatus == ABANDONED) )
    {
      pListParser->u32Token = 0;
      pListParser->eState = EMPTY;
      pListParser->u32Timestamp = G_u32SystemTime1ms;
    }
  }

  /* If the message was not found, the state is already set correctly, so just return */
  return(eStatus);
  
} /* end QueryMessageStatus() */


/*--------------------------------------------------------------------------------------------------------------------*/
/*! @protectedsection */                                                                                            
/*--------------------------------------------------------------------------------------------------------------------*/

/*!--------------------------------------------------------------------------------------------------------------------
@fn void MessagingInitialize(void)

@brief Runs required initialization for the task.  

Should only be called once in main init section.

Requires:
- No messaging in progress

Promises:
- Message queues are zeroed
- Flags and state machine are initialized

*/
void MessagingInitialize(void)
{
  /* Initialize variables */
  Msg_u8QueuedMessageCount = 0;
  Msg_u32Token = 1;

  /* Ensure all message slots are deallocated and the message status queue is empty */
  for(u8 i = 0; i < U8_TX_QUEUE_SIZE; i++)
  {
    /* Clear the Slot value */
    Msg_asPool[i].bFree = TRUE;
    
    /* Clear the slot's message values */
    Msg_asPool[i].Message.u32Token = 0;
    Msg_asPool[i].Message.u32Size = 0;
    Msg_asPool[i].Message.psNextMessage = NULL;
    
    /* Clear the slot's message's contents */
    for(u16 j = 0; j < U16_MAX_TX_MESSAGE_LENGTH; j++)
    {
      *(Msg_asPool[i].Message.pu8Message + j) = 0;
    }
  }

  /* Clear the message status queue */
  for(u8 i = 0; i < U8_STATUS_QUEUE_SIZE; i++)
  {
    Msg_asStatusQueue[i].u32Token = 0;
    Msg_asStatusQueue[i].eState = EMPTY;
    Msg_asStatusQueue[i].u32Timestamp = 0;
  }

  Msg_psNextStatus = &Msg_asStatusQueue[0];

  G_u32MessagingFlags = 0;
  Messaging_pfnStateMachine = MessagingSM_Idle;

} /* end MessagingInitialize() */


/*!--------------------------------------------------------------------------------------------------------------------
@fn void MessagingRunActiveState(void)

@brief Selects and runs one iteration of the current state in the state machine.

All state machines have a TOTAL of 1ms to execute, so on average n state machines
may take 1ms / n to execute.

Requires:
- State machine function pointer points at current state

Promises:
- Calls the function to pointed by the state machine function pointer

*/
void MessagingRunActiveState(void)
{
  Messaging_pfnStateMachine();

} /* end MessagingRunActiveState */


/*!--------------------------------------------------------------------------------------------------------------------
@fn u32 QueueMessage(MessageType** ppsTargetTxBuffer_, u32 u32MessageSize_, u8* pu8MessageData_)

@brief Allocates one of the positions in the message queue to the calling function's send queue.

Requires:
- Msg_asPool should not be full 

@param  ppsTargetTxBuffer_ is the peripheral transmit buffer where the message will be queued
@param  u32MessageSize_ is the size of the message data array in bytes
@param  pu8MessageData_ points to the message data array

Promises:
- The message is inserted into the target list and assigned a token
- If the message is created successfully, the message token is returned; otherwise, NULL is returned

*/
u32 QueueMessage(MessageType** ppsTargetTxBuffer_, u32 u32MessageSize_, u8* pu8MessageData_)
{
  MessageSlotType *psSlotParser;
  MessageType *psNewMessage;
  MessageType *psListParser;
  u8  u8SlotsRequired;
  u32 u32BytesRemaining = u32MessageSize_;
  u32 u32CurrentMessageSize = 0;
  u32 u32MaxTxMessageLength = (u32)(U16_MAX_TX_MESSAGE_LENGTH) & 0x0000FFFF;
  
  /* Check for empty message */
  if(u32MessageSize_ == 0)
  {
    return(0);
  }

  /* Carefully check for available space in the message pool */
  u8SlotsRequired = (u8)(u32MessageSize_ / u32MaxTxMessageLength);
  if( (u32MessageSize_ % u32MaxTxMessageLength) != 0 )
  {
    u8SlotsRequired++;
  }

  if( (Msg_u8QueuedMessageCount + u8SlotsRequired) > U8_TX_QUEUE_SIZE)
  {
    G_u32MessagingFlags |= _MESSAGING_TX_QUEUE_FULL;
    return(0);
  }

  /* Space available, so proceed with allocation.  Though only one message is queued at a time, we
  use a while loop to handle messages that are too big and must be split into different slots.  The slots
  are always sequential and the message processor will send the bytes continuously across slots */
  while(u32BytesRemaining)
  {
    /* Increment the message count.  Interrupts are disabled here since this global
    can be accessed by an interrupt. */
    __disable_irq();
    Msg_u8QueuedMessageCount++;
    __enable_irq();
  
    /* Flag if we're above the high watermark */
    if(Msg_u8QueuedMessageCount >= U8_TX_QUEUE_WATERMARK)
    {
      G_u32MessagingFlags |= _MESSAGING_TX_QUEUE_ALMOST_FULL;
    }
    else
    {
      G_u32MessagingFlags &= ~_MESSAGING_TX_QUEUE_ALMOST_FULL;
    }

    /* Find an empty slot: this is non-circular and there must be at least one free slot if we're here */
    psSlotParser = &Msg_asPool[0];
    while(!psSlotParser->bFree)
    {
      psSlotParser++;
    }
    
    /* Allocate the slot and set the message pointer */
    psSlotParser->bFree = FALSE;
    psNewMessage = &(psSlotParser->Message);
  
    /* Check the message size and split the message up if necessary */
    if(u32BytesRemaining > u32MaxTxMessageLength)
    {
      u32CurrentMessageSize = u32MaxTxMessageLength;
      u32BytesRemaining -= u32MaxTxMessageLength;
    }
    else
    {
      u32CurrentMessageSize = u32BytesRemaining;
      u32BytesRemaining = 0;
    }
    
    /* Copy all the data to the allocated message structure */
    psNewMessage->u32Token      = Msg_u32Token;
    psNewMessage->u32Size       = u32CurrentMessageSize;
    psNewMessage->psNextMessage = NULL;
    
    /* Add the data into the payload */
    for(u32 i = 0; i < psNewMessage->u32Size; i++)
    {
      *(psNewMessage->pu8Message + i) = *pu8MessageData_;
      pu8MessageData_++;
    }
  
    /* Link the new message into the client's transmit buffer.  This must happen
    with interrupts off since other functions can operate on the transmit buffer. */
    __disable_irq();
    
    /* Handle an empty list */
    if(*ppsTargetTxBuffer_ == NULL)
    {
      *ppsTargetTxBuffer_ = psNewMessage;
    }

    /* Add the message to the end of the list */
    else
    {
      /* Find the last node */
      psListParser = *ppsTargetTxBuffer_;
      while(psListParser->psNextMessage != NULL)
      {
        psListParser = psListParser->psNextMessage;
      }
     
      /* Found the end: add the new node */
      psListParser->psNextMessage = psNewMessage;
    }

    /* Safe to re-enable interrupts */
    __enable_irq();

    /* Update the Public status of the message in the status queue */
    AddNewMessageStatus(Msg_u32Token);
  
    /* Increment message token and catch the rollover every 4 billion messages... Token 0 is not allowed. */
    Msg_u32Token++;
    if(Msg_u32Token == 0)
    {
      Msg_u32Token = 1;
    }
      
  } /* end while */

  /* Return only the current (and highest) message token, as it will be the last portion to be sent if the message was split up */
  return(psNewMessage->u32Token);
  
} /* end QueueMessage() */


/*!--------------------------------------------------------------------------------------------------------------------
@fn void DeQueueMessage(MessageType** pTargetQueue_)

@brief Removes a message from a message queue and adds it back to the pool.

Requires:
- The message to be removed has been completely sent and is no longer in use
- New message cannot be added into the list during this function (via interrupts)

@param  pTargetQueue_ is a FIFO linked-list where the message that needs to be killed is at the front of the list

Promises:
- The first message in the list is deleted; the list is hooked back up
- The message space is added back to the available message queue

*/
void DeQueueMessage(MessageType** pTargetQueue_)
{
  MessageSlotType *psSlotParser;
      
  /* Make sure there is a message to kill */
  if(*pTargetQueue_ == NULL)
  {
    G_u32MessagingFlags |= _DEQUEUE_GOT_NULL;
    return;
  }
  
  /* Find the message's slot: this message pool is non-circular and the message must be one of the slots */
  psSlotParser = &Msg_asPool[0];
  while( (&psSlotParser->Message != *pTargetQueue_) && 
         (psSlotParser != &Msg_asPool[U8_TX_QUEUE_SIZE]) )
  {
    psSlotParser++;
  }

  /* Make sure the message has been found */
  if(psSlotParser == &Msg_asPool[U8_TX_QUEUE_SIZE])
  {
    G_u32MessagingFlags |= _DEQUEUE_MSG_NOT_FOUND;
    return;
  }

  /* Unhook the message from the current owner's queue and put it back in the pool */
  *pTargetQueue_ = (*pTargetQueue_)->psNextMessage;
  psSlotParser->bFree = TRUE;
  Msg_u8QueuedMessageCount--;
  
} /* end DeQueueMessage() */


/*!--------------------------------------------------------------------------------------------------------------------
@fn void UpdateMessageStatus(u32 u32Token_, MessageStateType eNewState_)

@brief Changes the status of a message in the statue queue.

Requires:
@param u32Token_ is message that should be in the status queue
@param eNewState_ is the desired status setting for the message

Promises:
- if the token is found, the eState of the message is set to eNewState_

*/
void UpdateMessageStatus(u32 u32Token_, MessageStateType eNewState_)
{
  MessageStatusType* pListParser = &Msg_asStatusQueue[0];
  
  /* Search for the token */
  while( (pListParser->u32Token != u32Token_) && (pListParser != &Msg_asStatusQueue[U8_STATUS_QUEUE_SIZE]) )
  {
    pListParser++;
  }

  /* If the token was found, change the status */
  if(pListParser != &Msg_asStatusQueue[U8_STATUS_QUEUE_SIZE])
  {
    pListParser->eState = eNewState_;
  }
  
} /* end UpdateMessageStatus() */


/*------------------------------------------------------------------------------------------------------------------*/
/*! @privatesection */                                                                                            
/*--------------------------------------------------------------------------------------------------------------------*/

/*!--------------------------------------------------------------------------------------------------------------------
@fn static void AddNewMessageStatus(u32 u32Token_)

@brief Adds a new message into the message status queue.  

Due to the tendency of applications to forget that they wrote a message here, 
this buffer is circular and will overwrite the oldest message if it needs space for a 
new message.

Requires:
- Msg_psNextStatus points to the next status location

@param u32Token_ is the token of the message of interest

Promises:
- A new status is created at Msg_psNextStatus indexed by u32Token_

*/
static void AddNewMessageStatus(u32 u32Token_)
{
  /* Install the new message */
  Msg_psNextStatus->u32Token = u32Token_;
  Msg_psNextStatus->eState = WAITING;
  Msg_psNextStatus->u32Timestamp = G_u32SystemTime1ms;
  
  /* Safely advance the pointer */
  Msg_psNextStatus++;
  if(Msg_psNextStatus == &Msg_asStatusQueue[U8_STATUS_QUEUE_SIZE])
  {
    Msg_psNextStatus = &Msg_asStatusQueue[0];
  }
  
} /* end AddNewMessageStatus() */

/**********************************************************************************************************************
State Machine Function Definitions
**********************************************************************************************************************/

/*!-------------------------------------------------------------------------------------------------------------------
@fn static void MessagingSM_Idle(void)

@brief Right now this doesn't do anything 
*/
static void MessagingSM_Idle(void)
{
  static u32 u32CleaningTime = U32_MSG_STATUS_CLEANING_TIME;
  
  /* Periodically check for stale messages */
  if(--u32CleaningTime == 0)
  {
    u32CleaningTime = U32_MSG_STATUS_CLEANING_TIME;
    
    /* ??? Probably should add clean of the main message queue to detect any messages that have become stuck */
  }
    
} /* end MessagingSM_Idle() */


#if 0
/*-------------------------------------------------------------------------------------------------------------------*/
/* Handle an error */
static void MessagingSM_Error(void)          
{
  
} /* end MessagingSM_Error() */
#endif



/*--------------------------------------------------------------------------------------------------------------------*/
/* End of File */
/*--------------------------------------------------------------------------------------------------------------------*/
