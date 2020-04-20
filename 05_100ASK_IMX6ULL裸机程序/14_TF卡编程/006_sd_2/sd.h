
#ifndef __SD_H__
#define __SD_H__

#include "common.h"
#include "fsl_specification.h"

/*===========================================================================*/
/*
 * cmd_x:  normal command,
 * acmd_x: app command,
 * scmd_x: sd command,
 * mcmd_x: mmc command
 */
#define CMD0	0x0000	/* GO_IDLE_STATE	no response */
#define CMD1	0x0102	/* SEND_OP_COND		expect 48 bit response (R3) */
#define CMD2	0x0209	/* ALL_SEND_CID		expect 136 bit response (R2) */
#define CMD3	0x031a	/* SET_RELATIVE_ADDR	expect 48 bit response (R6/R1) */
#define ACMD6	0x061a	/* SET_BUS_WIDTH	expect 48 bit response (R1) */
#define ACMD23	0x171a	/* SET_WR_BLK_ERASE_CNT	expect 48 bit response (R1) */
#define SCMD6	0x063a	/* SWITCH_BUSWIDTH	expect 48 bit response (R1) */
#define MCMD6	0x061b	/* SET_EXT_CSD		expect 48 bit response (R1B) */
#define CMD7	0x071b	/* SELECT_CARD		expect 48 bit response (R1B) */
#define CMD8	0x081a	/* SEND_IF_COND		expect 48 bit response (R7) */
#define MCMD8	0x083a	/* GET_EXT_CSD		expect 48 bit response (R1) */
#define CMD9	0x0909	/* GET_THE_CSD		expect 136 bit response (R2) */
#define CMD11	0x0b1a	/* SWITCH VOLTAGE	expect 48 bit response (R1) */
#define CMD13	0x0d1a	/* SEND_STATUS		expect 48 bit response (R1) */
#define ACMD13	0x0d3a	/* SEND_STATUS		expect 48 bit response (R1) */
#define CMD16	0x101a	/* SET_BLOCKLEN		expect 48 bit response (R1) */
#define CMD17	0x113a	/* READ_SINGLE_BLOCK	expect 48 bit response (R1) */
#define CMD18	0x123a	/* READ_MULTIPLE_BLOCK	expect 48 bit response (R1) */
#define SCMD19 0x133a  /* SEND_TUNING		expect 48 bit response (R1) */
#define MCMD21 0x153a  /* SEND_TUNING		expect 48 bit response (R1) */
#define CMD24	0x183a	/* WRITE_BLOCK		expect 48 bit response (R1) */
#define CMD25	0x193a	/* WRITE_MULTIPLE_BLOCK	expect 48 bit response (R1) */
#define CMD32	0x201a	/* ERASE_WR_BLK_START	expect 48 bit response (R1) */
#define CMD33	0x211a	/* ERASE_WR_BLK_END	expect 48 bit response (R1) */
#define CMD35	0x231a	/* ERASE_GROUP_START	expect 48 bit response (R1) */
#define CMD36	0x241a	/* ERASE_GROUP_END	expect 48 bit response (R1) */
#define CMD38	0x261b	/* ERASE		expect 48 bit response (R1B) */
#define ACMD41	0x2902	/* SD_SEND_OP_COND	expect 48 bit response (R3) */
#define ACMD42	0x2a1b	/* LOCK_UNLOCK		expect 48 bit response (R1B) */
#define ACMD51	0x333a	/* SEND_SCR		expect 48 bit response (R1) */
#define CMD55	0x371a	/* APP_CMD		expect 48 bit response (R1) */
		  			 		  						  					  				 	   		  	  	 	  
/*===========================================================================*/
/*! @brief Default block size */
#define FSL_SDMMC_DEFAULT_BLOCK_SIZE (512U)

/*!
 * @brief SD card state
 *
 * Define the card structure including the necessary fields to identify and describe the card.
 */
