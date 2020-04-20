#include "common.h"
#include "sd.h"
#include "string.h"

sd_card_t g_sd_card;
mmc_card_t g_mmc_card;

//just use table size 8 for test
usdhc_adma2_descriptor_t g_adma2_tablbe[8];//should align 4
static u8 sd_align_buf[1024] __attribute__ ((aligned(1024)));

void USDHC_Dump_All(USDHC_Type *base)
{
	printf("DS_ADDR is 0x%x\r\n", base->DS_ADDR);
	printf("BLK_ATT is 0x%x\r\n", base->BLK_ATT);
	printf("CMD_ARG is 0x%x\r\n", base->CMD_ARG);
	printf("CMD_XFR_TYP is 0x%x\r\n", base->CMD_XFR_TYP);
	printf("CMD_RSP0 is 0x%x\r\n", base->CMD_RSP0);
	printf("CMD_RSP1 is 0x%x\r\n", base->CMD_RSP1);
	printf("CMD_RSP2 is 0x%x\r\n", base->CMD_RSP2);
	printf("CMD_RSP3 is 0x%x\r\n", base->CMD_RSP3);
	printf("DATA_BUFF_ACC_PORT is 0x%x\r\n", base->DATA_BUFF_ACC_PORT);
	printf("PRES_STATE is 0x%x\r\n", base->PRES_STATE);
	printf("PROT_CTRL is 0x%x\r\n", base->PROT_CTRL);
	printf("SYS_CTRL is 0x%x\r\n", base->SYS_CTRL);
	printf("INT_STATUS is 0x%x\r\n", base->INT_STATUS);
	printf("INT_STATUS_EN is 0x%x\r\n", base->INT_STATUS_EN);
	printf("INT_SIGNAL_EN is 0x%x\r\n", base->INT_SIGNAL_EN);
	printf("AUTOCMD12_ERR_STATUS is 0x%x\r\n", base->AUTOCMD12_ERR_STATUS);
	printf("HOST_CTRL_CAP is 0x%x\r\n", base->HOST_CTRL_CAP);
	printf("WTMK_LVL is 0x%x\r\n", base->WTMK_LVL);
	printf("MIX_CTRL is 0x%x\r\n", base->MIX_CTRL);	
	printf("FORCE_EVENT is 0x%x\r\n", base->FORCE_EVENT);
	printf("ADMA_ERR_STATUS is 0x%x\r\n", base->ADMA_ERR_STATUS);
	printf("ADMA_SYS_ADDR is 0x%x\r\n", base->ADMA_SYS_ADDR);
	printf("DLL_CTRL is 0x%x\r\n", base->DLL_CTRL);
	printf("DLL_STATUS is 0x%x\r\n", base->DLL_STATUS);
	printf("CLK_TUNE_CTRL_STATUS is 0x%x\r\n", base->CLK_TUNE_CTRL_STATUS);
	printf("VEND_SPEC is 0x%x\r\n", base->VEND_SPEC);
	printf("MMC_BOOT is 0x%x\r\n", base->MMC_BOOT);
	printf("VEND_SPEC2 is 0x%x\r\n", base->VEND_SPEC2);
	printf("TUNING_CTRL is 0x%x\r\n", base->TUNING_CTRL);

}		  			 		  						  					  				 	   		  	  	 	  
static bool USDHC_Reset(USDHC_Type *base, uint32_t mask, uint32_t timeout)
{
	base->SYS_CTRL |= (mask & (USDHC_SYS_CTRL_RSTA_MASK | USDHC_SYS_CTRL_RSTC_MASK | USDHC_SYS_CTRL_RSTD_MASK));
	/* Delay some time to wait reset success. */
	while ((base->SYS_CTRL & mask) != 0U)
	{
		if (timeout == 0U)
		{
			break;
		}
		timeout--;
	}

	return ((!timeout) ? false : true);
}

static bool USDHC_Reset_All(USDHC_Type *base, uint32_t timeout)
{
	return USDHC_Reset(base, USDHC_SYS_CTRL_RSTA_MASK, timeout);
}

static bool USDHC_Reset_Cmd_Line(USDHC_Type *base, uint32_t timeout)
{
	return USDHC_Reset(base, USDHC_SYS_CTRL_RSTC_MASK, timeout);
}
		  			 		  						  					  				 	   		  	  	 	  
static bool USDHC_Reset_Data_Line(USDHC_Type *base, uint32_t timeout)
{
	return USDHC_Reset(base, USDHC_SYS_CTRL_RSTD_MASK, timeout);
}

static int USDHC_CreateDescTable(u8 *data, u32 len)
{
	int i;
	u32 dma_len, entries;

	entries = len / USDHC_ADMA2_DESCRIPTOR_MAX_LENGTH_PER_ENTRY;
	if ((len % USDHC_ADMA2_DESCRIPTOR_MAX_LENGTH_PER_ENTRY) != 0U)
		entries++;

	for (i = 0; i < entries; i++)
	{
		if (len > USDHC_ADMA2_DESCRIPTOR_MAX_LENGTH_PER_ENTRY)
		{
			dma_len = USDHC_ADMA2_DESCRIPTOR_MAX_LENGTH_PER_ENTRY;
			len -= USDHC_ADMA2_DESCRIPTOR_MAX_LENGTH_PER_ENTRY;
		}
		else
		{
			dma_len = len;
		}

		/* Each descriptor for ADMA2 is 64-bit in length */
		g_adma2_tablbe[i].address = (uint32_t *)data;
		g_adma2_tablbe[i].attribute = (dma_len << USDHC_ADMA2_DESCRIPTOR_LENGTH_SHIFT);
		g_adma2_tablbe[i].attribute |= kUSDHC_Adma2DescriptorTypeTransfer;
		data += dma_len;
	}
	g_adma2_tablbe[entries - 1].attribute |= kUSDHC_Adma2DescriptorEndFlag;
	/*for (i = 0; i < entries; i++) {
	  printf("g_adma2_tablbe[i] address is 0x%x, attribute is 0x%x\r\n", 
	  g_adma2_tablbe[i].address, g_adma2_tablbe[i].attribute);
	  }
	  */

	return 0;
}
		  			 		  						  					  				 	   		  	  	 	  

static inline void USDHC_ClearInterruptStatusFlags(USDHC_Type *base, uint32_t mask)
{
	base->INT_STATUS = mask;
}

static int USDHC_WaitCommandDone(USDHC_Type *base)
{
	int error = 0;
	uint32_t interruptStatus = 0U;

	/* Wait command complete or USDHC encounters error. */
	while (!(base->INT_STATUS & (USDHC_INT_STATUS_CC_MASK | kUSDHC_CommandErrorFlag)))
	{
	}

	interruptStatus = base->INT_STATUS;


	if ((interruptStatus & kUSDHC_CommandErrorFlag) != 0U)
	{
		printf("cmd errror, CMD is 0x%x, INT_STATUS is 0x%x\r\n", base->CMD_XFR_TYP, interruptStatus);
		USDHC_Dump_All(base);
		error = -1;
	}
	else
	{
	}

	USDHC_ClearInterruptStatusFlags(
			base, (kUSDHC_CommandCompleteFlag | kUSDHC_CommandErrorFlag | kUSDHC_TuningErrorFlag));

	return error;
}

