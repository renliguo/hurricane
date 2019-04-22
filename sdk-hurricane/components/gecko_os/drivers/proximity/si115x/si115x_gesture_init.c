#include "si115xDrv.h"

int16_t Si115xInitGesture( HANDLE si115x_handle )
{
    int16_t    retval;

    retval  = Si115xReset( si115x_handle );
    Si115xDelay_10ms();
    retval += Si115xParamSet( si115x_handle, SI115x_PARAM_MEASCOUNT0, 0x01);
    retval += Si115xParamSet( si115x_handle, SI115x_PARAM_LED1_A, 0x3f);
    retval += Si115xParamSet( si115x_handle, SI115x_PARAM_LED2_A, 0x3f);
    retval += Si115xParamSet( si115x_handle, SI115x_PARAM_LED3_A, 0x3f);
    retval += Si115xParamSet( si115x_handle, SI115x_PARAM_CH_LIST, 0x07);
    retval += Si115xParamSet( si115x_handle, SI115x_PARAM_ADCCONFIG0, 0x62);
    retval += Si115xParamSet( si115x_handle, SI115x_PARAM_MEASCONFIG0, 0x21);//61
    retval += Si115xParamSet( si115x_handle, SI115x_PARAM_ADCCONFIG1, 0x62);
    retval += Si115xParamSet( si115x_handle, SI115x_PARAM_MEASCONFIG1, 0x22);
    retval += Si115xParamSet( si115x_handle, SI115x_PARAM_ADCCONFIG2, 0x62);
    retval += Si115xParamSet( si115x_handle, SI115x_PARAM_MEASCONFIG2, 0x24);
    retval += Si115xParamSet( si115x_handle, SI115x_PARAM_MEASRATE_L, 0x19);
    retval += Si115xWriteToRegister( si115x_handle, SI115x_REG_IRQ_ENABLE, 0x07);
    return retval;
}

//
// To start autonomous measurements
//     Si115xStart( si115x_handle )
//


void Si115x_GestureHandler(HANDLE si115x_handle, Si115x_GestureSample *samples)
{
    uint8_t buffer[7];
    Si115xBlockRead( si115x_handle,
                      SI115x_REG_IRQ_STATUS,
                      7,
                      buffer);
    //samples->irq_status = buffer[0];
    samples->ps1  = buffer[1] <<  8;
    samples->ps1 |= buffer[2];
    samples->ps2  = buffer[3] <<  8;
    samples->ps2 |= buffer[4];
    samples->ps3  = buffer[5] <<  8;
    samples->ps3 |= buffer[6];
}


