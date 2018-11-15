/*
 * Copyright (c) 2016, Sollae Systems. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * - Redistributions of source code must retain the above copyright
 *   notice, this list of conditions and the following disclaimer.
 *
 * - Redistributions in binary form must reproduce the above copyright
 *   notice, this list of conditions and the following disclaimer in the
 *   documentation and/or other materials provided with the distribution.
 *
 * - Neither the name of the Sollae Systems nor the names of its
 *   contributors may be used to endorse or promote products derived from
 *   this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY SOLLAE SYSTEMS "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL SOLLAE SYSTEMS BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include <Sppc.h>
#include <PhpocExpansion.h>

#define SPI0C_TXBUF_SIZE 512 /* should be same as spi0c_txbuf_size in phpoc.ini */

static char *zero_string = "";

char PhpocExpansion::csv_buf[CSV_BUF_SIZE];
char *PhpocExpansion::respv[CSV_MAX_VALUE];
uint8_t PhpocExpansion::respl[CSV_MAX_VALUE];
uint8_t PhpocExpansion::respc;

int PhpocExpansion::csv2respv(void)
{
	char *csv, *end_ptr;
	int i;

	csv = csv_buf;

	for(i = 0; i < CSV_MAX_VALUE; i++)
	{
		respv[i] = zero_string;
		respl[i] = 0;
	}

	respc = 0;

	while(respc < CSV_MAX_VALUE)
	{
		/* ltrim */
		while(*csv == ' ')
			csv++;

		if(!*csv)
			break;

		respv[respc] = csv;

		while(*csv && (*csv != ','))
			csv++;

		respl[respc] = csv - respv[respc];

		end_ptr = csv;

		if(*csv == ',')
		{
			*csv = 0x00;
			csv++;
		}

		/* rtrim */
		while(respv[respc] < end_ptr)
		{
			end_ptr--;

			if(*end_ptr == ' ')
			{
				*end_ptr = 0x00;
				respl[respc]--;
			}
			else
				break;
		}

		respc++;
	}

	return respc;
}

int PhpocExpansion::spc0_get_error(void)
{
	int spc_error;

	spc_error = Sppc.command(F("spc0 ioctl get error"));

	if(!spc_error)
		return 0;

#ifdef PF_LOG_SPI
	if((Sppc.flags & PF_LOG_SPI) && Serial)
	{
		sppc_printf(F("log> expansion: "));

		if(Sppc.command(F("spc0 ioctl get error sto")))
			sppc_printf(F("timeout\r\n"));
		else
			sppc_printf(F("request error\r\n"));
		/*
		if(Sppc.command(F("spc0 ioctl get error mbit")))
			sppc_printf(F("M-bit error\r\n"));
		else
		if(Sppc.command(F("spc0 ioctl get error csum")))
			sppc_printf(F("csum error\r\n"));
		else
		if(Sppc.command(F("spc0 ioctl get error urg")))
			sppc_printf(F("U-bit error\r\n"));
		else
		if(Sppc.command(F("spc0 ioctl get error sid")))
			sppc_printf(F("sid mismatch\r\n"));
		else
		if(Sppc.command(F("spc0 ioctl get error addr")))
			sppc_printf(F("addr mismatch\r\n"));
		*/
	}
#endif

	return spc_error;
}

void PhpocExpansion::expansion_begin(void)
{
	Sppc.errno = 0;

	if(!(Sppc.flags & PF_EXPANSION))
	{
/*
#ifdef PF_LOG_SPI
		if((Sppc.flags & PF_LOG_SPI) && Serial)
			Serial.println(F("log> expansion not initialized"));
#endif
*/
		Sppc.errno = EPERM;
		return;
	}
}

int PhpocExpansion::expansion_request(uint8_t sid, uint8_t addr, const char *wbuf, int wlen)
{
	uint16_t status;
	int len;

	expansion_begin();
	if(Sppc.errno)
		return 0;

	Sppc.write(wbuf, wlen);
	if(Sppc.errno)
		return 0;

	Sppc.command(F("spc0 write"));
	if(Sppc.errno)
		return 0;

	Sppc.command(F("spc0 ioctl spc %d %d"), sid, addr);
	if(Sppc.errno)
		return 0;

	while(Sppc.command(F("spc0 ioctl get state")))
	{
		if(Sppc.errno)
			return 0;
		delay(1);
	}

	if(spc0_get_error()) /* check & log master-slave-transaction error */
		return 0;
	if(Sppc.errno)
		return 0;

	Sppc.command(F("spc0 read %d"), SPI0C_TXBUF_SIZE - 2);
	if(Sppc.errno)
		return 0;

	len = Sppc.read(csv_buf, CSV_BUF_SIZE - 2);
	if(Sppc.errno)
		return 0;
	csv_buf[len] = 0x00;

	csv2respv();

	if(!respc)
		return 0;

	status = atoi(respv[0]);

#ifdef PF_LOG_SPI
	if(status != SPC_MSG_CMD_OK)
	{
		if((Sppc.flags & PF_LOG_SPI) && Serial)
			sppc_printf(F("log> expansion: error %d\r\n"), status);
	}
#endif

	return status;
}

