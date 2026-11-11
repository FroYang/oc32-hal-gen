/**
 ******************************************************************************
 * @file    oc32_hal_iic.c
 * @author
 * @brief   OC32 HAL IIC (I2C) module driver.
 *
 *          Master- and slave-oriented operations for the single on-chip IIC
 *          peripheral (base 0x3FF08300, accessed directly via IICCON and the
 *          standalone IICTBUF/IICRBUF/IICADR/IICDA/IICBC register macros).
 *          Data transfers may use the
 *          register-buffer path (non-DMA, up to 4 bytes) or the DMA path
 *          (up to 1023 bytes), in either polling or interrupt mode.
 *
 *          This driver only touches IIC registers (and the DMA channel state
 *          via the shared HAL_DMA_Update helper). It does not modify any other
 *          module's code.
 ******************************************************************************
 */


/* Includes ------------------------------------------------------------------*/
#include "oc32_hal_iic.h"

#ifdef HAL_IIC_ENABLE

/* Private Typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static HAL_DMA_ChTypedef IicDmaCh = OC32_DMA_CH0;
static HAL_IIC_TransferStateTypedef IicTransState = IIC_TRANS_IDLE;
/* current in-flight packet, set by HAL_IIC_Transfer before a transfer and read
 * back by HAL_IIC_IRQHandler so the state machine can keep stepping with the
 * SAME packet/queue established by the main loop. Note: the caller's Packet
 * and its ->Data must stay alive (module scope / static buffer, not a stack
 * local that dies on return) until the transfer completes (IicDeInit -> IDLE). */
static HAL_IIC_PacketTypedef *IicCurPacket = NULL;

/* Private function prototypes -----------------------------------------------*/

/**
 * @brief  Writes the target device address into IICADR.
 * @param  Packet: pointer to a HAL_IIC_Packettypedef structure, specifies the data packet.
 * @note   10-bit mode (IICHE enabled) packs the 10-bit header + R/W + address
 *         into the 16-bit IICADR field; 7-bit mode shifts the address left one
 *         bit and ORs in the R/W flag at bit 0.
 */
__sram__ void IicSetAddress(HAL_IIC_PacketTypedef *Packet)
{
  if (IICCON->IICHE == HAL_ENABLE)
  {
    WRITE_SR(IICADR, (IIC_ADDR_10BIT_HEADER | ((Packet->DevAddr & 0x300U) << 1U) | Packet->RW << 8U | (Packet->DevAddr & 0xFFU)));
  }
  else
  {
    WRITE_SR(IICADR, (Packet->RW | ((Packet->DevAddr & 0x7FU) << 1U)));
  }
  return;
}

/**
 * @brief  Kick-starts the IIC peripheral.
 * @retval HAL_OK: on success
 * @retval HAL_ERROR: if slave mode is enabled and a late start error occurred
 * @note   Clears the done/ack/clock-timeout flags and sets the kick-start bit
 *         atomically in one write to minimize clock-stretching delay.
 */
__sram__ HAL_StatusTypedef IicKickStart(void)
{
  /* we have to clear flags and kick start at the same time,
     in order to remove clock stretching effect as near as possible. */
  uint32_t iicconval = READ_SR(IICCON) | IICKS_BIT | IICTCLR_BIT | IICRCLR_BIT;
  WRITE_SR(IICCON, iicconval);

  /* check if slave mode is enabled and start late error occurred */
  if ((IICCON->IICS == IIC_SLAVE) && (IICCON->IICSLE == HAL_FLAG_ERROR))
  {
    IICCON->IICCCLR = HAL_ENABLE;
    return HAL_ERROR;
  }

  return HAL_OK;
}

/**
 * @brief  Checks for a handshake (NACK) error.
 * @retval HAL_OK: no error
 * @retval HAL_ERROR: handshake NACK detected (flag is cleared on detection)
 */
__sram__ HAL_StatusTypedef IicHandshakeCheck(void)
{
  /* check if handshake error occurred */
  if (IICCON->IICHNACK == HAL_FLAG_ERROR)
  {
    IICCON->IICHCLR = HAL_ENABLE;
    return HAL_ERROR;
  }
  return HAL_OK;
}

