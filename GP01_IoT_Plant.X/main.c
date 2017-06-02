/*******************************************************************************
 * File: Main.c
 * Author: Armstrong Subero
 * PIC: dsPIC33EP128GP502 @ ~32 MHz, 3.3v
 * Program: GP01_IoT_Plant
 * Compiler: XC16 (Pro) (v1.31, MPLAX X v3.61)
 * Program Version: 1.0
 *                
 * Program Description: This project is an open source IoT plant monitoring 
 *                      device. The dsPIC33 monitors the temperature, soil
 *                      moisture content as well as the light intensity around
 *                      the plant. The light intensity data as well as the 
 *                      moisture and temperature data is transferred wirelessly
 *                      via Bluetooth to a device with which the plant monitor has 
 *                      been paired. There is also an OLED that provides on 
 *                      device monitoring and informs the user whether the plant
 *                      is happy or sad. 
 * 
 *                      Due to problems with probe degeneration, the probe is 
 *                      not always powered on. 
 *                      On power-up the probe takes a reading and after which
 *                      Timer1 is used to generate an  interrupt every 300 seconds 
 *                      and increment a variable.
 *                      When this variable reaches a particular count (24000), 
 *                      it turns on the moisture sensor and stores the result 
 *                      as a boolean.
 *                      The sensor then takes another measurement after the 
 *                      specified time (in this case 2 hours). This is possible
 *                      because the moisture sensor is powered via transistor
 *                      by pin RB15.
 * 
 *                      The program is state machine based and consists of four
 *                      states. State One measures light intensity, state two
 *                      temperature, state three soil moisture and state four
 *                      prints to the on board OLED. 
 * 
 *                       
 * Hardware Description: 
 *                     An SSD1306 is connected to the microcontroller as per
 *                     schematics as well as a  HC-05 Bluetooth module, a 
 *                     2N3906 transistor, a DS1722 based temperature sensor was
 *                     chosen since it uses SPI more sensors can easily be added.
 *                     There is also a Sparkfun soil moisture sensor attached to
 *                     the mirocontroller. See schematics in the Documents folder 
 *                     of this project to see connections. 
 *                    
 *                     Battery pack is Ni-MH (HR6 AA)
 *                     Life: 4 * 2500 mAh = 4.8v @ 2500 mAH current draw at 
 *                     ~50 mA  
 *                     Product needs recharging every 2 days.
 *  
 * Created June 2nd, 2017, 1:50 AM
 * 
 *
 * License:
 * 
 * "Copyright (c) 2017 Armstrong Subero ("AUTHORS")"
 * All rights reserved.
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose, without fee, and without written agreement is
 * hereby granted, provided that the above copyright notice, the following
 * two paragraphs and the authors appear in all copies of this software.
 *
 * IN NO EVENT SHALL THE "AUTHORS" BE LIABLE TO ANY PARTY FOR
 * DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES ARISING OUT
 * OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, EVEN IF THE "AUTHORS"
 * HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * THE "AUTHORS" SPECIFICALLY DISCLAIMS ANY WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY
 * AND FITNESS FOR A PARTICULAR PURPOSE.  THE SOFTWARE PROVIDED HEREUNDER IS
 * ON AN "AS IS" BASIS, AND THE "AUTHORS" HAS NO OBLIGATION TO
 * PROVIDE MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS.
 *
 * Please maintain this header in its entirety when copying/modifying
 * these files.
 * 
 ******************************************************************************/

/*******************************************************************************
 * Includes and defines
 ******************************************************************************/
#include "mcc_generated_files/mcc.h"
#include "dsPIC33_STD.h"
#include "PIC24_PIC33_I2C.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <math.h>
#include <ctype.h>
#include <stdbool.h>
#include "SSD1306_OLED.h"
#include "IoT_Plant_Specific.h"

// Number of states for SM
#define NUM_STATES 4


/*****************************
 * Variables of type extern
 * see TMR1 interrupt routine
 ****************************/
