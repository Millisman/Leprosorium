/* s3emul - "secu3 emulator"
* Copyright (C) 2016 andreika. Ukraine, Kiev
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*
* contacts:
*            http://secu-3.org/forum/ucp.php?i=pm&mode=compose&u=740
*
*/


#include <iostream>
#include <unistd.h>
#include <fstream>
#include <string>
#include <regex>
#include <array>
#include <error.h>
#include <termio.h>
#include <fcntl.h>
#include <time.h>
#include <getopt.h>
#include <sys/time.h>
#include <sys/signal.h>
#include <sys/select.h>
#include "s3emul.h"

static struct ecudata_t d;
static struct fw_data_t fw;
static f_data_t realtime_table;
static int curTblIdx = -1;
static uint8_t *dPtr = (uint8_t *)&d;
static std::vector<CSVRecord> logRecords;
static uartstate_t uart;
static int uartHandle;


struct
{
    int mode;
    const char *modeName;
} sendModes[] =
{
    { CHANGEMODE, "change mode" },
    { BOOTLOADER, "boot loader" },
    { TEMPER_PAR, "temperature" },
    { CARBUR_PAR, "carb." },
    { IDLREG_PAR, "idling reg." },
    { ANGLES_PAR, "advance angle" },
    { FUNSET_PAR, "functions" },
    { STARTR_PAR, "engine start" },
    { FNNAME_DAT, "names of func." },
    { SENSOR_DAT, "sensors data" },
    { ADCCOR_PAR, "ADC corr." },
    { ADCRAW_DAT, "raw values" },
    { CKPS_PAR, "CKP sensor" },
    { OP_COMP_NC, "suspended op" },
    { CE_ERR_CODES, "CE codes" },
    { KNOCK_PAR, "knock chip" },
    { CE_SAVED_ERR, "CE EEPROM codes" },
    { FWINFO_DAT, "firmware info" },
    { MISCEL_PAR, "misc." },
    { EDITAB_PAR, "RT tables" },
    { ATTTAB_PAR, "attenuator map" },
    { RPMGRD_PAR, "RPM grid" },
    { DBGVAR_DAT, "debug" },
    { DIAGINP_DAT, "diag. input" },
    { DIAGOUT_DAT, "diag. output" },
    { CHOKE_PAR, "choke" },
    { SECUR_PAR, "security" },
    { UNIOUT_PAR, "uni.outputs" },
    { INJCTR_PAR, "injection" },
    { LAMBDA_PAR, "lambda" },
    { ACCEL_PAR, "accel.enrichment" },
    { GASDOSE_PAR, "gas dose" },
    { SIGINF_DAT, "signature" },
    { -1, "" }
};

static std::string sendModeName[256];

