/* s3emul - "secu3 emulator"
 *   Copyright (C) 2016 andreika. Ukraine, Kiev
 *
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 *   contacts:
 *              http://secu-3.org/forum/ucp.php?i=pm&mode=compose&u=740
 *
 */


// #define UART_DEBUG
// #define HEX_DEBUG

#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <assert.h>
#include <stdio.h>
#include <stdint.h>
#include <time.h>
#include "s3.h"

#define S3EMUL_TITLE   "S3EMUL"
#define S3EMUL_DESC    " - SECU-3 & MEGASQUIRT bridge/emulator "
#define S3EMUL_VERSION "v0.4.1 (11.03.2017). "
#define S3EMUL_GPL     "Distributed under GPL license.\n(c) 2017 andreika."

typedef struct
{
    size_t offs;
    int size;       // use 1,2,4 for unsigned, and -1,-2,-4 for signed variables
    double coef;
    double add;     // targetValue = sourceValue * coef + add;
} SensorRecord;

typedef struct
{
    char *name;
    int logIdx;
    SensorRecord s3;
} SensorParam;

typedef struct
{
    char *name;
    int msTableIdx;
    int s3tableIdx;
    size_t msOffs;
    size_t s3Offs;
    int msTypeSize;
    int s3TypeSize;
    int width, height;
    double coef;
    bool inverseY;
    bool reciprocal;    // 1/x
} TableRT;

const int maxNumFields = 40;    // depends on version!
const double maxDeltaTime = 1.0;    // 5 seconds

typedef struct
{
    double time;
    double fields[maxNumFields];
} CSVRecord;

typedef struct
{
    int threadId;
    std::string str;
} LogRecord;

inline int round_int(double r)
{
    return (int)((r > 0.0) ? (r + 0.5) : (r - 0.5));
}

template <typename T> inline T Min(T a, T b)
{
    return (a <= b) ? a : b;
}

template <typename T> inline T Max(T a, T b)
{
    return (a >= b) ? a : b;
}

void send_s3_client_packet(uartstate_t &uart, struct ecudata_t* d, struct fw_data_t &fw_data, uint8_t send_mode);
int receive_s3_packet(uartstate_t &uart, struct ecudata_t* d, struct fw_data_t &fw_data);
int getTableIndex(const struct ecudata_t & d, int curTblIdx);
bool readFW(const std::string & fw, fw_data_t *fd, f_data_t *realtime_table, uint16_t *ce_errors);
void dumpEEPROM(const std::string & fName, ecudata_t *d);
bool readCSVFile(const std::string & fileName, std::vector<CSVRecord> & csvRecords, double *duration);
int emulS3(void);
