#include "delay.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

void init_CAN() {
	/* Configure CAN peripheral pins */
	// PA11==CAN_RX, PA12==CAN_TX
	// Clock the CAN peripheral

	/* Put into initialization mode */
	// clear CAN_MCR, INRQ // Initialization request
	// wait for CAN_MSR, INAK // Wait for hardware to enter initialization mode

	/* */

}

void transmit_CAN_extended() {
	// TME bit in the CAN_TSR register set // Check CAN mailbox is empty

	// CAN_TIxR, IDE // Set to use extended identifier
	// CAN_TIxR, EXID // Set Extended Identifier
	// CAN_TDTxR, DLC == length in bytes // Set data length code

	// CAN_TIxR, TXRQ // Set the transmit mailbox request
}

#ifdef __cplusplus
}
#endif /* __cplusplus */
