/* s3emul - "secu3 emulator"
   Copyright (C) 2016 andreika. Ukraine, Kiev

   The following code is a derivative work of the code from the SECU-3 project, 
   which is licensed GPLv3. This code therefore is also licensed under the terms 
   of the GNU Public License, verison 3.

   PORTIONS OF THIS CODE TAKEN FROM:

   SECU-3  - An open source, free engine control unit
   Copyright (C) 2007 Alexey A. Shabelnikov. Ukraine, Kiev

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.

   contacts:
              http://secu-3.org
              email: shabelnikov@secu-3.org
*/

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "s3.h"
#include "s3emul.h"

static PPEepromParam m_ep = { 0 };
static PPFlashParam m_fp = { 0 };

static char *platformNames[] = {"Atmega16","Atmega32","Atmega64","Atmega128","Atmega644",};

void fillMainFwParams(EECUPlatform i_platform, PPEepromParam &m_ep, PPFlashParam &m_fp)
{
    switch (i_platform) //initialize data depending on platform
    {
    case EP_ATMEGA16:
        //flash
        m_fp.m_page_size = 128;
        m_fp.m_total_size = 16384;
        m_fp.m_page_count = m_fp.m_total_size / m_fp.m_page_size;
        m_fp.m_bl_section_size = 512; //256 words
        m_fp.m_app_section_size = m_fp.m_total_size - m_fp.m_bl_section_size;
        //eeprom
        m_ep.m_size = 512;
        m_ep.m_platform_id = EP_ATMEGA16;
        //MCU clock frequency
        m_fp.m_fcpu_hz = 16000000;
        //Magic number
        strncpy(m_fp.m_magic, "16  ", PLATFORM_MN_SIZE);
        m_fp.m_platform_id = EP_ATMEGA16;
        break;

    case EP_ATMEGA32:
        m_fp.m_page_size = 128;
        m_fp.m_total_size = 32768;
        m_fp.m_page_count = m_fp.m_total_size / m_fp.m_page_size;
        m_fp.m_bl_section_size = 1024; //512 words
        m_fp.m_app_section_size = m_fp.m_total_size - m_fp.m_bl_section_size;
        //eeprom
        m_ep.m_size = 1024;
        m_ep.m_platform_id = EP_ATMEGA32;
        //MCU clock frequency
        m_fp.m_fcpu_hz = 16000000;
        //Magic number
        strncpy(m_fp.m_magic, "32  ", PLATFORM_MN_SIZE);
        m_fp.m_platform_id = EP_ATMEGA32;
        break;

        //ATmega644 and ATmega64 have same FLASH and EEPROM parameters
    case EP_ATMEGA644:
        m_fp.m_page_size = 256;
        m_fp.m_total_size = 65536;
        m_fp.m_page_count = m_fp.m_total_size / m_fp.m_page_size;
        m_fp.m_bl_section_size = 2048; //1024 words
        m_fp.m_app_section_size = m_fp.m_total_size - m_fp.m_bl_section_size;
        //eeprom
        m_ep.m_size = 2048;
        m_ep.m_platform_id = EP_ATMEGA644;
        //MCU clock frequency
        m_fp.m_fcpu_hz = 20000000;
        //Magic number
        strncpy(m_fp.m_magic, "644 ", PLATFORM_MN_SIZE);
        m_fp.m_platform_id = EP_ATMEGA644;
        break;

    case EP_ATMEGA64:
        m_fp.m_page_size = 256;
        m_fp.m_total_size = 65536;
        m_fp.m_page_count = m_fp.m_total_size / m_fp.m_page_size;
        m_fp.m_bl_section_size = 2048; //1024 words
        m_fp.m_app_section_size = m_fp.m_total_size - m_fp.m_bl_section_size;
        //eeprom
        m_ep.m_size = 2048;
        m_ep.m_platform_id = EP_ATMEGA64;
        //MCU clock frequency
        m_fp.m_fcpu_hz = 16000000;
        //Magic number
        strncpy(m_fp.m_magic, "64  ", PLATFORM_MN_SIZE);
        m_fp.m_platform_id = EP_ATMEGA64;
        break;

    case EP_ATMEGA128:
        m_fp.m_page_size = 256;
        m_fp.m_total_size = 131072;
        m_fp.m_page_count = m_fp.m_total_size / m_fp.m_page_size;
        m_fp.m_bl_section_size = 2048; //1024 words
        m_fp.m_app_section_size = m_fp.m_total_size - m_fp.m_bl_section_size;
        //eeprom
        m_ep.m_size = 4096;
        m_ep.m_platform_id = EP_ATMEGA128;
        //MCU clock frequency
        m_fp.m_fcpu_hz = 16000000;
        //Magic number
        strncpy(m_fp.m_magic, "128 ", PLATFORM_MN_SIZE);
        m_fp.m_platform_id = EP_ATMEGA128;
        break;

    }
}