static int USDHC_WaitDataDone(USDHC_Type *base)
{
	int error = 0;
	uint32_t interruptStatus = 0U;

	/* Wait command complete or USDHC encounters error. */
	while (!(base->INT_STATUS & (kUSDHC_DataCompleteFlag | kUSDHC_DataErrorFlag)))
	{
	}

	interruptStatus = base->INT_STATUS;


	if ((interruptStatus & kUSDHC_DataErrorFlag) != 0U)
	{
		printf("data errror, CMD is 0x%x, INT_STATUS is 0x%x\r\n", base->CMD_XFR_TYP, interruptStatus);
		USDHC_Dump_All(base);
		error = -1;
	}
	else
	{
	}

	USDHC_ClearInterruptStatusFlags(
			base, (kUSDHC_DataCompleteFlag | kUSDHC_DataErrorFlag));

	return error;
}


static int USDHC_SendCommand(USDHC_Type *base, u32 command, u32 argument)
{
	/* Wait until command/data bus out of busy status. */
	while (base->PRES_STATE & USDHC_PRES_STATE_CIHB_MASK)
	{
	}
	/*while (data && (base->PRES_STATE & USDHC_PRES_STATE_CDIHB_MASK))
	  {
	  }*/	
  
	/* config the command xfertype and argument */
	base->CMD_ARG = argument;
	base->CMD_XFR_TYP = command << 16;

	return USDHC_WaitCommandDone(base);

}

static int USDHC_SendCommand_with_data(USDHC_Type *base, u32 command, u32 argument, void *buf, u32 size)
{
	int err;

	/* Wait until command/data bus out of busy status. */
	while (base->PRES_STATE & USDHC_PRES_STATE_CIHB_MASK)
	{
	}
	while (base->PRES_STATE & USDHC_PRES_STATE_CDIHB_MASK)
	{
	}

	/* set ADMA descriptor */
	USDHC_CreateDescTable(sd_align_buf, size);

	/* When use ADMA, disable simple DMA */
	base->DS_ADDR = 0U;
	base->ADMA_SYS_ADDR = (u32) g_adma2_tablbe;

	/* config data block size/block count */
	base->BLK_ATT = (USDHC_BLK_ATT_BLKSIZE(size) | USDHC_BLK_ATT_BLKCNT(1));

	/* disable the external DMA if support */
	base->VEND_SPEC &= ~USDHC_VEND_SPEC_EXT_DMA_EN_MASK;
	/* select DMA mode and config the burst length */
	base->PROT_CTRL &= ~(USDHC_PROT_CTRL_DMASEL_MASK | USDHC_PROT_CTRL_BURST_LEN_EN_MASK);
	base->PROT_CTRL |=
		USDHC_PROT_CTRL_DMASEL(kUSDHC_DmaModeAdma2) | USDHC_PROT_CTRL_BURST_LEN_EN(kUSDHC_EnBurstLenForINCR);
	/* direction:read, enable DMA */
	base->MIX_CTRL |= USDHC_MIX_CTRL_DTDSEL_MASK | USDHC_MIX_CTRL_DMAEN_MASK;

	/* config the command xfertype and argument */
	base->CMD_ARG = argument;
	base->CMD_XFR_TYP = command << 16;

	err = USDHC_WaitCommandDone(base);
	if (err < 0)
		return err;

	err =  USDHC_WaitDataDone(base);
	if (err < 0)
		return err;

	if (buf != NULL)
		memcpy(buf, sd_align_buf, size);

	return 0;
}

void USDHC_Init(USDHC_Type *base)
{
	uint32_t proctl, sysctl, wml;

	/* Reset USDHC. */
	USDHC_Reset_All(base, 100U);

	proctl = base->PROT_CTRL;
	wml = base->WTMK_LVL;
	sysctl = base->SYS_CTRL;

	proctl &= ~(USDHC_PROT_CTRL_EMODE_MASK | USDHC_PROT_CTRL_DMASEL_MASK);
	/* Endian mode*/
	proctl |= USDHC_PROT_CTRL_EMODE(2);//little endian mode

	/* Watermark level */
	wml &= ~(USDHC_WTMK_LVL_RD_WML_MASK | USDHC_WTMK_LVL_WR_WML_MASK | USDHC_WTMK_LVL_RD_BRST_LEN_MASK |
			USDHC_WTMK_LVL_WR_BRST_LEN_MASK);
	wml |= (USDHC_WTMK_LVL_RD_WML(0x80) | USDHC_WTMK_LVL_WR_WML(0x80) |
			USDHC_WTMK_LVL_RD_BRST_LEN(8) | USDHC_WTMK_LVL_WR_BRST_LEN(8));

	/* config the data timeout value */
	sysctl &= ~USDHC_SYS_CTRL_DTOCV_MASK;
	sysctl |= USDHC_SYS_CTRL_DTOCV(0xF);

	base->SYS_CTRL = sysctl;
	base->WTMK_LVL = wml;
	base->PROT_CTRL = proctl;

	/* Disable all interrupts */
	base->INT_STATUS_EN = 0;
	base->INT_SIGNAL_EN = 0;

	base->INT_STATUS_EN = (kUSDHC_CommandFlag | kUSDHC_CardInsertionFlag | kUSDHC_DataFlag | kUSDHC_CardRemovalFlag |
			kUSDHC_SDR104TuningFlag);

	base->INT_SIGNAL_EN = (kUSDHC_CommandFlag | kUSDHC_CardInsertionFlag | kUSDHC_DataFlag | kUSDHC_CardRemovalFlag |
			kUSDHC_SDR104TuningFlag);
}

void CardInsertDetect(USDHC_Type *base)
{
	if ((base->PRES_STATE & USDHC_PRES_STATE_CDPL_MASK) == 0) {
		printf("please insert sd card\r\n");
	}

	while(!((base->PRES_STATE >> USDHC_PRES_STATE_CDPL_SHIFT) & 0x1));
	printf("sd card has been inserted\r\n");
}

void USDHC_SetDataBusWidth(USDHC_Type *base, usdhc_data_bus_width_t width)
{
	base->PROT_CTRL = ((base->PROT_CTRL & ~USDHC_PROT_CTRL_DTW_MASK) | USDHC_PROT_CTRL_DTW(width));
}

