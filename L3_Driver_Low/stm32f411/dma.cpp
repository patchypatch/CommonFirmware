#include <stdio.h>
#include "dma.h"
#include "nvic.h"
#include "rcc.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

uint8_t* dma_periph_data_by_spi_array[] = {nullptr, (uint8_t*)&(SPI1->DR),
											(uint8_t*)&(SPI2->DR), (uint8_t*)&(SPI3->DR),
											(uint8_t*)&(SPI4->DR), (uint8_t*)&(SPI5->DR)};

uint8_t* dma_periph_data_by_usart_array[] = {nullptr, (uint8_t*)&(USART1->DR),
											(uint8_t*)&(USART2->DR), nullptr,
											nullptr, nullptr, (uint8_t*)&(USART6->DR)};

DMA_TypeDef* DMA_array[] = {nullptr, DMA1, DMA2};

DMA_Stream_TypeDef* DMA_Stream_array[][8] = { {nullptr, nullptr, nullptr, nullptr,
											   nullptr, nullptr, nullptr, nullptr},
											  {DMA1_Stream0, DMA1_Stream1, DMA1_Stream2, DMA1_Stream3,
											   DMA1_Stream4, DMA1_Stream5, DMA1_Stream6, DMA1_Stream7},
											  {DMA2_Stream0, DMA2_Stream1, DMA2_Stream2, DMA2_Stream3,
											   DMA2_Stream4, DMA2_Stream5, DMA2_Stream6, DMA2_Stream7} };

volatile uint32_t* DMA_IFCR_array[][8] = { {nullptr, nullptr, nullptr, nullptr,
											nullptr, nullptr, nullptr, nullptr},
											{&(DMA1->LIFCR), &(DMA1->LIFCR), &(DMA1->LIFCR), &(DMA1->LIFCR),
	 										 &(DMA1->HIFCR), &(DMA1->HIFCR), &(DMA1->HIFCR), &(DMA1->HIFCR)},
											{&(DMA2->LIFCR), &(DMA2->LIFCR), &(DMA2->LIFCR), &(DMA2->LIFCR),
	 										 &(DMA2->HIFCR), &(DMA2->HIFCR), &(DMA2->HIFCR), &(DMA2->HIFCR)} };

volatile uint32_t DMA_CTCIF_array[8] = {DMA_LIFCR_CTCIF0, DMA_LIFCR_CTCIF1, DMA_LIFCR_CTCIF2, DMA_LIFCR_CTCIF3,
										DMA_HIFCR_CTCIF4, DMA_HIFCR_CTCIF5, DMA_HIFCR_CTCIF6, DMA_HIFCR_CTCIF7};

volatile uint32_t DMA_CHTIF_array[8] = {DMA_LIFCR_CHTIF0, DMA_LIFCR_CHTIF1, DMA_LIFCR_CHTIF2, DMA_LIFCR_CHTIF3,
										DMA_HIFCR_CHTIF4, DMA_HIFCR_CHTIF5, DMA_HIFCR_CHTIF6, DMA_HIFCR_CHTIF7};

volatile uint32_t DMA_CTEIF_array[8] = {DMA_LIFCR_CTEIF0, DMA_LIFCR_CTEIF1, DMA_LIFCR_CTEIF2, DMA_LIFCR_CTEIF3,
										DMA_HIFCR_CTEIF4, DMA_HIFCR_CTEIF5, DMA_HIFCR_CTEIF6, DMA_HIFCR_CTEIF7};

volatile uint32_t DMA_CDMEIF_array[8] = {DMA_LIFCR_CDMEIF0, DMA_LIFCR_CDMEIF1, DMA_LIFCR_CDMEIF2, DMA_LIFCR_CDMEIF3,
										 DMA_HIFCR_CDMEIF4, DMA_HIFCR_CDMEIF5, DMA_HIFCR_CDMEIF6, DMA_HIFCR_CDMEIF7};

volatile uint32_t DMA_CFEIF_array[8] = {DMA_LIFCR_CFEIF0, DMA_LIFCR_CFEIF1, DMA_LIFCR_CFEIF2, DMA_LIFCR_CTCIF3,
										DMA_HIFCR_CFEIF4, DMA_HIFCR_CFEIF5, DMA_HIFCR_CFEIF6, DMA_HIFCR_CFEIF7};

