/* EEE3096S Practical 3: ADC channel 5 on PA5 mirrored to DAC channel 1 on PA4.
 * Register fields follow RM0091, ADC and DAC chapters. Clock, GPIO, calibration
 * and startup order are supplied by board_io.c.
 */
#include "voltage_mirror.h"
#include "board_io.h"
#include "main.h"
#include <stddef.h>

/* One conversion is 239.5 + 12.5 = 252 cycles of the 14 MHz ADC clock, so 18 us.
 * This bound is well past that and still inside the 500 us sampling period.
 */
#define MIRROR_READ_TIMEOUT 2000u

/* Coarse mode keeps the six most significant bits of the 12-bit code. */
#define MIRROR_COARSE_MASK 63u

bool voltage_mirror_init(void)
{
    if (!board_converters_prepare()) return false;

    /* Reset state of CFGR1 is what this practical needs: RES 00 for 12 bits,
     * ALIGN 0 for right alignment, CONT 0 for single conversions and EXTEN 00
     * for a software start. Written explicitly rather than assumed.
     */
    ADC1->CFGR1 = 0u;
    ADC1->CHSELR = ADC_CHSELR_CHSEL5;
    ADC1->SMPR = ADC_SMPR_SMP; /* SMP 111: 239.5 ADC clock cycles. */

    /* Midscale before the output is enabled, so PA4 never starts on a stale code.
     * BOFF1 left clear enables the output buffer; TEN1 left clear makes a write
     * to DHR12R1 reach the output one APB cycle later with no trigger.
     */
    DAC->DHR12R1 = 2048u;
    DAC->CR = DAC_CR_EN1;

    return board_adc_enable();
}

bool voltage_mirror_read(uint16_t *code)
{
    if (code == NULL) return false;

    /* EOC, EOS and OVR are write-one-to-clear, so assign rather than or. */
    ADC1->ISR = ADC_ISR_EOC | ADC_ISR_EOS | ADC_ISR_OVR;
    ADC1->CR |= ADC_CR_ADSTART;

    for (uint32_t waited = 0u; waited < MIRROR_READ_TIMEOUT; ++waited)
    {
        if ((ADC1->ISR & ADC_ISR_EOC) != 0u)
        {
            /* Reading DR also clears EOC. */
            *code = (uint16_t)(ADC1->DR & MIRROR_MAX_CODE);
            return true;
        }
    }

    /* Caller stops sampling on false. Leave no conversion running behind us. */
    ADC1->CR |= ADC_CR_ADSTP;
    return false;
}

void voltage_mirror_write(uint16_t code)
{
    if (code > MIRROR_MAX_CODE) code = MIRROR_MAX_CODE;
    DAC->DHR12R1 = code;
}

uint16_t voltage_mirror_map(uint16_t code, bool coarse)
{
    if (code > MIRROR_MAX_CODE) code = MIRROR_MAX_CODE;
    if (!coarse) return code;

    /* Clearing six bits leaves 64 commands spaced 64 apart, the highest 4032.
     * The command stays on the 12-bit DAC scale; the DAC is still 12-bit.
     */
    return (uint16_t)(code & ~MIRROR_COARSE_MASK);
}
