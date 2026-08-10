/*
 * dma.h
 *
 *  Created on: Aug 5, 2026
 *      Author: busilam
 */

#ifndef DMA_H_
#define DMA_H_

void dma4_init(void);
void dma2_init(void);
void dma2_write(uint32_t msg, uint32_t len);
void dma4_write(uint32_t msg, uint32_t len);

#define DMAEN	(1U << 0) /* Enable DMA on RCC AHB Bus */
#define DIR		(1U << 4) /* Set DIR: read from memory */
#define CIRC	(1U << 5) /* Set CIRC: circular mode */
#define MIRC	(1U << 7) /* Memory increment mode */
#define TCIF4	(1U << 13) /* Interrupt on transfer complete channel 4 */
#define EN		(1U << 0) /* Enable DMA Channel 4 */
#define CGIF4	(1U << 12) /* Clear global flags channel 4*/
#define CGIF2	(1U << 4) /* Clear global flags channel 2*/
#define CTCIF4	(1U << 13) /* Clear transmission complete interrput flag for channel 4 */
#define TCIE	(1U << 1) /* Interrupt enable on tramission complete */
#define DMAT	(1U << 7) /* Bit to enable DMA transmitter */
#define TCIF2	(1U << 5) /* Interrupt on transfer complete channel 2 */
#define CTCIF2	(1U << 5) /* Clear transmission complete interrput flag for channel 2 */
#endif /* DMA_H_ */