void dma_config(uint8_t dma, uint8_t stream, uint8_t channel, dmaDirection_Enum direction,
				dmaPriority_Enum priority, bool isTransferCompleteInterrupt,
				uint32_t periphAddr, uint32_t memAddr, uint16_t numDataItemsPerTrans,
				dmaDataSize_Enum periphDataSize, dmaDataSize_Enum memDataSize,
				dmaFIFOLevel_Enum fifoThreshold, bool isFlowControlDMA,
				dmaBeatsPerBurst_Enum memBurstBeats, dmaBeatsPerBurst_Enum periphBurstBeats,
				bool isMemAddrIncr, bool isPeriphAddrIncr, bool isCircularMode) {
	/* Enable DMA clock */
	rcc_enable_clock_dma(dma);

	/* Disable stream */
	dma_disable(dma, stream);

	/* Data transfer direction */
	switch(direction) {
		case Peripheral2Memory: // 0b00
			DMA_Stream_array[dma][stream]->CR &= ~DMA_SxCR_DIR;
			break;
		case Memory2Peripheral:
			DMA_Stream_array[dma][stream]->CR &= ~DMA_SxCR_DIR;
			DMA_Stream_array[dma][stream]->CR |= (0b01UL << DMA_SxCR_DIR_Pos);
			break;
		case Memory2Memory:
			DMA_Stream_array[dma][stream]->CR &= ~DMA_SxCR_DIR;
			DMA_Stream_array[dma][stream]->CR |= (0b10UL << DMA_SxCR_DIR_Pos);
			break;
	}

	/* Addresses */
	DMA_Stream_array[dma][stream]->PAR = periphAddr; // Peripheral Address
	DMA_Stream_array[dma][stream]->M0AR = memAddr; // Memory Address

	/* Select channel */
	DMA_Stream_array[dma][stream]->CR &= ~DMA_SxCR_CHSEL;
	DMA_Stream_array[dma][stream]->CR |= (uint32_t)channel << DMA_SxCR_CHSEL_Pos;

	/* Configure priority */
	DMA_Stream_array[dma][stream]->CR &= ~DMA_SxCR_PL;
	DMA_Stream_array[dma][stream]->CR |= (uint32_t)priority << DMA_SxCR_PL_Pos;

	/* Data sizes and increment */
	// Memory data size (each item)
	DMA_Stream_array[dma][stream]->CR &= ~DMA_SxCR_MSIZE;
	DMA_Stream_array[dma][stream]->CR |= (uint32_t)memDataSize << DMA_SxCR_MSIZE_Pos;
	// Peripheral data size (each item)
	DMA_Stream_array[dma][stream]->CR &= ~DMA_SxCR_PSIZE;
	DMA_Stream_array[dma][stream]->CR |= (uint32_t)periphDataSize << DMA_SxCR_PSIZE_Pos;
	// Number of data items to transfer
	DMA_Stream_array[dma][stream]->NDTR &= ~DMA_SxNDT;
	DMA_Stream_array[dma][stream]->NDTR |= (uint32_t)numDataItemsPerTrans << DMA_SxNDT_Pos;
	// Increment memory pointer after each data transfer (Memory increment mode)
	if(isMemAddrIncr) {
		DMA_Stream_array[dma][stream]->CR |= DMA_SxCR_MINC;
	} else {
		DMA_Stream_array[dma][stream]->CR &= ~DMA_SxCR_MINC;
	}
	// Increment peripheral pointer after each data transfer (Peripheral increment mode)
	if(isPeriphAddrIncr) {
		DMA_Stream_array[dma][stream]->CR |= DMA_SxCR_PINC;
	} else {
		DMA_Stream_array[dma][stream]->CR &= ~DMA_SxCR_PINC;
	}
	// Peripheral increment offset size (0b0 is increment liked to PSIZE, see: RM0393r3 Table 31)
	DMA_Stream_array[dma][stream]->CR &= ~DMA_SxCR_PINCOS;

	/* FIFO config */
	if(fifoThreshold != DirectMode) {
		// Disable direct mode so FIFO is used (direct == no FIFO)
		DMA_Stream_array[dma][stream]->FCR &= ~DMA_SxFCR_DMDIS;
		DMA_Stream_array[dma][stream]->FCR |= (uint32_t)0x01 << DMA_SxFCR_DMDIS_Pos;
		// FIFO threshold selection
		DMA_Stream_array[dma][stream]->FCR &= ~DMA_SxFCR_FTH;
		DMA_Stream_array[dma][stream]->FCR |= (uint32_t)fifoThreshold << DMA_SxFCR_FTH_Pos;
		// Set number of bursts and beats -- Memory
		DMA_Stream_array[dma][stream]->CR &= ~DMA_SxCR_MBURST;
		DMA_Stream_array[dma][stream]->CR |= (uint32_t)memBurstBeats << DMA_SxCR_MBURST_Pos;
		// Set number of bursts and beats -- Peripheral
		DMA_Stream_array[dma][stream]->CR &= ~DMA_SxCR_PBURST;
		DMA_Stream_array[dma][stream]->CR |= (uint32_t)periphBurstBeats << DMA_SxCR_PBURST_Pos;
	} else { // direct mode
		// Enable direct mode so FIFO is not used (direct == no FIFO)
		DMA_Stream_array[dma][stream]->FCR &= ~DMA_SxFCR_DMDIS;
	}

	/* Configure peripheral flow controller (0b0 is DMA, 0b1 is peripheral) */
	if(isFlowControlDMA) {
		DMA_Stream_array[dma][stream]->CR &= ~DMA_SxCR_PFCTRL;
	} else {
		DMA_Stream_array[dma][stream]->CR &= ~DMA_SxCR_PFCTRL;
		DMA_Stream_array[dma][stream]->CR |= 1UL << DMA_SxCR_PFCTRL_Pos;
	}

	/* Configure circular mode */
	if(isCircularMode) {
		DMA_Stream_array[dma][stream]->CR &= ~DMA_SxCR_CIRC;
		DMA_Stream_array[dma][stream]->CR |= (uint32_t)1 << DMA_SxCR_CIRC_Pos;
	} else {
		DMA_Stream_array[dma][stream]->CR &= ~DMA_SxCR_CIRC;
	}

	/* Configure interrupt enables */
	if(isTransferCompleteInterrupt) {
		// Turn on transfer complete interrupt enable
		DMA_Stream_array[dma][stream]->CR |= DMA_SxCR_TCIE;
		// NVIC Setup
		nvic_enable_dma(dma, stream);
	} else {
		// Turn off transfer complete interrupt enable
		DMA_Stream_array[dma][stream]->CR &= ~DMA_SxCR_TCIE;
	}
}

