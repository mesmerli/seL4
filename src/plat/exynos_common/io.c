/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * This software may be distributed and modified according to the terms of
 * the GNU General Public License version 2. Note that NO WARRANTY is provided.
 * See "LICENSE_GPLv2.txt" for details.
 *
 * @TAG(GD_GPL)
 */

#include <config.h>
#include <stdint.h>
#include <util.h>
#include <machine/io.h>
#include <plat/machine/devices.h>

#define ULCON       0x0000 /* line control */
#define UCON        0x0004 /* control */
#define UFCON       0x0008 /* fifo control */
#define UMCON       0x000C /* modem control */
#define UTRSTAT     0x0010 /* TX/RX status */
#define UERSTAT     0x0014 /* RX error status */
#define UFSTAT      0x0018 /* FIFO status */
#define UMSTAT      0x001C /* modem status */
#define UTXH        0x0020 /* TX buffer */
#define URXH        0x0024 /* RX buffer */
#define UBRDIV      0x0028 /* baud rate divisor */
#define UFRACVAL    0x002C /* divisor fractional value */
#define UINTP       0x0030 /* interrupt pending */
#define UINTSP      0x0034 /* interrupt source pending */
#define UINTM       0x0038 /* interrupt mask */

#define UART_REG(X) ((volatile uint32_t *)(UART_PPTR + (X)))

/* UTRSTAT */
#define TX_EMPTY        BIT(2)
#define TXBUF_EMPTY     BIT(1)
#define RXBUF_READY     BIT(0)

#ifdef CONFIG_PRINTING

void
exynos_uart_putchar(char c)
{
    putDebugChar(c);
    if (c == '\n') {
        putDebugChar('\r');
    }
}

#endif

#if defined(CONFIG_DEBUG_BUILD) || defined(CONFIG_PRINTING)

void
putDebugChar(unsigned char c)
{
    while ( (*UART_REG(UTRSTAT) & TXBUF_EMPTY) == 0 );
    *UART_REG(UTXH) = (c & 0xff);
}

#endif

#ifdef CONFIG_DEBUG_BUILD

unsigned char
getDebugChar(void)
{
    if ( (*UART_REG(UTRSTAT) & RXBUF_READY)) {
        return (unsigned char) * UART_REG(URXH);
    } else {
        return -1;
    }
}


#endif /* CONFIG_DEBUG_BUILD */