static SensorParam sensorParams[] =
{
    { "frequen",        1,  { offsetof(D, sens.frequen), 2, 1.0 },},
    { "curr_angle",     2,  { offsetof(D, corr.curr_angle), -2, 32.0 },},
    { "map",            3,  { offsetof(D, sens.map), 2, 64.0 },},
    { "voltage",        4,  { offsetof(D, sens.voltage), 2, 400.0 },},
    { "temperat",       5,  { offsetof(D, sens.temperat), -2, 4.0 },},
    { "knock_k",        6,  { offsetof(D, sens.knock_k), 2, 1.0 },},
    { "knock_retard",   7,  { offsetof(D, corr.knock_retard), -2, 1.0 },},
    { "airflow",        8,  { offsetof(D, airflow), 1, 1.0 },},
    { "carb",           9,  { offsetof(D, sens.carb), 1, 1.0 },},
    { "gas",            10, { offsetof(D, sens.gas), 1, 1.0 },},
    { "ie_valve",       11, { offsetof(D, ie_valve), 1, 1.0 },},
    { "fe_valve",       12, { offsetof(D, fe_valve), 1, 1.0 },},
    { "cool_fan",       13, { offsetof(D, cool_fan), 1, 1.0 },},
    { "st_block",       14, { offsetof(D, st_block), 1, 1.0 },},
    { "acceleration",   15, { offsetof(D, acceleration), 1, 1.0 },},
    { "tps",            16, { offsetof(D, sens.tps), 1, 2.0 },},
    { "add_i1",         17, { offsetof(D, sens.add_i1), 2, 400.0 },},
    { "add_i2",         18, { offsetof(D, sens.add_i2), 2, 400.0 },},
    { "choke_pos",      19, { offsetof(D, choke_pos), 1, 2.0 },},
    { "speed",          20, { offsetof(D, sens.speed), 2, 1.0 },},
    { "distance",       21, { offsetof(D, sens.distance), 4, 1.0 },},
    { "gasdose_pos",    22, { offsetof(D, gasdose_pos), 1, 1.0 },},
    { "air_temp",       23, { offsetof(D, sens.air_temp), -2, 4.0 },},
    { "lambda",         31, { offsetof(D, corr.lambda), -2, 512.0 },},
    { "inj_pw",         32, { offsetof(D, inj_pw), 2, 320.0 },},
    { "tpsdot",         33, { offsetof(D, sens.tpsdot), -2, 1.0 },},
    { "ecuerrors",      34, { offsetof(D, ecuerrors_for_transfer), 2, 1.0 },},
    { "ce_state",       35, { offsetof(D, ce_state), 1, 1.0 },},
    { nullptr,          -1, { static_cast<size_t>(-1), 0, 1.0 },},
};

int getTableIndex(const struct ecudata_t & d, int curTblIdx)
{
    int tblIdx;
    uint8_t mapsel0 = 0; // = IOCFG_CHECK(IOP_MAPSEL0) ? IOCFG_GET(IOP_MAPSEL0) : 0;
    if (d.sens.gas) tblIdx = mapsel0 ? 1 : d.param.fn_gas;          //on gas
    else tblIdx = mapsel0 ? 0 : d.param.fn_gasoline;     //on petrol
    assert(tblIdx >= 0 && tblIdx < TABLES_NUMBER);
    return tblIdx;
}

bool readCSVFile(const std::string & fileName, std::vector<CSVRecord> & csvRecords, double *duration)
{
    std::ifstream fp(fileName);
    if (fp.fail())
        return false;
    std::string line;

    double time0 = -1.0;
    double prevTime = -1.0, startTime = 0.0;

    while (std::getline(fp, line))
    {
        CSVRecord rec;
        // parse - regex is too slow :(
        char *str = (char *)line.c_str();
        char *s0 = str;
        for (int i = 0; *str != '\0'; i++)
        {
            for (; *str != '\0' && *str != ',' && *str != ' '; str++)
                ;
            if (*str != '\0')
                *str++ = '\0';
            const char *match = s0;
            for (; *str != '\0' && (*str == ',' || *str == ' '); str++)
                ;
            s0 = str;

            assert(i < maxNumFields);
            rec.fields[i] = (double)atof(match);
            if (i == 0 && duration != nullptr)  // time
            {
                int t_h, t_m, t_s, t_cs;
                sscanf(match, "%d:%d:%d.%d", &t_h, &t_m, &t_s, &t_cs);
                rec.time = (double)t_h * 3600.0 + (double)t_m * 60.0 + (double)t_s + (double)t_cs / 100.0;
                if (time0 < 0)
                    time0 = rec.time;
                rec.time -= time0;
                if (prevTime < 0)
                {
                    prevTime = rec.time;
                    startTime = rec.time;
                }
                else if (std::abs(rec.time - prevTime) < 0.00001)
                    continue;
                // if clock starts over or if there's a big delay between records - timeshift!
                if (rec.time < prevTime || rec.time > prevTime + maxDeltaTime)
                {
                    double newTime = prevTime + 0.1;
                    time0 += rec.time - newTime;
                    rec.time = newTime;
                }
                prevTime = rec.time;
            }
        }
        csvRecords.push_back(rec);
    }
    if (duration != nullptr) *duration = (prevTime - startTime);
    return true;
}

