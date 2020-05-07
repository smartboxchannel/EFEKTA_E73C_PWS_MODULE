// ###############################################  Soil moisture sensor on nRF52840 ################################################### //
//                                                                                                                                       //
//                                                                                                                                       //
//                                                                                                                                       //
//                                                                                                                                       //
//                                                                                                                                       //
//                                                                                                                                       //
//                                                                                                                                       //
// ##################################################################################################################################### //

#ifndef _MYBOARDNRF5_H_
#define _MYBOARDNRF5_H_

#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus

// Number of pins defined in PinDescription array
#if defined(NRF52840)
#define GPIO_COUNT 2
#define PINS_COUNT           (48u)
#define NUM_DIGITAL_PINS     (48u)
#define NUM_ANALOG_INPUTS    (8u)
#define NUM_ANALOG_OUTPUTS   (8u)
#else
#define GPIO_COUNT 2
#define PINS_COUNT           (32u)
#define NUM_DIGITAL_PINS     (32u)
#define NUM_ANALOG_INPUTS    (8u)
#define NUM_ANALOG_OUTPUTS   (8u)
#endif

/* 
 *  LEDs
 *  
 *  This is optional
 *  
 *  With My Sensors, you can use
 *  hwPinMode() instead of pinMode()
 *  hwPinMode() allows to use advanced modes like OUTPUT_H0H1 to drive LEDs.
 *  https://github.com/mysensors/MySensors/blob/development/drivers/NRF5/nrf5_wiring_constants.h
 *
 */
#define PIN_LED1                (29)
#define PIN_LED2                (31)
#define PIN_LED3                (30)
#define RED_LED                 (PIN_LED1)
#define GREEN_LED            	  (PIN_LED2)
#define BLUE_LED			          (PIN_LED3)

/* 
 *  Buttons
 *  
 *  This is optional
 */
#define PIN_BUTTON            (2)

#define PIN_POWER_PWS         (24)

#define PIN_SENS_PWS          (5)

/* 
 * Analog ports
 *  
 * If you change g_APinDescription, replace PIN_AIN0 with
 * port numbers mapped by the g_APinDescription Array.
 * You can add PIN_AIN0 to the g_APinDescription Array if
 * you want provide analog ports MCU independed, you can add
 * PIN_AIN0..PIN_AIN7 to your custom g_APinDescription Array
 * defined in MyBoardNRF5.cpp
 */
static const uint8_t A0  = ADC_A0;
static const uint8_t A1  = ADC_A1;
static const uint8_t A2  = ADC_A2;
static const uint8_t A3  = ADC_A3;
static const uint8_t A4  = ADC_A4;
static const uint8_t A5  = ADC_A5;
static const uint8_t A6  = ADC_A6;
static const uint8_t A7  = ADC_A7;

/*
 * Serial interfaces
 * 
 * RX and TX are required.
 * If you have no serial port, use unused pins
 * CTS and RTS are optional.
 */
#define PIN_SERIAL_RX       (28)
#define PIN_SERIAL_TX       (3)

/*
 * SPI Interfaces
 * 
 * This is optional
 * 
 * If SPI is defined MISO, MOSI, SCK are required
 * SS is optional and can be used in your sketch.
 */
#define SPI_INTERFACES_COUNT 1

#define PIN_SPI_MISO         (2)
#define PIN_SPI_MOSI         (3)
#define PIN_SPI_SCK          (7)
#define PIN_SPI_SS           (8)

static const uint8_t SS   = PIN_SPI_SS;
static const uint8_t MOSI = PIN_SPI_MOSI;
static const uint8_t MISO = PIN_SPI_MISO;
static const uint8_t SCK  = PIN_SPI_SCK;

/*
 * Wire Interfaces
 *
 * This is optional
 */
#define WIRE_INTERFACES_COUNT 2

#define PIN_WIRE_SDA         (4u)
#define PIN_WIRE_SCL         (5u)

#define PIN_WIRE_SDA1        (13u)
#define PIN_WIRE_SCL1        (14u)

static const uint8_t SDA = PIN_WIRE_SDA;
static const uint8_t SCL = PIN_WIRE_SCL;

#ifdef __cplusplus
}
#endif

#endif