uint32_t USDHC_SetSdClock(USDHC_Type *base, uint32_t srcClock_Hz, uint32_t busClock_Hz)
{
	uint32_t totalDiv = 0U;
	uint32_t divisor = 0U;
	uint32_t prescaler = 0U;
	uint32_t sysctl = 0U;
	uint32_t nearestFrequency = 0U;
	uint32_t maxClKFS = ((USDHC_SYS_CTRL_SDCLKFS_MASK >> USDHC_SYS_CTRL_SDCLKFS_SHIFT) + 1U);
	bool enDDR = false;
	/* DDR mode max clkfs can reach 512 */
	if ((base->MIX_CTRL & USDHC_MIX_CTRL_DDR_EN_MASK) != 0U)
	{
		enDDR = true;
		maxClKFS *= 2U;
	}
	/* calucate total divisor first */
	totalDiv = srcClock_Hz / busClock_Hz;

	if (totalDiv != 0U)
	{
		/* calucate the divisor (srcClock_Hz / divisor) <= busClock_Hz */
		if ((srcClock_Hz / totalDiv) > busClock_Hz)
		{
			totalDiv++;
		}

		/* divide the total divisor to div and prescaler */
		if (totalDiv > USDHC_MAX_DVS)
		{
			prescaler = totalDiv / USDHC_MAX_DVS;
			/* prescaler must be a value which equal 2^n and smaller than SDHC_MAX_CLKFS */
			while (((maxClKFS % prescaler) != 0U) || (prescaler == 1U))
			{
				prescaler++;
			}
			/* calucate the divisor */
			divisor = totalDiv / prescaler;
			/* fine tuning the divisor until divisor * prescaler >= totalDiv */
			while ((divisor * prescaler) < totalDiv)
			{
				divisor++;
			}
			nearestFrequency = srcClock_Hz / divisor / prescaler;
		}
		else
		{
			/* in this situation , divsior and SDCLKFS can generate same clock
			   use SDCLKFS*/
			if ((USDHC_MAX_DVS % totalDiv) == 0U)
			{
				divisor = 0U;
				prescaler = totalDiv;
			}
			else
			{
				divisor = totalDiv;
				prescaler = 0U;
			}
			nearestFrequency = srcClock_Hz / totalDiv;
		}
	}
	/* in this condition , srcClock_Hz = busClock_Hz, */
	else
	{
		/* in DDR mode , set SDCLKFS to 0, divisor = 0, actually the
		   totoal divider = 2U */
		divisor = 0U;
		prescaler = 0U;
		nearestFrequency = srcClock_Hz;
	}

	/* calucate the value write to register */
	if (divisor != 0U)
	{
		USDHC_PREV_DVS(divisor);
	}
	/* calucate the value write to register */
	if (prescaler != 0U)
	{
		USDHC_PREV_CLKFS(prescaler, (enDDR ? 2U : 1U));
	}

	/* Set the SD clock frequency divisor, SD clock frequency select, data timeout counter value. */
	sysctl = base->SYS_CTRL;
	sysctl &= ~(USDHC_SYS_CTRL_DVS_MASK | USDHC_SYS_CTRL_SDCLKFS_MASK);
	sysctl |= (USDHC_SYS_CTRL_DVS(divisor) | USDHC_SYS_CTRL_SDCLKFS(prescaler));
	base->SYS_CTRL = sysctl;

	/* Wait until the SD clock is stable. */
	while (!(base->PRES_STATE & USDHC_PRES_STATE_SDSTB_MASK))
	{
	}

	return nearestFrequency;
}

bool USDHC_SetCardActive(USDHC_Type *base, uint32_t timeout)
{
	base->SYS_CTRL |= USDHC_SYS_CTRL_INITA_MASK;
	/* Delay some time to wait card become active state. */
	while ((base->SYS_CTRL & USDHC_SYS_CTRL_INITA_MASK) == USDHC_SYS_CTRL_INITA_MASK)
	{
		if (!timeout)
		{
			break;
		}
		timeout--;
	}

	return ((!timeout) ? false : true);
}

void USDHC_GetResponse(USDHC_Type *base, u32 *resp)
{
	resp[3] = (base->CMD_RSP3 << 8) | (base->CMD_RSP2 >> 24);
	resp[2] = (base->CMD_RSP2 << 8) | (base->CMD_RSP1 >> 24);
	resp[1] = (base->CMD_RSP1 << 8) | (base->CMD_RSP0 >> 24);
	resp[0] = (base->CMD_RSP0 << 8);
}

static void SD_DecodeCid(sd_card_t *card, uint32_t *rawCid)
{
	sd_cid_t *cid;

	cid = &(card->cid);
	cid->manufacturerID = (uint8_t)((rawCid[3U] & 0xFF000000U) >> 24U);
	cid->applicationID = (uint16_t)((rawCid[3U] & 0xFFFF00U) >> 8U);

	cid->productName[0U] = (uint8_t)((rawCid[3U] & 0xFFU));
	cid->productName[1U] = (uint8_t)((rawCid[2U] & 0xFF000000U) >> 24U);
	cid->productName[2U] = (uint8_t)((rawCid[2U] & 0xFF0000U) >> 16U);
	cid->productName[3U] = (uint8_t)((rawCid[2U] & 0xFF00U) >> 8U);
	cid->productName[4U] = (uint8_t)((rawCid[2U] & 0xFFU));

	cid->productVersion = (uint8_t)((rawCid[1U] & 0xFF000000U) >> 24U);

	cid->productSerialNumber = (uint32_t)((rawCid[1U] & 0xFFFFFFU) << 8U);
	cid->productSerialNumber |= (uint32_t)((rawCid[0U] & 0xFF000000U) >> 24U);

	cid->manufacturerData = (uint16_t)((rawCid[0U] & 0xFFF00U) >> 8U);
}

static void SD_DecodeCsd(sd_card_t *card, uint32_t *rawCsd)
{
	sd_csd_t *csd;

	csd = &(card->csd);
	csd->csdStructure = (uint8_t)((rawCsd[3U] & 0xC0000000U) >> 30U);
	csd->dataReadAccessTime1 = (uint8_t)((rawCsd[3U] & 0xFF0000U) >> 16U);
	csd->dataReadAccessTime2 = (uint8_t)((rawCsd[3U] & 0xFF00U) >> 8U);
	csd->transferSpeed = (uint8_t)(rawCsd[3U] & 0xFFU);
	csd->cardCommandClass = (uint16_t)((rawCsd[2U] & 0xFFF00000U) >> 20U);
	csd->readBlockLength = (uint8_t)((rawCsd[2U] & 0xF0000U) >> 16U);
	if (rawCsd[2U] & 0x8000U)
	{
		csd->flags |= kSD_CsdReadBlockPartialFlag;
	}
	if (rawCsd[2U] & 0x4000U)
	{
		csd->flags |= kSD_CsdReadBlockPartialFlag;
	}
	if (rawCsd[2U] & 0x2000U)
	{
		csd->flags |= kSD_CsdReadBlockMisalignFlag;
	}
	if (rawCsd[2U] & 0x1000U)
	{
		csd->flags |= kSD_CsdDsrImplementedFlag;
	}
	switch (csd->csdStructure)
	{
		case 0:
			csd->deviceSize = (uint32_t)((rawCsd[2U] & 0x3FFU) << 2U);
			csd->deviceSize |= (uint32_t)((rawCsd[1U] & 0xC0000000U) >> 30U);
			csd->readCurrentVddMin = (uint8_t)((rawCsd[1U] & 0x38000000U) >> 27U);
			csd->readCurrentVddMax = (uint8_t)((rawCsd[1U] & 0x7000000U) >> 24U);
			csd->writeCurrentVddMin = (uint8_t)((rawCsd[1U] & 0xE00000U) >> 20U);
			csd->writeCurrentVddMax = (uint8_t)((rawCsd[1U] & 0x1C0000U) >> 18U);
			csd->deviceSizeMultiplier = (uint8_t)((rawCsd[1U] & 0x38000U) >> 15U);

			/* Get card total block count and block size. */
			card->blockCount = ((csd->deviceSize + 1U) << (csd->deviceSizeMultiplier + 2U));
			card->blockSize = (1U << (csd->readBlockLength));
			if (card->blockSize != FSL_SDMMC_DEFAULT_BLOCK_SIZE)
			{
				card->blockCount = (card->blockCount * card->blockSize);
				card->blockSize = FSL_SDMMC_DEFAULT_BLOCK_SIZE;
				card->blockCount = (card->blockCount / card->blockSize);
			}
			card->capacity = card->blockCount * card->blockSize;
			break;
		case 1:
			card->blockSize = FSL_SDMMC_DEFAULT_BLOCK_SIZE;

			csd->deviceSize = (uint32_t)((rawCsd[2U] & 0x3FU) << 16U);
			csd->deviceSize |= (uint32_t)((rawCsd[1U] & 0xFFFF0000U) >> 16U);
			card->blockCount = ((csd->deviceSize + 1U) * 1024U);
			card->capacity = card->blockSize * card->blockCount;
			break;
		default:
			break;
	}
	if ((uint8_t)((rawCsd[1U] & 0x4000U) >> 14U))
	{
		csd->flags |= kSD_CsdEraseBlockEnabledFlag;
	}
	csd->eraseSectorSize = (uint8_t)((rawCsd[1U] & 0x3F80U) >> 7U);
	csd->writeProtectGroupSize = (uint8_t)(rawCsd[1U] & 0x7FU);
	if ((uint8_t)(rawCsd[0U] & 0x80000000U))
	{
		csd->flags |= kSD_CsdWriteProtectGroupEnabledFlag;
	}
	csd->writeSpeedFactor = (uint8_t)((rawCsd[0U] & 0x1C000000U) >> 26U);
	csd->writeBlockLength = (uint8_t)((rawCsd[0U] & 0x3C00000U) >> 22U);
	if ((uint8_t)((rawCsd[0U] & 0x200000U) >> 21U))
	{
		csd->flags |= kSD_CsdWriteBlockPartialFlag;
	}
	if ((uint8_t)((rawCsd[0U] & 0x8000U) >> 15U))
	{
		csd->flags |= kSD_CsdFileFormatGroupFlag;
	}
	if ((uint8_t)((rawCsd[0U] & 0x4000U) >> 14U))
	{
		csd->flags |= kSD_CsdCopyFlag;
	}
	if ((uint8_t)((rawCsd[0U] & 0x2000U) >> 13U))
	{
		csd->flags |= kSD_CsdPermanentWriteProtectFlag;
	}
	if ((uint8_t)((rawCsd[0U] & 0x1000U) >> 12U))
	{
		csd->flags |= kSD_CsdTemporaryWriteProtectFlag;
	}
	csd->fileFormat = (uint8_t)((rawCsd[0U] & 0xC00U) >> 10U);
}

