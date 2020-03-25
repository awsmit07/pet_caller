/*!**********************************************************************************************************************
@file typedefs.h                                                                
@brief Type definitions for use with the system.

The EiE system tries to simplify the most common variable types and remove ambiguity 
over type size or signed/unsigned.  
**********************************************************************************************************************/

#ifndef __TYPEDEFS_H
#define __TYPEDEFS_H


/**********************************************************************************************************************
Type Definitions

The following are equivalent in this system:

Legacy C        Historical       stdint.h       EiE
unsigned char   UCHAR            uint8_t        u8
unsigned short  USHORT           uint16_t       u16
unsigned long   ULONG            uint32_t       u32
signed char     CHAR             int8_t         s8
signed short    SHORT            int16_t        s16
signed long     LONG             int32_t        s32

Custom types should be defined with "Type" as the last word in the typedef.
**********************************************************************************************************************/

/* CHAR/SHORT/LONG types here for legacy code compatibility */
/*! @cond DOXYGEN_EXCLUDE */
typedef char CHAR;              /* Signed 8-bits */
typedef unsigned char UCHAR;    /* Unsigned 8-bits */
typedef short SHORT;            /* Signed 16-bits */
typedef unsigned short USHORT;  /* Unsigned 16-bits */
typedef long LONG;              /* Signed 32-bits */
typedef unsigned long ULONG;    /* Unsigned 32-bits */
typedef unsigned char BOOL;     /* Boolean */
/*! @endcond */


/* Standard Peripheral Library old types (maintained for legacy purpose) */
typedef long s32;           /*!< @brief EiE standard variable type name for signed 32-bit variables */ 
typedef short s16;          /*!< @brief EiE standard variable type name for signed 16-bit variables */
typedef signed char  s8;    /*!< @brief EiE standard variable type name for signed  8-bit variables */

typedef const long sc32;    /*!< @brief EiE standard variable type name for read-only signed 32-bit variables */
typedef const short sc16;   /*!< @brief EiE standard variable type name for read-only signed 16-bit variables */
typedef const char sc8;     /*!< @brief EiE standard variable type name for read-only signed  8-bit variables */

typedef ULONG  u32;         /*!< @brief EiE standard variable type name for unsigned 32-bit variables */
typedef USHORT u16;         /*!< @brief EiE standard variable type name for unsigned 16-bit variables */
typedef UCHAR  u8;          /*!< @brief EiE standard variable type name for unsigned  8-bit variables */

typedef const ULONG uc32;   /*!< @brief EiE standard variable type name for read-only unsigned 32-bit variables */
typedef const USHORT uc16;  /*!< @brief EiE standard variable type name for read-only unsigned 32-bit variables */
typedef const USHORT uc8;   /*!< @brief EiE standard variable type name for read-only unsigned 32-bit variables */

typedef void(*fnCode_type)(void);      /*!< @brief EiE standard variable type name for function pointer with no arguments */
typedef void(*fnCode_u16_type)(u16 x); /*!< @brief EiE standard variable type name for function pointer with one u16 argument */

#ifndef __cplusplus
typedef enum {FALSE = 0, TRUE = !FALSE} bool;  /*!< @brief EiE standard variable type name for boolean */
#endif

/*! 
@enum PortOffsetType
@brief Processor-specific port address offsets.
*/
typedef enum {PORTA = 0, PORTB = 0x80} PortOffsetType;               

/*! 
@enum GpioActiveType
@brief Real names for active low vs. active high GPIO.
*/
typedef enum {ACTIVE_LOW = 0, ACTIVE_HIGH = 1} GpioActiveType;      

/*! 
@struct PinConfigurationType
@brief Parameters necessary to fully describe a GPIO line.
*/
typedef struct
{
  u32 u32BitPosition;             /*!< @brief Pin bit position within port */
  PortOffsetType ePort;           /*!< @brief Pin port */
  GpioActiveType eActiveState;    /*!< @brief Pin hardware active type */
}PinConfigurationType;

/*! 
@struct ErrorStatus
@brief Standarized, self-documenting error codes.
*/
typedef enum {ERROR = 0, SUCCESS = !ERROR} ErrorStatus;


#endif /* __TYPEDEFS_H */