int PhpocExpansion::expansion_read(uint8_t sid, uint8_t addr, const char *rbuf, int rlen)
{
	char rlen_str[8];

	expansion_begin();
	if(Sppc.errno)
		return 0;

	if(rlen > SPI0C_TXBUF_SIZE - 2)
		rlen = SPI0C_TXBUF_SIZE - 2;

	sppc_sprintf(rlen_str, F("%d"), rlen);

	Sppc.write(rlen_str);

	Sppc.command(F("spc0 write"));
	if(Sppc.errno)
		return 0;

	Sppc.command(F("spc0 ioctl spc %d %d"), sid, addr);
	if(Sppc.errno)
		return 0;

	while(Sppc.command(F("spc0 ioctl get state")))
	{
		if(Sppc.errno)
			return 0;
		delay(1);
	}

	if(spc0_get_error()) /* check & log master-slave-transaction error */
		return 0;
	if(Sppc.errno)
		return 0;

	Sppc.command(F("spc0 read %d"), rlen);
	if(Sppc.errno)
		return 0;

	rlen = Sppc.read(rbuf, rlen);
	if(Sppc.errno)
		return 0;

	return rlen;
}

int PhpocExpansion::expansion_write(uint8_t sid, uint8_t addr, const char *wbuf, int wlen)
{
	expansion_begin();
	if(Sppc.errno)
		return 0;

	Sppc.write(wbuf, wlen);

	Sppc.command(F("spc0 write"));
	if(Sppc.errno)
		return 0;

	Sppc.command(F("spc0 ioctl spc %d %d"), sid, addr);
	if(Sppc.errno)
		return 0;

	while(Sppc.command(F("spc0 ioctl get state")))
	{
		if(Sppc.errno)
			return 0;
		delay(1);
	}

	if(spc0_get_error()) /* check & log master-slave-transaction error */
		return 0;
	if(Sppc.errno)
		return 0;

	return wlen;
}

int PhpocExpansion::command(uint8_t addr, const __FlashStringHelper *format, ...)
{
	char vsp_buf[VSP_COUNT_LIMIT + 2/*CRLF*/];
	va_list args;
	int cmd_len;

	va_start(args, format);
	cmd_len = sppc_vsprintf(vsp_buf, format, args);
	va_end(args);

	return expansion_request(spc_id, addr, vsp_buf, cmd_len);
}

int PhpocExpansion::command(uint8_t addr, const char *format, ...)
{
	char vsp_buf[VSP_COUNT_LIMIT + 2/*CRLF*/];
	va_list args;
	int cmd_len;

	va_start(args, format);
	cmd_len = sppc_vsprintf(vsp_buf, format, args);
	va_end(args);

	return expansion_request(spc_id, addr, vsp_buf, cmd_len);
}

int PhpocExpansion::command(const __FlashStringHelper *format, ...)
{
	char vsp_buf[VSP_COUNT_LIMIT + 2/*CRLF*/];
	va_list args;
	int cmd_len;

	va_start(args, format);
	cmd_len = sppc_vsprintf(vsp_buf, format, args);
	va_end(args);

	return expansion_request(spc_id, SPC_UADDR_DEV, vsp_buf, cmd_len);
}

int PhpocExpansion::command(const char *format, ...)
{
	char vsp_buf[VSP_COUNT_LIMIT + 2/*CRLF*/];
	va_list args;
	int cmd_len;

	va_start(args, format);
	cmd_len = sppc_vsprintf(vsp_buf, format, args);
	va_end(args);

	return expansion_request(spc_id, SPC_UADDR_DEV, vsp_buf, cmd_len);
}

int PhpocExpansion::getPID(void)
{
	int offset;

	if(command(SPC_UADDR_SYS, F("get did")) == SPC_MSG_CMD_OK)
	{
		offset = strlen(respv[1]);

		if(offset > 4)
			offset -= 4;
		else
			offset = 0;

		return atoi(respv[1] + offset);
	}
	else
		return 0;
}

char *PhpocExpansion::getName(void)
{
	if(command(SPC_UADDR_SYS, F("get did")) == SPC_MSG_CMD_OK)
		return respv[2];
	else
		return zero_string;
}

int PhpocExpansion::begin(long baud)
{
	if(!(Sppc.flags & PF_SYNC_V2))
	{
		Serial.println(F("log> expansion_begin: Phpoc/Sppc does not begin"));
		Sppc.errno = EPERM;
		return 0;
	}

	if(!(Sppc.flags & PF_SPC))
	{
#ifdef PF_LOG_SPI
		if((Sppc.flags & PF_LOG_SPI) && Serial)
			Serial.println(F("log> expansion_begin: shield doesn't support expansion"));
#endif
		Sppc.errno = EPERM;
		return 0;
	}

#ifdef PF_LOG_NET
	if((Sppc.flags & PF_LOG_NET) && Serial)
		Serial.print(F("log> expansion_begin: reset "));
#endif

	Sppc.command(F("spc0 ioctl reset 10 500"));
	if(Sppc.errno)
		return 0;

	while(Sppc.command(F("spc0 ioctl get state")))
	{
		if(Sppc.errno)
			return 0;
		delay(1);
	}


#ifdef PF_LOG_NET
	if((Sppc.flags & PF_LOG_NET) && Serial)
		Serial.print(F("sync "));
#endif

	Sppc.command(F("spc0 ioctl set baud %ld"), baud);
	if(Sppc.errno)
		return 0;

	Sppc.command(F("spc0 ioctl sync baud 1 100"));
	if(Sppc.errno)
		return 0;

	while(Sppc.command(F("spc0 ioctl get state")))
	{
		if(Sppc.errno)
			return 0;
		delay(1);
	}

#ifdef PF_LOG_NET
	if((Sppc.flags & PF_LOG_NET) && Serial)
		Serial.println(F("success "));
#endif

	Sppc.flags |= PF_EXPANSION;

	return 1;
}

PhpocExpansion Expansion;