static void SD_DecodeScr(sd_card_t *card, uint32_t *rawScr)
{
	sd_scr_t *scr;

	scr = &(card->scr);
	scr->scrStructure = (uint8_t)((rawScr[0U] & 0xF0000000U) >> 28U);
	scr->sdSpecification = (uint8_t)((rawScr[0U] & 0xF000000U) >> 24U);
	if ((uint8_t)((rawScr[0U] & 0x800000U) >> 23U))
	{
		scr->flags |= kSD_ScrDataStatusAfterErase;
	}
	scr->sdSecurity = (uint8_t)((rawScr[0U] & 0x700000U) >> 20U);
	scr->sdBusWidths = (uint8_t)((rawScr[0U] & 0xF0000U) >> 16U);
	if ((uint8_t)((rawScr[0U] & 0x8000U) >> 15U))
	{
		scr->flags |= kSD_ScrSdSpecification3;
	}
	scr->extendedSecurity = (uint8_t)((rawScr[0U] & 0x7800U) >> 10U);
	scr->commandSupport = (uint8_t)(rawScr[0U] & 0x3U);
	scr->reservedForManufacturer = rawScr[1U];
	/* Get specification version. */
	switch (scr->sdSpecification)
	{
		case 0U:
			card->version = kSD_SpecificationVersion1_0;
			break;
		case 1U:
			card->version = kSD_SpecificationVersion1_1;
			break;
		case 2U:
			card->version = kSD_SpecificationVersion2_0;
			if (card->scr.flags & kSD_ScrSdSpecification3)
			{
				card->version = kSD_SpecificationVersion3_0;
			}
			break;
		default:
			break;
	}
}

static status_t SD_SelectBusTiming(USDHC_Type *base)
{
	int err;
	u32 switch_func[64];
	u32 arg = 0x80fffff1;

	/* Check if card support high speed mode. */
	/* CMD6 switch function command, query function */
	err = USDHC_SendCommand_with_data(base, SCMD6, 0, switch_func, 64);
	if (err < 0)
		return -1;

	switch_func[3] = SWAP_32(switch_func[3]);
	if ((switch_func[13] >> 16) && 0) {
		printf("the card can not supprt high speed\r\n");
		return -1;
	}
	printf("the card supported mode is 0x%x\r\n", switch_func[3] >> 16);

	printf("=====\r\n");

	/* Switch to high speed mode. */
	/* CMD6 switch function command, select function */
	err = USDHC_SendCommand_with_data(base, SCMD6, arg, switch_func, 64);
	if (err < 0)
		return -1;
	switch_func[4] = SWAP_32(switch_func[4]);
	if((switch_func[4] >> 24) == 1)
		printf("set card high speed success\r\n", switch_func[3] >> 16);

	return 0;
}