/**
 * @brief  Checks for a clock-stretching timeout (SCL held low too long).
 * @retval HAL_OK: no error
 * @retval HAL_ERROR: clock low-timeout detected (flag is cleared on detection)
 */
__sram__ HAL_StatusTypedef IicClockTimeOutCheck(void)
{
  /* check clock low time out (indicates clock stretching time too long) or not */
  if (IICCON->IICCLTO == HAL_FLAG_ERROR)
  {
    IICCON->IICCCLR = HAL_ENABLE;
    return HAL_ERROR;
  }
  return HAL_OK;
}

/**
 * @brief  Transmits a single IIC unit (address / data / start / stop) once.
 * @retval HAL_OK: on success
 * @retval HAL_ERROR: on slave late-start, NACK, or clock-stretch timeout
 * @note   Sets the direction to TX and kicks. In interrupt mode returns
 *         immediately; in polling mode busy-waits for IICTBV while monitoring
 *         for NACK / clock-timeout. The done flag is not cleared when clock
 *         stretching is active.
 */
__sram__ HAL_StatusTypedef IicTxOnce(void)
{
  IICCON->IICTX = IIC_TX;

  /* kick start */
  if (IicKickStart() != HAL_OK)
    return HAL_ERROR;

  /* return if interrupt mode is enabled */
  if (IICCON->IICIE == HAL_ENABLE)
    return HAL_OK;

  while (IICCON->IICTBV == HAL_FLAG_INVALID)
  {
    /* check address is accepted or not */
    if (IicHandshakeCheck() != HAL_OK)
      return HAL_ERROR;

    if (IicClockTimeOutCheck() != HAL_OK)
      return HAL_ERROR;
  }

  /* we will not clear flag if clock stretching is enabled. */
  if ((IICCON->IICSCL == HAL_DISABLE) || (IICCON->IICS == IIC_MASTER))
    IICCON->IICTCLR = HAL_ENABLE;

  return HAL_OK;
}

/**
 * @brief  Receives a single IIC data byte once.
 * @retval HAL_OK: on success
 * @retval HAL_ERROR: on slave late-start or clock-stretch timeout
 * @note   Sets the direction to RX and kicks. In interrupt mode returns
 *         immediately; in polling mode busy-waits for IICRBV. The done flag
 *         is not cleared when clock stretching is active.
 */
__sram__ HAL_StatusTypedef IicRxOnce(void)
{
  IICCON->IICTX = IIC_RX;

  /* kick start */
  if (IicKickStart() != HAL_OK)
    return HAL_ERROR;

  /* return if interrupt mode is enabled */
  if (IICCON->IICIE == HAL_ENABLE)
    return HAL_OK;

  /* wait for data to be received */
  while (IICCON->IICRBV == HAL_FLAG_INVALID)
  {
    if (IicClockTimeOutCheck() != HAL_OK)
      return HAL_ERROR;
  }

  /* we will not clear flag if clock stretching is enabled. */
  if ((IICCON->IICSCL == HAL_DISABLE) || (IICCON->IICS == IIC_MASTER))
    IICCON->IICRCLR = HAL_ENABLE;

  return HAL_OK;
}

/**
 * @brief  Configures the data buffer / byte count for the next transfer.
 * @param  Packet: pointer to a HAL_IIC_Packettypedef structure, specifies the data packet.
 * @retval HAL_OK: on success
 * @retval HAL_ERROR: if Packet or Packet->Data is NULL
 * @note   Waits for any previous DMA transfer to finish, then loads the first
 *         byte into IICTBUF, the byte count into IICBC, and the buffer base
 *         address into IICDA.
 */
__sram__ HAL_StatusTypedef IicBufConfig(HAL_IIC_PacketTypedef *Packet)
{
  if (Packet == NULL || Packet->Data == NULL)
    return HAL_ERROR;

  /* wait previous packet ends */
  while (IICCON->IICDMAE == HAL_ENABLE)
    ;

  /* setup data buffer */
  WRITE_SR(IICTBUF, *Packet->Data);
  WRITE_SR(IICBC, Packet->Length);
  WRITE_SR(IICDA, (uint32_t)Packet->Data);
  return HAL_OK;
}

/**
 * @brief  Master mode sends a Start Condition.
 * @retval HAL_OK: on success
 * @retval HAL_ERROR: if the bus is not idle or a transfer step fails
 */
