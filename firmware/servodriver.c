/*! ************************************************************************** *
 * @file servodriver.c
 * @brief Servo control library. Generate 20 servomotor PWM outputs.
 *
 *
 * @author Benjamin Balga
 * @version 1.0
 *
 * @changelog 17-11-2013
 *
 *  ************************************************************************** */

#include <lpc17xx.h>
//#include "chip.h"
//#include "FreeRTOSConfig.h"
#include "servodriver.h"

#define CLK_US  (( ( unsigned long ) SystemCoreClock )/1000000)
#define PERIOD_REG_US(DELAY)    ((DELAY)*(CLK_US))

#define IIR_PWMMR0 0x01
#define IIR_PWMMR1  0x02
#define IIR_PWMMR2  0x04

#define PCLK_PWM1      (1 << 12)    // PCLK_periph = CCLK
#define PCLK_PWM1_MASK (3 << 12)

#define BASE_PERIOD    2100


static unsigned int prv_au32ServoSerie1HighTime_us[10] = {0};
static unsigned int prv_au32ServoSerie2HighTime_us[10] = {0};


typedef struct {
    unsigned int *port;
    unsigned int pin;
} xServoOut;

#define SERVO_OUT_ENTRY(INDEX)  {SERVO##INDEX##_OUT_PORT, SERVO##INDEX##_OUT_PIN}


#ifndef SERVO_DRIVER_DEBUG
static xServoOut prv_axServoOutputs[20] = 
{
    SERVO_OUT_ENTRY(1),
    SERVO_OUT_ENTRY(2),
    SERVO_OUT_ENTRY(3),
    SERVO_OUT_ENTRY(4),
    SERVO_OUT_ENTRY(5),
    SERVO_OUT_ENTRY(6),
    SERVO_OUT_ENTRY(7),
    SERVO_OUT_ENTRY(8),
    SERVO_OUT_ENTRY(9),
    SERVO_OUT_ENTRY(10),

    SERVO_OUT_ENTRY(11),
    SERVO_OUT_ENTRY(12),
    SERVO_OUT_ENTRY(13),
    SERVO_OUT_ENTRY(14),
    SERVO_OUT_ENTRY(15),
    SERVO_OUT_ENTRY(16),
    SERVO_OUT_ENTRY(17),
    SERVO_OUT_ENTRY(18),
    SERVO_OUT_ENTRY(19),
    SERVO_OUT_ENTRY(20)
}
#else
extern unsigned int prv_axServoOutputs[20];
#endif



#define SERIE_1_PIN(INDEX)   (INDEX)
#define SERIE_2_PIN(INDEX)   (10+INDEX)

#define MAP(VAL,FROMLOW,FROMHIGH,TOLOW,TOHIGH) ((VAL-FROMLOW)*(TOHIGH-TOLOW) / (FROMHIGH-FROMLOW) + TOLOW)



void prv_vServoDriver_pwmInit(void);




/*! ************************************************************************** *
 * @brief Initialisation des 20 PWM ( réglage GPIO et timer)
 *
 * @param Frequency, Fréquence du CCLK en MHz
 * @return Void
 *  ************************************************************************** */
//configCPU_CLOCK_HZ

void vServoDriver_init(void)
{
    unsigned int i;
    for (i = 0; i < 10; i++)
    {
        prv_au32ServoSerie1HighTime_us[i] = 1500;
        prv_au32ServoSerie2HighTime_us[i] = 1500;
    }

    prv_vServoDriver_pwmInit();
}

/**
 * @brief Initialize PWM1 peripheral
 *
 * @return None
 */
void prv_vServoDriver_pwmInit(void)
{
    // Match 0 : period
    // Match 1 : high time servo 1-10
    // Match 2 : high time servo 11-20
    
  // Clock setup in system_LPC17xx.c
    // Turn on power to PWM1
    LPC_SC->PCONP |=  (1 << 6);
    // Setup Clock
    LPC_SC->PCLKSEL0 &= ~(PCLK_PWM1_MASK);
    LPC_SC->PCLKSEL0 |=   PCLK_PWM1;     // PCLK_periph = CCLK
    
    LPC_PWM1->CTCR = 0x00;  // Count on prescaler
    LPC_PWM1->PR = 0;    // Prescaler = 0 -> Count = CLK

    LPC_PWM1->MCR = (1<<0) | (1<<1) // IT on MR0, Reset on MR0
                  | (1<<3)          // IT on MR1
                  | (1<<6);         // IT on MR2
    LPC_PWM1->PCR = 0x00;       // Single Edge, no outputs enabled

    // Set base period
    LPC_PWM1->MR0 = PERIOD_REG_US(BASE_PERIOD);       // PWM Period : 2ms (from 10MHz)

    // Set base match
    LPC_PWM1->MR1 = PERIOD_REG_US(1500);
    LPC_PWM1->MR2 = PERIOD_REG_US(BASE_PERIOD - 1500);

    LPC_PWM1->LER = (1<<0) | (1<<1) | (1<<2);   // Enable MR0/1/2 Latches

    NVIC_EnableIRQ(PWM1_IRQn); // Enable PWM interrupt

    LPC_PWM1->TCR = (1<<0)      // Counter enable
                  | (1<<3);     // PWM Enable
}