bool processFile(fw_data_t *fw, f_data_t *realtime_table, uint16_t *ce_errors, uint8_t *buf, int fsize)
{
    for (size_t i = 0; i < EP_NR_OF_PLATFORMS; ++i)
    {
        PPEepromParam m_ep;
        PPFlashParam m_fp;
        fillMainFwParams((EECUPlatform)i, m_ep, m_fp);
        if (fsize == m_fp.m_total_size)
        {
            uint8_t *data = buf + (m_fp.m_app_section_size - sizeof(fw_data_t));
            int dataSize = sizeof(fw_data_t)-sizeof(fw_data_t().code_crc);
            // copy all fw-data
            memcpy(fw, data, dataSize);
            if (fw->fw_data_size != sizeof(fw_data_t)-sizeof(cd_data_t))
            {
                printf("ERROR! Unsupported firmware version! Sorry.\n");
                return false;
            }
			::m_fp = m_fp;
			printf("* \"%s\" FW Detected!\n", platformNames[m_fp.m_platform_id]);

            return true;
        }
        if (fsize == m_ep.m_size)
        {
            // check magic
            if (0 == strncmp(m_fp.m_magic, (const char*)(buf + fsize - PLATFORM_MN_SIZE), PLATFORM_MN_SIZE))
            {
                // copy parameters from EEPROM
                memcpy(&fw->def_param, buf + EEPROM_PARAM_START, sizeof(params_t));
				// copy CE errors from EEPROM
				memcpy(ce_errors, buf + EEPROM_ECUERRORS_START, sizeof(uint16_t));
                // copy tables from EEPROM
                memcpy(realtime_table, buf + EEPROM_REALTIME_TABLES_START, sizeof(f_data_t));
				
				::m_ep = m_ep;
				::m_fp = m_fp;

				printf("* \"%s\" EEPROM Detected!\n", platformNames[m_ep.m_platform_id]);

				return true;
            }
        }
    }
    return false;
}

bool readFW(const std::string & fw, fw_data_t *fd, f_data_t *realtime_table, uint16_t *ce_errors)
{
    FILE *fp = fopen(fw.c_str(), "rb");
    if (fp == nullptr) return false;
    fseek(fp, 0, SEEK_END);
    int fsize = ftell(fp);
    rewind(fp);
    uint8_t *buf = (uint8_t *)malloc(fsize + 1);
    fread(buf, fsize, 1, fp);
    fclose(fp);

    bool ret = processFile(fd, realtime_table, ce_errors, buf, fsize);

    free(buf);
    return ret;
}

void dumpEEPROM(const std::string & fName, ecudata_t *d)
{
	FILE *fp = fopen(fName.c_str(), "wb");
	fputc(0xff, fp);
	fwrite(&d->param, sizeof(params_t), 1, fp);
	fwrite(&d->ecuerrors_saved_transfer, sizeof(uint16_t), 1, fp);
	fputc(0x0ff, fp);
	fputc(0x0ff, fp);
	fputc(0x0ff, fp);
	fwrite(&d->tables_ram, sizeof(f_data_t), 1, fp);
	char *plat = m_fp.m_magic[0] != (char)'\0' ? m_fp.m_magic : (char *)"644 ";
	fwrite(plat, PLATFORM_MN_SIZE, 1, fp);
	fclose(fp);
}