int sd_init(USDHC_Type *base)
{
	int err;
	u32 retries, acmd41arg = 0, resp[4], raw_scr[2];

	USDHC_Init(base);
	CardInsertDetect(base);

	/* set DATA bus width */
	USDHC_SetDataBusWidth(base, kUSDHC_DataBusWidth1Bit);
	/*set card freq to 400KHZ*/
	g_sd_card.busClock_Hz = USDHC_SetSdClock(base, 198000000U, SDMMC_CLOCK_400KHZ);
	/* send card active */
	USDHC_SetCardActive(base, 100U);
	/* Get host capability. ignore£¬just decision form spec HOST_CTRL_CAP*/ 

	/* CMD0 - GO_IDLE_STATE software reset and set into idle */
	err = USDHC_SendCommand(base, CMD0, 0x0);
	if (err < 0)
		return -1;

	/* verify card interface operating condition. */
	for (retries = 0; retries < 10; retries++) {
		/* CMD8 (physical layer spec Ver2.0 is mandatory) */		
		err = USDHC_SendCommand(base, CMD8, 0x01aa);
		if (err == 0)
			break;
	}

	if (err == 0) {
		/* SDHC or SDXC card */
		acmd41arg |= kSD_OcrHostCapacitySupportFlag;
	} else {
		/* SDSC card */
		err = USDHC_SendCommand(base, CMD0, 0x0);
		if (err !=  0)
			return -1;
	}

	acmd41arg |= (kSD_OcrVdd32_33Flag | kSD_OcrVdd33_34Flag);
	for(retries = 0; retries < 5000; retries++) {
		/* rca = 0 since card is in IDLE state */
		err = USDHC_SendCommand(base, CMD55, 0x0);
		if (err < 0)
			return -1;

		/* ACMD41 to query OCR */
		err = USDHC_SendCommand(base, ACMD41, acmd41arg);
		if (err < 0)
			return -1;

		if (base->CMD_RSP0 & kSD_OcrPowerUpBusyFlag) {
			g_sd_card.ocr = base->CMD_RSP0;
			printf("ocr is 0x%x\r\n", g_sd_card.ocr);
			break;
		}
	}

	if (retries >= 1000 ) {
		printf("HandShakeOperationConditionFailed\r\n");
		return -1;
	}

	/* check 1.8V support */
	if (g_sd_card.ocr & kSD_OcrSwitch18AcceptFlag) {
		printf("support 1.8v\r\n");
	}
	// our board just support 3.3v, ignore it
	//SD_SwitchVoltage(card))

	/* get CID number */
	err = USDHC_SendCommand(base, CMD2, 0x0);
	if (err < 0)
		return -1;	
	USDHC_GetResponse(base, resp);
	memcpy(g_sd_card.rawCid, resp, sizeof(resp));
	SD_DecodeCid(&g_sd_card, g_sd_card.rawCid);

	/* publish a new relative card address(RCA) */
	err = USDHC_SendCommand(base, CMD3, 0x0);
	if (err < 0)
		return -1;
	g_sd_card.relativeAddress = base->CMD_RSP0 >> 16;
	printf("relative address is 0x%x\r\n", g_sd_card.relativeAddress);

	/* get CID number */
	err = USDHC_SendCommand(base, CMD9, g_sd_card.relativeAddress << 16);
	if (err < 0)
		return -1;	
	USDHC_GetResponse(base, resp);
	memcpy(g_sd_card.rawCsd, resp, sizeof(resp));
	SD_DecodeCsd(&g_sd_card, g_sd_card.rawCsd);
	printf("card is %s", g_sd_card.csd.csdStructure ? "SDHC/SDXC" : "SDSC");
	printf("card block count is %d\r\n", g_sd_card.blockCount);
	printf("card sector size is %d\r\n", g_sd_card.blockSize);
	printf("card command class is 0x%x\r\n", g_sd_card.csd.cardCommandClass);

	/* CMD7: SelectCard */
	err = USDHC_SendCommand(base, CMD7, g_sd_card.relativeAddress << 16);
	if (err < 0)
		return err;

	/* ACMD51: Read SCR */
	err = USDHC_SendCommand(base, CMD55, g_sd_card.relativeAddress << 16);
	if (err < 0)
		return err;
	err = USDHC_SendCommand_with_data(base, ACMD51, 0, raw_scr, 8);
	if (err < 0)
		return err;
	raw_scr[0] = SWAP_32(raw_scr[0]);
	raw_scr[1] = SWAP_32(raw_scr[1]);
	memcpy(g_sd_card.rawScr, raw_scr, sizeof(raw_scr));
	SD_DecodeScr(&g_sd_card, g_sd_card.rawScr);

	printf("scr[0] is 0x%x, scr[1] is 0x%x\r\n", raw_scr[0], raw_scr[1]);
	printf("sd specification is 0x%x\r\n", g_sd_card.scr.sdSpecification);
	if ((g_sd_card.scr.sdBusWidths & 0x4U) == 0) {
		printf("The card can not support 4bit width");
		return -1;
	}
	/* speed class control cmd */
	if ((g_sd_card.scr.commandSupport & 0x01U) == 0)
	{
		printf("The card can not support Speed Class Control (CMD20)\r\n");
	}
	/* set block count cmd */
	if ((g_sd_card.scr.commandSupport & 0x02U) == 0)
	{
		printf("The card can not support Support SetBlockCountCmd (CMD23)\r\n");
	}

	/* Set to max frequency in non-high speed mode. */
	g_sd_card.busClock_Hz = USDHC_SetSdClock(base, 198000000U, SD_CLOCK_25MHZ);

	/* Set to 4-bit data bus mode. */
	/* set card to 4 bit width*/
	err = USDHC_SendCommand(base, CMD55, g_sd_card.relativeAddress << 16);
	if (err < 0)
		return err;	
	err =  USDHC_SendCommand(base, ACMD6, 2);
	if (err < 0)
		return err;
	/* set host to 4 bit width*/
	base->PROT_CTRL = ((base->PROT_CTRL & ~USDHC_PROT_CTRL_DTW_MASK) | USDHC_PROT_CTRL_DTW(1));

	/* set block size to 512 : CMD16 SET_BLOCKLEN */
	err = USDHC_SendCommand(base, CMD16, FSL_SDMMC_DEFAULT_BLOCK_SIZE);
	if (err < 0)
		return err;	

	/* select high speed successful, switch clock to 50M */
	if (SD_SelectBusTiming(base) == 0)
		g_sd_card.busClock_Hz = USDHC_SetSdClock(base, 198000000U, SD_CLOCK_50MHZ);
		//g_sd_card.busClock_Hz = USDHC_SetSdClock(base, 396000000U, SD_CLOCK_50MHZ);
	printf("clock is %d, base->SYS_CTRL is 0x%x\r\n", g_sd_card.busClock_Hz, base->SYS_CTRL);

	return 0;
}

static int SD_WaitReadWriteComplete(USDHC_Type *base)
{
	int err;

	do {
		/* ugly code just distinguish usdhc1 and usdhc2 */
		if( base == USDHC1)
			err = USDHC_SendCommand(base, CMD13, g_sd_card.relativeAddress << 16);
		else if (base == USDHC2)
			err = USDHC_SendCommand(base, CMD13, g_mmc_card.relativeAddress << 16);
		if (err < 0)
			goto exit;

		if ((base->CMD_RSP0 & (kSDMMC_R1ErrorAllFlag | kSDMMC_R1SwitchErrorFlag)))
		{
			return -1;
		}
		
		if ((base->CMD_RSP0 & kSDMMC_R1ReadyForDataFlag) &&
				(SDMMC_R1_CURRENT_STATE(base->CMD_RSP0) != kSDMMC_R1StateProgram))
		{
			break;
		}

	} while (true);

exit:
	return base->CMD_RSP0;
}


int sd_read_blocks(USDHC_Type *base, uint8_t *buffer, uint32_t startBlock, uint32_t blockCount)
{
	int err = 0;

	/* Wait until command/data bus out of busy status. */
	while (base->PRES_STATE & USDHC_PRES_STATE_CIHB_MASK)
	{
	}
	while (base->PRES_STATE & USDHC_PRES_STATE_CDIHB_MASK)
	{
	}
	/* set ADMA descriptor */
	USDHC_CreateDescTable(buffer, blockCount * 512);

	/* When use ADMA, disable simple DMA */
	base->DS_ADDR = 0U;
	base->ADMA_SYS_ADDR = (u32) g_adma2_tablbe;

	/* config data block size/block count */
	base->BLK_ATT = (USDHC_BLK_ATT_BLKSIZE(512) | USDHC_BLK_ATT_BLKCNT(blockCount));

	/* disable the external DMA if support */
	base->VEND_SPEC &= ~USDHC_VEND_SPEC_EXT_DMA_EN_MASK;
	/* select DMA mode and config the burst length */
	base->PROT_CTRL &= ~(USDHC_PROT_CTRL_DMASEL_MASK | USDHC_PROT_CTRL_BURST_LEN_EN_MASK);
	base->PROT_CTRL |=
		USDHC_PROT_CTRL_DMASEL(kUSDHC_DmaModeAdma2) | USDHC_PROT_CTRL_BURST_LEN_EN(kUSDHC_EnBurstLenForINCR);

	if (blockCount == 1) {
		/* single block read*/
		/* direction:read, enable DMA */
		base->MIX_CTRL &= ~(USDHC_MIX_CTRL_MSBSEL_MASK | USDHC_MIX_CTRL_BCEN_MASK | USDHC_MIX_CTRL_DTDSEL_MASK |
				USDHC_MIX_CTRL_AC12EN_MASK | USDHC_MIX_CTRL_DMAEN_MASK);
		base->MIX_CTRL |= USDHC_MIX_CTRL_DTDSEL_MASK | USDHC_MIX_CTRL_DMAEN_MASK;

		/* config the command xfertype and argument */
		base->CMD_ARG = startBlock;
		base->CMD_XFR_TYP = CMD17 << 16;
	} else {
		/* multi block read */
		/* block count enable, Multiblcok, direction:read, enable DMA */
		base->MIX_CTRL &= ~(USDHC_MIX_CTRL_MSBSEL_MASK | USDHC_MIX_CTRL_BCEN_MASK | USDHC_MIX_CTRL_DTDSEL_MASK |
				USDHC_MIX_CTRL_AC12EN_MASK | USDHC_MIX_CTRL_DMAEN_MASK);
		base->MIX_CTRL |= USDHC_MIX_CTRL_MSBSEL_MASK | USDHC_MIX_CTRL_BCEN_MASK | 
			USDHC_MIX_CTRL_DTDSEL_MASK | USDHC_MIX_CTRL_AC12EN_MASK | USDHC_MIX_CTRL_DMAEN_MASK;

		/* config the command xfertype and argument */
		base->CMD_ARG = startBlock;
		base->CMD_XFR_TYP = CMD18 << 16;
	}

	err = USDHC_WaitCommandDone(base);
	if (err < 0)
		return err;

	err =  USDHC_WaitDataDone(base);
	if (err < 0)
		return err;

	err = SD_WaitReadWriteComplete(base);
	if (err & 0xc0200000) {
		err = -1;
		printf("%s read error\r\n", __func__);
	}

	return err;
}