__sram__ HAL_StatusTypedef IicMasterStart(void)
{
  /* check bus state */
  if (IICCON->IICBSTA != IIC_BUS_IDLE)
    return HAL_ERROR;

  /* send start condition */
  IICCON->IICTXC = IIC_TX_START;

  return IicTxOnce();
}

/**
 * @brief  Slave mode start condition and address-match check.
 * @param  Packet: pointer to a HAL_IIC_Packettypedef structure, specifies the data packet.
 * @retval HAL_OK: on success (address matched, or pending in interrupt mode)
 * @retval HAL_ERROR: if Packet is NULL or clock-stretch timeout occurs
 * @note   In interrupt mode, returns immediately after enabling IICIE; the IRQ
 *         handler polls IICSAM. In polling mode, blocks until IICSAM is set,
 *         bounded only by clock-stretching timeouts (no dedicated counter).
 */
__sram__ HAL_StatusTypedef IicSlaveStart(HAL_IIC_PacketTypedef *Packet)
{
  if (Packet == NULL)
    return HAL_ERROR;

  /* setup address */
  IicSetAddress(Packet);

  /* wait bus idle */
  while (IICCON->IICBSTA != IIC_BUS_IDLE)
    ;

  /* setup slave mode, then it starts to match address */
  IICCON->IICS = IIC_SLAVE;

  if (Packet->IntE == HAL_ENABLE)
  {
    IICCON->IICIE = Packet->IntE;
    /* return immediately, the interrupt handler will check the address match */
    return HAL_OK;
  }

  /* blocking mode */
  while (1)
  {
    if (IicClockTimeOutCheck() != HAL_OK)
      return HAL_ERROR;

    /* check address match */
    if (IICCON->IICSAM == IIC_ADDR_MATCH)
      /* we do not clear IICRBV here in order to keep the clock stretching */
      return HAL_OK;
  }
}

/**
 * @brief  Master mode sends a Stop Condition.
 * @retval HAL_OK: on success
 * @retval HAL_ERROR: if the bus is not active or a transfer step fails
 */
__sram__ HAL_StatusTypedef IicMasterStop(void)
{
  /* check bus state */
  if (IICCON->IICBSTA != IIC_BUS_ACTIVE)
    return HAL_ERROR;

  /* send stop condition */
  IICCON->IICTXC = IIC_TX_STOP;

  return IicTxOnce();
}

/**
 * @brief  Master mode sends the device address.
 * @param  Packet: pointer to a HAL_IIC_Packettypedef structure, specifies the data packet.
 * @retval HAL_OK: on success
 * @retval HAL_ERROR: if Packet is NULL, the bus is not active, or a transfer step fails
 */
__sram__ HAL_StatusTypedef IicMasterAddress(HAL_IIC_PacketTypedef *Packet)
{
  /* check bus state */
  if (Packet == NULL || IICCON->IICBSTA != IIC_BUS_ACTIVE)
    return HAL_ERROR;

  /* send device address */
  IICCON->IICTXC = IIC_TX_ADDR;
  IicSetAddress(Packet);

  return IicTxOnce();
}

/**
 * @brief  Transfers data (read or write) in the current bus transaction.
 * @param  Packet: pointer to a HAL_IIC_Packettypedef structure, specifies the data packet.
 * @retval HAL_OK: on success
 * @retval HAL_ERROR: if Packet is NULL, the bus is not active, or a transfer step fails
 * @note   Enables DMA automatically when Packet->Length > 4 bytes. Direction is
 *         chosen from the read/write flag (IIC_READ/IIC_WRITE vs. IIC_MASTER/IIC_SLAVE).
 */
__sram__ HAL_StatusTypedef IicData(HAL_IIC_PacketTypedef *Packet)
{
  /* check bus state */
  if (Packet == NULL || IICCON->IICBSTA != IIC_BUS_ACTIVE)
    return HAL_ERROR;

  /* data transfer */
  IICCON->IICTXC = IIC_DATA;

  /* setup dma if length is greater than 4 bytes */
  if (Packet->Length > 4U)
  {
    if (HAL_DMA_Update(IICCON->IICDMAE, IicDmaCh, OC32_DMA_IIC, OC32_DMA_BUSY) != HAL_OK)
      return HAL_ERROR;
    /* setup dma */
    IICCON->IICDMAE = HAL_ENABLE;
  }

  if (Packet->RW == ((IICCON->IICS == IIC_MASTER) ? IIC_READ : IIC_WRITE))
  {
    return IicRxOnce();
  }
  else
  {
    return IicTxOnce();
  }
}

