#ifndef _TAMPILAN_H
#define _TAMPILAN_H

#include <SPI.h>
#include <DMD3.h>
#include <font/SystemFont5x7Gemuk.h>
#include <font/SystemFont5x7.h>
#include <font/AllahMuhammad.h>
#include <font/Arial14.h>
#include <font/angka6x14.h>

#include "setting.h"
#include "fungsi.h"

#define PANJANG_DMD 3
#define TINGGI_DMD 1

#define OE_PIN 9
#define SQW_PIN A3
#define BUZZER_PIN 4

#define totalEfek 4

int count_waktu_solat;

double timeMillis;
double interval = 2000;
double lastTime;

int acakEfek;
int kecerehanDmd;
boolean flip;

char c_jam[16];

DMD3 display(PANJANG_DMD, TINGGI_DMD);
boolean plip;

void scan()
{
    display.refresh();
    Timer1.setPwmDuty(OE_PIN, EEPROM.read(offsetof(Setting, kecerahan)));
    flip = digitalRead(SQW_PIN);
    if (flip)
    {
        if (plip)
            plip = false;
        if (!plip)
            plip = true;
    }
}

class tampilan
{
private:
public:
    void mulai();
    void startUp();
};

class Efek
{
private:
public:
    void centerCircleOut();
    void centerLineOut();
    void centerOut(int x, int y, int x1, int y1);
    void leftout(int x, int y, int x1, int y1);
    void rightOut(int x, int y, int x1, int y1);
    void upOut(int x, int y, int x1, int y1);
    void downOut(int x, int y, int x1, int y1);
    void rightIn(int x, int y, int x1, int y1);
    void leftIn(int x, int y, int x1, int y1);
};

void tampilan::mulai()
{
    Timer1.initialize(500);
    Timer1.attachInterrupt(scan);
    Timer1.pwm(OE_PIN, 2);
}

void tampilan::startUp()
{
    int centerWidth;
    int tampilKanan;
    int tampilKiri = 0;
    byte lafad_allah = 33;
    byte lafad_muhammad = 34;
    int line_1 = 0;
    int line_2 = 8;
    char msg[16];

    // Tampilan startup
    display.setFont(SystemFont5x7Gemuk); // pilih jenis font
    strcpy(msg, "Jam");
    centerWidth = (display.width() - display.textWidth(msg)) / 2;

    display.drawText(centerWidth, line_1, msg);

    strcpy(msg, "Solat");
    centerWidth = (display.width() - display.textWidth(msg)) / 2;

    display.drawText(centerWidth, line_2, msg);

    display.setFont(AllahMuhammad);
    tampilKanan = display.width() - display.charWidth(lafad_muhammad);
    display.drawChar(tampilKiri, line_1, lafad_allah);
    display.drawChar(tampilKanan, line_1, lafad_muhammad);
    display.swapBuffersAndCopy();

    delay(mem.read(offsetof(Setting, tundaStartUp)) * 200);
}

void Efek::centerCircleOut()
{
    int centerX = display.width() / 2;
    int centerY = display.height() / 2;
    //int width = display.width();
    for (int r = 0; r < centerX + 8; r++)
    {
        display.drawFilledCircle(centerX, centerY, r, 0);
        display.drawCircle(centerX, centerY, r);
        display.swapBuffersAndCopy();
        delay(mem.read(offsetof(Setting, kecepatanEfek)));
    }
}

void Efek::centerLineOut()
{
    int centerX = display.width() / 2;
    //int centerY = display.height() / 2;
    //int width = display.width();
    int height = display.height();
    for (int x = 0; x < centerX + 1; x++)
    {
        int clearXPlus = centerX + x;
        int clearXMin = centerX - x;
        display.drawFilledRect(centerX, 0, clearXPlus, height, 0);
        display.drawFilledRect(centerX, 0, clearXMin, height, 0);
        display.drawLine(clearXMin, 0, clearXMin, height);
        display.drawLine(clearXPlus, 0, clearXPlus, height);
        display.swapBuffersAndCopy();
        delay(mem.read(offsetof(Setting, kecepatanEfek)));
    }
}

void Efek::centerOut(int x, int y, int x1, int y1)
{
    int centerX = display.width() / 2;
    //int centerY = display.height() / 2;
    //int width = display.width();
    int height = y1 - y;
    for (int xm = 0; xm < centerX + 1; xm++)
    {
        int clearXPlus = xm;
        int clearXMin = display.width() - xm;
        display.drawFilledRect(x, y, clearXPlus, height, 0);
        display.drawFilledRect(x1, y, clearXMin, height, 0);
        display.swapBuffersAndCopy();
        delay(mem.read(offsetof(Setting, kecepatanEfek)));
    }
    delay(EEPROM.read(offsetof(Setting, tundaEfek)));
}