typedef struct _sd_card
{
    bool isHostReady;                          /*!< use this flag to indicate if need host re-init or not*/
    uint32_t busClock_Hz;                      /*!< SD bus clock frequency united in Hz */
    uint32_t relativeAddress;                  /*!< Relative address of the card */
    uint32_t version;                          /*!< Card version */
    uint32_t flags;                            /*!< Flags in _sd_card_flag */
    uint32_t rawCid[4U];                       /*!< Raw CID content */
    uint32_t rawCsd[4U];                       /*!< Raw CSD content */
    uint32_t rawScr[2U];                       /*!< Raw CSD content */
    uint32_t ocr;                              /*!< Raw OCR content */
    sd_cid_t cid;                              /*!< CID */
    sd_csd_t csd;                              /*!< CSD */
    sd_scr_t scr;                              /*!< SCR */
    uint32_t blockCount;                       /*!< Card total block number */
    uint32_t blockSize;                        /*!< Card block size */
	long long capacity;
    sd_timing_mode_t currentTiming;            /*!< current timing mode */
    sd_driver_strength_t driverStrength;       /*!< driver strength */
    sd_max_current_t maxCurrent;               /*!< card current limit */
} sd_card_t;
		  			 		  						  					  				 	   		  	  	 	  

typedef enum _usdhc_data_bus_width
{
    kUSDHC_DataBusWidth1Bit = 0U, /*!< 1-bit mode */
    kUSDHC_DataBusWidth4Bit = 1U, /*!< 4-bit mode */
    kUSDHC_DataBusWidth8Bit = 2U, /*!< 8-bit mode */
} usdhc_data_bus_width_t;

/*! @brief Interrupt status flag mask */
enum _usdhc_interrupt_status_flag
{
    kUSDHC_CommandCompleteFlag = USDHC_INT_STATUS_CC_MASK,   /*!< Command complete */
    kUSDHC_DataCompleteFlag = USDHC_INT_STATUS_TC_MASK,      /*!< Data complete */
    kUSDHC_BlockGapEventFlag = USDHC_INT_STATUS_BGE_MASK,    /*!< Block gap event */
    kUSDHC_DmaCompleteFlag = USDHC_INT_STATUS_DINT_MASK,     /*!< DMA interrupt */
    kUSDHC_BufferWriteReadyFlag = USDHC_INT_STATUS_BWR_MASK, /*!< Buffer write ready */
    kUSDHC_BufferReadReadyFlag = USDHC_INT_STATUS_BRR_MASK,  /*!< Buffer read ready */
    kUSDHC_CardInsertionFlag = USDHC_INT_STATUS_CINS_MASK,   /*!< Card inserted */
    kUSDHC_CardRemovalFlag = USDHC_INT_STATUS_CRM_MASK,      /*!< Card removed */
    kUSDHC_CardInterruptFlag = USDHC_INT_STATUS_CINT_MASK,   /*!< Card interrupt */

#if defined(FSL_FEATURE_USDHC_HAS_SDR50_MODE) && (!FSL_FEATURE_USDHC_HAS_SDR50_MODE)
    kUSDHC_ReTuningEventFlag = 0U, /*!< Re-Tuning event,only for SD3.0 SDR104 mode */
    kUSDHC_TuningPassFlag = 0U,    /*!< SDR104 mode tuning pass flag */
    kUSDHC_TuningErrorFlag = 0U,   /*!< SDR104 tuning error flag */
#else
    kUSDHC_ReTuningEventFlag = USDHC_INT_STATUS_RTE_MASK,         /*!< Re-Tuning event,only for SD3.0 SDR104 mode */
    kUSDHC_TuningPassFlag = USDHC_INT_STATUS_TP_MASK,             /*!< SDR104 mode tuning pass flag */
    kUSDHC_TuningErrorFlag = USDHC_INT_STATUS_TNE_MASK,           /*!< SDR104 tuning error flag */
#endif