/**
 * @brief  Resets the IIC peripheral to an idle, non-interrupting state
 *         (private helper, not the public HAL_IIC_DeInit entry point).
 * @note   Clears the control-register fields, returns the DMA channel state to
 *         IDLE, and zeroes the module transfer state (IicTransState) and the
 *         in-flight packet pointer (IicCurPacket) so no dangling pointer is left
 *         after a completed transfer.
 */
__sram__ HAL_StatusTypedef IicDeInit(void)
{
  IICCON->IICS = IIC_MASTER;
  IICCON->IICIE = HAL_DISABLE;
  IICCON->IICDMAE = HAL_DISABLE;
  IICCON->IICRCLR = HAL_ENABLE;
  IICCON->IICTCLR = HAL_ENABLE;
  IICCON->IICHCLR = HAL_ENABLE;
  IICCON->IICCCLR = HAL_ENABLE;
  IicTransState = IIC_TRANS_IDLE;
  /* drop the completed transfer's packet pointer so a later stray interrupt
     sees NULL (guarded in HAL_IIC_IRQHandler) instead of a stale pointer */
  IicCurPacket = NULL;
  return HAL_DMA_Update(IICCON->IICDMAE, IicDmaCh, OC32_DMA_IIC, OC32_DMA_IDLE);
}

/**
 * @brief  Advances the IIC transfer state machine by one step.
 * @param  Packet: pointer to a HAL_IIC_Packettypedef structure, specifies the data packet.
 * @retval HAL_OK: on success
 * @retval HAL_ERROR: if a step fails or the state is unrecognized
 * @note   Each call executes exactly one state (IDLE -> START -> ADDR -> DATA ->
 *         STOP -> IDLE). In interrupt mode it is invoked once per IIC interrupt
 *         (via HAL_IIC_IRQHandler); in polling mode HAL_IIC_Transfer loops it
 *         until IicTransState returns to IDLE.
 */
__sram__ HAL_StatusTypedef IicTransStm(HAL_IIC_PacketTypedef *Packet)
{
  HAL_StatusTypedef status = HAL_OK;

  switch (IicTransState)
  {
  case IIC_TRANS_IDLE:
    /* setup data buffer */
    status = IicBufConfig(Packet);
    IicTransState = IIC_TRANS_START;
    break;

  case IIC_TRANS_START:
    /* send start condition */
    status = (IICCON->IICS == IIC_SLAVE) ? IicSlaveStart(Packet) : IicMasterStart();
    /* slavestart including address match, thus we can directly transfer data */
    IicTransState = (IICCON->IICS == IIC_SLAVE) ? IIC_TRANS_DATA : IIC_TRANS_ADDR;
    break;

  case IIC_TRANS_ADDR:
    /* send device address */
    status = IicMasterAddress(Packet);
    IicTransState = IIC_TRANS_DATA;
    break;

  case IIC_TRANS_DATA:
    /* data transfer */
    status = IicData(Packet);
    IicTransState = IIC_TRANS_STOP;
    break;

  case IIC_TRANS_STOP:
    /* master mode send stop condition, skip it for slave mode */
    status = (IICCON->IICS == IIC_SLAVE) ? HAL_OK : IicMasterStop();
    /* transfer done, save rx data
      - if dma is disabled(Length <= 4 bytes), then we load IICRBUF to Packet->Data
      - if dma is enabled, rx data is already in Packet->Data */
    if ((IICCON->IICDMAE == HAL_DISABLE) && (IICCON->IICTX == IIC_RX))
      Packet->Data[0] = READ_SR(IICRBUF);

    IicDeInit();
    break;

  default:
    status = HAL_ERROR;
    break;
  }

  return status;
}

/* Private functions ---------------------------------------------------------*/

/**
 * @brief  Initializes the IIC peripheral according to the specified parameters.
 * @param  Init: pointer to a HAL_IIC_Inittypedef structure.
 * @retval HAL_OK: on success
 * @retval HAL_ERROR: if Init is NULL
 */
