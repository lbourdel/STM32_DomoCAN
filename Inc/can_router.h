#ifndef CAN_ROUTER_H
#define CAN_ROUTER_H

#include "main.h"

// ************************************************************************************
// *********************************** ENUM *******************************************
// ************************************************************************************


// ************************************************************************************
// ********************************** STRUCT*******************************************
// ************************************************************************************
/**
  * @brief  CAN Rx message header structure definition
  */
typedef struct
{
	uint8_t destinataire;
	uint8_t commande;
	uint8_t carte_cible;
	uint8_t param;	

	uint8_t DLC;      /*!< Specifies the length of the frame that will be transmitted.
                         This parameter must be a number between Min_Data = 0 and Max_Data = 8. */
	uint8_t data[8];
	
  uint32_t FilterMatchIndex; /*!< Specifies the index of matching acceptance filter element.
																This parameter must be a number between Min_Data = 0 and Max_Data = 0xFF. */
} CAN_FrameTypeDef;

/**
  * @brief  CAN Rx FIFO structure definition
  */
#define Rx_Fifo_Size  100
typedef struct
{
	CAN_FrameTypeDef CanRxFrame[Rx_Fifo_Size];
	uint8_t first;
	uint8_t last;	
	uint8_t NumberOfFrame;
} CAN_FifoRxTypeDef;

// ************************************************************************************
// ********************************** DEFINE ******************************************
// ************************************************************************************

#define Err_G1    0x80
#define Err_G2    0x81
#define Err_G3    0x82
#define Err_G4    0x83
#define Err_G5    0x84
#define Err_G6    0x85
#define Err_G7    0x86
#define Err_G8    0x87
#define Err_G9    0x88
#define Err_G10   0x89
#define Err_G11   0x8A
#define Err_G12   0x8B
#define Err_G13   0x8C
#define Err_G14   0x8D
#define Err_G15   0x8E
#define Err_G16   0x8F
#define Err_H1    0x90
#define Err_S1    0x98





#define CMD_COMM  0
#define CMD_INPUT  1

#define destinataire_RX		CAN_FIFO_Rx.CanRxFrame[CAN_FIFO_Rx.first].destinataire
#define commande_RX 			CAN_FIFO_Rx.CanRxFrame[CAN_FIFO_Rx.first].commande
#define carte_cible_RX 		CAN_FIFO_Rx.CanRxFrame[CAN_FIFO_Rx.first].carte_cible
#define param_RX 					CAN_FIFO_Rx.CanRxFrame[CAN_FIFO_Rx.first].param

// ************************************************************************************
// *********************************** VARs *******************************************
// ************************************************************************************
extern uint8_t flagReceived;

extern CAN_HandleTypeDef     CanHandle;
extern CAN_TxHeaderTypeDef   TxHeader;

extern uint8_t               TxData[8];
extern uint32_t              TxMailbox;

extern CAN_FifoRxTypeDef CAN_FIFO_Rx ;

extern uint8_t destinataire_TX;
extern uint8_t commande_TX;
extern uint8_t carte_cible_TX;
extern uint8_t param_TX;

extern uint16_t CountTransmit;
extern uint16_t FailTransmit;
extern uint16_t CountReceivedFifo0;

// ************************************************************************************
// ********************************* FUNCTION *****************************************
// ************************************************************************************
void Error_Handler(void);
void CAN_Config(void);
void CanRouter(void);
void Error_Handler(void);
void Can_SendMessage(void);


void CanSendError(uint8_t ErrorCode);

#endif /* CAN_ROUTER_H */

