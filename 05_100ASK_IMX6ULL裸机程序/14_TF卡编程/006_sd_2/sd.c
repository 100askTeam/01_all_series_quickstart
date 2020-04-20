#include "common.h"
#include "sd.h"
#include "string.h"


sd_card_t g_sd_card;
//static u8 cmd_desc_fifo[1000]  __attribute__ ((aligned(32))) = {1,2,3};
//static u8 cmd_desc_fifo[1000]  __attribute__ ((aligned(1024*1024))) = {1,2,3};

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

/*static bool USDHC_Reset_Cmd_Line(USDHC_Type *base, uint32_t timeout)
{
	return USDHC_Reset(base, USDHC_SYS_CTRL_RSTC_MASK, timeout);
}
*/

/*static bool USDHC_Reset_Data_Line(USDHC_Type *base, uint32_t timeout)
{
	return USDHC_Reset(base, USDHC_SYS_CTRL_RSTD_MASK, timeout);
}
*/
		  			 		  						  					  				 	   		  	  	 	  
int USDHC_CreateDescTable(u8 *data, u32 len)
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
	for (i = 0; i < entries; i++) {
		printf("g_adma2_tablbe[i] address is 0x%x, attribute is 0x%x\r\n", 
			g_adma2_tablbe[i].address, g_adma2_tablbe[i].attribute);
	}

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
//printf("base->INT_STATUS  is 0x%x\r\n",base->INT_STATUS );
	//USDHC_Dump_All(base);

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
    /* Receive response when command completes successfully. */
    /*if (error == kStatus_Success)
    {
        error = USDHC_ReceiveCommandResponse(base, command);
    }*/

    USDHC_ClearInterruptStatusFlags(
        base, (kUSDHC_CommandCompleteFlag | kUSDHC_CommandErrorFlag | kUSDHC_TuningErrorFlag));

    return error;
}

static int USDHC_WaitDataDone(USDHC_Type *base)
{
    int error = 0;
    uint32_t interruptStatus = 0U;
//printf("base->INT_STATUS  is 0x%x\r\n",base->INT_STATUS );
	//USDHC_Dump_All(base);

    /* Wait command complete or USDHC encounters error. */
    while (!(base->INT_STATUS & (kUSDHC_DataCompleteFlag | kUSDHC_DataErrorFlag)))
    {
    }

    interruptStatus = base->INT_STATUS;


    if ((interruptStatus & kUSDHC_DataErrorFlag) != 0U)
    {
    	printf("data errror, CMD is 0x%x, INT_STATUS is 0x%x\r\n", base->CMD_XFR_TYP, interruptStatus);
        error = -1;
    }
    else
    {
    }
    /* Receive response when command completes successfully. */
    /*if (error == kStatus_Success)
    {
        error = USDHC_ReceiveCommandResponse(base, command);
    }*/

    USDHC_ClearInterruptStatusFlags(
        base, (kUSDHC_DataCompleteFlag | kUSDHC_DataErrorFlag));

    return error;
}


static int USDHC_SendCommand(USDHC_Type *base, u32 command, u32 argument)
{
	 //uint32_t mixCtrl, xferType;

	 /* Wait until command/data bus out of busy status. */
    while (base->PRES_STATE & USDHC_PRES_STATE_CIHB_MASK)
    {
    }
    /*while (data && (base->PRES_STATE & USDHC_PRES_STATE_CDIHB_MASK))
    {
    }*/	
 //printf("%s prepare cmd\r\n", __func__);   
	 /* config the command xfertype and argument */
	 base->CMD_ARG = argument;
	 base->CMD_XFR_TYP = command << 16;

	 return USDHC_WaitCommandDone(base);

}

static int USDHC_SendCommand_with_data(USDHC_Type *base, u32 command, u32 argument, void *buf, u32 size)
{
	int err;
	 //uint32_t mixCtrl, xferType;

	 /* Wait until command/data bus out of busy status. */
    while (base->PRES_STATE & USDHC_PRES_STATE_CIHB_MASK)
    {
    }
    while (base->PRES_STATE & USDHC_PRES_STATE_CDIHB_MASK)
    {
    }
 //printf("%s prepare cmd\r\n", __func__);

	/* config the command xfertype and argument */
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
    //base->MIX_CTRL &= ~USDHC_MIX_CTRL_AC23EN_MASK;
    //base->VEND_SPEC2 &= ~USDHC_VEND_SPEC2_ACMD23_ARGU2_EN_MASK;


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

int sd_init(USDHC_Type *base)
{
	int err;
	u32 retries, acmd41arg = 0, resp[4], raw_scr[2];

	USDHC_Init(base);
	CardInsertDetect(base);

	/* set DATA bus width */
    USDHC_SetDataBusWidth(base, kUSDHC_DataBusWidth1Bit);
    /*set card freq to 400KHZ*/
    USDHC_SetSdClock(base, 396000000U, SDMMC_CLOCK_400KHZ);
    /* send card active */
    USDHC_SetCardActive(base, 100U);
    /* Get host capability. ignore£¬just decision form spec HOST_CTRL_CAP*/ 
printf("ACTIVE\r\n");

	/* CMD0 - GO_IDLE_STATE software reset and set into idle */
	err = USDHC_SendCommand(base, CMD0, 0x0);
	if (err < 0)
		return -1;
printf("CMD0\r\n");
	/* verify card interface operating condition. */
	for (retries = 0; retries < 10; retries++) {
		/* CMD8 (physical layer spec Ver2.0 is mandatory) */		
		err = USDHC_SendCommand(base, CMD8, 0x01aa);
		if (err == 0)
			break;
	}
printf("cmd 8 response is 0x%x\r\n", base->CMD_RSP0);

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
if(base->CMD_RSP0 != 0){
	printf("base->CMD_RSP0 is 0x%x\r\n", base->CMD_RSP0);
}
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
	printf("card is %s", g_sd_card.csd.csdStructure ? "SDSC" : "SDHC/SDXC");
	printf("card capacity is %d\r\n", g_sd_card.blockCount);
	printf("card sector size is %d\r\n", g_sd_card.blockSize);
	printf("card command class is %d\r\n", g_sd_card.csd.cardCommandClass);

	/* CMD7: SelectCard */
	err = USDHC_SendCommand(base, CMD7, g_sd_card.relativeAddress << 16);
	if (err < 0)
		return err;

	/* ACMD51: Read SCR */
	err = USDHC_SendCommand(base, CMD55, g_sd_card.relativeAddress << 16);
	if (err < 0)
		return err;
	USDHC_Dump_All(base);

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

	return 0;
}


static status_t sd_read_blocks(USDHC_Type *base, uint8_t *buffer, uint32_t startBlock, uint32_t blockSize, uint32_t blockCount)
{
	return 0;
}