void initRTTables()
{
    printf("* Init params...\n");
    memcpy(&d.param, &fw.def_param, sizeof(params_t));     // copy from firmware/eeprom
    if (curTblIdx < TABLES_NUMBER_PGM)
        memcpy(&d.tables_ram, &fw.tables[curTblIdx], sizeof(f_data_t)); // from firmware
        else memcpy(&d.tables_ram, &realtime_table, sizeof(f_data_t));   // from EEPROM
}

void updateFromLogRecord(double *fields)
{
    for (int i = 0; sensorParams[i].name != nullptr; i++)
    {
        if (sensorParams[i].logIdx < 0)
            continue;
        assert(sensorParams[i].logIdx < maxNumFields);
        double f = fields[sensorParams[i].logIdx];
        int res = round_int(f * sensorParams[i].s3.coef + sensorParams[i].s3.add);

        switch (sensorParams[i].s3.size)
        {
            case -1: *((int8_t *)&dPtr[sensorParams[i].s3.offs]) = (int8_t)res; break;
            case -2: *((int16_t *)&dPtr[sensorParams[i].s3.offs]) = (int16_t)res; break;
            case -4: *((int32_t *)&dPtr[sensorParams[i].s3.offs]) = (int32_t)res; break;
            case 1: *((uint8_t *)&dPtr[sensorParams[i].s3.offs]) = (uint8_t)res; break;
            case 2: *((uint16_t *)&dPtr[sensorParams[i].s3.offs]) = (uint16_t)res; break;
            case 4: *((uint32_t *)&dPtr[sensorParams[i].s3.offs]) = (uint32_t)res; break;
        }
    }

    int tI = getTableIndex(d, curTblIdx);
    if (tI != curTblIdx)
    {
        curTblIdx = tI;
        initRTTables();
    }
}

bool initS3Params()
{
    memset(&d, 0, sizeof(d));
    memset(&fw, 0, sizeof(fw));
    int i;
    for (i = 0; i < 256; i++)
        sendModeName[i] = std::string();
    for (i = 0; sendModes[i].mode >= 0; i++) sendModeName[sendModes[i].mode] = std::string(sendModes[i].modeName);
    curTblIdx = -1;
    strcpy((char *)fw.fw_signature_info, "S3EMUL " S3EMUL_VERSION);
    fw.cddata.config = 0xFFFFFFFF;
    //     fw.cddata.config = (findFwOption("DWELL_CONTROL") << 7) | (findFwOption("COOLINGFAN_PWM") << 8) | (findFwOption("REALTIME_TABLES") << 9) | (findFwOption("ICCAVR_COMPILER") << 10) |
    //     (findFwOption("AVRGCC_COMPILER") << 11) | (findFwOption("DEBUG_VARIABLES") << 12) | (findFwOption("PHASE_SENSOR") << 13) | (findFwOption("PHASED_IGNITION") << 14) |
    //     (findFwOption("FUEL_PUMP") << 15) | (findFwOption("THERMISTOR_CS") << 16) | (1 << 17) | (findFwOption("DIAGNOSTICS") << 18) | (findFwOption("HALL_OUTPUT") << 19) |
    //     (findFwOption("REV9_BOARD") << 20) | (findFwOption("STROBOSCOPE") << 21) | (findFwOption("SM_CONTROL") << 22) | (findFwOption("VREF_5V") << 23) |
    //     (findFwOption("HALL_SYNC") << 24) | (findFwOption("UART_BINARY") << 25) | (findFwOption("CKPS_2CHIGN") << 26) | (1 << 27) |
    //     (findFwOption("FUEL_INJECT") << 28) | (findFwOption("GD_CONTROL") << 29) | (findFwOption("CARB_AFR") << 30) | (findFwOption("CKPS_NPLUS1") << 31);
    static const int16_t rpg_grid[] = { 600, 720, 840, 990, 1170, 1380, 1650, 1950, 2310, 2730, 3210, 3840, 4530, 5370, 6360, 7500 };
    memcpy(fw.exdata.rpm_grid_points, rpg_grid, sizeof(rpg_grid));
    return true;
}