int sd_write_blocks(USDHC_Type *base, uint8_t *buffer, uint32_t startBlock, uint32_t blockCount)
{
	int err = 0;

	/* Wait until command/data bus out of busy status. */
	while (base->PRES_STATE & USDHC_PRES_STATE_CIHB_MASK)
	{
	}
	while (base->PRES_STATE & USDHC_PRES_STATE_CDIHB_MASK)
	{
	}
	/* set ADMA descriptor */
	USDHC_CreateDescTable(buffer, blockCount * 512);

	/* When use ADMA, disable simple DMA */
	base->DS_ADDR = 0U;
	base->ADMA_SYS_ADDR = (u32) g_adma2_tablbe;

	/* config data block size/block count */
	base->BLK_ATT = (USDHC_BLK_ATT_BLKSIZE(512) | USDHC_BLK_ATT_BLKCNT(blockCount));

	/* disable the external DMA if support */
	base->VEND_SPEC &= ~USDHC_VEND_SPEC_EXT_DMA_EN_MASK;
	/* select DMA mode and config the burst length */
	base->PROT_CTRL &= ~(USDHC_PROT_CTRL_DMASEL_MASK | USDHC_PROT_CTRL_BURST_LEN_EN_MASK);
	base->PROT_CTRL |=
		USDHC_PROT_CTRL_DMASEL(kUSDHC_DmaModeAdma2) | USDHC_PROT_CTRL_BURST_LEN_EN(kUSDHC_EnBurstLenForINCR);

	if (blockCount == 1) {
		/* single block wrtie*/
		/* direction:read, enable DMA */
		base->MIX_CTRL &= ~(USDHC_MIX_CTRL_MSBSEL_MASK | USDHC_MIX_CTRL_BCEN_MASK | USDHC_MIX_CTRL_DTDSEL_MASK |
				USDHC_MIX_CTRL_AC12EN_MASK | USDHC_MIX_CTRL_DMAEN_MASK);
		base->MIX_CTRL |= USDHC_MIX_CTRL_DMAEN_MASK;

		/* config the command xfertype and argument */
		base->CMD_ARG = startBlock;
		base->CMD_XFR_TYP = CMD24 << 16;
	} else {
		/* multi block write */
		/* block count enable, Multiblcok, direction:read, enable DMA */
		base->MIX_CTRL &= ~(USDHC_MIX_CTRL_MSBSEL_MASK | USDHC_MIX_CTRL_BCEN_MASK | USDHC_MIX_CTRL_DTDSEL_MASK |
				USDHC_MIX_CTRL_AC12EN_MASK | USDHC_MIX_CTRL_DMAEN_MASK);
		base->MIX_CTRL |= USDHC_MIX_CTRL_MSBSEL_MASK | USDHC_MIX_CTRL_BCEN_MASK |
			USDHC_MIX_CTRL_AC12EN_MASK | USDHC_MIX_CTRL_DMAEN_MASK;

		/* config the command xfertype and argument */
		base->CMD_ARG = startBlock;
		base->CMD_XFR_TYP = CMD25 << 16;
	}

	err = USDHC_WaitCommandDone(base);
	if (err < 0)
		return err;

	err =  USDHC_WaitDataDone(base);
	if (err < 0)
		return err;

	err = SD_WaitReadWriteComplete(base);
	if (err & 0xe4000000) {
		err = -1;
		printf("%s write error\r\n", __func__);
	}

	return err;
}


/******************************************************************/
/******************************************************************/
static void MMC_DecodeCsd(mmc_card_t *card, uint32_t *rawCsd)
{
    mmc_csd_t *csd;
    uint32_t multiplier;

    csd = &(card->csd);
    csd->csdStructureVersion = (uint8_t)((rawCsd[3U] & 0xC0000000U) >> 30U);
    csd->systemSpecificationVersion = (uint8_t)((rawCsd[3U] & 0x3C000000U) >> 26U);
    csd->dataReadAccessTime1 = (uint8_t)((rawCsd[3U] & 0xFF0000U) >> 16U);
    csd->dataReadAccessTime2 = (uint8_t)((rawCsd[3U] & 0xFF00U) >> 8U);
    csd->transferSpeed = (uint8_t)(rawCsd[3U] & 0xFFU);
    csd->cardCommandClass = (uint16_t)((rawCsd[2U] & 0xFFF00000U) >> 20U);
    /* Max block length read/write one time */
    csd->readBlockLength = (uint8_t)((rawCsd[2U] & 0xF0000U) >> 16U);
    if (rawCsd[2U] & 0x8000U)
    {
        csd->flags |= kMMC_CsdReadBlockPartialFlag;
    }
    if (rawCsd[2U] & 0x4000U)
    {
        csd->flags |= kMMC_CsdWriteBlockMisalignFlag;
    }
    if (rawCsd[2U] & 0x2000U)
    {
        csd->flags |= kMMC_CsdReadBlockMisalignFlag;
    }
    if (rawCsd[2U] & 0x1000U)
    {
        csd->flags |= kMMC_CsdDsrImplementedFlag;
    }
    csd->deviceSize = (uint16_t)(((rawCsd[2U] & 0x3FFU) << 2U) + ((rawCsd[1U] & 0xC0000000U) >> 30U));
    csd->readCurrentVddMin = (uint8_t)((rawCsd[1U] & 0x38000000U) >> 27U);
    csd->readCurrentVddMax = (uint8_t)((rawCsd[1U] & 0x07000000U) >> 24U);
    csd->writeCurrentVddMin = (uint8_t)((rawCsd[1U] & 0x00E00000U) >> 21U);
    csd->writeCurrentVddMax = (uint8_t)((rawCsd[1U] & 0x001C0000U) >> 18U);
    csd->deviceSizeMultiplier = (uint8_t)((rawCsd[1U] & 0x00038000U) >> 15U);
    csd->eraseGroupSize = (uint8_t)((rawCsd[1U] & 0x00007C00U) >> 10U);
    csd->eraseGroupSizeMultiplier = (uint8_t)((rawCsd[1U] & 0x000003E0U) >> 5U);
    csd->writeProtectGroupSize = (uint8_t)(rawCsd[1U] & 0x0000001FU);
    if (rawCsd[0U] & 0x80000000U)
    {
        csd->flags |= kMMC_CsdWriteProtectGroupEnabledFlag;
    }
    csd->defaultEcc = (uint8_t)((rawCsd[0U] & 0x60000000U) >> 29U);
    csd->writeSpeedFactor = (uint8_t)((rawCsd[0U] & 0x1C000000U) >> 26U);
    csd->maxWriteBlockLength = (uint8_t)((rawCsd[0U] & 0x03C00000U) >> 22U);
    if (rawCsd[0U] & 0x00200000U)
    {
        csd->flags |= kMMC_CsdWriteBlockPartialFlag;
    }
    if (rawCsd[0U] & 0x00010000U)
    {
        csd->flags |= kMMC_ContentProtectApplicationFlag;
    }
    if (rawCsd[0U] & 0x00008000U)
    {
        csd->flags |= kMMC_CsdFileFormatGroupFlag;
    }
    if (rawCsd[0U] & 0x00004000U)
    {
        csd->flags |= kMMC_CsdCopyFlag;
    }
    if (rawCsd[0U] & 0x00002000U)
    {
        csd->flags |= kMMC_CsdPermanentWriteProtectFlag;
    }
    if (rawCsd[0U] & 0x00001000U)
    {
        csd->flags |= kMMC_CsdTemporaryWriteProtectFlag;
    }
    csd->fileFormat = (uint8_t)((rawCsd[0U] & 0x00000C00U) >> 10U);
    csd->eccCode = (uint8_t)((rawCsd[0U] & 0x00000300U) >> 8U);

    /* Calculate the device total block count. */
    /* For the card capacity of witch higher than 2GB, the maximum possible value should be set to this register
    is 0xFFF. */
    if (card->csd.deviceSize != 0xFFFU)
    {
        multiplier = (2U << (card->csd.deviceSizeMultiplier + 2U - 1U));
        card->userPartitionBlocks = (((card->csd.deviceSize + 1U) * multiplier) / FSL_SDMMC_DEFAULT_BLOCK_SIZE);
    }
    card->blockSize = FSL_SDMMC_DEFAULT_BLOCK_SIZE;
}


