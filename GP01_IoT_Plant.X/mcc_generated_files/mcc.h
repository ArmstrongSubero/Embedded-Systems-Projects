/**
  @Generated MPLAB(c) Code Configurator Header File

  @Company:
    Microchip Technology Inc.

  @File Name:
    mcc.h

  @Summary:
    This is the mcc.h file generated using MPLAB(c) Code Configurator

  @Description:
    This header file provides implementations for driver APIs for all modules selected in the GUI.
    Generation Information :
        Product Revision  :  MPLAB(c) Code Configurator - pic24-dspic-pic32mm : v1.26
        Device            :  dsPIC33EP128GP502
    The generated drivers are tested against the following:
        Compiler          :  XC16 1.30
        MPLAB             :  MPLAB X 3.45
*/

/*
    (c) 2016 Microchip Technology Inc. and its subsidiaries. You may use this
    software and any derivatives exclusively with Microchip products.

    THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER
    EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED
    WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A
    PARTICULAR PURPOSE, OR ITS INTERACTION WITH MICROCHIP PRODUCTS, COMBINATION
    WITH ANY OTHER PRODUCTS, OR USE IN ANY APPLICATION.

    IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE,
    INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND
    WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS
    BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO THE
    FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN
    ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
    THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.

    MICROCHIP PROVIDES THIS SOFTWARE CONDITIONALLY UPON YOUR ACCEPTANCE OF THESE
    TERMS.
*/

#ifndef MCC_H
#define	MCC_H
#include <xc.h>
#include "pin_manager.h"
#include <stdint.h>
#include <stdbool.h>
#include "spi2.h"
#include "uart1.h"
#include "tmr1.h"
#include "i2c1.h"
#include "interrupt_manager.h"
#include "traps.h"
#include "adc1.h"

#define _XTAL_FREQ  64026875UL

/**
  Section: Type defines
 */ 
    
/** 
 * CORCON initialization type enumerator. Supported types:
 * CORCON_MODE_PORVALUES
 * CORCON_MODE_ENABLEALLSATNORMAL_ROUNDBIASED
 * CORCON_MODE_ENABLEALLSATNORMAL_ROUNDUNBIASED
 * CORCON_MODE_DISABLEALLSAT_ROUNDBIASED
 * CORCON_MODE_DISABLEALLSAT_ROUNDUNBIASED
 * CORCON_MODE_ENABLEALLSATSUPER_ROUNDBIASED
 * CORCON_MODE_ENABLEALLSATSUPER_ROUNDUNBIASED
 */
typedef enum tagCORCON_MODE_TYPE
{ 
    CORCON_MODE_PORVALUES   = 0x0020,                       /** Use POR values of CORCON */
    CORCON_MODE_ENABLEALLSATNORMAL_ROUNDBIASED = 0x00E2,    /** Enable saturation for ACCA, ACCB
                                                             *  and Dataspace write, enable normal
                                                             *  ACCA/ACCB saturation mode and set
                                                             *  rounding to Biased (conventional)
                                                             *  mode. Rest of CORCON settings are
                                                             *  set to the default POR values.
                                                             *  */
    CORCON_MODE_ENABLEALLSATNORMAL_ROUNDUNBIASED = 0x00E0,  /** Enable saturation for ACCA, ACCB
                                                             *  and Dataspace write, enable normal
                                                             *  ACCA/ACCB saturation mode and set
                                                             *  rounding to Unbiased (convergent)
                                                             *  mode. Rest of CORCON settings are
                                                             *  set to the default POR values.
                                                             *  */
    CORCON_MODE_DISABLEALLSAT_ROUNDBIASED = 0x0022,         /** Disable saturation for ACCA, ACCB
                                                             *  and Dataspace write and set
                                                             *  rounding to Biased (conventional)
                                                             *  mode. Rest of CORCON settings are
                                                             *  set to the default POR values.
                                                             *  */
    CORCON_MODE_DISABLEALLSAT_ROUNDUNBIASED = 0x0020,       /** Disable saturation for ACCA, ACCB
                                                             *  and Dataspace write and set
                                                             *  rounding to Unbiased (convergent)
                                                             *  mode. Rest of CORCON settings are
                                                             *  set to the default POR values.
                                                             *  */
    CORCON_MODE_ENABLEALLSATSUPER_ROUNDBIASED = 0x00F2,    /** Enable saturation for ACCA, ACCB
                                                             *  and Dataspace write, enable super
                                                             *  ACCA/ACCB saturation mode and set
                                                             *  rounding to Biased (conventional)
                                                             *  mode. Rest of CORCON settings are
                                                             *  set to the default POR values.
                                                             *  */
    CORCON_MODE_ENABLEALLSATSUPER_ROUNDUNBIASED = 0x00F0,  /** Enable saturation for ACCA, ACCB
                                                             *  and Dataspace write, enable super
                                                             *  ACCA/ACCB saturation mode and set
                                                             *  rounding to Unbiased (convergent)
                                                             *  mode. Rest of CORCON settings are
                                                             *  set to the default POR values.
                                                             *  */
} SYSTEM_CORCON_MODES;

/** 
 * RCON error type enumerator. Supported types:
 * ERR_RCON_TRAPR
 * ERR_RCON_IOPUWR
 * ERR_RCON_CM 
 * ERR_RCON_WDTO_ISR
 */