void dma_disable(uint8_t dma, uint8_t stream) {
	/* Disable stream */
	DMA_Stream_array[dma][stream]->CR &= ~DMA_SxCR_EN; // Disable
	while(DMA_Stream_array[dma][stream]->CR & DMA_SxCR_EN); // Wait until stream is disabled

}

void dma_enable(uint8_t dma, uint8_t stream, uint16_t numDataItems) {
	/* Clear interrupt flags in IFSR using IFCR (clear register) */
	*DMA_IFCR_array[dma][stream] |= DMA_CTCIF_array[stream];
	*DMA_IFCR_array[dma][stream] |= DMA_CHTIF_array[stream];
	*DMA_IFCR_array[dma][stream] |= DMA_CTEIF_array[stream];
	*DMA_IFCR_array[dma][stream] |= DMA_CDMEIF_array[stream];
	*DMA_IFCR_array[dma][stream] |= DMA_CFEIF_array[stream];

	/* Set number of data items to transfer */
	DMA_Stream_array[dma][stream]->NDTR &= ~DMA_SxNDT;
	DMA_Stream_array[dma][stream]->NDTR |= (uint32_t)numDataItems << DMA_SxNDT_Pos;

	/* Enable stream */
	DMA_Stream_array[dma][stream]->CR |= DMA_SxCR_EN;
}

/* ------------------- Stream handlers ---------------------- */

void dma1_stream0_handler(void) {
	/* Call associated L4 handler */
	interrupt_handler_dma(1U, 0U);

	/* Clear all Stream 0 interrupt flags */
	DMA1->LIFCR |= DMA_LIFCR_CTCIF0;
	DMA1->LIFCR |= DMA_LIFCR_CHTIF0;
	DMA1->LIFCR |= DMA_LIFCR_CTEIF0;
	DMA1->LIFCR |= DMA_LIFCR_CDMEIF0;
	DMA1->LIFCR |= DMA_LIFCR_CFEIF0;
}