// void Efek::leftout(int x, int y, int x1, int y1)
// {
//     int panjangEfek = x + x1;
//     for (int i = 0; i < panjangEfek; i++)
//     {
//         display.drawFilledRect(x1, y1, x1 - i, y, 0);
//         display.swapBuffersAndCopy();
//         delay(mem.read(offsetof(Setting, kecepatanEfek)) / 2);
//     }
// }

//

// void Efek::upOut(int x, int y, int x1, int y1)
// {
//     int tinggiEfek = y + y1;
//     for (int i = 0; i < tinggiEfek; i++)
//     {
//         display.drawFilledRect(x, y + i, x1, y, 0);
//         display.swapBuffersAndCopy();
//         delay(mem.read(offsetof(Setting, kecepatanEfek)));
//     }
// }

// void Efek::downOut(int x, int y, int x1, int y1)
// {
//     int panjangEfek = x + x1;
//     for (int i = 0; i < panjangEfek; i++)
//     {
//         display.drawFilledRect(x, y1, x1, y1 - i, 0);
//         display.swapBuffersAndCopy();
//         delay(mem.read(offsetof(Setting, kecepatanEfek)));
//     }
// }

// void Efek::rightIn(int x, int y, int x1, int y1)
// {
// }

void pindahJam(byte pos)
{
    (count < 3) ? count++ : count = 0;
    display.setFont(angka6x14);
    if (pos == 0)
    {
        p_jam = jam / 10;

        if (pl_jam == p_jam)
        {
            for (byte x = 0; x < 16; x++)
            {
                display.drawFilledRect(19, 0, 29, 16, 0);
                display.drawChar(13, 1, puluhan(jam));
                display.drawChar(20, 0 - x, satuan(l_jam));
                display.drawChar(20, 16 - x, satuan(jam));
                display.swapBuffersAndCopy();
                delay(15);
            }
        }

        if (pl_jam != p_jam)
        {
            for (byte x = 0; x < 16; x++)
            {
                display.drawFilledRect(13, 0, 29, 16, 0);
                display.drawText(13, 0 - x, String(konvert(l_jam)));
                display.drawText(13, 16 - x, String(konvert(jam)));
                display.swapBuffersAndCopy();
                delay(15);
            }
            pl_jam = p_jam;
        }
    }
    if (pos == 1)
    {
        p_mnt = menit / 10;
        if (pl_mnt == p_mnt)
        {
            for (byte x = 0; x < 16; x++)
            {
                display.drawFilledRect(48, 0, 58, 16, 0);
                display.drawChar(42, 1, puluhan(menit));
                display.drawChar(49, 0 - x, satuan(l_mnt));
                display.drawChar(49, 16 - x, satuan(menit));
                display.swapBuffersAndCopy();
                delay(15);
            }
        }

        if (pl_mnt != p_mnt)
        {
            for (byte x = 0; x < 16; x++)
            {
                display.drawFilledRect(42, 0, 58, 16, 0);
                display.drawText(42, 0 - x, String(konvert(l_mnt)));
                display.drawText(42, 16 - x, String(konvert(menit)));
                display.swapBuffersAndCopy();
                delay(15);
            }
            pl_mnt = p_mnt;
        }
    }
    if (pos == 2)
    {
        display.drawFilledRect(30, 0, 37, 17, 0);
        display.drawFilledRect(59, 0, 66, 17, 0);
        display.drawFilledCircle(33, 3, count);
        display.drawFilledCircle(33, 12, count);
        display.drawFilledCircle(62, 3, count);
        display.drawFilledCircle(62, 12, count);
        if (count > 0)
        {
            display.drawFilledCircle(33, 3, count - 1, 0);
            display.drawFilledCircle(33, 12, count - 1, 0);
            display.drawFilledCircle(62, 3, count - 1, 0);
            display.drawFilledCircle(62, 12, count - 1, 0);
        }

        p_dtk = detik / 10;
        if (pl_dtk == p_dtk)
        {
            for (byte x = 0; x < 16; x++)
            {
                display.drawFilledRect(76, 0, 97, 16, 0);

                display.drawChar(70, 1, puluhan(detik));
                display.drawChar(77, 0 - x, satuan(l_dtk));
                display.drawChar(77, 16 - x, satuan(detik));
                display.swapBuffersAndCopy();
                delay(15);
            }
        }
        if (pl_dtk != p_dtk)
        {
            for (byte x = 0; x < 16; x++)
            {
                display.drawFilledRect(70, 0, 97, 16, 0);
                display.drawText(70, 0 - x, String(konvert(l_dtk)));
                display.drawText(70, 16 - x, String(konvert(detik)));
                display.swapBuffersAndCopy();
                delay(15);
            }
            pl_dtk = p_dtk;
        }
    }
}

