/******************************************************************************
*
* Freescale Semiconductor Inc.
* (c) Copyright 2013 Freescale Semiconductor, Inc.
* ALL RIGHTS RESERVED.
*
***************************************************************************
*
* THIS SOFTWARE IS PROVIDED BY FREESCALE "AS IS" AND ANY EXPRESSED OR
* IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
* OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
* IN NO EVENT SHALL FREESCALE OR ITS CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
* INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
* SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
* HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
* STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
* IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
* THE POSSIBILITY OF SUCH DAMAGE.
*
***************************************************************************//*!
*
* @file uCOS-II_KE02.c
*
* @author Freescale
*
* @version 0.0.1
*
* @date Jun. 25, 2013
*
* @brief providing framework of demo cases for MCU. 
*
*******************************************************************************/

#include "common.h"
#include "ics.h"
#include "uart.h"
#include "sysinit.h"
#include "ucos_ii.h"
#include "system_MKE02Z2.h"

/******************************************************************************
* Global variables
******************************************************************************/

/******************************************************************************
* Constants and macros
******************************************************************************/

/******************************************************************************
* Local types
******************************************************************************/

/******************************************************************************
* Local function prototypes
******************************************************************************/

/******************************************************************************
* Local variables
******************************************************************************/
static OS_STK   AppTaskStartStk[APP_CFG_TASK_START_STK_SIZE];
static OS_STK   AppTaskLedRStk[APP_CFG_TASK_LED_R_STK_SIZE];
static OS_STK   AppTaskLedGStk[APP_CFG_TASK_LED_G_STK_SIZE];
static OS_STK   AppTaskLedBStk[APP_CFG_TASK_LED_B_STK_SIZE];

static OS_EVENT* RSem;
static OS_EVENT* GSem;
static OS_EVENT* BSem;
static OS_EVENT* OffSem;
/******************************************************************************
* Local functions
******************************************************************************/
int main (void);
static void AppTaskStart (void *p_arg);
static void AppTaskLedR(void *p_arg);
static void AppTaskLedG(void *p_arg);
static void AppTaskLedB(void *p_arg);
/******************************************************************************
* Global functions
******************************************************************************/


/********************************************************************/
int main (void)
{
    
    OSInit();
    
    OSTaskCreate(AppTaskStart, (void*)0, &AppTaskStartStk[APP_CFG_TASK_START_STK_SIZE-1], (INT8U)APP_CFG_TASK_START_PRIO );
    
    OSStart();
    
    return (1);
    
}

static void AppTaskStart (void *p_arg)
{
  INT8U err;
  
  (void)p_arg;
  
  sysinit();
  OS_CPU_SysTickInit(20000000/OS_TICKS_PER_SEC); 
  
  PORT->HDRVE |=0x02;
  
  LED0_Init(); 
  LED1_Init();
  LED2_Init();
  LED0_Off();
  LED1_Off();
  LED2_Off();
  
  RSem = OSSemCreate(1);
  GSem = OSSemCreate(1);
  BSem = OSSemCreate(1);
  OffSem = OSSemCreate(1);
  
  OSTaskCreate(AppTaskLedR, (void*)0, &AppTaskLedRStk[APP_CFG_TASK_LED_R_STK_SIZE-1], (INT8U)APP_CFG_TASK_LED_R_PRIO );
  OSTaskCreate(AppTaskLedG, (void*)0, &AppTaskLedGStk[APP_CFG_TASK_LED_G_STK_SIZE-1], (INT8U)APP_CFG_TASK_LED_G_PRIO );
  OSTaskCreate(AppTaskLedB, (void*)0, &AppTaskLedBStk[APP_CFG_TASK_LED_B_STK_SIZE-1], (INT8U)APP_CFG_TASK_LED_B_PRIO );
  
  while (1)
  {
    OSTimeDlyHMSM(0,0,3,0);
    OSSemPost(RSem);
    OSSemPend(OffSem, 0, &err); 
    
  }
}

static void AppTaskLedR(void *p_arg)
{
  INT8U err;
  
  while(1)
  {
    OSSemPend(RSem,0,&err);
    LED0_On();LED2_Off();
    OSTimeDlyHMSM(0,0,1,0);
    OSSemPost(GSem);
  }
}

static void AppTaskLedG(void *p_arg)
{
  INT8U err;
  
  while(1)
  {
    OSSemPend(GSem,0,&err);
    LED1_On();LED0_Off();
    OSTimeDlyHMSM(0,0,1,0);
    OSSemPost(BSem);
  }
}

static void AppTaskLedB(void *p_arg)
{
  INT8U err;
  
  while(1)
  {
    OSSemPend(BSem,0,&err);
    LED2_On();LED1_Off();
    OSTimeDlyHMSM(0,0,1,0);
    OSSemPost(OffSem);
  }
}
