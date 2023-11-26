#include "RTClib.h"

RTC_DS3231 rtc;


/**
* Função para iniciar o RTC
*/
void Init_RTC() {
  if (!rtc.begin()) {
    Serial.println("Não foi possível encontrar o RTC");
    while (1)
      ;
  } else {
    //rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    if (rtc.lostPower()) {
      Serial.println("RTC lost power, let's set the time!");
      rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    }
  }
}

/**
* Função para atualizar Data e Hora
*/
void Task_RTC(void* pvParameters) {
  while (1) {
    xSemaphoreTake(i2cMutex, portMAX_DELAY);
    DateTime now = rtc.now();

    data = zeroFill(String(now.day(), DEC)) + "/" + zeroFill(String(now.month(), DEC)) + "/" + zeroFill(String(now.year(), DEC));
    hora = zeroFill(String(now.hour(), DEC)) + ":" + zeroFill(String(now.minute(), DEC)) + ":" + zeroFill(String(now.second(), DEC));

    xSemaphoreGive(i2cMutex);
    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }
}


String zeroFill(String value) {
  int num = value.toInt();

  if(num < 10){
    return "0" + value;
  } else{
    return value;
  }
}