uint16_t msCount;
bool check_soil;
 
/*******************************************************************************
 * Function Prototypes
 ******************************************************************************/
 void initMain(void);

 void SM_STATE_ONE(void);      // Light Intensity 
 void SM_STATE_TWO(void);      // Temperature
 void SM_STATE_THREE(void);    // Soil Moisture
 void SM_STATE_FOUR(void);     // OLED
 
 // Prototypes for temp sensor
 int16_t Read_DS1722();
 void Config_DS1722(uint8_t u8_i);
 
 // booleans to store plant mood
 bool Good_Light  = true;
 bool Good_Moisture = true;
 bool Happy_State = true;
 
/*******************************************************************************
 * Global Variables
 ******************************************************************************/
 
 // enum of each state
 typedef enum
 {
     STATE_ONE,
     STATE_TWO,
     STATE_THREE,
     STATE_FOUR
 }StateType;

 // define state machine table structure
 typedef struct
 {
     StateType State;       // Define the command
     void(*func)(void);     // Defines the function to run
 }StateMachineType;
 
// Table that defines valid states of the sm and function to be executed for 
StateMachineType StateMachine[] =
 {
     {STATE_ONE, SM_STATE_ONE},
     {STATE_TWO, SM_STATE_TWO},
     {STATE_THREE, SM_STATE_THREE},
     {STATE_FOUR, SM_STATE_FOUR}
 };

// Store current state of state machine
StateType SM_STATE = STATE_ONE;

/*******************************************************************************
 * Function:        void Config_DS1722(uint8_t u8_i)
 *
 * PreCondition:    SPI module should have been enabled
 *
 * Input:           Arguments to configure DS1722
 *
 * Output:          None
 *
 * Overview:        Configures the DS1722 temperature sensor
 * 
 * Usage:           void Config_DS1722()
 *
 * Note:            None
 ******************************************************************************/
void Config_DS1722(uint8_t u8_i)
{
    // Enable slave
    LATBbits.LATB7 = 1;
   
    // As per datasheet
    SPI2_Exchange8bit(0x80);
    SPI2_Exchange8bit(u8_i);
    
    // Disable slave
    LATBbits.LATB7 = 0;
}

/*******************************************************************************
 * Function:        int16_t Read_DS1722()
 *
 * PreCondition:    SPI module should have been enabled and DS1722 initialized
 *
 * Input:           None
 *
 * Output:          None
 *
 * Overview:        Reads the DS1722 temperature sensor as per Table 4 of 
 *                  datasheet 
 * 
 * Usage:           uint16_t temp = Read_DS1722()
 *
 * Note:            None
 ******************************************************************************/
int16_t Read_DS1722()
{
    uint16_t u16_lo, u16_hi;
    
    LATBbits.LATB7 = 1;                // assert chip select
    SPI2_Exchange8bit(0x01);           // LSB address
    u16_lo = SPI2_Exchange8bit(0x00);  // read LSByte
    u16_hi = SPI2_Exchange8bit(0x00);  // read MSbyte
    
    LATBbits.LATB7 = 0;                // chip select off
    
    return ((u16_hi<<8) | u16_lo);     // return raw temp values
}

/*******************************************************************************
 * Function:        void SM_STATE_ONE(void)
 *
 * PreCondition:    Table structure for states should have been initialized
 *                  as well as enum for each state
 *
 * Input:           None
 *
 * Output:          None
 *
 * Overview:        State One reads the lighting conditions
 * 
 * Usage:           None
 *
 * Note:            None
 ******************************************************************************/