/**
 * @brief Set the servo position in microseconds
 *
 * @param u32ServoIndex Servo index (0-19)
 * @param u32PulseWidth Pulse time in microseconds (1000-2000)
 * @return None
 */
void vServoDriver_setServoPosition(unsigned int u32ServoIndex, unsigned int u32PulseWidth)
{
    if (u32PulseWidth < 1000 || u32PulseWidth > 2000)
        return;
    
    if (u32ServoIndex <= 10) {
        prv_au32ServoSerie1HighTime_us[u32ServoIndex] = u32PulseWidth;
    } else if (u32ServoIndex < 20) {
        prv_au32ServoSerie2HighTime_us[u32ServoIndex] = u32PulseWidth;
    }
}


/**
 * @brief Set the servo position as angle
 *
 * @param u32ServoIndex Servo index (0-19)
 * @param u32PulseWidth Servo angle in range 0-180°
 * @return None
 */
void vServoDriver_setServoAngle(unsigned int u32ServoIndex, float fAngle)
{
    if (u32ServoIndex <= 10) {
        prv_au32ServoSerie1HighTime_us[u32ServoIndex] = MAP(fAngle, 0.0, 180.0, 1000, 2000);
    } else if (u32ServoIndex < 20) {
        prv_au32ServoSerie2HighTime_us[u32ServoIndex] = MAP(fAngle, 0.0, 180.0, 1000, 2000);
    }
}



static
void prv_vServoDriver_setCompareMatch1(unsigned int u32ServoIndex)
{
    LPC_PWM1->MR1 = PERIOD_REG_US(prv_au32ServoSerie1HighTime_us[u32ServoIndex]);
}

static
void prv_vServoDriver_setCompareMatch2(unsigned int u32ServoIndex)
{
    LPC_PWM1->MR2 = PERIOD_REG_US(BASE_PERIOD - prv_au32ServoSerie2HighTime_us[u32ServoIndex]);
}

static
void prv_vServoDriver_setServoOutputHigh(unsigned int servo)
{
    // Set pin high
#ifndef SERVO_DRIVER_DEBUG
    xServoOut *servo = &prv_axServoOutputs[servo];
    //*servo.port 
#else
    prv_axServoOutputs[servo] = 1;
#endif
}


static
void prv_vServoDriver_setServoOutputLow(unsigned int servo)
{
    // Set pin low
#ifndef SERVO_DRIVER_DEBUG
    xServoOut *servo = &prv_axServoOutputs[servo];
    //*servo.port 
#else
    prv_axServoOutputs[servo] = 0;
#endif
}



/*! ************************************************************************** *
 * @brief PWM1 Interrupt
 *
 * @return None
 *  ************************************************************************** */
void PWM1_IRQHandler(void) __irq
{
    unsigned int IRValue = LPC_PWM1->IR;
    static unsigned int u32CurrentServoIndex = 9;


    if (IRValue & IIR_PWMMR0)
    {
        // Match 0 IT
        u32CurrentServoIndex++;
        if (u32CurrentServoIndex >= 10)
            u32CurrentServoIndex = 0;

        // Set servo out pin high of current servo serie 1
        prv_vServoDriver_setServoOutputHigh(SERIE_1_PIN(u32CurrentServoIndex));
        // Set servo out pin low of current servo serie 2
        prv_vServoDriver_setServoOutputLow(SERIE_2_PIN(((u32CurrentServoIndex-1)%10)));

        // Set compare match of next servo
        prv_vServoDriver_setCompareMatch1((u32CurrentServoIndex)%10);
        prv_vServoDriver_setCompareMatch2((u32CurrentServoIndex)%10);
        LPC_PWM1->LER = (1<<1) | (1<<2);   // Enable MR1/2 Latches to update match values
        
        LPC_PWM1->IR |= IIR_PWMMR0; // Clear flag
    }

    if (IRValue & IIR_PWMMR1)
    {
        // Match 1 IT
        // Set servo out pin low of current servo serie 1
        prv_vServoDriver_setServoOutputLow(SERIE_1_PIN(u32CurrentServoIndex));
        
        LPC_PWM1->IR |= IIR_PWMMR1; // Clear flag
    }

    if (IRValue & IIR_PWMMR2)
    {
        // Match 2 IT
        // Set servo out pin high of current servo serie 2
        prv_vServoDriver_setServoOutputHigh(SERIE_2_PIN(u32CurrentServoIndex));
        
        LPC_PWM1->IR |= IIR_PWMMR2; // Clear flag
    }
}