    kUSDHC_CommandTimeoutFlag = USDHC_INT_STATUS_CTOE_MASK,      /*!< Command timeout error */
    kUSDHC_CommandCrcErrorFlag = USDHC_INT_STATUS_CCE_MASK,      /*!< Command CRC error */
    kUSDHC_CommandEndBitErrorFlag = USDHC_INT_STATUS_CEBE_MASK,  /*!< Command end bit error */
    kUSDHC_CommandIndexErrorFlag = USDHC_INT_STATUS_CIE_MASK,    /*!< Command index error */
    kUSDHC_DataTimeoutFlag = USDHC_INT_STATUS_DTOE_MASK,         /*!< Data timeout error */
    kUSDHC_DataCrcErrorFlag = USDHC_INT_STATUS_DCE_MASK,         /*!< Data CRC error */
    kUSDHC_DataEndBitErrorFlag = USDHC_INT_STATUS_DEBE_MASK,     /*!< Data end bit error */
    kUSDHC_AutoCommand12ErrorFlag = USDHC_INT_STATUS_AC12E_MASK, /*!< Auto CMD12 error */
    kUSDHC_DmaErrorFlag = USDHC_INT_STATUS_DMAE_MASK,            /*!< DMA error */

    kUSDHC_CommandErrorFlag = (kUSDHC_CommandTimeoutFlag | kUSDHC_CommandCrcErrorFlag | kUSDHC_CommandEndBitErrorFlag |
                               kUSDHC_CommandIndexErrorFlag), /*!< Command error */
    kUSDHC_DataErrorFlag = (kUSDHC_DataTimeoutFlag | kUSDHC_DataCrcErrorFlag | kUSDHC_DataEndBitErrorFlag |
                            kUSDHC_AutoCommand12ErrorFlag),                                    /*!< Data error */
    kUSDHC_ErrorFlag = (kUSDHC_CommandErrorFlag | kUSDHC_DataErrorFlag | kUSDHC_DmaErrorFlag), /*!< All error */
    kUSDHC_DataFlag = (kUSDHC_DataCompleteFlag | kUSDHC_DmaCompleteFlag | kUSDHC_BufferWriteReadyFlag |
                       kUSDHC_BufferReadReadyFlag | kUSDHC_DataErrorFlag | kUSDHC_DmaErrorFlag), /*!< Data interrupts */
    kUSDHC_CommandFlag = (kUSDHC_CommandErrorFlag | kUSDHC_CommandCompleteFlag), /*!< Command interrupts */
    kUSDHC_CardDeteactFlag = (kUSDHC_CardInsertionFlag | kUSDHC_CardRemovalFlag), /*!< Card detection interrupts */
    kUSDHC_SDR104TuningFlag = (kUSDHC_TuningErrorFlag | kUSDHC_TuningPassFlag | kUSDHC_ReTuningEventFlag),

    kUSDHC_AllInterruptFlags = (kUSDHC_BlockGapEventFlag | kUSDHC_CardInterruptFlag | kUSDHC_CommandFlag |
                                kUSDHC_DataFlag | kUSDHC_ErrorFlag | kUSDHC_SDR104TuningFlag), /*!< All flags mask */
};

/* ADMA2 descriptor table
 * |----------------|---------------|-------------|--------------------------|
 * | Address field  |     Length    | Reserved    |         Attribute        |
 * |----------------|---------------|-------------|--------------------------|
 * |63            32|31           16|15         06|05  |04  |03|02 |01 |00   |
 * |----------------|---------------|-------------|----|----|--|---|---|-----|
 * | 32-bit address | 16-bit length | 0000000000  |Act2|Act1| 0|Int|End|Valid|
 * |----------------|---------------|-------------|----|----|--|---|---|-----|
 *
 *
 * | Act2 | Act1 |     Comment     | Operation                                                         |
 * |------|------|-----------------|-------------------------------------------------------------------|
 * |   0  |   0  | No op           | Don't care                                                        |
 * |------|------|-----------------|-------------------------------------------------------------------|
 * |   0  |   1  | Reserved        | Read this line and go to next one                                 |
 * |------|------|-----------------|-------------------------------------------------------------------|
 * |   1  |   0  | Transfer data   | Transfer data with address and length set in this descriptor line |
 * |------|------|-----------------|-------------------------------------------------------------------|
 * |   1  |   1  | Link descriptor | Link to another descriptor                                        |
 * |------|------|-----------------|-------------------------------------------------------------------|
 */