double getTimeInSeconds()
{
    double time_in_secs;
    struct timeval  tv;
    gettimeofday(&tv, NULL);
    time_in_secs = (double)tv.tv_sec + (double)(tv.tv_usec) / 1000000.0;
    return time_in_secs;
}


void hexDump (char *desc, void *addr, int len) {
    int i;
    unsigned char buff[17];
    unsigned char *pc = (unsigned char*)addr;
    // Output description if given.
    if (desc != NULL) printf ("%s:\n", desc);
    if ((len == 0) || (len < 0)) return;
    // Process every byte in the data.
    for (i = 0; i < len; i++) {
        // Multiple of 16 means new line (with line offset).
        if ((i % 16) == 0) {
            // Just don't print ASCII for the zeroth line.
            if (i != 0) printf ("  %s\n", buff);
            printf ("  %04x ", i); // Output the offset.
        }
        printf (" %02x", pc[i]);// Now the hex code for the specific character.
        // And store a printable ASCII character for later.
        if ((pc[i] < 0x20) || (pc[i] > 0x7e)) buff[i % 16] = '.'; else buff[i % 16] = pc[i];
        buff[(i % 16) + 1] = '\0';
    }
    // Pad out last line if not exactly 16 characters.
    while ((i % 16) != 0) { printf ("   "); i++; }
    printf ("  %s\n", buff); // And print the final ASCII bit.
}


void sendToSoft(uartstate_t uart)
{
    uart.send_buf[uart.send_size++] = '\n';
    int n = write(uartHandle, uart.send_buf, uart.send_size);
    if (n != uart.send_size)
    {
        printf("\nERROR on WRITE to SOFT %d/%d\n", n, uart.send_size);
    }
    usleep(1000);
    #ifdef HEX_DEBUG
    hexDump ("send_buf", uart.send_buf, sizeof(uart.send_buf));
    #endif
    for (int x = 0; x < UART_SEND_BUFF_SIZE; x++) uart.send_buf[x] = 0x00;
}





void signal_handler_IO (int status)
{
    uint8_t buf[UART_RECV_BUFF_SIZE];
    for (int x = 0; x < UART_RECV_BUFF_SIZE; x++) buf[x] = 0x00;
    int n = read(uartHandle, buf, UART_RECV_BUFF_SIZE);
    if ((n>2) && (buf[0] == '!') && (buf[n-1] == '\r'))
    {
        for (int x = 0; x < UART_RECV_BUFF_SIZE; x++) uart.recv_buf[x] = 0x00;
        uart.recv_index = 0;
        uart.recv_size = 0;
        for (int x= 1; x < n - 1; x++)
        {
            uart.recv_buf[uart.recv_index++] = buf[x];
            uart.recv_size++;
        }
    } else
    {
        printf("\nERROR on READ from SOFT\n");
        printf("%x\n", &buf);
    }
    #ifdef HEX_DEBUG
    hexDump ("read buf", buf, n);
    #endif
}

bool ptym_open()
{
    char          *pty_slave_name;
    // Get a master pty
    // posix_openpt() opens a pseudo-terminal master and returns its file descriptor.
    // It is equivalent to open("/dev/ptmx",O_RDWR|O_NOCTTY) on Linux systems :
    //       . O_RDWR Open the device for both reading and writing
    //       . O_NOCTTY Do  not  make  this device the controlling terminal for the process
    uartHandle = posix_openpt( O_RDWR | O_NOCTTY | O_NDELAY| O_NONBLOCK ); //| O_NDELAY| O_NONBLOCK |O_NOCTTY | O_SYNC   | O_SYNC  | O_NONBLOCK | O_NDELAY
    if (uartHandle < 0)
    {
        printf("Impossible to get a master pseudo-terminal - errno = '%m' (%d)\n", errno);
        return false;
    }
    if (0 != grantpt(uartHandle)) // Grant access to the slave pseudo-terminal (Chown the slave to the calling user)
    {
        printf("Impossible to grant access to slave pseudo-terminal - errno = '%m' (%d)\n", errno);
        close(uartHandle);
        return false;
    }
    if (0 != unlockpt(uartHandle)) // Unlock pseudo-terminal master/slave pair (Release an internal lock so the slave can be opened)
    {
        printf("Impossible to unlock pseudo-terminal master/slave pair - errno = '%m' (%d)\n", errno);
        close(uartHandle);
        return false;
    }
    pty_slave_name = ptsname(uartHandle); // Get the name of the slave pty
    if (NULL == pty_slave_name)
    {
        printf("Impossible to get the name of the slave pseudo-terminal - errno = '%m' (%d)\n", errno);
        close(uartHandle);
        return false;
    }
    printf("\nDevice = %s\n", pty_slave_name);
    return true;
}