HAL_StatusTypedef HAL_IIC_Init(HAL_IIC_InitTypedef *Init)
{
  if (Init == NULL)
    return HAL_ERROR;

  /* record dma channel for iic (persists for the life of the peripheral;
     IicData() / IicDeInit() use it to drive HAL_DMA_Update) */
  IicDmaCh = Init->DmaCh;

  /* default is master mode */
  IICCON->IICS = IIC_MASTER;

  /* setup address mode */
  IICCON->IICHE = Init->AddrMode;

  /* setup master clock divider */
  IICCON->IICMCD = Init->ClkDiv;

  /* setup clock stretching (slave only) */
  IICCON->IICSCL = Init->Stretch;

  /* setup pins */
  IICCON->IICPORTS = Init->Pins;

  /* enable module */
  IICCON->IICE = HAL_ENABLE;
  IicTransState = IIC_TRANS_IDLE;

  return HAL_OK;
}

/**
 * @brief  DeInitializes the IIC peripheral (public entry point).
 * @note   Writes 0 to IICCON to disable the module, then calls the private
 *         IicDeInit() to reset state fields and release the DMA channel.
 */
HAL_StatusTypedef HAL_IIC_DeInit(void)
{
  WRITE_SR(IICCON, 0U);
  return IicDeInit();
}

/**
 * @brief  Transfer Tx/Rx packet
 * @param  Packet: pointer to a HAL_IIC_Packettypedef structure, specifies the data packet
 * @retval HAL_OK: on success
 * @retval HAL_ERROR: if Packet is NULL, bus is not idle, or a transfer step fails
 * @note   Master-oriented transfer. In interrupt mode (Packet->IntE != 0), starts
 *         the state machine and returns immediately; completion is handled by
 *         HAL_IIC_IRQHandler. In polling mode, blocks until IicTransState reaches
 *         IDLE. Note that caller's Packet and its ->Data buffer must remain valid
 *         (module scope / static buffer, not a stack local that dies on return)
 *         until the transfer completes.
 */
__sram__ HAL_StatusTypedef HAL_IIC_Transfer(HAL_IIC_PacketTypedef *Packet)
{
  if (Packet == NULL)
    return HAL_ERROR;

  /* record this packet so HAL_IIC_IRQHandler can continue stepping the state
     machine with the SAME packet/queue the main loop set up */
  IicCurPacket = Packet;

  /* setup master mode */
  IICCON->IICS = IIC_MASTER;

  /* setup interrupt enable */
  IICCON->IICIE = Packet->IntE;

  if (Packet->IntE == HAL_ENABLE)
  {
    /* return immediately, the interrupt handler will call it step by step */
    return IicTransStm(IicCurPacket);
  }
  else
  {
    /* blocking mode */
    if (IicTransStm(IicCurPacket) != HAL_OK)
      return HAL_ERROR;
    while (IicTransState != IIC_TRANS_IDLE)
    {
      if (IicTransStm(IicCurPacket) != HAL_OK)
        return HAL_ERROR;
    }
    return HAL_OK;
  }
}

/**
 * @brief  Handles IIC interrupt request at user side.
 * @retval HAL_OK: on success
 * @note   This is a weak implementation that can be overridden by the user.
 */
__weak__ __sram__ HAL_StatusTypedef HAL_IIC_UserIRQ(void)
{
  return HAL_OK;
}

/**
 * @brief  Handles IIC interrupt request.
 * @retval HAL_OK: on success
 * @note   Invoked from the global interrupt handler
 */
__sram__ HAL_StatusTypedef HAL_IIC_IRQHandler(void)
{
  /* check error flags */
  if (IicHandshakeCheck() != HAL_OK)
    return HAL_ERROR;

  if (IicClockTimeOutCheck() != HAL_OK)
    return HAL_ERROR;

  /* resume stepping the state machine with the same packet the main loop
     started (IicCurPacket, set by HAL_IIC_Transfer). Defensive guard: a stray
     interrupt with no in-flight transfer must not dereference NULL. */
  if (IicCurPacket == NULL)
    return HAL_ERROR;

  if (IicTransStm(IicCurPacket) != HAL_OK)
    return HAL_ERROR;

  return HAL_IIC_UserIRQ();
}

#endif /* HAL_IIC_ENABLE */
