#ifndef _FUNGSI_H
#define _FUNGSI_H
#include <Arduino.h>
#include <string.h>
#include "setting.h"

#include <RTClib.h>

#define epochHijriah 1948439.5f      //math.harvard.edu
#define hariPerTahunHijriah 354.365f //math.harvard.edu
#define hariPerPerideHijriah (hariPerTahunHijriah * 30.0)

RTC_DS1307 rtc;

uint16_t jumlahHariPerBulanHijriah[] = {0, 30, 59, 89, 118, 148, 177, 207, 236, 266, 295, 325, 354};
uint16_t jumlahHariPerBulanMasehi[] = {0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334};
double tahunBulan = 354.367068;

#define swap(value) asm volatile("swap %0"     \
                                 : "=r"(value) \
                                 : "0"(value))

struct Tanggal
{
    uint8_t tanggal;
    uint8_t bulan;
    uint32_t tahun;
};

boolean clearState;

const char namaJadwal[][JumlahWaktuShalat] PROGMEM =
    {
        "Imsyak",
        "Subuh",
        "Terbit",
        "Duha",
        "Dhuhur",
        "Ashar",
        "Terbenam",
        "Maghrib",
        "Isya"};

const char namaHari[][7] PROGMEM = {
    "Minggu",
    "Senin",
    "Selasa",
    "Rabu",
    "Kamis",
    "Jum'at",
    "Sabtu",
};

const char namaBulan[][12] PROGMEM = {
    "Januari",
    "Febuari",
    "Maret",
    "April",
    "Mei",
    "Juni",
    "Juli",
    "Agustus",
    "September",
    "Oktober",
    "November",
    "Desember",
};

const char strNamaBulanHijriah[][14] PROGMEM = {
    "Muharram",
    "Shafar",
    "Rabiul Awal",
    "Rabiul Akhir",
    "Jumadil Awal",
    "Jumadil Akhir",
    "Rajab",
    "Sya'ban",
    "Ramadhan",
    "Syawal",
    "Dzulqa'dah",
    "Dzulhijah"};

int posisiWaktuSolat;
boolean stateWaktuSolat[9] = {
    false, // pemberitahuan imsyak
    true,  //pemberitahuan solat subuh
    false, //pemberitahuan terbit
    false, //pemberitahua  duha
    true,  //pemberitahuan dzuhur
    true,  //pemberitahuan ashar
    false, //pemberitahuan terbanam
    true,  //pemberitahuan magrib
    true   //pemberitahuan isa
};

boolean stateSebelumWaktuSolat;
boolean stateSesudahWaktuSolat;
enum posisiTampil
{
    mulai,
    jamMenitDetik,
    jamtanggal,
    jamJadwal,
    waktuSolat,
    hitungMundur,
    modePengaturan
};

static posisiTampil posisi_tampil;
DateTime now;

uint8_t jam;
uint8_t menit;
uint8_t detik;
uint8_t hari;
uint8_t tgl;
uint8_t bln;
uint16_t thn;

uint8_t l_dtk;
uint8_t l_mnt;
uint8_t l_jam;
uint8_t count;
uint8_t p_jam;
uint8_t p_mnt;
uint8_t p_dtk;
uint8_t pl_jam;
uint8_t pl_mnt;
uint8_t pl_dtk;
uint8_t count_tampil;
uint8_t count_jadwal;
uint8_t l_count_jadwal;

uint8_t tanggalSebelumnya = 0;

char buffer[100];

byte bin2bcd(byte val)
{
    return val + 6 * (val / 10);
}

void bcdToArray(char *buf, byte nilai)
{
    swap(nilai);
    *buf++ = (nilai & 0x0f) + 0x30;
    swap(nilai);
    *buf++ = (nilai & 0x0f) + 0x30;
    *buf = 0;
    return;
}