void mulaiFullJam()
{
    l_dtk = detik;
    l_mnt = menit;
    l_jam = jam;
    display.setFont(angka6x14);
    for (int y = 0; y < 16; y++)
    {
        display.clear();
        display.drawFilledCircle(33, 3 + 16 - y - 1, 1);
        display.drawFilledCircle(33, 12 + 16 - y - 1, 1);
        display.drawFilledCircle(62, 3 + 16 - y - 1, 1);
        display.drawFilledCircle(62, 12 + 16 - y - 1, 1);
        display.drawText(13, 16 - y, String(konvert(jam)));
        display.drawText(42, 16 - y, String(konvert(menit)));
        display.drawText(70, 16 - y, String(konvert(detik)));
        display.swapBuffersAndCopy();
        delay(EEPROM.read(offsetof(Setting, kecepatanEfek)));
    }
}

void tampilJamInfo(const char *msg, bool s)
{
    update();
    bufferJamMenitDetik(c_jam, jam, menit, detik);

    //display.setDoubleBuffer(1);
    display.setFont(SystemFont5x7Gemuk);
    uint8_t center_y = display.height() / 2;
    uint8_t width = display.width();
    uint8_t center_text = (width - display.textWidth(c_jam)) / 2;
    int text_length = width + display.textWidth(msg) + 8;

    if (count_tampil == 0)
    {
        for (uint8_t i = 0; i < center_y; i++)
        {
            display.clear();
            display.drawText(center_text, -center_y + i, c_jam);
            display.swapBuffers();
            delay(EEPROM.read(offsetof(Setting, kecepatanEfek)) * 3);
        }
        count_tampil = 1;
    }
    for (int i = 0; i < text_length; i++)
    {
        // updateJadwal();
        update();
        if (s)
        {
            digitalWrite(BUZZER_PIN, flip);
            if (flip)
            {
                bufferJamMenitDetik(c_jam, jam, menit, detik);
            }
            if (!flip)
            {
                display.drawFilledRect(0, 0, width, 7, 0);
                for (byte ind = 0; ind < 16; ind++)
                {
                    c_jam[ind] = '\0';
                }
            }
        }
        else if (!s)
        {
            bufferJamMenitDetik(c_jam, jam, menit, detik);
        }
        center_text = (width - display.textWidth(c_jam)) / 2;
        //display.clear();
        display.setFont(SystemFont5x7Gemuk);
        display.drawText(center_text, 0, c_jam);
        display.drawRect(width + display.textWidth(msg) - i, 8, width, display.height(), 0);
        display.setFont(SystemFont5x7);
        display.drawText(width - i, 8, buffer);
        display.swapBuffers();
        //display.setDoubleBuffer(0);

        delay(EEPROM.read(offsetof(Setting, kecepatanEfek)) * 4);
        count_tampil++;
    }
}
void flipDisplay()
{
    if (flip)
    {
        display.setFont(angka6x14);
        display.drawText(80, 1, ":");
        display.swapBuffersAndCopy();
    }
    else
    {
        display.setFont(angka6x14);
        display.drawFilledRect(80, 1, 82, 16, 0);
        display.swapBuffersAndCopy();
    }
}

