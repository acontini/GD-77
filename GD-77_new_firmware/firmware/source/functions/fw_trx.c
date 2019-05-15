/*
 * Copyright (C)2019 Kai Ludwig, DG4KLU
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *   1. Redistributions of source code must retain the above copyright notice,
 *      this list of conditions and the following disclaimer.
 *   2. Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in the
 *      documentation and/or other materials provided with the distribution.
 *   3. The name of the author may not be used to endorse or promote products
 *      derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
 * EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "fw_trx.h"

bool open_squelch=false;
bool HR_C6000_datalogging=false;

void trx_set_mode_band_freq_and_others()
{
	uint32_t f = current_frequency*1.6f;
	uint8_t fl_l = (f & 0x000000ff) >> 0;
	uint8_t fl_h = (f & 0x0000ff00) >> 8;
	uint8_t fh_l = (f & 0x00ff0000) >> 16;
	uint8_t fh_h = (f & 0xff000000) >> 24;

	uint8_t squelch = 0x00;
	if ((current_mode == MODE_ANALOG) && (!open_squelch))
	{
		squelch = 0x08;
	}

	write_I2C_reg_2byte(I2C_MASTER_SLAVE_ADDR_7BIT, 0x30, 0x40, 0x06 | squelch); // RX off
	write_I2C_reg_2byte(I2C_MASTER_SLAVE_ADDR_7BIT, 0x05, 0x87, 0x63); // select 'normal' frequency mode
	write_I2C_reg_2byte(I2C_MASTER_SLAVE_ADDR_7BIT, 0x29, fh_h, fh_l);
	write_I2C_reg_2byte(I2C_MASTER_SLAVE_ADDR_7BIT, 0x2a, fl_h, fl_l);
	write_I2C_reg_2byte(I2C_MASTER_SLAVE_ADDR_7BIT, 0x49, 0x0C, 0x15); // setting SQ open and shut threshold
	write_I2C_reg_2byte(I2C_MASTER_SLAVE_ADDR_7BIT, 0x30, 0x40, 0x26 | squelch); // RX on

	if (current_mode == MODE_SILENT)
	{
		write_I2C_reg_2byte(I2C_MASTER_SLAVE_ADDR_7BIT, 0x44, 0x06, 0x00); // set internal volume to 0%
	    GPIO_PinWrite(GPIO_speaker_mute, Pin_speaker_mute, 0); // speaker off
	    GPIO_PinWrite(GPIO_RX_audio_mux, Pin_RX_audio_mux, 0); // connect AT1846S audio to HR_C6000
		GPIO_PinWrite(GPIO_VHF_RX_amp_power, Pin_VHF_RX_amp_power, 0);
		GPIO_PinWrite(GPIO_UHF_RX_amp_power, Pin_UHF_RX_amp_power, 0);
		terminate_sound();
	    terminate_digital();
	}
	else
	{
		if (current_mode == MODE_ANALOG)
		{
			write_I2C_reg_2byte(I2C_MASTER_SLAVE_ADDR_7BIT, 0x44, 0x06, 0x80); // set internal volume to 50%
		    GPIO_PinWrite(GPIO_speaker_mute, Pin_speaker_mute, 1); // speaker on
		    GPIO_PinWrite(GPIO_RX_audio_mux, Pin_RX_audio_mux, 1); // connect AT1846S audio to speaker
			terminate_sound();
		    terminate_digital();
		}
		else if (current_mode == MODE_DIGITAL)
		{
			write_I2C_reg_2byte(I2C_MASTER_SLAVE_ADDR_7BIT, 0x44, 0x06, 0xCC); // set internal volume to 80%
		    GPIO_PinWrite(GPIO_speaker_mute, Pin_speaker_mute, 1); // speaker on
		    GPIO_PinWrite(GPIO_RX_audio_mux, Pin_RX_audio_mux, 0); // connect AT1846S audio to HR_C6000
		    init_sound();
		    init_digital();
		}
		if (current_band == BAND_VHF)
		{
			GPIO_PinWrite(GPIO_VHF_RX_amp_power, Pin_VHF_RX_amp_power, 1);
			GPIO_PinWrite(GPIO_UHF_RX_amp_power, Pin_UHF_RX_amp_power, 0);
		}
		else if (current_band == BAND_UHF)
		{
			GPIO_PinWrite(GPIO_VHF_RX_amp_power, Pin_VHF_RX_amp_power, 0);
			GPIO_PinWrite(GPIO_UHF_RX_amp_power, Pin_UHF_RX_amp_power, 1);
		}
	}
}
