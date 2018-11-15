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

#ifndef PhpocExpansion_h
#define PhpocExpansion_h

#define SPC_MSG_CMD_OK  200
#define SPC_MSG_BAD_CMD 300
#define SPC_MSG_EINVAL  301
#define SPC_MSG_EPERM   320
#define SPC_MSG_EBUSY   321

#define SPC_UADDR_SYS   0x00 /* unicast system request */
#define SPC_UADDR_DEV   0x04 /* unicast device request */
#define SPC_UADDR_READ  0x06 /* master read from slave UART RX */
#define SPC_UADDR_WRITE 0x07 /* master write to slave UART TX */
#define SPC_UADDR_PEEK  0x08 /* master peek from slave UART RX */

#define CSV_MAX_VALUE 6
#define CSV_BUF_SIZE  80

class PhpocExpansion
{
	private:
		int  csv2respv(void);
		int  spc0_get_error(void);
		void expansion_begin(void);
		int  expansion_request(uint8_t sid, uint8_t addr, const char *wbuf, int wlen);

	protected:
		int expansion_read(uint8_t sid, uint8_t addr, const char *rbuf, int rlen);
		int expansion_write(uint8_t sid, uint8_t addr, const char *wbuf, int wlen);

	protected:
		static char csv_buf[CSV_BUF_SIZE];
		static char *respv[CSV_MAX_VALUE];
		static uint8_t respl[CSV_MAX_VALUE];
		static uint8_t respc;

	protected:
		uint8_t spc_id;

	public:
		int command(uint8_t addr, const __FlashStringHelper *format, ...);
		int command(uint8_t addr, const char *format, ...);
		int command(const __FlashStringHelper *format, ...);
		int command(const char *format, ...);
		//int scanDevice();
		//int findDevice();
		int  getPID(void);
		//int  getGID(void);
		char *getName(void);
		int  readInt(void) { return atoi(respv[1]); }
		long readLong(void) { return atol(respv[1]); }
		int  begin(long baud = 115200);
};

class ExpansionRelayOutput : public PhpocExpansion
{
	private:
		uint8_t out_port;

	public:
		ExpansionRelayOutput(uint8_t sid, uint8_t port) { spc_id = sid; out_port = port; }
		void on(void) { command(F("set %d output high"), out_port); }
		void off(void) { command(F("set %d output low"), out_port); }
		bool isOn(void) { command(F("get %d output"), out_port); return readInt() ? true : false; }
		bool isOff(void) { command(F("get %d output"), out_port); return readInt() ? false : true; }
		void setDelay(int ms) { command(F("set %d delay %d"), out_port, ms); };
};

class ExpansionPhotoInput : public PhpocExpansion
{
	private:
		uint8_t in_port;

	public:
		ExpansionPhotoInput(uint8_t sid, uint8_t port) { spc_id = sid; in_port = port; }
		bool isOn(void) { command(F("get %d input"), in_port); return readInt() ? true : false; }
		bool isOff(void) { command(F("get %d input"), in_port); return readInt() ? false : true; }
		void setDelay(int ms) { command(F("set %d delay %d"), in_port, ms); };
};

#define DCM_DECAY_SLOW 0
#define DCM_DECAY_FAST 1

class ExpansionDCMotor : public PhpocExpansion
{
	private:
		uint8_t dcm_port;

	public:
		ExpansionDCMotor(uint8_t sid, uint8_t port) { spc_id = sid; dcm_port = port; }
		void setPolarity(int pol)
		{ pol >= 0 ? command(F("dc%d pwm set pol +"), dcm_port) :
		             command(F("dc%d pwm set pol -"), dcm_port); }
		void setDirection(int dir)
		{ dir >= 0 ? command(F("dc%d pwm set dir +"), dcm_port) :
		             command(F("dc%d pwm set dir -"), dcm_port); }
		void setPeriod(long period) { command(F("dc%d pwm set period %ld"), dcm_port, period); }
		void setWidth(long width) { command(F("dc%d pwm set width %ld"), dcm_port, width); }
		void setDecay(int decay)
		{ decay ? command(F("dc%d pwm set decay fast"), dcm_port) :
		          command(F("dc%d pwm set decay slow"), dcm_port); }
		void setEncoderPolarity(int pol)
		{ pol >= 0 ? command(F("dc%d enc set pol +"), dcm_port) :
		             command(F("dc%d enc set pol -"), dcm_port); }
		void setEncoderPosition(long pos) { command(F("dc%d enc set pos %ld"), dcm_port, pos); }
		void setEncoderPSR(int psr) { command(F("dc%d enc set psr %d"), dcm_port, psr); }
		long getEncoderPosition(void) { command(F("dc%d enc get pos"), dcm_port); return readLong(); }
		long getEncoderPeriod(void) { command(F("dc%d enc get period"), dcm_port); return readLong(); }
		void setFilterFrequency(long freq) { command(F("dc%d lpf set freq %ld"), dcm_port, freq); }
		void setFilterPNC(int pnc) { command(F("dc%d lpf set pnc %d"), dcm_port, pnc); }
		long getFilterPNC(void) { command(F("dc%d lpf get pnc"), dcm_port); return readLong(); }
};

