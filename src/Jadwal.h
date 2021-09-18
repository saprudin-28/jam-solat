
#include <Arduino.h>
#include "Trigonometri.h"
#include "setting.h"
#include "fungsi.h"

#define interasiKomputasiJadwalSholat 5


double lintang;
double bujur;
double zonaWaktu;
double julian_date;
double sun_declination;
double equation_of_time;



double fix_angle(double a)
{
    a = a - 360 * floor(a / 360);
    a = a < 0.0 ? 360 : a;
    return a;
}

double fix_hour(double a)
{
    a = a - 24 * floor(a / 24);
    a = a < 0.0 ? 24 : a;
    return a;
}

double night_portion(double angle)
{
    return 1.0 * angle / 60.0;
}

void get_float_time_parts(double time, int& hours, int& minutes)
{
  time = fix_hour(time + 0.5 / 60);		// add 0.5 minutes to round
  hours = floor(time);
  minutes = floor((time - hours) * 60);
}

double time_diff(double time1, double time2)
{
    return fix_hour(time2 - time1);
}

void getSunEquation(double jd)
{
  double d = jd - 2451545.0;
  double g = fix_angle(357.529 + 0.98560028 * d);
  double q = fix_angle(280.459 + 0.98564736 * d);
  double la = fix_angle(q + 1.915 * dsin(g) + 0.020 * dsin(2 * g));

  double e = 23.439 - 0.00000036 * d;

  sun_declination = darcsin(dsin(e) * dsin(la));
  double ra = darctan2(dcos(e) * dsin(la), dcos(la)) / 15.0;
  ra = fix_hour(ra);
  equation_of_time = q / 15.0 - ra;
}

void adj_times(double times[])
{
    for (int i = 0; i < JumlahWaktuShalat; i++)
    {
        times[i] += zonaWaktu - bujur / 15.0;
    }

    double sudut;

    double night_time = time_diff(times[Terbenam], times[Terbit]); // sunset to sunrise

    // Adjust Fajr
    EEPROM.get(offsetof(Setting, sudutSubuh), sudut);
    double fajr_diff = night_portion(sudut) * night_time;
    if (isnan(times[Subuh]) || time_diff(times[Subuh], times[Terbit]) > fajr_diff)
        times[Subuh] = times[Terbit] - fajr_diff;

    // Adjust Isha
    EEPROM.get(offsetof(Setting, sudutIsya), sudut);
    double isha_diff = night_portion(sudut) * night_time;
    if (isnan(times[Isya]) || time_diff(times[Terbenam], times[Isya]) > isha_diff)
        times[Isya] = times[Terbenam] + isha_diff;

    // Adjust Magrib
    EEPROM.get(offsetof(Setting, sudutMagrib), sudut);
    double magrib_diff = night_portion(sudut) * night_time;
    if (isnan(times[Maghrib]) || time_diff(times[Terbenam], times[Maghrib]) > magrib_diff)
        times[Maghrib] = times[Terbenam] + magrib_diff;

    for (byte i = 0; i < 8; i++)
    {
        times[i + Subuh] += EEPROM.read(offsetof(Setting, aturSubuh) + i) / 60.0; // waktu ihtiyat (pengaman 2menit)
    }
}

double compute_mid_day(double time)
{
    getSunEquation(julian_date + time);
    double z = fix_hour(12 - equation_of_time);
    return z;
}

double compute_time(double sudut, double time)
{
    double z = compute_mid_day(time);
    getSunEquation(julian_date + time);
    double v = 1.0 / 15.0 * darccos((-dsin(sudut) - dsin(sun_declination) * dsin(lintang)) / (dcos(sun_declination) * dcos(lintang)));
    return z + (sudut > 90.0 ? -v : v);
}

double compute_asr(int step, uint8_t time) // shafii step = 1, hannafi step = 2
{
    getSunEquation(julian_date - time);
    double g = -darccot(step + dtan(fabs(lintang - sun_declination)));
    return compute_time(g, time);
}

void compute_times(double times[])
{
    for (int i = 0; i < JumlahWaktuShalat; i++)
        times[i] /= 24.0;

    double sudut;
    EEPROM.get(offsetof(Setting, sudutSubuh), sudut);
    times[Subuh] = compute_time(180.0 - sudut, times[Subuh]);

    EEPROM.get(offsetof(Setting, sudutTerbit), sudut);
    times[Terbit] = compute_time(180.0 - sudut, times[Terbit]);

    EEPROM.get(offsetof(Setting, sudutDuha), sudut);
    times[Duha] = compute_time(180.0 + sudut, times[Duha]);

    times[Dzuhur] = compute_mid_day(times[Dzuhur]);

    int m = EEPROM.read(offsetof(Setting, metode));
    times[Ashar] = compute_asr(1 + m, times[Ashar]);

    EEPROM.get(offsetof(Setting, sudutTerbit), sudut);
    times[Terbenam] = compute_time(sudut, times[Terbenam]);

    EEPROM.get(offsetof(Setting, sudutMagrib), sudut);
    times[Maghrib] = compute_time(sudut, times[Maghrib]);

    EEPROM.get(offsetof(Setting, sudutIsya), sudut);
    times[Isya] = compute_time(sudut, times[Isya]);
}

void coumpute_day_times(double times[])
{
    byte default_time[JumlahWaktuShalat] = {0, 5, 6, 6, 12, 13, 18, 18, 18};
    for (byte i = 0; i < JumlahWaktuShalat; i++)
    {
        times[i] = default_time[i];
    }

    for (int i = 0; i < interasiKomputasiJadwalSholat; i++)
    {
        compute_times(times);
    }

    adj_times(times);
}

void get_prayer_time(Tanggal tanggal)
{
    double waktuShalatDouble[JumlahWaktuShalat];

    EEPROM.get(offsetof(Setting, lintang), lintang);
    EEPROM.get(offsetof(Setting, bujur), bujur);
    zonaWaktu = EEPROM.read(offsetof(Setting, zonaWaktu));

    julian_date = get_julian_date(tanggal) - bujur / (double)(15 * 24);
    coumpute_day_times(waktuShalatDouble);

    waktuShalatDouble[Imsyak] = waktuShalatDouble[Subuh] - (1.0 * EEPROM.read(offsetof(Setting, aturImsyak)) / 60.0); // hitung waktu imsyak

    for (byte i = 0; i < JumlahWaktuShalat; i++)
    {
        int menit;
        int jam;

        get_float_time_parts(waktuShalatDouble[i], jam, menit);

        mem.write(i, jam);
        mem.write(i + 10, menit);

        char d[16];
        memset(d, 0, sizeof(d));
        strcpy_P(d, namaJadwal[i]);
        Serial.print(d);
        Serial.print(" : ");
        if (jam < 10)
            Serial.print(0);
        Serial.print(jam);
        Serial.print(":");
        if (menit < 10)
            Serial.print(0);
        Serial.println(menit);
    }
}
