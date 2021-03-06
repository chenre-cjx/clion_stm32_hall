/**
  ******************************************************************************
  * @file    tim.c
  * @brief   This file provides code for the configuration
  *          of the TIM instances.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "tim.h"

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

TIM_HandleTypeDef htim5;

/* TIM5 init function */
void MX_TIM5_Init(void)
{

  /* USER CODE BEGIN TIM5_Init 0 */

  /* USER CODE END TIM5_Init 0 */

  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_IC_InitTypeDef sConfigIC = {0};

  /* USER CODE BEGIN TIM5_Init 1 */

  /* USER CODE END TIM5_Init 1 */
  htim5.Instance = TIM5;
  htim5.Init.Prescaler = 83;
  htim5.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim5.Init.Period = 4294967295;
  htim5.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim5.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
  if (HAL_TIM_IC_Init(&htim5) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim5, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigIC.ICPolarity = TIM_INPUTCHANNELPOLARITY_RISING;
  sConfigIC.ICSelection = TIM_ICSELECTION_DIRECTTI;
  sConfigIC.ICPrescaler = TIM_ICPSC_DIV1;
  sConfigIC.ICFilter = 1;
  if (HAL_TIM_IC_ConfigChannel(&htim5, &sConfigIC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM5_Init 2 */
    HAL_TIM_IC_Start_IT(&htim5,TIM_CHANNEL_1);
    __HAL_TIM_ENABLE_IT(&htim5,TIM_IT_UPDATE);
  /* USER CODE END TIM5_Init 2 */

}

void HAL_TIM_IC_MspInit(TIM_HandleTypeDef* tim_icHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(tim_icHandle->Instance==TIM5)
  {
  /* USER CODE BEGIN TIM5_MspInit 0 */

  /* USER CODE END TIM5_MspInit 0 */
    /* TIM5 clock enable */
    __HAL_RCC_TIM5_CLK_ENABLE();

    __HAL_RCC_GPIOA_CLK_ENABLE();
    /**TIM5 GPIO Configuration
    PA0-WKUP     ------> TIM5_CH1
    */
    GPIO_InitStruct.Pin = GPIO_PIN_0;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_PULLDOWN;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF2_TIM5;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /* TIM5 interrupt Init */
    HAL_NVIC_SetPriority(TIM5_IRQn, 2, 3);
    HAL_NVIC_EnableIRQ(TIM5_IRQn);
  /* USER CODE BEGIN TIM5_MspInit 1 */

  /* USER CODE END TIM5_MspInit 1 */
  }
}

void HAL_TIM_IC_MspDeInit(TIM_HandleTypeDef* tim_icHandle)
{

  if(tim_icHandle->Instance==TIM5)
  {
  /* USER CODE BEGIN TIM5_MspDeInit 0 */

  /* USER CODE END TIM5_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_TIM5_CLK_DISABLE();

    /**TIM5 GPIO Configuration
    PA0-WKUP     ------> TIM5_CH1
    */
    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_0);

    /* TIM5 interrupt Deinit */
    HAL_NVIC_DisableIRQ(TIM5_IRQn);
  /* USER CODE BEGIN TIM5_MspDeInit 1 */

  /* USER CODE END TIM5_MspDeInit 1 */
  }
}

/* USER CODE BEGIN 1 */
//????????????
//[7] 0:??????????; 1:????????????
//[6] 0:??????????????; 1:????????????
//[5:0] ??????????????????????
//
uint8_t tim5_capture_state = 0;     //????????????
uint32_t tim5_capture_value = 0;     //??????????



void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
    printf("captuer\n\t");
    if((tim5_capture_state & 0x80)== 0)
    {
        if (tim5_capture_state & 0x40){
            tim5_capture_state |= 0x80;
            tim5_capture_value = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_1);
            TIM_RESET_CAPTUREPOLARITY(htim, TIM_CHANNEL_1); //??????????????????
            TIM_SET_CAPTUREPOLARITY(htim, TIM_CHANNEL_1,TIM_ICPOLARITY_RISING);    //??????????????
        } else {
            tim5_capture_state = 0;
            tim5_capture_value = 0;     //??????????????
            tim5_capture_state |= 0x40;
            __HAL_TIM_ENABLE(htim);     //??????????
            __HAL_TIM_SET_COUNTER(htim,0);      //????CNT
            TIM_RESET_CAPTUREPOLARITY(htim, TIM_CHANNEL_1); //??????????????????
            TIM_SET_CAPTUREPOLARITY(htim, TIM_CHANNEL_1,TIM_ICPOLARITY_FALLING);    //??????????????
            __HAL_TIM_ENABLE(htim);     //??????????
        }
    }
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if ((tim5_capture_state & 0x80) == 0){
        if (tim5_capture_state & 0x40){
            if ((tim5_capture_state & 0x3f) == 0x3f){
                tim5_capture_state = 0x80;
                tim5_capture_value = 0xffffffff;
            }else {
                tim5_capture_state++;
            }
        }
    }
}
/* USER CODE END 1 */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
