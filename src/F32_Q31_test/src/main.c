#include <stdio.h>
#include <stm32f10x.h>
#include <dsp_math_type.h>
#include <dsp_pid.h>

/* Measure CPU Cycle */
#define ARM_CM_DEMCR (*(uint32_t *)0xE000EDFC)
#define ARM_CM_DWT_CTRL (*(uint32_t *)0xE0001000)
#define ARM_CM_DWT_CYCCNT (*(uint32_t *)0xE0001004)

void DWT_Init(void)
{
    if (ARM_CM_DWT_CTRL != 0)
    { // See if DWT is available

        ARM_CM_DEMCR |= 1 << 24; // Set bit 24

        ARM_CM_DWT_CYCCNT = 0;

        ARM_CM_DWT_CTRL |= 1 << 0; // Set bit 0
    }
}

int main(void)
{
    PID_INSTANCE_Q31 pid_instance_q31;
    PID_INSTANCE_FLOAT pid_instance_float;
    DWT_Init();
    uint32_t start, stop, delta;

    start = ARM_CM_DWT_CYCCNT;
    pid_instance_q31->Kp = float_to_q31(0.3f);
    pid_instance_q31->Ki = float_to_q31(0.3f);
    pid_instance_q31->Kd = float_to_q31(0.2f);
    pid_init_q31(pid_instance_q31, 1);
    stop = ARM_CM_DWT_CYCCNT;
    delta = stop - start;
    printf("Q31 Initialization cpu cycle : %d\n", delta);

    start = ARM_CM_DWT_CYCCNT;
    __IO q31_t output = pid_q31(pid_instance_q31, float_to_q31(0.6f) - float_to_q31(0.4f));
    stop = ARM_CM_DWT_CYCCNT;
    delta = stop - start;
    printf("Q31 Caculation cpu cycle : %d\n", delta);

    start = ARM_CM_DWT_CYCCNT;
    pid_instance_float->Kp = 0.3f;
    pid_instance_float->Ki = 0.3f;
    pid_instance_float->Kd = 0.2f;
    pid_init_float(pid_instance_float, 1);
    stop = ARM_CM_DWT_CYCCNT;
    delta = stop - start;
    printf("F32 Initialization cpu cycle : %d\n", delta);

    start = ARM_CM_DWT_CYCCNT;
    __IO float outputf = pid_float(pid_instance_float, 0.6f - 0.4f);
    stop = ARM_CM_DWT_CYCCNT;
    delta = stop - start;
    printf("F32 Caculation cpu cycle : %d\n", delta);
    while (1)
        ;
    return 0;
}