static void MMC_DecodeCid(mmc_card_t *card, uint32_t *rawCid)
{
    mmc_cid_t *cid;

    cid = &(card->cid);
    cid->manufacturerID = (uint8_t)((rawCid[3U] & 0xFF000000U) >> 24U);
    cid->applicationID = (uint16_t)((rawCid[3U] & 0xFFFF00U) >> 8U);

    cid->productName[0U] = (uint8_t)((rawCid[3U] & 0xFFU));
    cid->productName[1U] = (uint8_t)((rawCid[2U] & 0xFF000000U) >> 24U);
    cid->productName[2U] = (uint8_t)((rawCid[2U] & 0xFF0000U) >> 16U);
    cid->productName[3U] = (uint8_t)((rawCid[2U] & 0xFF00U) >> 8U);
    cid->productName[4U] = (uint8_t)((rawCid[2U] & 0xFFU));

    cid->productVersion = (uint8_t)((rawCid[1U] & 0xFF000000U) >> 24U);

    cid->productSerialNumber = (uint32_t)((rawCid[1U] & 0xFFFFFFU) << 8U);
    cid->productSerialNumber |= (uint32_t)((rawCid[0U] & 0xFF000000U) >> 24U);

    cid->manufacturerData = (uint16_t)((rawCid[0U] & 0xFFF00U) >> 8U);
}


static void MMC_DecodeExtendedCsd(mmc_card_t *card, uint32_t *rawExtendedCsd)
{
    uint8_t *buffer = (uint8_t *)rawExtendedCsd;
    mmc_extended_csd_t *extendedCsd = &(card->extendedCsd);

    /* Extended CSD is transferred as a data block from least byte indexed 0. */
    extendedCsd->highDensityEraseGroupDefinition = buffer[175U];
    extendedCsd->bootDataBusWidth = buffer[177U];
    extendedCsd->partitionConfig = buffer[179U];
    extendedCsd->eraseMemoryContent = buffer[181U];
    extendedCsd->dataBusWidth = buffer[183U];
    extendedCsd->highSpeedTiming = buffer[185U];
    extendedCsd->powerClass = buffer[187U];
    extendedCsd->commandSetRevision = buffer[189U];
    extendedCsd->commandSet = buffer[191U];
    extendedCsd->extendecCsdVersion = buffer[192U];
    extendedCsd->csdStructureVersion = buffer[194U];
    extendedCsd->partitionAttribute = buffer[156U];
    extendedCsd->extPartitionSupport = buffer[494U];
    extendedCsd->cardType = buffer[196U];
    /* This field define the type of the card. The only currently valid values for this field are 0x01 and 0x03. */
    card->flags |= extendedCsd->cardType;

    extendedCsd->ioDriverStrength = buffer[197U];

    extendedCsd->powerClass52MHz195V = buffer[200U];
    extendedCsd->powerClass26MHz195V = buffer[201U];
    extendedCsd->powerClass52MHz360V = buffer[202U];
    extendedCsd->powerClass26MHz360V = buffer[203U];
    extendedCsd->powerClass200MHZVCCQ130VVCC360V = buffer[236U];
    extendedCsd->powerClass200MHZVCCQ195VVCC360V = buffer[237U];
    extendedCsd->powerClass52MHZDDR195V = buffer[238U];
    extendedCsd->powerClass52MHZDDR360V = buffer[239U];
    extendedCsd->powerClass200MHZDDR360V = buffer[253U];
    extendedCsd->minimumReadPerformance4Bit26MHz = buffer[205U];
    extendedCsd->minimumWritePerformance4Bit26MHz = buffer[206U];
    extendedCsd->minimumReadPerformance8Bit26MHz4Bit52MHz = buffer[207U];
    extendedCsd->minimumWritePerformance8Bit26MHz4Bit52MHz = buffer[208U];
    extendedCsd->minimumReadPerformance8Bit52MHz = buffer[209U];
    extendedCsd->minimumWritePerformance8Bit52MHz = buffer[210U];
    extendedCsd->minReadPerformance8bitAt52MHZDDR = buffer[234U];
    extendedCsd->minWritePerformance8bitAt52MHZDDR = buffer[235U];
    /* Get user partition size. */
    extendedCsd->sectorCount = ((((uint32_t)buffer[215U]) << 24U) + (((uint32_t)buffer[214U]) << 16U) +
                                (((uint32_t)buffer[213U]) << 8U) + (uint32_t)buffer[212U]);
    if (card->flags & kMMC_SupportHighCapacityFlag)
    {
        card->userPartitionBlocks = card->extendedCsd.sectorCount;
    }

    extendedCsd->sleepAwakeTimeout = buffer[217U];
    extendedCsd->sleepCurrentVCCQ = buffer[219U];
    extendedCsd->sleepCurrentVCC = buffer[220U];
    extendedCsd->highCapacityWriteProtectGroupSize = buffer[221U];
    extendedCsd->reliableWriteSectorCount = buffer[222U];
    extendedCsd->highCapacityEraseTimeout = buffer[223U];
    extendedCsd->highCapacityEraseUnitSize = buffer[224U];
    extendedCsd->accessSize = buffer[225U];

    /* Get boot partition size. */
    extendedCsd->bootSizeMultiplier = buffer[226U];
    card->bootPartitionBlocks = ((128U * 1024U * extendedCsd->bootSizeMultiplier) / FSL_SDMMC_DEFAULT_BLOCK_SIZE);

    /* Check if card support alternate boot. */
    extendedCsd->bootInformation = buffer[228U];
    if (extendedCsd->bootInformation & kMMC_SupportAlternateBoot)
    {
        card->flags |= kMMC_SupportAlternateBootFlag;
    }
    else if (extendedCsd->bootInformation & kMMC_SupportDDRBootFlag)
    {
        card->flags |= kMMC_SupportDDRBootFlag;
    }
    else if (extendedCsd->bootInformation & kMMC_SupportHighSpeedBoot)
    {
        card->flags |= kMMC_SupportHighSpeedBootFlag;
    }
    else
    {
    }
    /* cache size unit 1kb */
    extendedCsd->cacheSize = (((uint32_t)buffer[252U]) << 24) | (((uint32_t)buffer[251U]) << 16) |
                             (((uint32_t)buffer[250U]) << 8) | (((uint32_t)buffer[249U]));

    extendedCsd->supportedCommandSet = buffer[504U];
}

