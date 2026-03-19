/*
 * can_if.h
 *
 *  Created on: Jan 27, 2026
 *      Author: USER
 */

#ifndef SRC_CAN_IF_H_
#define SRC_CAN_IF_H_

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "stm32u5xx_hal.h"
/* USER CODE END Includes */

void MX_FDCAN1_Init(void);
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim);
void HAL_FDCAN_RxFifo0Callback(FDCAN_HandleTypeDef *hfdcan, uint32_t RxFifo0ITs);

#endif /* SRC_CAN_IF_H_ */