/*! @brief The bit shift for LENGTH field in ADMA2's descriptor */
#define USDHC_ADMA2_DESCRIPTOR_LENGTH_SHIFT (16U)
/*! @brief The bit mask for LENGTH field in ADMA2's descriptor */
#define USDHC_ADMA2_DESCRIPTOR_LENGTH_MASK (0xFFFFU)
/*! @brief The maximum value of LENGTH field in ADMA2's descriptor */
#define USDHC_ADMA2_DESCRIPTOR_MAX_LENGTH_PER_ENTRY (USDHC_ADMA2_DESCRIPTOR_LENGTH_MASK - 3U)


/*! @brief Defines the ADMA2 descriptor structure. */
typedef struct _usdhc_adma2_descriptor
{
    uint32_t attribute;      /*!< The control and status field */
    const uint32_t *address; /*!< The address field */
} usdhc_adma2_descriptor_t;

/*! @brief ADMA1 descriptor control and status mask */
enum _usdhc_adma2_descriptor_flag
{
    kUSDHC_Adma2DescriptorValidFlag = (1U << 0U),     /*!< Valid flag */
    kUSDHC_Adma2DescriptorEndFlag = (1U << 1U),       /*!< End flag */
    kUSDHC_Adma2DescriptorInterruptFlag = (1U << 2U), /*!< Interrupt flag */
    kUSDHC_Adma2DescriptorActivity1Flag = (1U << 4U), /*!< Activity 1 mask */
    kUSDHC_Adma2DescriptorActivity2Flag = (1U << 5U), /*!< Activity 2 mask */

    kUSDHC_Adma2DescriptorTypeNop = (kUSDHC_Adma2DescriptorValidFlag), /*!< No operation */
    kUSDHC_Adma2DescriptorTypeReserved =
        (kUSDHC_Adma2DescriptorActivity1Flag | kUSDHC_Adma2DescriptorValidFlag), /*!< Reserved */
    kUSDHC_Adma2DescriptorTypeTransfer =
        (kUSDHC_Adma2DescriptorActivity2Flag | kUSDHC_Adma2DescriptorValidFlag), /*!< Transfer type */
    kUSDHC_Adma2DescriptorTypeLink = (kUSDHC_Adma2DescriptorActivity1Flag | kUSDHC_Adma2DescriptorActivity2Flag |
                                      kUSDHC_Adma2DescriptorValidFlag), /*!< Link type */
};

/*! @brief DMA mode */
typedef enum _usdhc_dma_mode
{
    kUSDHC_DmaModeSimple = 0U, /*!< external DMA */
    kUSDHC_DmaModeAdma1 = 1U,  /*!< ADMA1 is selected */
    kUSDHC_DmaModeAdma2 = 2U,  /*!< ADMA2 is selected */
    kUSDHC_ExternalDMA = 3U,   /*!< external dma mode select */
} usdhc_dma_mode_t;

/*! @brief dma transfer burst len config. */
typedef enum _usdhc_burst_len
{
    kUSDHC_EnBurstLenForINCR = 0x01U,         /*!< enable burst len for INCR */
    kUSDHC_EnBurstLenForINCR4816 = 0x02U,     /*!< enable burst len for INCR4/INCR8/INCR16 */
    kUSDHC_EnBurstLenForINCR4816WRAP = 0x04U, /*!< enable burst len for INCR4/8/16 WRAP */
} usdhc_burst_len_t;


/*******************************************************************************
 * Definitions
 ******************************************************************************/
/*! @brief Clock setting */
/* Max SD clock divisor from base clock */
#define USDHC_MAX_DVS ((USDHC_SYS_CTRL_DVS_MASK >> USDHC_SYS_CTRL_DVS_SHIFT) + 1U)
#define USDHC_PREV_DVS(x) ((x) -= 1U)
#define USDHC_PREV_CLKFS(x, y) ((x) >>= (y))




int sd_init(USDHC_Type *base);

#endif