bool initPort()
{
    struct sigaction saio;
    struct termios options;
    if (ptym_open())
    {
        saio.sa_handler  = signal_handler_IO;
        saio.sa_flags    = 0;
        saio.sa_restorer = NULL;
        sigaction(SIGIO, &saio, NULL);
        fcntl(uartHandle, F_SETOWN, getpid());
        //fcntl(uartHandle, F_SETFL, FNDELAY); // non blocking mode for Read
        //fcntl(uartHandle, F_SETFL, 0);       // blocking mode for Read
        fcntl(uartHandle, F_SETFL, FASYNC);    // Make the file descriptor asynchronous (the manual page says only O_APPEND and O_NONBLOCK, wi*l*l work with F_SETFL...)
        tcgetattr(uartHandle, &options);
        cfmakeraw(&options);
        cfsetispeed(&options, B576000);
        cfsetospeed(&options, B576000);
        //options.c_oflag = 0;
        //options.c_lflag = 0;
        //options.c_cc[VMIN]  = 3;
        //options.c_cc[VTIME] = 0;
        //options.c_cflag = B576000 | CS8 | CLOCAL | CREAD;
        tcsetattr(uartHandle, TCSANOW, &options);
        tcflush(uartHandle, TCIFLUSH);
        return true;
    } else return false;
}



