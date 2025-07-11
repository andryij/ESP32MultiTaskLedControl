#include <Arduino.h>

#define LED_GREEN_PIN GPIO_NUM_4
static const BaseType_t app_cpu0 = 0;
static const BaseType_t app_cpu1 = 1;

static int blink_rate = 500;

//task handles
TaskHandle_t handle_led_blink = NULL;
TaskHandle_t handle_update_rate = NULL;

//task functions
void task_led_blink(void *params){
  Serial.print("task_led_blink running on core ");
  Serial.println(xPortGetCoreID());
  while(1){
    gpio_set_level(LED_GREEN_PIN, 1);
    vTaskDelay(blink_rate / portTICK_PERIOD_MS);
    gpio_set_level(LED_GREEN_PIN, 0);
    vTaskDelay(blink_rate / portTICK_PERIOD_MS);
  }
}

void task_update_rate(void *params){
  Serial.print("task_update_rate running on core ");
  Serial.println(xPortGetCoreID());
  int numread;

  while(1){
    if (Serial.available() > 0)
    {
      numread = Serial.parseInt();
      if ((numread > 499) && (numread < 5001))
      {
        blink_rate = numread;
        Serial.print(" New rate =");
        Serial.println(numread);        
        Serial.println("Enter blink rate in ms (500 .. 5000)");
      }
      else if (numread != 0 )
      {
        Serial.println(" Invalid rate ");
        Serial.println("Enter blink rate in ms (500 .. 5000)");
      }
    }
    vTaskDelay(100 / portTICK_PERIOD_MS);
  }
}


void setup() {
  // config led
  gpio_set_direction(LED_GREEN_PIN, GPIO_MODE_OUTPUT);
  // config serial
  Serial.begin(115200);
  vTaskDelay(1000 / portTICK_PERIOD_MS);
  Serial.println();
  Serial.println("Multi task led control");
  Serial.println("Enter blink rate in ms (500 .. 5000)");


  // config tasks
  xTaskCreatePinnedToCore(
      task_led_blink,
      "led blink task",
      1024,
      NULL,
      2,
      &handle_led_blink,
      app_cpu1
  );

  xTaskCreatePinnedToCore(
      task_update_rate,
      "update blink rate",
      1024,
      NULL,
      1, // make sure it's higher priority than blink task, so that communication is not interrupted by blink
      &handle_update_rate,
      app_cpu0
  );

  // cleanup setup and loop tasks
  vTaskDelete(NULL);
}

void loop() {

}

