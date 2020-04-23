/**
 * \file      can_common_cmds.c
 * \author    L. BOURDEL
 * \version   1.0
 * \date      2020
 * \brief     Main function
 *
 * \details   !!! Good Luck !!!
 */
 
#include "can_common_cmds.h"


void CanCommonCmdRouter(void)
{

	destinataire_TX = destinataire_RX;
	commande_TX = 0x1F;
	carte_cible_TX = 0x00;
	param_TX = 12;

	TxHeader.DLC = CAN_FIFO_Rx.CanRxFrame[CAN_FIFO_Rx.first].DLC;
	TxData[0] = 1;
	TxData[1] = 2;
	TxData[2] = 3;

	flagReceived=0;

	/* Start the Transmission process */
	Can_SendMessage();
	CountTransmit++;
}