void bufferJamMenitDetik(char *buf, uint8_t bin1, uint8_t bin2, int8_t bin3)
{
    char pembatas = ':';
    byte bcd1 = bin2bcd(bin1);
    byte bcd2 = bin2bcd(bin2);
    byte bcd3 = bin2bcd(bin3);

    bcdToArray(buf + 0, bcd1);
    buf[2] = pembatas;
    bcdToArray(buf + 3, bcd2);
    if (bin3 == -1)
    {
        buf[5] = 0;
    }
    else
    {
        buf[5] = pembatas;
        bcdToArray(buf + 6, bcd3);
        buf[8] = 0;
    }
}

void bufferJamMenit(char *buf, uint8_t bin1, uint8_t bin2)
{
    char pembatas = ':';
    byte bcd1 = bin2bcd(bin1);
    byte bcd2 = bin2bcd(bin2);

    bcdToArray(buf + 0, bcd1);
    buf[2] = pembatas;
    bcdToArray(buf + 3, bcd2);
}

char satuan(byte nilai)
{
    if (nilai < 10)
    {
        return nilai + 48;
    }
    else
    {
        return (nilai % 10) + 48;
    }
}
char puluhan(byte nilai)
{
    if (nilai < 10)
    {
        return '0';
    }
    else
    {
        return ((int)nilai / 10) + 48;
    }
}
String konvert(byte nilai)
{
    if (nilai < 10)
    {
        return "0" + String(nilai);
    }
    else
    {
        return String(nilai);
    }
}

double get_julian_date(Tanggal tanggal)
{
    if (tanggal.bulan <= 2)
    {
        tanggal.tahun -= 1;
        tanggal.bulan += 12;
    }

    double a = floor(tanggal.tahun / 100.0);
    double b = 2 - a + floor(a / 4.0);

    if (tanggal.tahun < 1583)
        b = 0;
    if (tanggal.tahun == 1582)
    {
        if (tanggal.bulan > 10)
            b = -10;
        if (tanggal.bulan == 10)
        {
            b = 0;
            if (tanggal.tanggal > 4)
                b = -10;
        }
    }

    return floor(365.25 * (tanggal.tahun + 4716)) + floor(30.6001 * (tanggal.bulan + 1)) + tanggal.tanggal + b - 1524.5;
}

double konversiTanggalHijriahKeJulianDate(uint16_t th, uint8_t bl, uint8_t tg)
{
    return (epochHijriah + tg + ceil(29.5 * (bl - 1)) + (354L * (th - 1)) + floor((3 + (11 * th)) / 30)) - 1;
}

uint32_t jumlahHariDariTanggal(DateTime now)
{
    byte tahun = now.year() - 2000;
    uint32_t jumlahHari = (uint32_t)tahun * 365;
    for (uint16_t i = 0; i < tahun; i++)
    {
        if (!(i % 4))
        {
            jumlahHari++;
        }
    }
    jumlahHari += jumlahHariPerBulanMasehi[now.month() - 1];
    if ((now.month() > 2) && !(tahun % 4))
    {
        jumlahHari++;
    }
    jumlahHari += now.day();
    return jumlahHari + 259; // base 18 April 1999
}

void masehiKeHijriah(DateTime masehi, jamMenit waktuSholatMagrib, int8_t koreksiHijriah, Tanggal &hijriah)
{
    //uint16_t sisaHari;

    Tanggal tanggal = {masehi.day(), masehi.month(), masehi.year()};
    double julianDate = get_julian_date(tanggal);

    uint16_t menitMagrib = waktuSholatMagrib.jam * 60 + waktuSholatMagrib.menit;
    uint16_t menitSekarang = masehi.hour() * 60 + masehi.minute();

    if (menitSekarang >= menitMagrib)
    {
        julianDate++; //Pergantian hari hijrah pada magrib
    }

    julianDate = 0.5 + floor(julianDate) + koreksiHijriah;

    //Tanggal tanggalHijriah;

    hijriah.tahun = floor(((30 * (julianDate - epochHijriah)) + 10646) / 10631);
    hijriah.bulan = min(12.0, ceil((julianDate - (29 + konversiTanggalHijriahKeJulianDate(hijriah.tahun, 1, 1))) / 29.5) + 1);
    hijriah.tanggal = (julianDate - konversiTanggalHijriahKeJulianDate(hijriah.tahun, hijriah.bulan, 1)) + 1;
}

