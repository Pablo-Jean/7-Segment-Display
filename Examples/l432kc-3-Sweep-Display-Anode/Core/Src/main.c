/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>

#include "disp7.h"
#include "disp7_sweep.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define DISP7_TEST_STRING
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
TIM_HandleTypeDef htim6;

UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */
disp7_t Disp7 = {0};
disp7_alone_t Disp7Alone[3] = {0};
disp7_sweep_t Sweep = {0};
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_TIM6_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* Display Segments Pin Function */
void Disp7_Write_Pin(disp7_pins_e Pin, uint8_t Value){
	switch(Pin){
	case 	DISP7_PIN_A:
		HAL_GPIO_WritePin(DISP7_A_GPIO_Port, DISP7_A_Pin, Value);
		break;
	case	DISP7_PIN_B:
		HAL_GPIO_WritePin(DISP7_B_GPIO_Port, DISP7_B_Pin, Value);
		break;
	case	DISP7_PIN_C:
		HAL_GPIO_WritePin(DISP7_C_GPIO_Port, DISP7_C_Pin, Value);
		break;
	case	DISP7_PIN_D:
		HAL_GPIO_WritePin(DISP7_D_GPIO_Port, DISP7_D_Pin, Value);
		break;
	case	DISP7_PIN_E:
		HAL_GPIO_WritePin(DISP7_E_GPIO_Port, DISP7_E_Pin, Value);
		break;
	case	DISP7_PIN_F:
		HAL_GPIO_WritePin(DISP7_F_GPIO_Port, DISP7_F_Pin, Value);
		break;
	case	DISP7_PIN_G:
		HAL_GPIO_WritePin(DISP7_G_GPIO_Port, DISP7_G_Pin, Value);
		break;
	case	DISP7_PIN_dp:
		HAL_GPIO_WritePin(DISP7_DP_GPIO_Port, DISP7_DP_Pin, Value);
		break;
	default:

		break;
	}
}

/* Display PNP Control Functions */
/* The displays are controlled by a PNP Transistor, so, the Turnon stage where the GPIO is on LOW state */
void Disp7_Control_0(uint8_t TurnOn){
	if (TurnOn)
		HAL_GPIO_WritePin(DISP7_1_GPIO_Port, DISP7_1_Pin, GPIO_PIN_RESET);
	else
		HAL_GPIO_WritePin(DISP7_1_GPIO_Port, DISP7_1_Pin, GPIO_PIN_SET);
}

void Disp7_Control_1(uint8_t TurnOn){
	if (TurnOn)
		HAL_GPIO_WritePin(DISP7_2_GPIO_Port, DISP7_2_Pin, GPIO_PIN_RESET);
	else
		HAL_GPIO_WritePin(DISP7_2_GPIO_Port, DISP7_2_Pin, GPIO_PIN_SET);
}

void Disp7_Control_2(uint8_t TurnOn){
	if (TurnOn)
		HAL_GPIO_WritePin(DISP7_3_GPIO_Port, DISP7_3_Pin, GPIO_PIN_RESET);
	else
		HAL_GPIO_WritePin(DISP7_3_GPIO_Port, DISP7_3_Pin, GPIO_PIN_SET);
}

/* timer Callback for sweep */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim){
	if (htim->Instance == TIM6){
		disp7Sweep_timer_interrupt(&Sweep);
	}
}
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */
	disp7_params_t Disp7Params;

#ifdef DISP7_TEST_STRING
	float ValueToShow = 0;
	char StringToShow[32];
#else
	uint32_t ValueToShow = 90;
