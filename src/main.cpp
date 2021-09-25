#include "tampilan.h"
#include "setting.h"
#include "Jadwal.h"
#include <SoftwareSerial.h>
#include <Arduino_FreeRTOS.h>

SoftwareSerial bt(2, 3);

#define BUZZ_STARTUP              \
  digitalWrite(BUZZER_PIN, HIGH); \
  delay(100);                     \
  digitalWrite(BUZZER_PIN, LOW);  \
  delay(100);

#define BAUDRATE_BLUETOOTH 115200
#define BOUDRATE_SERIAL 9600

tampilan tampil;
Efek efek;

int index;
char message[50];
char r_char;

void bluetoothEvent();
void update();
void tampilSemua();
//void (*resetFunc)(void) = 0;

void setup()
{
  tampil.mulai();                // dmd di mulai
  Serial.begin(BOUDRATE_SERIAL); // serial mulai
  bt.begin(BAUDRATE_BLUETOOTH);
  Serial.println("Jam Sholat Versi 1.0 \n");
  pinMode(BUZZER_PIN, OUTPUT);

  BUZZ_STARTUP;
  BUZZ_STARTUP;

  if (!rtc.begin())
  {
    Serial.println("Rtc Tidak Terbaca...!!");
  }

  if (EEPROM.read(offsetof(Setting, token)) != tokenSetting)
  {
    pengaturanAwal();
    Serial.println("Pengaturan Awal Di set...");
  }

  //Tanggal tanggal;
  //tanggal.bulan = 2;
  //tanggal.tahun = 2020;
  //tanggal.tanggal = 5;
  //get_prayer_time(tanggal);
  /*
  // update tanggal
  update1Day();
  Tanggal tanggal_j;
  tanggal_j.tanggal = now.day();
  tanggal_j.bulan = now.month();
  tanggal_j.tahun = now.year();

  get_prayer_time(tanggal_j);
  */

  rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  // rtc.adjust(DateTime(2021, 9, 17, 14, 55, 30));

  //display.setDoubleBuffer(1); // swapbuffer
  // tampil startup
  // tampil.startUp();
  // efek.centerCircleOut();
}

void loop()
{
  // update setiap pergantian tanggal (jam 00:00);
  if (tanggalSebelumnya != now.day())
  {
    update1Day();

    Tanggal tanggal_j;
    tanggal_j.tanggal = now.day();
    tanggal_j.bulan = now.month();
    tanggal_j.tahun = now.year();

    get_prayer_time(tanggal_j);
    tanggalSebelumnya = now.day();
  }
  tampilSemua();
}

void tampilSemua()
{
  update(); //update tanggal
  switch (posisi_tampil)
  {
  case mulai:
    display.clear();
    mulaiFullJam();
    pl_dtk = p_dtk - 1;
    posisi_tampil = jamMenitDetik;
    break;

  case jamMenitDetik:
    // updateJadwal();
    if (!updateJadwal())
    {
      if (l_dtk != detik)
      {
        pindahJam(2);
        count_tampil++;
        l_dtk = detik;
      }
      if (l_mnt != menit)
      {
        pindahJam(1);
        l_mnt = menit;
      }
      if (l_jam != jam)
      {
        pindahJam(0);
        l_jam = jam;
      }
      if (count_tampil > EEPROM.read(offsetof(Setting, pindahTampil)))
      {
        efek.centerLineOut();
        count_tampil = 0;
        mem.readChars(100, buffer, sizeof(buffer));
        posisi_tampil = jamtanggal;
      }
    }
    break;

  case jamtanggal:
    // updateJadwal();
    //periksa jadwal dulu
    if (!updateJadwal())
    {
      mem.readChars(100, buffer, sizeof(buffer));
      tampilJamInfo(buffer, false);
      if (count_tampil > EEPROM.read(offsetof(Setting, pindahTampil)) / 10)
      {
        // updateJadwal();
        efek.centerOut(0, 0, 96, 8);
        posisi_tampil = jamJadwal;
        count_tampil = 0;
      }
    }
    break;

  case jamJadwal:
    if (!updateJadwal())
    {
      tampilJamJadwal();
      if (l_dtk != detik)
      {
        count_tampil++;
        l_dtk = detik;
      }
      if (count_tampil > (EEPROM.read(offsetof(Setting, pindahTampil)) / 3))
      {
        count_tampil = 1;
        count_jadwal++;
        if (count_jadwal == 6) // sembunyikan waktu terbenam
          count_jadwal = 7;
        //if (count_jadwal == 3) // sembunyikan waktu duha
        // count_jadwal = 4;

        if (count_jadwal >= JumlahWaktuShalat)
        {
          jamJadwalOut();
          posisi_tampil = mulai;
          count_tampil = 0;
          count_jadwal = 0;
        }
        // updateJadwal();
      }
    }
    break;

  case waktuSolat:
    stateSesudahWaktuSolat = true;
    memset(buffer, 0, sizeof(buffer));
    strcpy(buffer, "Waktu Solat ");
    strcpy_P(buffer + strlen(buffer), namaJadwal[posisiWaktuSolat]);
    update();
    if (clearState)
    {
      display.clear();
      clearState = false;
    }
    tampilJamInfo(buffer, true);

    count_waktu_solat++;
    if (count_waktu_solat >= 2)
    {
      posisi_tampil = mulai;
      digitalWrite(BUZZER_PIN, HIGH);
      delay(1000);
      digitalWrite(BUZZER_PIN, LOW);
      display.clear();
      stateSebelumWaktuSolat = false;
      stateSesudahWaktuSolat = false;
      count_tampil = 0;
      count_waktu_solat = 0;
      update1Day();
    }
    break;

  case hitungMundur:
    update();

    break;

  case modePengaturan:
    break;
  }
}

void bluetoothEvent()
{
  if (bt.available())
  {
    index = 0;
    while (bt.available() > 0)
    {
      r_char = bt.read();
      message[index] = r_char;
      index++;
      message[index] = '\0';
    }

    if (strncmp(message, "SET", 3) == 0)
    {
      bt.println("Masuk Pengaturan");
      display.clear();
      display.setFont(SystemFont5x7Gemuk);
      memset(buffer, 0, sizeof(buffer));
      strcpy(buffer, "Pengaturan");
      display.drawText((display.width() - display.textWidth(buffer)) / 2, 0, buffer);
      Timer1.stop();
      posisi_tampil = modePengaturan;
    }

    if (strncmp(message, "CLS", 3) == 0)
    {
      bt.println("Keluar Pengaturan");
      display.clear();
      Timer1.start();
      posisi_tampil = mulai;
    }

    if (strncmp(message, "KEC", 3) == 0)
    {
      int br = String((String)message[3] + (String)message[4]).toInt();
      Timer1.start();
      display.clear();
      display.setFont(SystemFont5x7);
      strcpy(buffer, "Kecerahan: ");
      bcdToArray(buffer + strlen(buffer), bin2bcd(br));
      display.drawText((display.width() - display.textWidth(buffer)) / 2, 8, buffer);
      display.setFont(SystemFont5x7Gemuk);
      memset(buffer, 0, sizeof(buffer));
      strcpy(buffer, "Pengaturan");
      display.drawText((display.width() - display.textWidth(buffer)) / 2, 0, buffer);
      Timer1.stop();
      if (br > 0)
        EEPROM.write(offsetof(Setting, kecerahan), br);
    }

    Serial.flush();
  }
}