int emulS3()
{
    char * fwFileName ="test_fw.bin";
    char * eepromFileName = "test_eeprom.bin";
    char * eepromSaveFileName = "dump_eeprom.bin";
    char * logFileName ="test.csv";

    printf("Starting SECU Emulator!\n");

    if (!initPort()) { printf("ERROR!\n"); return -4; }

    printf("Emulating SECU-3 board...\n");

    if (!initS3Params()) { return -2; }

    printf("Reading SECU-3 firmware file...\n");
    if (!readFW(fwFileName, &fw, &realtime_table, &d.ecuerrors_saved_transfer))
    {
        printf("ERROR!\n");
        return -1;
    }
    // copy from firmware/eeprom
    memcpy(&d.param, &fw.def_param, sizeof(params_t));

    printf("Reading SECU-3 EEPROM file...\n");
    if (!readFW(eepromFileName, &fw, &realtime_table, &d.ecuerrors_saved_transfer))
    {
        printf("ERROR!\n");
        return -1;
    }
    //d->ecuerrors_for_transfer = d.ecuerrors_saved_transfer;

    if (logFileName[0] != '\0')
    {
        printf("Reading log file %s...\n", logFileName);
        double duration;
        logRecords.clear();
        if (!readCSVFile(logFileName, logRecords, &duration))
        {
            printf("ERROR reading log file!\n");
            return false;
        }

        printf("Found %d records. Total duration: %d secs\n", logRecords.size(), round_int(duration));
    } else
    {
        curTblIdx = getTableIndex(d, curTblIdx);
        initRTTables();
    }
    printf("Sending/reading packages...\n");
    uart.send_size = 0;
    uart.recv_size = 0;
    uart.send_mode = SENSOR_DAT;
    uart.send_index = 0;
    uart.recv_index = 0;
    uart.tblIdx = 0;
    uart.state = 0;
    uart.wrk_index = 0;
    uart.index = 0;
    int pType = 0, sentPType = 0;
    size_t cnt = 0;
    double startTime = getTimeInSeconds();
    double printTime = 0.0;
    int printCnt = 0;

    for (;;)
    {
        if (logRecords.size() > 0)
        {
            double curTime = getTimeInSeconds() - startTime;
            for (; cnt < logRecords.size() && logRecords[cnt].time < curTime; cnt++)
            {
                updateFromLogRecord(logRecords[cnt].fields);
                send_s3_client_packet(uart, &d, fw, pType);
                sendToSoft(uart);
            }

            if (curTime - printTime > 2.0 || cnt >= logRecords.size())  // every 2 secs
            {
                printf("  + %lu records added (%lu%%)\n", (cnt - printCnt), (cnt * 100 / logRecords.size()));
                printTime = curTime;
                printCnt = cnt;
            }
            if (cnt >= logRecords.size())
            {   printf("end log...\n");
                break;
            }
        }
        else
        {
            send_s3_client_packet(uart, &d, fw, pType);
            sendToSoft(uart);
        }

        sentPType = pType;
        //pType = 0;
        usleep(1000);

        if (uart.recv_size > 0)
        {
            int descr = receive_s3_packet(uart, &d, fw);
            printf("descr = %d\n", descr);

            switch (descr)
            {
                case BOOTLOADER:
                    printf("* WARNING! BOOTLOADER mode not supported!\n");
                    break;

                case OP_COMP_NC:
                {
                    printf("OP_COMP_NC opcode=%d\n", _AB(d.op_actn_code, 0));
                    int opCode = _AB(d.op_actn_code, 0);
                    switch (opCode)
                    {
                        case OPCODE_READ_FW_SIG_INFO:
                            if (sentPType != FWINFO_DAT) printf("* Sending \"FW Info\" by request...\n");
                            pType = FWINFO_DAT;
                            break;

                        case OPCODE_EEPROM_PARAM_SAVE:
                        case OPCODE_SAVE_TABLSET:
                            printf("* EEPROM save request...\n");
                            printf("  Dumping EEPROM to %s\n", eepromSaveFileName);
                            dumpEEPROM(eepromSaveFileName, &d);
                            break;
                        case OPCODE_DIAGNOST_ENTER:
                            printf("* WARNING! DIAGNOSTIC mode not supported!\n");
                            break;
                    }
                    _AB(d.op_actn_code, 0) = 0; // processed ;-)
                }
                break;

                        case EDITAB_PAR:
                            printf("* Table %d changed!\n", uart.tblIdx);
                            break;

                        case CHANGEMODE:
                        {
                            static int printedSendMode = -1;
                            if (uart.send_mode != printedSendMode)
                            {
                                printf("* Sending \"%s\" by request...\n", sendModeName[uart.send_mode].c_str());
                                printedSendMode = uart.send_mode;
                            }
                            send_s3_client_packet(uart, &d, fw, 0);
                            sendToSoft(uart);
                            pType = 0;
                        }
                        break;

                        default:
                            printf("--%d ('%c') mode='%c'\n", descr, descr, uart.send_mode);
            }
        }
        uart.recv_size = 0;
    }
    close(uartHandle);
    return 0;
}



static struct option s3emu_pts[] =
{
    { "script",     required_argument, NULL, 's' }, //optional_argument
    { "help",       no_argument,       NULL, 'h' },
    {NULL, 0, NULL, 0 }
};

int main(int argc, char *argv[])
{
    int opt;
    while ((opt = getopt_long(argc, argv, "s:h:?", s3emu_pts, NULL)) != EOF)
    {
        switch(opt)
        {
            case 's': {printf("key s, optarg=%s\n", optarg);} break;
            case 'h':
            case '?':
            default : {printf("this help (default)\n"); exit(1);}
        }
    }
    char timbuf[80];
    time_t tim = time(nullptr);
    strftime(timbuf, sizeof(timbuf), "\n[%Y-%m-%d %X] ", localtime(&tim));
    std::string datetime(timbuf);
    printf("%s %s\n\n", datetime.c_str(), S3EMUL_TITLE " " S3EMUL_VERSION "\n" S3EMUL_GPL);
    return emulS3();
}