#endif
  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USART2_UART_Init();
  MX_TIM6_Init();
  /* USER CODE BEGIN 2 */
  Disp7Params.WriteToPinFxn = Disp7_Write_Pin;
  Disp7Params.eType = DISP7_COMMON_ANODE;

  disp7_init(&Disp7, &Disp7Params);

  disp7Sweep_init(&Sweep);

  Disp7Alone[0].fxnControlDisplay = Disp7_Control_0;
  Disp7Alone[0].psDisplay = &Disp7;

  Disp7Alone[1].fxnControlDisplay = Disp7_Control_1;
  Disp7Alone[1].psDisplay = &Disp7;

  Disp7Alone[2].fxnControlDisplay = Disp7_Control_2;
  Disp7Alone[2].psDisplay = &Disp7;

  disp7Sweep_add_display(&Sweep, &Disp7Alone[0]);
  disp7Sweep_add_display(&Sweep, &Disp7Alone[1]);
  disp7Sweep_add_display(&Sweep, &Disp7Alone[2]);

  HAL_TIM_Base_Start_IT(&htim6);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
#ifdef DISP7_TEST_STRING

	  sprintf(StringToShow, "%.2f", ValueToShow);
	  Disp7Sweep_print_string(&Sweep, StringToShow);

	  ValueToShow += 0.01;
	  if (ValueToShow > 9.99){
		  ValueToShow = 0;
	  }
	  HAL_Delay(50);
#else

	  disp7Sweep_print_number(&Sweep, ValueToShow);
	  Disp7Sweep_set_dot(&Sweep, 0, ValueToShow % 2);

	  ValueToShow++;
	  if (ValueToShow > 999){
		  ValueToShow = 0;
	  }
	  HAL_Delay(500);
#endif
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure LSE Drive Capability
  */
  HAL_PWR_EnableBkUpAccess();
  __HAL_RCC_LSEDRIVE_CONFIG(RCC_LSEDRIVE_LOW);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSE|RCC_OSCILLATORTYPE_MSI;
  RCC_OscInitStruct.LSEState = RCC_LSE_ON;
  RCC_OscInitStruct.MSIState = RCC_MSI_ON;
  RCC_OscInitStruct.MSICalibrationValue = 0;
  RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_6;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_MSI;
  RCC_OscInitStruct.PLL.PLLM = 1;
  RCC_OscInitStruct.PLL.PLLN = 16;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV7;
  RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Enable MSI Auto calibration
  */
  HAL_RCCEx_EnableMSIPLLMode();
}

/**
  * @brief TIM6 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM6_Init(void)
{

  /* USER CODE BEGIN TIM6_Init 0 */

  /* USER CODE END TIM6_Init 0 */

  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM6_Init 1 */

  /* USER CODE END TIM6_Init 1 */
  htim6.Instance = TIM6;
  htim6.Init.Prescaler = 31;
  htim6.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim6.Init.Period = 999;
  htim6.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim6) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim6, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM6_Init 2 */

  /* USER CODE END TIM6_Init 2 */

}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  huart2.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart2.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  /* USER CODE BEGIN MX_GPIO_Init_1 */

  /* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, DISP7_1_Pin|DISP7_2_Pin|DISP7_3_Pin|DISP7_F_Pin
                          |DISP7_G_Pin|DISP7_A_Pin, GPIO_PIN_SET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, DISP7_B_Pin|DISP7_E_Pin|DISP7_DP_Pin|DISP7_D_Pin
                          |DISP7_C_Pin, GPIO_PIN_SET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(LD3_GPIO_Port, LD3_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : DISP7_1_Pin DISP7_2_Pin DISP7_3_Pin DISP7_F_Pin
                           DISP7_G_Pin DISP7_A_Pin */
  GPIO_InitStruct.Pin = DISP7_1_Pin|DISP7_2_Pin|DISP7_3_Pin|DISP7_F_Pin
                          |DISP7_G_Pin|DISP7_A_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : DISP7_B_Pin DISP7_E_Pin DISP7_DP_Pin DISP7_D_Pin
                           DISP7_C_Pin */
  GPIO_InitStruct.Pin = DISP7_B_Pin|DISP7_E_Pin|DISP7_DP_Pin|DISP7_D_Pin
                          |DISP7_C_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : LD3_Pin */
  GPIO_InitStruct.Pin = LD3_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LD3_GPIO_Port, &GPIO_InitStruct);

  /* USER CODE BEGIN MX_GPIO_Init_2 */

  /* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
