# include <DHT.h>
# include <DS3231.h>
# include <SPI.h>
# include <SD.h>


#define DHT_PIN 2
#define DHTTYPE DHT22
DHT dht(DHT_PIN, DHTTYPE);
const int chipSelect = 10;
const long longDelay = 300000;
bool SD_CardError = false;
DS3231 rtc(SDA, SCL);

class EnviroSampler
{
  public:
    static float getTemperature()
    {
      float temp = dht.readTemperature();
      return temp;
    }
    static float getHumidity()
    {
      float humi = dht.readHumidity();
      return humi;
    }
};

class DateOracle
{
  public:
    static void Initialize_RTC()
    {
      rtc.begin();
      rtc.setDOW(TUESDAY); // Not used
      rtc.setTime(0, 0, 0);
      rtc.setDate(11, 17, 2020);

    }
    static String getDateString()
    {
      String date_ = rtc.getDateStr();
      return date_;
    }

    static String getTimeString()
    {
      String time_ = rtc.getTimeStr();
      return time_;
    }
};
class EnviroLogger
{
  public:
    static String Write(String dateVal, String timeVal, float tempVal, float humiVal)
    {
      return " | " + dateVal + " | " + timeVal + " | " + tempVal + "°C | " + humiVal + "% | ";
    }
};
class SystemLogger
{
  public:
    static void Initialize_SDCard()
    {
      if (!SD.begin(chipSelect))
      {
        Serial.println("SD CARD ERROR...");
        return;
      }
    }
    static void serialTitle()
    {
      Serial.println("| DATE | TIME | TEMPERATURE | HUMIDITY |"); //sets the "title" for the reading starts in the serial monitor
    }
    static void Log()
    {

      File dataFile = SD.open("datalog.txt", FILE_WRITE);
      float temp = EnviroSampler::getTemperature();
      float humi = EnviroSampler::getHumidity();
      String timeVal = DateOracle::getTimeString();
      String dateVal = DateOracle::getDateString();
      if (dataFile) //if data is available
      {

        dataFile.println(EnviroLogger::Write(dateVal, timeVal, temp, humi));
        Serial.println(EnviroLogger::Write(dateVal, timeVal, temp, humi));
        SD_CardError = false;
        dataFile.close();
      }
      else
      {
        Serial.println("FAILED TO WRITE TO SD CARD");
        SD_CardError = true;
      }
      SystemLogger::systemLog();
    }

    static void systemLog()
    {
      File systemFile = SD.open("sysLogg.txt", FILE_WRITE);
      float temp = EnviroSampler::getTemperature();
      float humi = EnviroSampler::getHumidity();
      String timeVal = DateOracle::getTimeString();
      String dateVal = DateOracle::getDateString();

      if (systemFile)
      {

        systemFile.println(" | " + dateVal + " | " + timeVal + " | " + "SYSTEM START..." + " | ");
        systemFile.println(" | " + dateVal + " | " + timeVal + " | " + "TEMP READING..." + " | ");
        if (isnan(humi) || isnan(temp))
        {
          systemFile.println(" | " + dateVal + " | " + timeVal + " | " + "DHT22 READING FAILED..." + " | ");
        }
        else
        {
          systemFile.println(" | " + dateVal + " | " + timeVal + " | " + temp + " °C" + " | ");
          systemFile.println(" | " + dateVal + " | " + timeVal + " | " + humi + " %" + " | ");
        }

        systemFile.println(" | " + dateVal + " | " + timeVal + " | " + "IN PROCESS : WRITE TO SD CARD" + " | ");
        if (SD_CardError == false)
        {
          systemFile.println(" | " + dateVal + " | " + timeVal + " | " + "SUCCESS : WRITE TO SD CARD" + " | ");
        }

        else if (SD_CardError == true)
        {
          systemFile.println(" | " + dateVal + " | " + timeVal + " | " + "FAILED : WRITE TO SD CARD" + " | ");
        }
        systemFile.close();

      }
      else
      {
        Serial.println(" SD CARD WRITE SYSTEM LOG ERROR...");
      }

    }
};


void setup()
{
  Serial.begin(9600);
  dht.begin();
  DateOracle::Initialize_RTC();
  SystemLogger::Initialize_SDCard();
  SystemLogger::serialTitle();
}

void loop()
{
  SystemLogger::Log();
  delay(longDelay);
}