typedef enum tagERROR_TYPE
{
    ERR_RCON_TRAPR      = 1, /** A Trap Conflict Reset has occurred **/
    ERR_RCON_IOPUWR     = 2, /** An illegal opcode detection, an illegal address mode or Uninitialized W register used as an
                             *   Address Pointer caused a Reset **/
    ERR_RCON_CM         = 3, /** A Configuration Mismatch Reset has occurred **/
    ERR_RCON_WDTO_ISR   = 4  /** WDT time-out has occurred **/
}SYSTEM_RESET_TYPES;

/** 
 * RESET CAUSE Masks. Supported masks:
 * RESET_MASK_WDTO
 * RESET_MASK_SWR
 * REST_MASK_EXTR
 * RESET_MASK_CM
 * RESET_MASK_IOPUWR
 * RESET_MASK_TRAPR
 */
typedef enum tagRESET_MASKS
{ 
  RESET_MASK_WDTO = 0x0010,
  RESET_MASK_SWR = 0x0040,
  RESET_MASK_EXTR = 0x0080, 
  RESET_MASK_CM = 0x0200, 
  RESET_MASK_IOPUWR = 0x4000, 
  RESET_MASK_TRAPR = 0x8000, 
} RESET_MASKS;

/**
 * @Param
    none
 * @Returns
    none
 * @Description
    Initializes the device to the default states configured in the
 *                  MCC GUI
 * @Example
    SYSTEM_Initialize(void);
 */
void SYSTEM_Initialize(void);

/**
 * @Param
    none
 * @Returns
    none
 * @Description
    Initializes the oscillator to the default states configured in the
 *                  MCC GUI
 * @Example
    OSCILLATOR_Initialize(void);
 */
void OSCILLATOR_Initialize(void);

/**
* Checks reset cause, flashes UI with an error code as a result.
* 
* Note: this function should be called before any use of CLRWDT
* since it has a side-effect of clearing the appropriate bits in the
* register showing reset cause (see DS70602B page 8-10)
*/
uint16_t SYSTEM_GetResetCause(void);


/**
 * Enables Watch Dog Timer (WDT) using the software bit.
 * @example
 * <code>
 * WDT_WatchdogtimerSoftwareEnable();
 * </code>
 */
inline static void WDT_WatchdogtimerSoftwareEnable(void)
{
    RCONbits.SWDTEN = 1;
}

/**
 * Disables Watch Dog Timer (WDT) using the software bit.
 * @example
 * <code>
 * WDT_WatchdogtimerSoftwareDisable();
 * </code>
 */
inline static void WDT_WatchdogtimerSoftwareDisable(void)
{
    RCONbits.SWDTEN = 0;
}

/**
 * Clears the Watch Dog Timer (WDT).
 * @example
 * <code>
 * WDT_WatchdogTimerClear();
 * </code>
 */
inline static void WDT_WatchdogTimerClear(void)
{
    ClrWdt();
}

/**
 * Gets the base address of the DEVID register for the currently selected device
 * @return base address of the DEVID register
 * @example
 * <code>
 * uint32_t devIdAddress;
 * devIdAddress = DEVICE_DeviceIdRegisterAddressGet();
 * </code>
 */
inline static uint32_t DEVICE_DeviceIdRegisterAddressGet(void)
{
    return __DEVID_BASE;
}

/**
 * Initializes the CPU core control register.
 * @example
 * <code>
 * CORCON_Initialize();
 * </code>
 */
inline static void CORCON_Initialize()
{
    CORCON = (CORCON & 0x00F2) | CORCON_MODE_PORVALUES;    // POR value
}

/**
 * Sets the CPU core control register operating mode to a value that is decided by the
 * SYSTEM_CORCON_MODES argument.
 * @param modeValue SYSTEM_CORCON_MODES initialization mode specifier
 * @example
 * <code>
 * CORCON_ModeOperatingSet(CORCON_MODE_ENABLEALLSATNORMAL_ROUNDUNBIASED);
 * </code>
 */
inline static void CORCON_ModeOperatingSet(SYSTEM_CORCON_MODES modeValue)
{
    CORCON = (CORCON & 0x00F2) | modeValue;
}

/**
 * Sets the value of CPU core control register.
 * @param value value that needs to be written to the CPU core control register
 * @example
 * <code>
 * CORCON_RegisterValueSet(0x00E2);
 * </code>
 */
inline static void CORCON_RegisterValueSet(uint16_t value)
{
    CORCON = value;
}

/**
 * Gets the value of CPU core control register.
 * @return value of the CPU core control register
 * @example
 * <code>
 * corconSave = CORCON_RegisterValueGet();
 * </code>
 */
inline static uint16_t CORCON_RegisterValueGet(void)
{    
    return CORCON;
}

/**
 * It handles the reset cause by clearing the cause register values.
 * Its a weak function user can override this function.
 * @return None
 * @example
 * <code>
 * SYSTEM_ResetCauseHandler();
 * </code>
 */
void __attribute__ ((weak)) SYSTEM_ResetCauseHandler(void);

/**
 * This function resets the reset cause register.
 * @return None
 * @example
 * <code>
 * SYSTEM_ResetCauseClearAll();
 * </code>
 */
void SYSTEM_ResetCauseClearAll();

#endif	/* MCC_H */
/**
 End of File
*/