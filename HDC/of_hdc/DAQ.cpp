#pragma once

#include "ofMain.h"
#include "DAQ.h"

extern "C" {
    #include <NIDAQmx.h>
    #define DAQmxErrChk(functionCall) if( DAQmxFailed(error=(functionCall)) ) goto Error; else
	int32 CVICALLBACK EveryNCallback(TaskHandle taskHandle, int32 everyNsamplesEventType, uInt32 nSamples, void *callbackData);
	int32 CVICALLBACK DoneCallback(TaskHandle taskHandle, int32 status, void *callbackData);
}

TaskHandle  taskHandle;
TaskHandle  oscHandle;
TaskHandle  fingHandle;
TaskHandle  eegHandle;

int initDAQ(){
    int32       error=0;
    char        errBuff[2048]={'\0'};

    /*********************************************/
    // DAQmx Configure Code
    /*********************************************/
    DAQmxErrChk (DAQmxCreateTask("",&taskHandle));
    DAQmxErrChk (DAQmxCreateTask("",&oscHandle));
    DAQmxErrChk (DAQmxCreateTask("",&fingHandle));
    DAQmxErrChk (DAQmxCreateTask("",&eegHandle));

    //INPUT STUFF
    //DAQmxErrChk (DAQmxCreateAIVoltageChan(taskHandle,"Dev1/ai0","",DAQmx_Val_Cfg_Default,-10.0,10.0,DAQmx_Val_Volts,NULL));
    DAQmxErrChk (DAQmxCreateAIVoltageChan(taskHandle,"Dev1/ai0,Dev1/ai1","",DAQmx_Val_RSE,-10.0,10.0,DAQmx_Val_Volts,NULL));
    //DAQmxErrChk (DAQmxCreateAIVoltageChan(taskHandle,"Dev1/ai1","i1",DAQmx_Val_RSE,-10.0,10.0,DAQmx_Val_Volts,NULL));
    DAQmxErrChk (DAQmxCfgSampClkTiming(taskHandle,"",850.0,DAQmx_Val_Rising,DAQmx_Val_ContSamps,850));
    DAQmxErrChk (DAQmxRegisterEveryNSamplesEvent(taskHandle,DAQmx_Val_Acquired_Into_Buffer,1000,0,EveryNCallback,NULL));
    DAQmxErrChk (DAQmxRegisterDoneEvent(taskHandle,0,DoneCallback,NULL));

    //make sure we get the most recent sample acquired
    DAQmxErrChk (DAQmxSetReadRelativeTo(taskHandle, DAQmx_Val_MostRecentSamp));
    DAQmxErrChk (DAQmxSetReadOffset(taskHandle, -1));
    /*********************************************/
    // DAQmx Start Code
    /*********************************************/
    DAQmxErrChk (DAQmxStartTask(taskHandle));
    printf("DAQ started aquisition");

    //OUTPUT STUFF
    DAQmxErrChk (DAQmxCreateAOVoltageChan(oscHandle,"Dev1/ao0","",-10.0,10.0,DAQmx_Val_Volts,NULL));
    DAQmxErrChk (DAQmxCreateAOVoltageChan(fingHandle,"Dev1/ao1","",-10.0,10.0,DAQmx_Val_Volts,NULL));
    DAQmxErrChk (DAQmxCreateDOChan(eegHandle,"Dev1/port0","",DAQmx_Val_ChanForAllLines));

    DAQmxErrChk (DAQmxStartTask(oscHandle));
    DAQmxErrChk (DAQmxStartTask(fingHandle));
    DAQmxErrChk (DAQmxStartTask(eegHandle));

    return 0;

Error:
    if( DAQmxFailed(error) )
        DAQmxGetExtendedErrorInfo(errBuff,2048);
    if( taskHandle!=0 ) {
        /*********************************************/
        // DAQmx Stop Code
        /*********************************************/
        DAQmxStopTask(taskHandle);
        DAQmxClearTask(taskHandle);
    }
    if( DAQmxFailed(error) )
        printf("DAQmx Error: %s\n",errBuff);


    printf("End of program, press Enter key to quit\n");
    getchar();
    return 0;
}

//--------------------------------------------------------------
int32 CVICALLBACK EveryNCallback(TaskHandle taskHandle, int32 everyNsamplesEventType, uInt32 nSamples, void *callbackData)
{
    int32       error=0;
    char        errBuff[2048]={'\0'};
    static int  totalRead=0;
    int32       read=0;
    float64     data[1000]={0};

    if(read > 0)
        printf("data val: %f\r",data[0]);

    return 0;
}

//--------------------------------------------------------------
int32 CVICALLBACK DoneCallback(TaskHandle taskHandle, int32 status, void *callbackData)
{
    int32   error=0;
    char    errBuff[2048]={'\0'};

    // Check to see if an error stopped the task.
    DAQmxErrChk (status);

Error:
    if( DAQmxFailed(error) ) {
        DAQmxGetExtendedErrorInfo(errBuff,2048);
        DAQmxClearTask(taskHandle);
        printf("DAQmx Error: %s\n",errBuff);
    }
    return 0;
}

//--------------------------------------------------------------
float readDAQ(int input){
    float64 readArray[2];
    int32 sampsPerChanRead;
    DAQmxReadAnalogF64(taskHandle,1,10.0,DAQmx_Val_GroupByChannel,readArray,2,&sampsPerChanRead,NULL);
    return(float)(readArray[input-1]);
}

//--------------------------------------------------------------
void writeDAQ(float x, float y){
    float64 oscOutArr[1]={0};
    float64 fingOutArr[1]={0};
    // Vertical Oscilloscope
    oscOutArr[0]=y;
    // Horizontal Oscilloscope
    fingOutArr[0]=x;
    int32 sampsPerChanWrittenFing;
    int32 sampsPerChanWrittenOsc;
    DAQmxWriteAnalogF64(oscHandle,1,true,0,DAQmx_Val_GroupByChannel,oscOutArr,&sampsPerChanWrittenOsc,NULL);
    DAQmxWriteAnalogF64(fingHandle,1,true,0,DAQmx_Val_GroupByChannel,fingOutArr,&sampsPerChanWrittenFing,NULL);
}

//--------------------------------------------------------------
void trigger(int n){
    uInt32 data;
    switch(n){
        case 0:
            data=0xffffffff;
            break;
        case 1:
            data=0xfffffffe;
            break;
        case 2:
            data=0xfffffffd;
            break;
        case 3:
            data=0xfffffffc;
            break;
        default:
			break;
    }
	int32		written;
    DAQmxWriteDigitalU32(eegHandle,1,1,10.0,DAQmx_Val_GroupByChannel,&data,&written,NULL);
}
