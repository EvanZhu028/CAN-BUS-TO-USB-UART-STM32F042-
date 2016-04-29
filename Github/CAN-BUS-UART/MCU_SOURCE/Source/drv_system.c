#include "board.h"

typedef struct gpio_config_t
{
    GPIO_TypeDef *gpio;
    uint16_t pin;
    GPIOMode_TypeDef mode;
		GPIOSpeed_TypeDef speed;
	//    GPIOMode_TypeDef otype;
    GPIOPuPd_TypeDef pupd;
	
} gpio_config_t;


// cycles per microsecond
static volatile uint32_t usTicks = 0;
// current uptime for 1kHz systick timer. will rollover after 49 days. hopefully we won't care.
static volatile uint32_t sysTickUptime = 0;

static void cycleCounterInit(void)
{
    RCC_ClocksTypeDef clocks;
    RCC_GetClocksFreq(&clocks);
    usTicks = clocks.SYSCLK_Frequency / 1000000;
}


// SysTick
void SysTick_Handler(void)
{
    sysTickUptime++;
}

// Return system uptime in microseconds (rollover in 70minutes)
uint32_t micros(void)
{
    register uint32_t ms, cycle_cnt;
    do {
        ms = sysTickUptime;
        cycle_cnt = SysTick->VAL;
    } while (ms != sysTickUptime);
    return (ms * 1000) + (48000 - cycle_cnt) / 48;		//72����
}


// Return system uptime in milliseconds (rollover in 49 days)
uint32_t millis(void) {
    return sysTickUptime;
}

void systemInit(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    uint32_t i;
     gpio_config_t gpio_cfg[] = {
         { LED0_GPIO, LED0_PIN, GPIO_Mode_OUT ,GPIO_Speed_2MHz, GPIO_PuPd_NOPULL}, 
         { LED1_GPIO, LED1_PIN, GPIO_Mode_OUT ,GPIO_Speed_2MHz, GPIO_PuPd_NOPULL}, 
         { LCD_CS_GPIO, LCD_CS_PIN, GPIO_Mode_OUT ,GPIO_Speed_10MHz, GPIO_PuPd_NOPULL}, 
         { LCD_DC_GPIO, LCD_DC_PIN, GPIO_Mode_OUT ,GPIO_Speed_10MHz, GPIO_PuPd_NOPULL}, 
         { LCD_RST_GPIO, LCD_RST_PIN, GPIO_Mode_OUT ,GPIO_Speed_10MHz, GPIO_PuPd_NOPULL}, 


//         { J5_GPIO, J5_PIN, GPIO_Mode_IN, GPIO_Speed_2MHz, GPIO_PuPd_DOWN},
     };
    uint8_t gpio_count = sizeof(gpio_cfg) / sizeof(gpio_cfg[0]);

    // This is needed because some shit inside Keil startup fucks with SystemCoreClock, setting it back to 72MHz even on HSI.
//    SystemCoreClockUpdate();



    // Turn on clocks for stuff we use
	
	//		RCC_APB2PeriphClockCmd(RCC_AHBPeriph_GPIOA | RCC_AHBPeriph_GPIOB, ENABLE);
		 
			RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA | RCC_AHBPeriph_GPIOB, ENABLE);
//		 RCC_APB1PeriphClockCmd( RCC_APB1Periph_CAN | RCC_APB1Periph_CEC , ENABLE);
//		 RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
//	    RCC_AHBPeriphClockCmd(RCC_APB2Periph_TIM1 |RCC_APB1Periph_TIM14  , ENABLE);
		//	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM14, ENABLE);
	  
//    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2 |RCC_APB1Periph_TIM3|  RCC_APB1Periph_I2C1|RCC_APB1Periph_CAN1 ,ENABLE);
//    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO |RCC_APB2Periph_TIM1|RCC_APB2Periph_SPI1|RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC  | RCC_APB2Periph_GPIOD , ENABLE);//
//    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
//    RCC_ClearFlag();

	
     for (i = 0; i < gpio_count; i++) {
         GPIO_InitStructure.GPIO_Pin = gpio_cfg[i].pin;
         GPIO_InitStructure.GPIO_Mode = gpio_cfg[i].mode;
         GPIO_InitStructure.GPIO_Speed = gpio_cfg[i].speed;// GPIO_Speed_50MHz;;
//         GPIO_InitStructure.GPIO_OType = gpio_cfg[i].otype;
         GPIO_InitStructure.GPIO_PuPd = gpio_cfg[i].pupd;

         GPIO_Init(gpio_cfg[i].gpio, &GPIO_InitStructure);
     }
	
/*		 
     for (i = 0; i < gpio_count2; i++) {
         GPIO_InitStructure.GPIO_Pin = gpio_cfg[i].pin;
         GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
         GPIO_InitStructure.GPIO_Mode = gpio_cfg[i].mode;
         GPIO_InitStructure.GPIO_PuPd = gpio_cfg[i].pupd;
			 
         GPIO_Init(gpio_cfg[i].gpio, &GPIO_InitStructure);
     }		 
		 
	*/	 
	
//    LED0_OFF;
//    LED1_OFF;


    // Init cycle counter
    cycleCounterInit();

    // SysTick
    SysTick_Config(SystemCoreClock / 1000);

    // Configure the rest of the stuff

    // sleep for 100ms
    delay(100);
}


#if 1
void delayMicroseconds(uint32_t us)
{
    uint32_t now = micros();
    while (micros() - now < us);
}
#else
void delayMicroseconds(uint32_t us)
{
    uint32_t elapsed = 0;
    uint32_t lastCount = SysTick->VAL;

    for (;;) {
        register uint32_t current_count = SysTick->VAL;
        uint32_t elapsed_us;

        // measure the time elapsed since the last time we checked
        elapsed += current_count - lastCount;
        lastCount = current_count;

        // convert to microseconds
        elapsed_us = elapsed / usTicks;
        if (elapsed_us >= us)
            break;

        // reduce the delay by the elapsed time
        us -= elapsed_us;

        // keep fractional microseconds for the next iteration
        elapsed %= usTicks;
    }
}
#endif

void delay(uint32_t ms)
{
    while (ms--)
        delayMicroseconds(1000);
}



void failureMode(uint8_t mode)
{
  //  LED1_ON;
   LED1_OFF;
    while (1) {
//        LED1_TOGGLE;
        LED1_TOGGLE;
        delay(475 * mode - 2);

   
    }
}
/*
#define AIRCR_VECTKEY_MASK    ((uint32_t)0x05FA0000)

void systemReset(bool toBootloader)
{
    if (toBootloader) {
        // 1FFFF000 -> 20000200 -> SP
        // 1FFFF004 -> 1FFFF021 -> PC
        *((uint32_t *)0x20004FF0) = 0xDEADBEEF; // 20KB STM32F103
    }        

    // Generate system reset
    SCB->AIRCR = AIRCR_VECTKEY_MASK | (uint32_t)0x04;
}
*/