void tampilJamJadwal()
{

    int textCenter;
    int width = display.width();
    int height = display.height();
    const int panjangFrame = display.width() - 32;
    const int centerFrame = panjangFrame / 2;
    const int tinggiFrame = display.height();
    bufferJamMenit(c_jam, jam, menit);
    if (count_tampil == 0)
    {

        for (int i = 0; i < 32; i++)
        {
            int y;
            (i > 23) ? y = i - 23 : y = 0;
            display.setFont(angka6x14);
            display.drawFilledRect((width)-i, 0, width, height, 0);
            display.drawLine(width - i - 1, 0, width - i - 1, height);
            display.drawText(width - i + 1, 1, c_jam);

            display.setFont(SystemFont5x7Gemuk);
            memset(buffer, 0, sizeof(buffer));
            strcpy_P(buffer, namaJadwal[0]);
            textCenter = (panjangFrame - display.textWidth(buffer)) / 2;
            display.drawText(textCenter, (-7) + y, buffer);

            bufferJamMenit(buffer, mem.readInt(0), mem.readInt(10));
            textCenter = (panjangFrame - display.textWidth(buffer)) / 2;
            display.drawText(textCenter, 16 - y, buffer);
            display.swapBuffersAndCopy();
            delay(EEPROM.read(offsetof(Setting, kecepatanEfek)));
        }
        count_jadwal = 0;
        count_tampil = 1;
    }
    if (count_tampil >= 1)
    {
        if (count_tampil < EEPROM.read(offsetof(Setting, pindahTampil)))
        {
            display.setFont(angka6x14);
            bcdToArray(c_jam, bin2bcd(jam));
            display.drawText(66, 1, c_jam);
            bcdToArray(c_jam, bin2bcd(menit));
            display.drawText(83, 1, c_jam);
            display.swapBuffersAndCopy();

            flipDisplay();

            if (l_count_jadwal != count_jadwal)
            {
                if (count_jadwal > 0)
                {
                    for (int y = 0; y < centerFrame; y++)
                    {
                        flipDisplay();

                        display.drawFilledRect(0, 0, 62, 16, 0);
                        display.setFont(SystemFont5x7Gemuk);

                        memset(buffer, 0, sizeof(buffer));
                        int j;
                        (count_jadwal == 7) ? j = count_jadwal - 2 : j = count_jadwal - 1;

                        strcpy_P(buffer, namaJadwal[j]);

                        textCenter = (panjangFrame - display.textWidth(buffer)) / 2;
                        display.drawText(textCenter, 0 - y, buffer);

                        bufferJamMenit(buffer, mem.readInt(j), mem.readInt(j + 10));
                        textCenter = (panjangFrame - display.textWidth(buffer)) / 2;
                        display.drawText(textCenter, 7 + y, buffer);

                        display.swapBuffersAndCopy();
                        delay(EEPROM.read(offsetof(Setting, kecepatanEfek)));
                    }

                    for (int y = 0; y < tinggiFrame / 2; y++)
                    {

                        flipDisplay();

                        display.setFont(SystemFont5x7Gemuk);
                        memset(buffer, 0, sizeof(buffer));
                        strcpy_P(buffer, namaJadwal[count_jadwal]);
                        textCenter = (panjangFrame - display.textWidth(buffer)) / 2;
                        display.drawText(textCenter, (-7) + y, buffer);

                        bufferJamMenit(buffer, mem.readInt(count_jadwal), mem.readInt(count_jadwal + 10));
                        textCenter = (panjangFrame - display.textWidth(buffer)) / 2;
                        display.drawText(textCenter, 15 - y, buffer);
                        display.swapBuffersAndCopy();

                        delay(EEPROM.read(offsetof(Setting, kecepatanEfek)));
                    }
                }

                l_count_jadwal = count_jadwal;
            }
        }
    }
}

// Keluar tampilan jadwal
void jamJadwalOut()
{
    const int width = display.width();
    const int height = display.height();
    int textCenter;
    const int panjangFrame = width - 32;
    bufferJamMenit(c_jam, jam, menit);
    for (int i = 0; i < 32; i++)
    {
        int y = i;
        (y > 8) ? y = y : y = i;
        display.setFont(angka6x14);
        display.drawFilledRect(panjangFrame - 1, 0, panjangFrame + i, height, 0);
        display.drawLine(panjangFrame + i, 0, panjangFrame + i, height);
        display.drawText(panjangFrame + i + 2, 1, c_jam);

        display.setFont(SystemFont5x7Gemuk);
        memset(buffer, 0, sizeof(buffer));
        strcpy_P(buffer, namaJadwal[8]);
        textCenter = (panjangFrame - display.textWidth(buffer)) / 2;
        display.drawFilledRect(0, 8, panjangFrame, 8 - y, 0);
        display.drawText(textCenter, 0 - y, buffer);

        bufferJamMenit(buffer, mem.readInt(8), mem.readInt(18));
        textCenter = (panjangFrame - display.textWidth(buffer)) / 2;
        display.drawFilledRect(0, 8, panjangFrame, 8 + y, 0);
        display.drawText(textCenter, 9 + y, buffer);

        display.swapBuffersAndCopy();

        delay(EEPROM.read(offsetof(Setting, kecepatanEfek)));
    }
}

#endif