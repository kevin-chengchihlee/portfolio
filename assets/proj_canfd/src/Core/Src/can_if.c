/*
 * can_if.c
 *
 *  Created on: Jan 27, 2026
 *      Author: USER
 */

#include "can_if.h"
#include <string.h>
/**
  * @brief FDCAN1 Initialization Function
  * @param None
  * @retval None
  */
extern FDCAN_HandleTypeDef hfdcan1;
static FDCAN_TxHeaderTypeDef can0_txHeader = {
		.IdType              = FDCAN_STANDARD_ID,
		.TxFrameType         = FDCAN_DATA_FRAME,
		.DataLength          = FDCAN_DLC_BYTES_8,
		.ErrorStateIndicator = FDCAN_ESI_ACTIVE,
		.BitRateSwitch       = FDCAN_BRS_ON,
		.FDFormat            = FDCAN_FD_CAN,
		.TxEventFifoControl  = FDCAN_NO_TX_EVENTS,
		.MessageMarker       = 0
		};

uint8_t can0_txData[8];

FDCAN_RxHeaderTypeDef can0_rxHeader;
uint8_t can0_rxData[8];

extern TIM_HandleTypeDef htim2;

/*Data Fields*/
uint8_t Test_val1_tx[8];
uint8_t Test_val1_rx[8];
uint8_t Test_val2_tx[8];
uint8_t Test_val2_rx[8];
uint8_t Test_val3_tx[8];
uint8_t Test_val3_rx[8];
/*Data Fields*/

void MX_FDCAN1_Init(void)
{

  /* USER CODE BEGIN FDCAN1_Init 0 */

  /* USER CODE END FDCAN1_Init 0 */

  /* USER CODE BEGIN FDCAN1_Init 1 */

  /* USER CODE END FDCAN1_Init 1 */
  hfdcan1.Instance = FDCAN1;
  hfdcan1.Init.ClockDivider = FDCAN_CLOCK_DIV1;
  hfdcan1.Init.FrameFormat = FDCAN_FRAME_FD_BRS;
  hfdcan1.Init.Mode = FDCAN_MODE_NORMAL;
  hfdcan1.Init.AutoRetransmission = DISABLE;
  hfdcan1.Init.TransmitPause = DISABLE;
  hfdcan1.Init.ProtocolException = DISABLE;
  hfdcan1.Init.NominalPrescaler = 6;
  hfdcan1.Init.NominalSyncJumpWidth = 1;
  hfdcan1.Init.NominalTimeSeg1 = 13;
  hfdcan1.Init.NominalTimeSeg2 = 2;
  hfdcan1.Init.DataPrescaler = 2;
  hfdcan1.Init.DataSyncJumpWidth = 2;
  hfdcan1.Init.DataTimeSeg1 = 9;
  hfdcan1.Init.DataTimeSeg2 = 2;
  hfdcan1.Init.StdFiltersNbr = 0;
  hfdcan1.Init.ExtFiltersNbr = 0;
  hfdcan1.Init.TxFifoQueueMode = FDCAN_TX_QUEUE_OPERATION;
  if (HAL_FDCAN_Init(&hfdcan1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN FDCAN1_Init 2 */
  HAL_FDCAN_Start(&hfdcan1);
  HAL_FDCAN_ActivateNotification(&hfdcan1, FDCAN_IT_RX_FIFO0_NEW_MESSAGE, 0);
  /* USER CODE END FDCAN1_Init 2 */

}

void HAL_FDCAN_RxFifo0Callback(FDCAN_HandleTypeDef *hfdcan, uint32_t RxFifo0ITs)
{
  /* Prevent unused argument(s) compilation warning */
  if(hfdcan == &hfdcan1){
	  if ((RxFifo0ITs & FDCAN_IT_RX_FIFO0_NEW_MESSAGE) != 0)
	  {
		  HAL_FDCAN_GetRxMessage(hfdcan, FDCAN_RX_FIFO0, &can0_rxHeader, &can0_rxData[0]);

		  switch (can0_rxHeader.Identifier){
		  case 0x100:
			  memcpy(Test_val1_rx, can0_rxData, 8);
			  break;
		  case 0x110:
			  memcpy(Test_val2_rx, can0_rxData, 8);
			  break;
		  case 0x120:
			  memcpy(Test_val3_rx, can0_rxData, 8);
			  break;
		  default: break;
		}
	  }

  }

  /* NOTE: This function Should not be modified, when the callback is needed,
            the HAL_FDCAN_RxFifo1Callback could be implemented in the user file
   */
}

/**
  * @brief  Period elapsed callback in non-blocking mode
  * @param  htim TIM handle
  * @retval None
  */

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)//100ms
{
  /* Prevent unused argument(s) compilation warning */
	static uint8_t cnt;

	if(htim == &htim2){
		//tim2 interrupt callback for CAN TX
		cnt+=1;
		Test_val1_tx[2] = cnt;
		can0_txHeader.Identifier = 0x100;
		memcpy(can0_txData, Test_val1_tx, 8);
		HAL_FDCAN_AddMessageToTxFifoQ(&hfdcan1, &can0_txHeader, &can0_txData[0]);

		Test_val2_tx[2] = cnt*2;
		can0_txHeader.Identifier = 0x110;
		memcpy(can0_txData, Test_val2_tx, 8);
		HAL_FDCAN_AddMessageToTxFifoQ(&hfdcan1, &can0_txHeader, &can0_txData[0]);

		Test_val3_tx[2] = cnt*3;
		can0_txHeader.Identifier = 0x120;
		memcpy(can0_txData, Test_val3_tx, 8);
		HAL_FDCAN_AddMessageToTxFifoQ(&hfdcan1, &can0_txHeader, &can0_txData[0]);
	}

  /* NOTE : This function should not be modified, when the callback is needed,
            the HAL_TIM_PeriodElapsedCallback could be implemented in the user file
   */
}