void SM_STATE_ONE(void)
{
    
    uint16_t conversion;  // Variable for conversion
    uint16_t i;           // Loop variable
    
    // Set channel 0
    ADC1_ChannelSelectSet(ADC1_CHANNEL_AN0);
        
    // Start sampling
    ADC1_SamplingStart();
        
    //Provide Delay
    for(i=0;i <1000;i++)
    {
    }
    
    // Stop sampling
    ADC1_SamplingStop();
      
    // Wait for conversion to complete
    while(!ADC1_IsConversionComplete())
    {
        ADC1_Tasks();   
    }
      
    
    // Read channel 0 (photoresistor)
    conversion = ADC1_Channel0ConversionResultGet();
    
    
    // Bright Sun or Lamp
    if (conversion >= 500)
    {
        SSD1306_Write_Text ( 0, 0, "Lighting Good", 1, WHITE);
        printf("Lighting Good\n");
        Good_Light = true;
    }
    
    // Dim Lamp
    else if ((conversion >= 300) && (conversion <= 499))
    {
        SSD1306_Write_Text ( 0, 0, "Lighting Fair", 1, WHITE);
        printf("Lighting Fair\n");
        Good_Light = true;
    }
    
    // Room with lights off
    else if (conversion <= 299)
    {
        SSD1306_Write_Text ( 0, 0, "Lighting Poor", 1, WHITE);
        printf("Lighting Poor\n");
        Good_Light = false;
    }
    
    // Go to state two
    SM_STATE = STATE_TWO;
}


/*******************************************************************************
 * Function:        void SM_STATE_TWO(void)
 *
 * PreCondition:    Table structure for states should have been initialized
 *                  as well as enum for each state
 *
 * Input:           None
 *
 * Output:          None
 *
 * Overview:        State Two reads the temperature of the DS1722
 * 
 * Usage:           None
 *
 * Note:            None
 ******************************************************************************/

void SM_STATE_TWO(void)
{
    uint16_t i16_temp;
    float f_tempC;
    float f_tempF;
    
    // Read temp sensor
    i16_temp = Read_DS1722();
    
    // Perform Celsius conversion 
    f_tempC = i16_temp;
    f_tempC = f_tempC/256;
    
    // Perform Farenheit conversion
    f_tempF = f_tempC*9/5 + 32;
        
    // Write temperature in Celsius
    SSD1306_Write_Float(0,   20, f_tempC, 1);
    SSD1306_Write_Text (35,  20, "C", 1, WHITE);
        
    // Write temperature in Farenheit
    SSD1306_Write_Float(55,  20, f_tempF, 1);
    SSD1306_Write_Text (95,  20, "F", 1, WHITE);
    
    // Send temp Celsius and Farenheit to BT
    printf("Temp: %.2f C \t %.2f F\n", f_tempC, f_tempF);
        
    // Transition to state three
    SM_STATE = STATE_THREE;  
}

/*******************************************************************************
 * Function:        void SM_STATE_THREE(void)
 *
 * PreCondition:    Table structure for states should have been initialized
 *                  as well as enum for each state
 *
 * Input:           None
 *
 * Output:          None
 *
 * Overview:        State Three reads the soil moisture
 * 
 * Usage:           None
 *
 * Note:            None
 ******************************************************************************/
void SM_STATE_THREE(void)
{
    uint16_t Moisture_Conversion;  // Variable for conversion
    uint16_t i;                    // Loop variable
    
    if (check_soil == true){
        printf("Checking soil\n");
        // Select channel 1 (Moisture sensor)
        ADC1_ChannelSelectSet(ADC1_CHANNEL_AN1);
        
        // Start Sampling
        ADC1_SamplingStart();
        
        //Provide Delay
        for(i=0;i <1000;i++)
        {
        }
    
        // Stop sampling
        ADC1_SamplingStop();
        
        // Wait for conversion to complete
        while(!ADC1_IsConversionComplete())
        {
            ADC1_Tasks();   
        }
    
        // Read ADC1 (Moisture sensor)
        Moisture_Conversion = ADC1_Channel1ConversionResultGet();
   
        // If Moisture Good, set bool true
        if (Moisture_Conversion >= 400)
        {
            Good_Moisture = true;   
        }
    
        // If moisture poor set bool false
        else if (Moisture_Conversion <= 399)
        {
            Good_Moisture = false;
        }
    
        check_soil = false;
        POWER_PIN = OFF;
   }
     
    // Transition to state 4
    SM_STATE = STATE_FOUR;
}