void masehiKeHijriah(uint32_t HariMasehi, Tanggal *tanggal)
{
    uint16_t sisaHari;
    tanggal->tahun = floor(HariMasehi / tahunBulan);
    sisaHari = HariMasehi - (tahunBulan * tanggal->tahun);

    for (byte i = 0; i < sizeof(jumlahHariPerBulanHijriah); i++)
    {
        if (sisaHari <= jumlahHariPerBulanHijriah[i])
        {
            sisaHari -= jumlahHariPerBulanHijriah[i - 1];
            tanggal->bulan = i;
            break;
        }
    }
    tanggal->tanggal = sisaHari;
    tanggal->tahun += 1420;
}

void ambilHijriah(DateTime now, Tanggal *hijriah)
{
    uint32_t jumlahHari;

    jumlahHari = jumlahHariDariTanggal(now) + EEPROM.read(offsetof(Setting, koreksiHijriah));
    uint8_t jadwalJam = mem.read(Maghrib);
    uint8_t jadwalMenit = mem.read(Maghrib + 10);

    uint16_t unixWaktu = (now.hour() * 60) + now.minute();
    uint16_t unixMagrib = (jadwalJam * 60) + jadwalMenit;
    if (unixWaktu >= unixMagrib)
    {
        jumlahHari++; //Pergantian hari hijrah pada magrib
    }
    masehiKeHijriah(jumlahHari, hijriah);
}

void update()
{
    now = rtc.now();
    jam = now.hour();
    menit = now.minute();
    detik = now.second();
    hari = now.dayOfTheWeek();
    tgl = now.day();
    bln = now.month();
    thn = now.year();

    //masehiKeHijriah(now, {17, 45}, 2, hijriyah);

    //hijriyah.tahun = 2012;
}
void update1Day()
{
    update();
    Tanggal hijriah;
    ambilHijriah(now, &hijriah);
    memset(buffer, 0, sizeof(buffer));
    strcpy_P(buffer, namaHari[hari]);
    strcpy(buffer + strlen(buffer), " ");
    bcdToArray(buffer + strlen(buffer), bin2bcd(tgl));
    strcpy(buffer + strlen(buffer), " ");
    strcpy_P(buffer + strlen(buffer), namaBulan[bln - 1]);
    strcpy(buffer + strlen(buffer), " ");
    bcdToArray(buffer + strlen(buffer), bin2bcd(thn / 100));
    bcdToArray(buffer + strlen(buffer), bin2bcd(thn % 100));

    strcpy(buffer + strlen(buffer), "  ");
    bcdToArray(buffer + strlen(buffer), bin2bcd(hijriah.tanggal));
    strcpy(buffer + strlen(buffer), " ");
    strcpy_P(buffer + strlen(buffer), strNamaBulanHijriah[hijriah.bulan - 1]);
    strcpy(buffer + strlen(buffer), " ");
    bcdToArray(buffer + strlen(buffer), bin2bcd(hijriah.tahun / 100));
    bcdToArray(buffer + strlen(buffer), bin2bcd(hijriah.tahun % 100));
    strcpy(buffer + strlen(buffer), " ");
    strcpy(buffer + strlen(buffer), "H");

    mem.writeChars(100, buffer, sizeof(buffer));
}

boolean updateJadwal()
{
    update();
    int jam, menit;
    boolean mState;
    for (int i = 0; i < 9; i++)
    {
        jam = mem.read(i);
        menit = mem.read(i + 10);

        if (jam == now.hour() && menit == now.minute() && stateWaktuSolat[i])
        {
            if (stateSebelumWaktuSolat)
            {
                posisi_tampil = waktuSolat;
                posisiWaktuSolat = i;
                clearState = true;
            }
            mState = true;
        }
        else
        {
            mState = false;
        }
    }
    if (mem.read(posisiWaktuSolat + 10) != now.minute())
    {
        stateSebelumWaktuSolat = true;
    }
    return mState;
}

#endif