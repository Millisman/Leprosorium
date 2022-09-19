#include <stdlib.h>
#include <stdio.h>
#include <libgen.h>
#include <string.h>

#include "sim_avr.h"
#include "avr_ioport.h"
#include "avr_eeprom.h"
#include "sim_elf.h"

#include "../../simavr/cores/sim_megax4.h"

#include "sim_hex.h"
#include "sim_gdb.h"
#include "sim_vcd_file.h"

#if __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif
#include <pthread.h>

#include "hd44780_glut.h"

#include "uart_modem.h"

#define EESize 2048


int window;

avr_t *avr = NULL;
elf_firmware_t f = {{0}};
hd44780_t hd44780;

int color = 0;
uint32_t colors[][4] = {
        { 0x00aa00ff, 0x00cc00ff, 0x000000ff, 0x00000055 },    // fluo green
        { 0xaa0000ff, 0xcc0000ff, 0x000000ff, 0x00000055 },    // red
};

void eeprom_read_file(avr_t *avr) {
    struct mcu_t * mcu = (struct mcu_t*)avr;
    printf("Read %d eeprom.bin...\n", mcu->eeprom.size);
    FILE *file = fopen("eeprom.bin", "rb");
    if (!file) {
        printf("ERROR!\n");
        return;
    }
    fread(mcu->eeprom.eeprom , mcu->eeprom.size, 1, file);
    fclose(file);
}

void eeprom_write_file(avr_t *avr) {
    struct mcu_t * mcu = (struct mcu_t*)avr;
    printf("Write %d eeprom.bin...\n", mcu->eeprom.size);
    FILE *file = fopen("eeprom.bin", "wb");
    if (!file) {
        printf("ERROR!\n");
        return;
    }
    fwrite(mcu->eeprom.eeprom , mcu->eeprom.size, 1, file);
    fclose(file);
}




static void *avr_run_thread(void * ignore) {
    while (1) {
        avr_run(avr);
    }
    return NULL;
}

void keyCB(unsigned char key, int x, int y) {
    printf("Pressed key=%c, x=%d, y=%d\n\n", key, x, y);
    if (key == 'q') {
        eeprom_write_file(avr);
        exit(EXIT_SUCCESS);
    }
}


void displayCB(void) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW); // Select modelview matrix
    glPushMatrix();
    glLoadIdentity(); // Start with an identity matrix
    glScalef(3, 3, 1);

    hd44780_gl_draw(
        &hd44780,
            colors[color][0], /* background */
            colors[color][1], /* character background */
            colors[color][2], /* text */
            colors[color][3] /* shadow */ );
    glPopMatrix();
    glutSwapBuffers();
}

void timerCB(int i) {
    glutTimerFunc(1000/64, timerCB, 0);
    glutPostRedisplay();
}

int initGL(int w, int h) {
    // Set up projection matrix
    glMatrixMode(GL_PROJECTION); // Select projection matrix
    glLoadIdentity(); // Start with an identity matrix
    glOrtho(0, w, 0, h, 0, 10);
    glScalef(1,-1,1);
    glTranslatef(0, -1 * h, 0);

    glutDisplayFunc(displayCB);        /* set window's display callback */
    glutKeyboardFunc(keyCB);        /* set window's key callback */
    glutTimerFunc(1000 / 24, timerCB, 0);

    glEnable(GL_TEXTURE_2D);
    glShadeModel(GL_SMOOTH);

    glClearColor(0.8f, 0.8f, 0.8f, 1.0f);
    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);

    ////////////////////////////
    hd44780_gl_init();

    return 1;
}

uint8_t arrrt[EESize+1];


uart_modem_t uart_modem;

int main(int argc, char *argv[]) {

    if (argc < 2) {
        exit(EXIT_FAILURE);
    }

    f.frequency = 20000000;
    sprintf(f.mmcu, "atmega644p");
    sim_setup_firmware(argv[1], AVR_SEGMENT_OFFSET_FLASH, &f, argv[0]);

    avr = avr_make_mcu_by_name(f.mmcu);
    if (!avr) {
        fprintf(stderr, "%s: AVR '%s' not known\n", argv[0], f.mmcu);
        exit(1);
    }

    avr_init(avr);
    avr_load_firmware(avr, &f);

    eeprom_read_file(avr);

    uart_modem_init(avr, &uart_modem);
    uart_modem_connect(&uart_modem, '1');

    hd44780_init(avr, &hd44780, 16, 2);

    avr_connect_irq(avr_io_getirq(avr, AVR_IOCTL_IOPORT_GETIRQ('C'), 2), hd44780.irq + IRQ_HD44780_D4);
    avr_connect_irq(avr_io_getirq(avr, AVR_IOCTL_IOPORT_GETIRQ('C'), 3), hd44780.irq + IRQ_HD44780_D5);
    avr_connect_irq(avr_io_getirq(avr, AVR_IOCTL_IOPORT_GETIRQ('C'), 4), hd44780.irq + IRQ_HD44780_D6);
    avr_connect_irq(avr_io_getirq(avr, AVR_IOCTL_IOPORT_GETIRQ('C'), 5), hd44780.irq + IRQ_HD44780_D7);
    avr_connect_irq(avr_io_getirq(avr, AVR_IOCTL_IOPORT_GETIRQ('C'), 0), hd44780.irq + IRQ_HD44780_RS);
    avr_connect_irq(avr_io_getirq(avr, AVR_IOCTL_IOPORT_GETIRQ('C'), 1), hd44780.irq + IRQ_HD44780_E);

    // OpenGL init, can be ignored
    int g_argc = 1;
    glutInit(&g_argc, argv);        /* initialize GLUT system */

    int w = 5 + hd44780.w * 6;
    int h = 5 + hd44780.h * 8;
    int pixsize = 3;

    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
    glutInitWindowSize(w * pixsize, h * pixsize);
    window = glutCreateWindow("Press 'q' to quit");

    initGL(w * pixsize, h * pixsize);

    pthread_t run;
    pthread_create(&run, NULL, avr_run_thread, NULL);

    glutMainLoop();
}