/*******************************************************************************
 * Function:        void SM_STATE_FOUR(void)
 *
 * PreCondition:    Table structure for states should have been initialized
 *                  as well as enum for each state
 *
 * Input:           None
 *
 * Output:          None
 *
 * Overview:        State four writes accumulated buffer to  the SSD1306, prints
 *                  plant status and current count
 * 
 * Usage:           None
 *
 * Note:            None
 ******************************************************************************/
void SM_STATE_FOUR(void)
{
    // print current count max = 12000
    printf("Count: %d\n", msCount);
    
    //////////////////////////////////
    // Display last moisture reading
    //////////////////////////////////
    if (Good_Moisture == true)
    {
       SSD1306_Write_Text ( 0, 10, "Moisture Good", 1, WHITE);
       printf("Moisture Good\n");
    }
    
    else if (Good_Moisture == false)
    {
        SSD1306_Write_Text ( 0, 10, "Dry, water plant", 1, WHITE);
        printf("Dry, water plant\n");
    }
    
    ////////////////////////
    // Display plant mood
    ////////////////////////
    if ((Good_Light == true) && (Good_Moisture == true))
    {
        Happy_State = true;
    }
    
    else
    {
        Happy_State = false;
    }
    
    if (Happy_State == true)
    {
         SSD1306_Write_Text ( 0, 40, "Happy", 3, WHITE);
    }
    
    else if (Happy_State == false)
    {
         SSD1306_Write_Text ( 0, 40, "Sad", 3, WHITE);
    }
    
    ///////////////////////////
    // Write buffer to OLED
    //////////////////////////
    SSD1306_Write_Buffer();
    __delay_ms(1000);
    SSD1306_Clear_Display();
      
    SM_STATE = STATE_ONE; 
}

/*******************************************************************************
 * Function:        RUN_STATEMACHINE(void)
 *
 * PreCondition:    Table structure for states should have been initialized
 *                  as well as enum for each state
 *
 * Input:           None
 *
 * Output:          None
 *
 * Overview:        Allows state machine to run
 * 
 * Usage:           RUN_STATEMACHINE()
 *
 * Note:            None
 ******************************************************************************/
void RUN_STATEMACHINE (void)
{
    // Make Sure States is valid
    if (SM_STATE < NUM_STATES)
    {
        // Call function for state
        (*StateMachine[SM_STATE].func)();
    }
    else
    {
        // Code should never reach here
        while(1)
        {
            // Some exception handling;
        }
    }
}

/*******************************************************************************
 * Function:        int main(void)
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          None
 *
 * Overview:        Main program entry point
 * 
 * Usage:           None
 *
 * Note:            None
 ******************************************************************************/ 
int main(void)
{
    // Initialize main
    initMain();
    
    // Display Logo
    SSD1306_Write_Buffer();
    __delay_ms(3000);
    
    // Clear SSD1306 
    SSD1306_Clear_Display();
    
    // Configure DS1722 as per datasheet
    Config_DS1722(0xE8);
  
    // Initially turn on moisture sensor and perform check on soil
    POWER_PIN = ON;
    __delay_ms(1000);
    check_soil = true;
 
    while(1)
    {
        // Start the state machine
        RUN_STATEMACHINE();
    }
    
    return 0;
 }


/*******************************************************************************
 * Function:        void initMain(void)
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          None
 *
 * Overview:        Contains all initializations required to setup main
 * 
 * Usage:           initMain()
 *
 * Note:            None
 ******************************************************************************/
void initMain(void)
{
     // Turn watchdog timer off
    _SWDTEN = 0;
   
    // Initialize system
    SYSTEM_Initialize();
    __delay_ms(1000);
    
    // Initialize SSD1306
    SSD1306_INIT();
    
    // Moisture sensor pin
    TRISBbits.TRISB15 = 0;
    
    __delay_ms(2000);
}

