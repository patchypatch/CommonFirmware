#include "stm32f4xx.h"
#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

extern uint8_t* dma_periph_data_by_spi_array[];
extern uint8_t* dma_periph_data_by_usart_array[];

typedef enum {
	Low,
	Medium,
	High,
	VeryHigh
} dmaPriority_Enum;

typedef enum {
	Peripheral2Memory,
	Memory2Peripheral,
	Memory2Memory
} dmaDirection_Enum;

typedef enum {
	Byte,
	HalfWord,
	Word
} dmaDataSize_Enum;

typedef enum {
	QuarterFull,
	HalfFull,
	ThreeQuarterFull,
	Full,
	DirectMode
} dmaFIFOLevel_Enum;

typedef enum {
	OneBeat,
	FourBeats,
	EightBeats,
	SixteenBeats
} dmaBeatsPerBurst_Enum;

void dma_config(uint8_t dma, uint8_t stream, uint8_t channel, dmaDirection_Enum direction,
				dmaPriority_Enum priority, bool isTransferCompleteInterrupt,
				uint32_t periphAddr, uint32_t memAddr, uint16_t numDataItemsPerTrans,
				dmaDataSize_Enum periphDataSize, dmaDataSize_Enum memDataSize,
				dmaFIFOLevel_Enum fifoThreshold, bool isFlowControlDMA,
				dmaBeatsPerBurst_Enum memBurstBeats, dmaBeatsPerBurst_Enum periphBurstBeats,
				bool isMemAddrIncr, bool isPeriphAddrIncr, bool isCircularMode);

void dma_enable(uint8_t dma, uint8_t stream, uint16_t numDataItems);
void dma_disable(uint8_t dma, uint8_t stream);

void dma1_stream0_handler(void);
void dma1_stream1_handler(void);
void dma1_stream2_handler(void);
void dma1_stream3_handler(void);
void dma1_stream4_handler(void);
void dma1_stream5_handler(void);
void dma1_stream6_handler(void);
void dma1_stream7_handler(void);
void dma2_stream0_handler(void);
void dma2_stream1_handler(void);
void dma2_stream2_handler(void);
void dma2_stream3_handler(void);
void dma2_stream4_handler(void);
void dma2_stream5_handler(void);
void dma2_stream6_handler(void);
void dma2_stream7_handler(void);

extern void interrupt_handler_dma(uint8_t dma, uint8_t stream);

#ifdef __cplusplus
}
#endif /* __cplusplus */