static status_t MMC_SetExtendedCsdConfig(USDHC_Type *base, mmc_extended_csd_access_mode_t access_mode, uint8_t index, uint8_t value)
{
	int err;

	err = USDHC_SendCommand(base, MCMD6, (access_mode << 24) | (index << 16) | (value << 8));
	if (err < 0)
		return -1;

	return 0;

}

static int MMC_SetBusWidth(USDHC_Type *base, int width)
{
	int err;

	/* CMD6: write EXT_CSD register to set bus width */
	err = MMC_SetExtendedCsdConfig(base, kMMC_ExtendedCsdAccessModeWriteBits, 183, width / 4);
	if (err < 0)
		return -1;

	err = SD_WaitReadWriteComplete(base);
	if (err & (1 << 7)) {
		printf("failed switch to bus width %d\r\n", width);
		return -1;
	}

	/* set host bus width*/
    base->PROT_CTRL &=  ~USDHC_PROT_CTRL_DTW_MASK;
    base->PROT_CTRL |= USDHC_PROT_CTRL_DTW(width / 4);

	return 0;
}

static status_t MMC_SwitchBusMode(USDHC_Type *base, mmc_high_speed_timing_t mode)
{
	int err;

	err = MMC_SetExtendedCsdConfig(base, kMMC_ExtendedCsdAccessModeWriteBits, 185,  mode);
	if (err < 0)
		return -1;

	err = SD_WaitReadWriteComplete(base);
	if (err & (1 << 7)) {
		printf("failed switch bus mode\r\n");
		return -1;
	}

	return 0;
}

static status_t MMC_SelectPartition(USDHC_Type *base, mmc_extended_csd_access_mode_t access_mode, mmc_access_partition_t part_number)
{
	int err;

	err = MMC_SetExtendedCsdConfig(base, access_mode, 179,  part_number);
	if (err < 0)
		return -1;

	err = SD_WaitReadWriteComplete(base);
	if (err & (1 << 7)) {
		printf("failed select partition\r\n");
		return -1;
	}

	return 0;
}

int MMC_SelectBoot1Partition(USDHC_Type *base)
{

	return	MMC_SelectPartition(base, kMMC_ExtendedCsdAccessModeSetBits, 1);
}

int MMC_ExitBoot1Partition(USDHC_Type *base)
{
	return	MMC_SelectPartition(base, kMMC_ExtendedCsdAccessModeClearBits, 1);
}

int mmc_init(USDHC_Type *base)
{
	int err;
	u32 retries, resp[4], maxBusClock_Hz, bus_width = 4;

	USDHC_Init(base);

	/* set DATA bus width */
	USDHC_SetDataBusWidth(base, kUSDHC_DataBusWidth1Bit);
	/*set card freq to 400KHZ*/
	g_mmc_card.busClock_Hz = USDHC_SetSdClock(base, 198000000U, SDMMC_CLOCK_400KHZ);
	/* send card active */
	USDHC_SetCardActive(base, 100U);
	/* Get host capability. ignore£¬just decision form spec HOST_CTRL_CAP*/ 

	/* CMD0 - GO_IDLE_STATE software reset the bus and set into idle */
	err = USDHC_SendCommand(base, CMD0, 0x0);
	if (err < 0)
		return -1;

    /* Hand-shaking with card to validata the voltage range Host first sending its expected
       information.*/
    err = USDHC_SendCommand(base, CMD1, 0x0);
	if (err < 0)
		return -1;
	g_mmc_card.ocr = base->CMD_RSP0;
	g_mmc_card.ocr |= 2 << 29;/* set access mode to sector mode */
	for (retries = 0; retries < 1000; retries++) {		
		err = USDHC_SendCommand(base, CMD1, 0x0);
		if (err < 0)
			return -1;

		if ((base->CMD_RSP0 & MMC_OCR_BUSY_MASK))
			break;
	}

	if (retries >= 1000 ) {
		printf("HandShakeOperationConditionFailed\r\n");
		return -1;
	}
	g_mmc_card.ocr = base->CMD_RSP0;
	printf("ocr is 0x%x\r\n", g_mmc_card.ocr);

	/* switch the host voltage which the card can support */
	/* ignore switch voltage, our board just support 3.3v */


	/* CMD2 Get card CID */
	err = USDHC_SendCommand(base, CMD2, 0x0);
	if (err < 0)
		return -1;
	USDHC_GetResponse(base, resp);
	memcpy(g_mmc_card.rawCid, resp, sizeof(resp));
	MMC_DecodeCid(&g_mmc_card, g_mmc_card.rawCid);

	/* Send CMD3 with a chosen relative address, with value greater than 1 */
	g_mmc_card.relativeAddress = 2;
	err = USDHC_SendCommand(base, CMD3, g_mmc_card.relativeAddress << 16);
	if (err < 0)
		return -1;

	/* CMD9 Get the CSD register content */
	err = USDHC_SendCommand(base, CMD9, g_mmc_card.relativeAddress << 16);
	if (err < 0)
		return -1;
	USDHC_GetResponse(base, resp);
	memcpy(g_mmc_card.rawCsd, resp, sizeof(resp));
	MMC_DecodeCsd(&g_mmc_card, g_mmc_card.rawCsd);

	/* Set to maximum speed in normal mode. */
	/*used to calculate the max speed in normal
    mode not high speed mode.
    For cards supporting version 4.0, 4.1, and 4.2 of the specification, the value shall be 20MHz(0x2A).
    For cards supporting version 4.3, the value shall be 26 MHz (0x32H). In High speed mode, the max
    frequency is decided by CARD_TYPE in Extended CSD. */
	printf("csd tran speed is 0x%x\r\n", g_mmc_card.csd.transferSpeed);
	if (g_mmc_card.csd.transferSpeed == 0x32)
		maxBusClock_Hz = 26000000;
	else if ( g_mmc_card.csd.transferSpeed == 0x2A )
		maxBusClock_Hz = 20000000;

	g_mmc_card.busClock_Hz = USDHC_SetSdClock(base, 198000000U, maxBusClock_Hz);

    /* Send CMD7 with the card's relative address to place the card in transfer state. Puts current selected card in
    transfer state. */
	err = USDHC_SendCommand(base, CMD7, g_mmc_card.relativeAddress << 16);
	if (err < 0)
		return err;

	/* Get Extended CSD register content. */
	err = USDHC_SendCommand_with_data(base, MCMD8, 0, g_mmc_card.rawExtendedCsd, 512);
	if (err < 0)
		return err;
	MMC_DecodeExtendedCsd(&g_mmc_card, g_mmc_card.rawExtendedCsd);
	printf("g_mmc_card.extendedCsd.sectorCount is %d\r\n", g_mmc_card.extendedCsd.sectorCount);

	/* set block size to 512 : CMD16 SET_BLOCKLEN */
	err = USDHC_SendCommand(base, CMD16, FSL_SDMMC_DEFAULT_BLOCK_SIZE);
	if (err < 0)
		return err;	

    /* switch to host support speed mode, then switch MMC data bus width and select power class */
	/* select bus width */
	err = MMC_SetBusWidth(base, bus_width);
	if (err < 0)
		return -1;
	printf("bus width is %d\r\n", bus_width);

	/* switch to high speed mode */
	err = MMC_SwitchBusMode(base, kMMC_HighSpeedTiming);
	if (err < 0)
		return err;
	g_mmc_card.busClock_Hz = USDHC_SetSdClock(base, 198000000U, MMC_CLOCK_52MHZ);

	printf("init MMC sucessful\r\n");

	return 0;
}