void dma1_stream1_handler(void) {
	/* Call associated L4 handler */
	interrupt_handler_dma(1U, 1U);

	/* Clear all Stream 1 interrupt flags */
	DMA1->LIFCR |= DMA_LIFCR_CTCIF1;
	DMA1->LIFCR |= DMA_LIFCR_CHTIF1;
	DMA1->LIFCR |= DMA_LIFCR_CTEIF1;
	DMA1->LIFCR |= DMA_LIFCR_CDMEIF1;
	DMA1->LIFCR |= DMA_LIFCR_CFEIF1;
}

void dma1_stream2_handler(void) {
	/* Call associated L4 handler */
	interrupt_handler_dma(1U, 2U);

	/* Clear all Stream 2 interrupt flags */
	DMA1->LIFCR |= DMA_LIFCR_CTCIF2;
	DMA1->LIFCR |= DMA_LIFCR_CHTIF2;
	DMA1->LIFCR |= DMA_LIFCR_CTEIF2;
	DMA1->LIFCR |= DMA_LIFCR_CDMEIF2;
	DMA1->LIFCR |= DMA_LIFCR_CFEIF2;
}

void dma1_stream3_handler(void) {
	/* Call associated L4 handler */
	interrupt_handler_dma(1U, 3U);

	/* Clear all Stream 3 interrupt flags */
	DMA1->LIFCR |= DMA_LIFCR_CTCIF3;
	DMA1->LIFCR |= DMA_LIFCR_CHTIF3;
	DMA1->LIFCR |= DMA_LIFCR_CTEIF3;
	DMA1->LIFCR |= DMA_LIFCR_CDMEIF3;
	DMA1->LIFCR |= DMA_LIFCR_CFEIF3;
}

void dma1_stream4_handler(void) {
	/* Call associated L4 handler */
	interrupt_handler_dma(1U, 4U);

	/* Clear all Stream 4 interrupt flags */
	DMA1->HIFCR |= DMA_HIFCR_CTCIF4;
	DMA1->HIFCR |= DMA_HIFCR_CHTIF4;
	DMA1->HIFCR |= DMA_HIFCR_CTEIF4;
	DMA1->HIFCR |= DMA_HIFCR_CDMEIF4;
	DMA1->HIFCR |= DMA_HIFCR_CFEIF4;
}

void dma1_stream5_handler(void) {
	/* Call associated L4 handler */
	interrupt_handler_dma(1U, 5U);

	/* Clear all Stream 5 interrupt flags */
	DMA1->HIFCR |= DMA_HIFCR_CTCIF5;
	DMA1->HIFCR |= DMA_HIFCR_CHTIF5;
	DMA1->HIFCR |= DMA_HIFCR_CTEIF5;
	DMA1->HIFCR |= DMA_HIFCR_CDMEIF5;
	DMA1->HIFCR |= DMA_HIFCR_CFEIF5;
}

void dma1_stream6_handler(void) {
	/* Call associated L4 handler */
	interrupt_handler_dma(1U, 6U);

	/* Clear all Stream 6 interrupt flags */
	DMA1->HIFCR |= DMA_HIFCR_CTCIF6;
	DMA1->HIFCR |= DMA_HIFCR_CHTIF6;
	DMA1->HIFCR |= DMA_HIFCR_CTEIF6;
	DMA1->HIFCR |= DMA_HIFCR_CDMEIF6;
	DMA1->HIFCR |= DMA_HIFCR_CFEIF6;
}

void dma1_stream7_handler(void) {
	/* Call associated L4 handler */
	interrupt_handler_dma(1U, 7U);

	/* Clear all Stream 7 interrupt flags */
	DMA1->HIFCR |= DMA_HIFCR_CTCIF7;
	DMA1->HIFCR |= DMA_HIFCR_CHTIF7;
	DMA1->HIFCR |= DMA_HIFCR_CTEIF7;
	DMA1->HIFCR |= DMA_HIFCR_CDMEIF7;
	DMA1->HIFCR |= DMA_HIFCR_CFEIF7;
}

void dma2_stream0_handler(void) {
	/* Call associated L4 handler */
	interrupt_handler_dma(2U, 0U);

	/* Clear all Stream 0 interrupt flags */
	DMA2->LIFCR |= DMA_LIFCR_CTCIF0;
	DMA2->LIFCR |= DMA_LIFCR_CHTIF0;
	DMA2->LIFCR |= DMA_LIFCR_CTEIF0;
	DMA2->LIFCR |= DMA_LIFCR_CDMEIF0;
	DMA2->LIFCR |= DMA_LIFCR_CFEIF0;
}

