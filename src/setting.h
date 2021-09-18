#ifndef _SETTING_H
#define _SETTING_H

#include <Arduino.h>
#include <Wire.h>
#include <AT24Cx.h>
#include <EEPROM.h>

#define tokenSetting 21
#define ADDRESS 0x50

AT24CX mem(ADDRESS, 32);

enum waktuShalatId
{
    Imsyak,
    Subuh,
    Terbit,
    Duha,
    Dzuhur,
    Ashar,
    Terbenam,
    Maghrib,
    Isya,

    JumlahWaktuShalat
};

struct jamMenit
{
    byte jam;
    byte menit;
};

enum methode
{
    shafii,
    hanafi
};

struct Setting
{
    byte token;
    byte kecerahan;
    byte kecepatanEfek;
    byte tundaEfek;
    byte tundaStartUp;
    byte pindahTampil;

    double bujur;
    double lintang;
    uint8_t zonaWaktu;
    uint8_t koreksiHijriah;

    uint8_t aturImsyak;
    uint8_t aturSubuh;
    uint8_t aturTerbit;
    uint8_t aturDuha;
    uint8_t aturDzuhur;
    uint8_t aturAshar;
    uint8_t aturTerbenam;
    uint8_t aturMaghrib;
    uint8_t aturIsya;

    methode metode;
    double sudutSubuh;
    double sudutTerbit;
    double sudutDuha;
    double sudutMagrib;
    double sudutIsya;

} setting;

const Setting setingAwal =
    {
        tokenSetting, // token pengaturan
        15,           // kecerahan runing text
        20,           // kecepatan efek
        200,          // tunda efek
        5,            // tunda startup
        20,           //pindah tampilan

        108.59, // longitude
        -6.97,  // latitude
        7,      // zona waktu
        0,      //Koreksi Hijriyah

        10, // atur imsyak
        2,  //atur subuh
        0,  //atur terbit
        2,  // atur duha
        2,  //atur dzuhur
        2,  //atur ashar
        2,  //atur terbenam
        2,  //atur magrib
        2,  //atur isya

        shafii, //asar metode
        20.0,   //sudut subuh
        0.833,  //sudut terbit
        4.5,    //sudut duha
        0.833,  //sudut magrib
        18.0    //sudut isya

};

void pengaturanAwal()
{
    for (byte i = 0; i < sizeof(Setting); i++)
    {

        EEPROM.write(i, *((char *)&setingAwal + i));
    }
}

#endif