#define EIO_MODE_INPUT 0
#define EIO_MODE_LOCK  1

class ExpansionStepper : public PhpocExpansion
{
	public:
		ExpansionStepper(uint8_t sid) { spc_id = sid; }
		void reset(void) { command(F("reset")); }
		void unlock(void) { command(F("unlock")); }
		void setMode(uint8_t mode) { command(F("set mode %d"), mode); }
		void setVrefStop(uint8_t vref) { command(F("set vref stop %d"), vref); }
		void setVrefDrive(uint8_t vref) { command(F("set vref drive %d"), vref); }
		void setVrefLock(uint8_t vref) { command(F("set vref lock %d"), vref); }
		void setResonance(int low, int high) { command(F("set rsnc %d %d"), low, high); }
		void setSpeed(long speed) { command(F("set speed %ld"), speed); }
		void setAccel(long accel) { command(F("set accel %ld"), accel); }
		void setAccel(long accel, long decel) { command(F("set accel %ld %ld"), accel, decel); }
		void setPosition(long pos) { command(F("set pos %ld"), pos); }
		int  getState(void) { command(F("get state")); return readInt(); }
		long getPosition(void) { command(F("get pos")); return readLong(); }
		void stepMove(long step) { command(F("move %ld"), step); while(getState()); }
		void stepMove(long step, long speed)
		{ command(F("move %ld %ld"), step, speed); while(getState()); }
		void stepMove(long step, long speed, long accel)
		{ command(F("move %ld %ld %ld"), step, speed, accel); while(getState()); }
		void stepGoto(long pos) { command(F("goto %ld"), pos); }
		void stepGoto(long pos, long speed) { command(F("goto %ld %ld"), pos, speed); }
		void stepGoto(long pos, long speed, long accel)
		{ command(F("goto %ld %ld %ld"), pos, speed, accel); }
		void stepGotoSW(int id, int dir)
		{ dir >=0 ? command(F("goto +sw%d"), id) : command(F("goto -sw%d"), id); }
		void stepGotoSW(int id, int dir, long speed)
		{ dir >=0 ? command(F("goto +sw%d %ld"), id, speed) :
		            command(F("goto -sw%d %ld"), id, speed); }
		void stepGotoSW(int id, int dir, long speed, long accel)
		{ dir >=0 ? command(F("goto +sw%d %ld %ld"), id, speed, accel) :
		            command(F("goto -sw%d %ld %ld"), id, speed, accel); }
		void stop(long decel = -1)
		{ decel < 0 ? command(F("stop")) : command(F("stop %ld"), decel); }
		void setEioMode(int id, int mode)
		{ mode ? command(F("eio set %d mode lock"), id) : command(F("eio set %d mode input"), id); }
		int  getEio(int id) { command(F("eio get %d input"), id); return readInt(); }
};

class ExpansionSerial : public PhpocExpansion, public Stream
{
	public:
		ExpansionSerial(uint8_t sid) { spc_id = sid; }
		void begin(const __FlashStringHelper *sets) { command(F("set uart %S"), sets); }
		void begin(const char *sets) { command(F("set uart %s"), sets); }
		void begin(long baud) { command(F("set uart %ld"), baud); }
    virtual int available(void) { command(F("get rxlen")); return readInt(); }
    virtual int peek(void) { char ch; return expansion_read(spc_id, SPC_UADDR_PEEK, &ch, 1) ? ch : -1; }
    virtual int read(void) { char ch; return expansion_read(spc_id, SPC_UADDR_READ, &ch, 1) ? ch : -1; }
		int availableForWrite(void) { command(F("get txfree")); return readInt(); }
    virtual void flush(void) { while(1){ command(F("get txlen")); if(!readInt())break; delay(10); } }
		virtual size_t write(uint8_t byte) { return expansion_write(spc_id, SPC_UADDR_WRITE, &byte, 1); }
		virtual size_t write(const uint8_t *wbuf, size_t wlen) { return expansion_write(spc_id, 7, wbuf, wlen); }
		using Print::write;
		operator bool() { return true; }
		// extended command list
		// reset
		// set modem BB
		// set modem rts/dtr B
		// set count rx/tx/rf/tf/pe/fe/oe/be/rbo/rfo/tbo/tfo n
		// set ifg bits
		// set ifd del
		// set ifd start_del end_del
		// set ifd
		// set txdelay bits
		// set break n[us]
		// get uart
		// get modem [ri/cts/rts/dsr/dtr/cd]
		// get count rx/tx/rf/tf/pe/fe/oe/be/rbo/rfo/tbo/tfo
		// get rxlen [del]
		// get txlen
		// get txfree
		// get rxbuf
		// get txbuf
		// get ifg
		// get ifd
		// get txdelay
};

/* PhpocExpansion.cpp */
extern PhpocExpansion Expansion;

#endif