void dma2_stream1_handler(void) {
	/* Call associated L4 handler */
	interrupt_handler_dma(2U, 1U);

	/* Clear all Stream 1 interrupt flags */
	DMA2->LIFCR |= DMA_LIFCR_CTCIF1;
	DMA2->LIFCR |= DMA_LIFCR_CHTIF1;
	DMA2->LIFCR |= DMA_LIFCR_CTEIF1;
	DMA2->LIFCR |= DMA_LIFCR_CDMEIF1;
	DMA2->LIFCR |= DMA_LIFCR_CFEIF1;
}

void dma2_stream2_handler(void) {
	/* Call associated L4 handler */
	interrupt_handler_dma(2U, 2U);

	/* Clear all Stream 2 interrupt flags */
	DMA2->LIFCR |= DMA_LIFCR_CTCIF2;
	DMA2->LIFCR |= DMA_LIFCR_CHTIF2;
	DMA2->LIFCR |= DMA_LIFCR_CTEIF2;
	DMA2->LIFCR |= DMA_LIFCR_CDMEIF2;
	DMA2->LIFCR |= DMA_LIFCR_CFEIF2;
}

void dma2_stream3_handler(void) {
	/* Call associated L4 handler */
	interrupt_handler_dma(2U, 3U);

	/* Clear all Stream 3 interrupt flags */
	DMA2->LIFCR |= DMA_LIFCR_CTCIF3;
	DMA2->LIFCR |= DMA_LIFCR_CHTIF3;
	DMA2->LIFCR |= DMA_LIFCR_CTEIF3;
	DMA2->LIFCR |= DMA_LIFCR_CDMEIF3;
	DMA2->LIFCR |= DMA_LIFCR_CFEIF3;
}

void dma2_stream4_handler(void) {
	/* Call associated L4 handler */
	interrupt_handler_dma(2U, 4U);

	/* Clear all Stream 4 interrupt flags */
	DMA2->HIFCR |= DMA_HIFCR_CTCIF4;
	DMA2->HIFCR |= DMA_HIFCR_CHTIF4;
	DMA2->HIFCR |= DMA_HIFCR_CTEIF4;
	DMA2->HIFCR |= DMA_HIFCR_CDMEIF4;
	DMA2->HIFCR |= DMA_HIFCR_CFEIF4;
}

void dma2_stream5_handler(void) {
	/* Call associated L4 handler */
	interrupt_handler_dma(2U, 5U);

	/* Clear all Stream 5 interrupt flags */
	DMA2->HIFCR |= DMA_HIFCR_CTCIF5;
	DMA2->HIFCR |= DMA_HIFCR_CHTIF5;
	DMA2->HIFCR |= DMA_HIFCR_CTEIF5;
	DMA2->HIFCR |= DMA_HIFCR_CDMEIF5;
	DMA2->HIFCR |= DMA_HIFCR_CFEIF5;
}

void dma2_stream6_handler(void) {
	/* Call associated L4 handler */
	interrupt_handler_dma(2U, 6U);

	/* Clear all Stream 6 interrupt flags */
	DMA2->HIFCR |= DMA_HIFCR_CTCIF6;
	DMA2->HIFCR |= DMA_HIFCR_CHTIF6;
	DMA2->HIFCR |= DMA_HIFCR_CTEIF6;
	DMA2->HIFCR |= DMA_HIFCR_CDMEIF6;
	DMA2->HIFCR |= DMA_HIFCR_CFEIF6;
}

void dma2_stream7_handler(void) {
	/* Call associated L4 handler */
	interrupt_handler_dma(2U, 7U);

	/* Clear all Stream 7 interrupt flags */
	DMA2->HIFCR |= DMA_HIFCR_CTCIF7;
	DMA2->HIFCR |= DMA_HIFCR_CHTIF7;
	DMA2->HIFCR |= DMA_HIFCR_CTEIF7;
	DMA2->HIFCR |= DMA_HIFCR_CDMEIF7;
	DMA2->HIFCR |= DMA_HIFCR_CFEIF7;
}

#ifdef __cplusplus
}
#endif /* __cplusplus */