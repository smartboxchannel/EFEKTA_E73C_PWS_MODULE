// SDK PORT
extern "C" {
#include "app_gpiote.h"
#include "nrf_gpio.h"
}
#define APP_GPIOTE_MAX_USERS 1

static app_gpiote_user_id_t m_gpiote_user_id;
uint32_t PIN_BUTTON_MASK;
volatile byte buttIntStatus = 0;
byte err_delivery_beat;
byte problem_mode_count;
uint8_t sendTime;
uint16_t m_s_m;
uint16_t m_s_m2;
uint16_t m_s_m_calc;
uint16_t lastm_s_m;
uint16_t lastm_s_m_calc;
uint16_t m_s_mThreshold = 50;
uint16_t m_s_m_calcThreshold = 1;
uint8_t  countbatt = 0;
int16_t myid;
int16_t mypar;
int16_t old_mypar = -1;
int16_t master_id;
bool check;
bool configMode;
bool nosleep = 0;
bool button_flag = 0;
bool onoff = 1;
bool flag_fcount;
bool Ack_TL;
bool Ack_FP;
bool flag_update_transport_param;
bool flag_sendRoute_parent;
bool flag_no_present;
bool flag_nogateway_mode;
bool flag_find_parent_process;
float celsius = 0.0;
float lastcelsius;
float celsiusThreshold = 1.0;
uint32_t rawTemperature = 0;
uint32_t rawTemperature2 = 0;
uint16_t currentBatteryPercent;
uint16_t batteryVoltage;
int16_t linkQuality;
int16_t old_linkQuality;
uint8_t batt_cap;
uint8_t old_batt_cap = 100;
uint32_t BATT_TIME = 21600000; //6 hours
uint32_t SLEEP_TIME_TEMP = 60000; //1 minute
uint32_t SLEEP_TIME;
uint32_t C_BATT_TIME;
//uint32_t SLEEP_NOGW = 60000;
uint32_t SLEEP_TIME_W;
uint32_t previousMillis;
uint32_t lightMillisR;
uint32_t configMillis;

//#define MY_DEBUG
#ifndef MY_DEBUG
#define MY_DISABLED_SERIAL
#endif
#define MY_RADIO_NRF5_ESB
int16_t mtwr;
#define MY_TRANSPORT_WAIT_READY_MS (mtwr)
#define MY_NRF5_ESB_PA_LEVEL (NRF5_PA_MAX)
#define SN "PWS E73C"
#define SV "1.0"

#include <MySensors.h>

#define MSM_SENS_ID 1
#define MSM_SENS_C_ID 2
#define TEMP_INT_ID 3
#define SEND_TIME_CHILD_ID 200

MyMessage msg_msm(MSM_SENS_ID, V_LEVEL);
MyMessage msg_msm2(MSM_SENS_C_ID, V_LEVEL);
MyMessage msg_temp(TEMP_INT_ID, V_TEMP);
MyMessage conf_stMsg(SEND_TIME_CHILD_ID, V_VAR1);


void before()
{
  board_Init();
  happy_init();
}


void presentation()
{
  check = sendSketchInfo(SN, SV);
  if (!check) {
    _transportSM.failedUplinkTransmissions = 0;
    wait(15);
    check = sendSketchInfo(SN, SV);
    _transportSM.failedUplinkTransmissions = 0;
  }

  if (check) {
    blinky(1, 1, BLUE_LED);
  } else {
    _transportSM.failedUplinkTransmissions = 0;
    blinky(1, 1, RED_LED);
  }

  check = present(MSM_SENS_C_ID, S_CUSTOM, "% - SOIL MOISTURE");
  if (!check) {
    _transportSM.failedUplinkTransmissions = 0;
    wait(30);
    check = present(MSM_SENS_C_ID, S_CUSTOM, "% - SOIL MOISTURE");
    _transportSM.failedUplinkTransmissions = 0;
  }

  if (check) {
    blinky(1, 1, BLUE_LED);
  } else {
    _transportSM.failedUplinkTransmissions = 0;
    blinky(1, 1, RED_LED);
  }

  check = present(MSM_SENS_ID, S_CUSTOM, "DATA - SOIL MOISTURE");
  if (!check) {
    _transportSM.failedUplinkTransmissions = 0;
    wait(45);
    check = present(MSM_SENS_ID, S_CUSTOM, "DATA - SOIL MOISTURE");
    _transportSM.failedUplinkTransmissions = 0;
  }

  if (check) {
    blinky(1, 1, BLUE_LED);
  } else {
    _transportSM.failedUplinkTransmissions = 0;
    blinky(1, 1, RED_LED);
  }

  check = present(TEMP_INT_ID, S_TEMP, "TEMPERATURE");
  if (!check) {
    _transportSM.failedUplinkTransmissions = 0;
    wait(60);
    check = present(TEMP_INT_ID, S_TEMP, "TEMPERATURE");
    _transportSM.failedUplinkTransmissions = 0;
  }

  if (check) {
    blinky(1, 1, BLUE_LED);
  } else {
    _transportSM.failedUplinkTransmissions = 0;
    blinky(1, 1, RED_LED);
  }

  check = present(SEND_TIME_CHILD_ID, S_CUSTOM, "SET SEND TIME");
  if (!check) {
    _transportSM.failedUplinkTransmissions = 0;
    wait(75);
    check = present(SEND_TIME_CHILD_ID, S_CUSTOM, "SET SEND TIME");
    _transportSM.failedUplinkTransmissions = 0;
  }

  if (check) {
    blinky(1, 1, BLUE_LED);
  } else {
    _transportSM.failedUplinkTransmissions = 0;
    blinky(1, 1, RED_LED);
  }

  check = send(conf_stMsg.set(sendTime));
  if (!check) {
    _transportSM.failedUplinkTransmissions = 0;
    wait(90);
    send(conf_stMsg.set(sendTime));
    _transportSM.failedUplinkTransmissions = 0;
  }

  if (check) {
    blinky(1, 1, BLUE_LED);
  } else {
    _transportSM.failedUplinkTransmissions = 0;
    blinky(1, 1, RED_LED);
  }

}

void setup() {
  interrupt_Init();
  config_Happy_node();
  sensors_Init();
}

void loop() {
  if (flag_update_transport_param == 1) {
    update_Happy_transport();
  }
  if (flag_sendRoute_parent == 1) {
    present_only_parent();
  }

  if (isTransportReady() == true) {
    if (flag_nogateway_mode == 0) {
      if (flag_find_parent_process == 1) {
        find_parent_process();
      }
      if (configMode == 0) {
        if (buttIntStatus == PIN_BUTTON) {
          if (digitalRead(PIN_BUTTON) == 0 && button_flag == 0) {
            button_flag = 1;
            previousMillis = millis();
            ledsOff();
          }
          if (digitalRead(PIN_BUTTON) == 0 && button_flag == 1) {
            if ((millis() - previousMillis > 0) && (millis() - previousMillis <= 1750)) {
              if (millis() - lightMillisR > 70) {
                lightMillisR = millis();
                onoff = !onoff;
                digitalWrite(GREEN_LED, onoff);
              }
            }
            if ((millis() - previousMillis > 1750) && (millis() - previousMillis <= 2000)) {
              ledsOff();
            }
            if ((millis() - previousMillis > 2000) && (millis() - previousMillis <= 3750)) {
              if (millis() - lightMillisR > 55) {
                lightMillisR = millis();
                onoff = !onoff;
                digitalWrite(BLUE_LED, onoff);
              }
            }
            if ((millis() - previousMillis > 3750) && (millis() - previousMillis <= 4000)) {
              ledsOff();
            }
            if ((millis() - previousMillis > 4000) && (millis() - previousMillis <= 5750)) {
              if (millis() - lightMillisR > 40) {
                lightMillisR = millis();
                onoff = !onoff;
                digitalWrite(BLUE_LED, onoff);
              }
            }
            if ((millis() - previousMillis > 5750) && (millis() - previousMillis <= 6000)) {
              ledsOff();
            }
            if ((millis() - previousMillis > 6000) && (millis() - previousMillis <= 7750)) {
              if (millis() - lightMillisR > 25) {
                lightMillisR = millis();
                onoff = !onoff;
                digitalWrite(RED_LED, onoff);
              }
            }

            if (millis() - previousMillis > 7750) {
              ledsOff();
            }
          }

          if (digitalRead(PIN_BUTTON) == 1 && button_flag == 1) {
            if (millis() - previousMillis <= 1750 && button_flag == 1)
            {
              ledsOff();
              itemp();
              msm();
              button_flag = 0;
              buttIntStatus = 0;
              nosleep = 0;
            }
            if ((millis() - previousMillis > 2000) && (millis() - previousMillis <= 3750) && button_flag == 1)
            {
              ledsOff();
              presentation();
              button_flag = 0;
              buttIntStatus = 0;
              nosleep = 0;
            }
            if ((millis() - previousMillis > 4000) && (millis() - previousMillis <= 5750) && button_flag == 1)
            {
              ledsOff();
              NRF5_ESB_startListening();
              wait(10);
              configMillis = millis();
              configMode = 1;
              button_flag = 0;
              buttIntStatus = 0;
            }
            if ((millis() - previousMillis > 6000) && (millis() - previousMillis <= 7750) && button_flag == 1)
            {
              ledsOff();
              button_flag = 0;
              buttIntStatus = 0;
              nosleep = 0;
              //new_device();
            }
            if (((millis() - previousMillis > 1750) && (millis() - previousMillis <= 2000)) || ((millis() - previousMillis > 3750) && (millis() - previousMillis <= 4000)) || ((millis() - previousMillis > 5750) && (millis() - previousMillis <= 6000)) || ((millis() - previousMillis > 7750)) && button_flag == 1)
            {
              ledsOff();
              button_flag = 0;
              buttIntStatus = 0;
              nosleep = 0;
            }
          }
        } else {
          countbatt++;
          if (countbatt == C_BATT_TIME) {
            wait(50);
            batteryVoltage = hwCPUVoltage();
            wait(5);
          }
          itemp();
          msm ();
          if (countbatt == C_BATT_TIME) {
            sendBatteryStatus(1);
            countbatt = 0;
          }
          nosleep = 0;
        }
      } else {
        if (millis() - configMillis > 15000) {
          blinky(3, 3, GREEN_LED);
          configMode = 0;
          nosleep = 0;
          button_flag = 0;
          buttIntStatus = 0;
        }
      }

    } else {
      if (buttIntStatus == PIN_BUTTON) {
        if (digitalRead(PIN_BUTTON) == 0 && button_flag == 0) {
          button_flag = 1;
          previousMillis = millis();
          ledsOff();
        }
        if (digitalRead(PIN_BUTTON) == 0 && button_flag == 1) {
          if ((millis() - previousMillis > 0) && (millis() - previousMillis <= 1750)) {
            if (millis() - lightMillisR > 25) {
              lightMillisR = millis();
              onoff = !onoff;
              digitalWrite(RED_LED, onoff);
            }
          }
          if ((millis() - previousMillis > 1750) && (millis() - previousMillis <= 2000)) {
            ledsOff();
          }
          if ((millis() - previousMillis > 2000) && (millis() - previousMillis <= 3750)) {
            if (millis() - lightMillisR > 50) {
              lightMillisR = millis();
              onoff = !onoff;
              digitalWrite(RED_LED, onoff);
            }
          }
          if (millis() - previousMillis > 3750) {
            ledsOff();
          }
        }

        if (digitalRead(PIN_BUTTON) == 1 && button_flag == 1) {
          if (millis() - previousMillis <= 1750 && button_flag == 1)
          {
            ledsOff();
            blinky(2, 2, BLUE_LED);
            button_flag = 0;
            buttIntStatus = 0;
            check_parent();
            nosleep = 0;
          }
          if ((millis() - previousMillis > 2000) && (millis() - previousMillis <= 3750) && button_flag == 1)
          {
            ledsOff();
            button_flag = 0;
            buttIntStatus = 0;
            nosleep = 0;
            //new_device();
          }

          if (((millis() - previousMillis > 1750) && (millis() - previousMillis <= 2000)) || ((millis() - previousMillis > 3750)) && button_flag == 1)
          {
            ledsOff();
            nosleep = 0;
            button_flag = 0;
            buttIntStatus = 0;
          }
        }
      } else {
        check_parent();
      }
    }
  }

  if (_transportSM.failureCounter > 0)
  {
    _transportConfig.parentNodeId = loadState(101);
    _transportConfig.nodeId = myid;
    _transportConfig.distanceGW = loadState(103);
    mypar = _transportConfig.parentNodeId;
    nosleep = 0;
    flag_fcount = 1;
    err_delivery_beat = 5;
    happy_node_mode();
    gateway_fail();
  }

  if (nosleep == 0) {

    buttIntStatus = 0;

    wait(10);
    sleep(SLEEP_TIME_W, false);
    wait(50);
    nosleep = 1;
  }
}


void itemp() {
  for (byte i = 0; i < 10; i++) {
    NRF_TEMP->TASKS_START = 1;
    wait(5);
    while (!(NRF_TEMP->EVENTS_DATARDY)) {}
    rawTemperature = NRF_TEMP->TEMP;
    rawTemperature2 = rawTemperature2 + rawTemperature;
    wait(5);
  }
  celsius = ((((float)rawTemperature2) / 10) / 4.0);
  rawTemperature2 = 0;
  if (abs(celsius - lastcelsius) >= celsiusThreshold) {
    check = send(msg_temp.set(celsius, 1));
    if (!check) {
      _transportSM.failedUplinkTransmissions = 0;
      wait(20);
      check = send(msg_temp.set(celsius, 1));
      if (!check) {
        _transportSM.failedUplinkTransmissions = 0;
        wait(40);
        check = send(msg_temp.set(celsius, 1));
      }
    }
    if (check) {
      blinky(1, 1, GREEN_LED);
      err_delivery_beat = 0;
      if (flag_nogateway_mode == 1) {
        flag_nogateway_mode = 0;
        CORE_DEBUG(PSTR("MyS: NORMAL GATEWAY MODE\n"));
        err_delivery_beat = 0;
      }
    } else {
      blinky(1, 1, RED_LED);
      _transportSM.failedUplinkTransmissions = 0;
      if (err_delivery_beat < 5) {
        err_delivery_beat++;
      }
      if (err_delivery_beat == 4) {
        if (flag_nogateway_mode == 0) {
          gateway_fail();
          CORE_DEBUG(PSTR("MyS: LOST GATEWAY MODE\n"));
        }
      }
    }
  }
}

void msm () {
  digitalWrite(PIN_POWER_PWS, LOW);
  wait(20);
  for (byte i = 0; i < 5; i++) {
    m_s_m = analogRead(PIN_SENS_PWS);
    m_s_m2 = m_s_m2 + m_s_m;
    wait(10);
  }
  m_s_m = m_s_m2 / 5;
  m_s_m2 = 0;
  digitalWrite(PIN_POWER_PWS, HIGH);
  wait(20);
  //if (m_s_m > 3300) {
  //  m_s_m = 3300;
  //}
  //if (m_s_m < 2000) {
  // m_s_m = 2000;
  // }
  m_s_m_calc = map(m_s_m, 3300, 1300, 0, 100);

  if (abs(m_s_m_calc - lastm_s_m_calc) >= m_s_m_calcThreshold) {
    check = send(msg_msm2.set(m_s_m_calc));
    if (!check) {
      _transportSM.failedUplinkTransmissions = 0;
      wait(20);
      check = send(msg_msm2.set(m_s_m_calc));
      if (!check) {
        _transportSM.failedUplinkTransmissions = 0;
        wait(40);
        check = send(msg_msm2.set(m_s_m_calc));
      }
    }
    if (check) {
      blinky(1, 1, BLUE_LED);
      err_delivery_beat = 0;
      if (flag_nogateway_mode == 1) {
        flag_nogateway_mode = 0;
        CORE_DEBUG(PSTR("MyS: NORMAL GATEWAY MODE\n"));
        err_delivery_beat = 0;
      }
    } else {
      blinky(1, 1, RED_LED);
      _transportSM.failedUplinkTransmissions = 0;
      if (err_delivery_beat < 5) {
        err_delivery_beat++;
      }
      if (err_delivery_beat == 4) {
        if (flag_nogateway_mode == 0) {
          gateway_fail();
          CORE_DEBUG(PSTR("MyS: LOST GATEWAY MODE\n"));
        }
      }
    }
  }

  if (abs(m_s_m - lastm_s_m) >= m_s_mThreshold) {
    check = send(msg_msm.set(m_s_m));
    if (!check) {
      _transportSM.failedUplinkTransmissions = 0;
      wait(20);
      check = send(msg_msm.set(m_s_m));
      if (!check) {
        _transportSM.failedUplinkTransmissions = 0;
        wait(40);
        check = send(msg_msm.set(m_s_m));
      }
    }
    if (check) {
      blinky(1, 1, BLUE_LED);
    } else {
      blinky(1, 1, RED_LED);
      _transportSM.failedUplinkTransmissions = 0;
    }
  }
}

void board_Init() {
  pinMode(PIN_BUTTON, INPUT);
  pinMode(PIN_SENS_PWS, INPUT);
  pinMode(PIN_POWER_PWS, OUTPUT);
  digitalWrite(PIN_POWER_PWS, HIGH);
  pinMode(RED_LED, OUTPUT);
  pinMode(GREEN_LED, OUTPUT);
  pinMode(BLUE_LED, OUTPUT);
  ledsOff();

  NRF_POWER->DCDCEN = 1;
  wait(5);
#ifndef MY_DEBUG
  NRF_UART0->ENABLE = 0;
  wait(5);
#endif

  NRF_NFCT->TASKS_DISABLE = 1;
  NRF_NVMC->CONFIG = 1;
  NRF_UICR->NFCPINS = 0;
  NRF_NVMC->CONFIG = 0;
  NRF_SAADC ->ENABLE = 0;
  NRF_PWM0  ->ENABLE = 0;
  NRF_PWM1  ->ENABLE = 0;
  NRF_PWM2  ->ENABLE = 0;
  NRF_TWIM1 ->ENABLE = 0;
  NRF_TWIS1 ->ENABLE = 0;

  analogReadResolution(12);
  analogReference(AR_VDD4);

  NRF_CLOCK->TASKS_HFCLKSTART = 1;
  NRF_TEMP->TASKS_STOP;
  NRF_TEMP->EVENTS_DATARDY = 0;
  NRF_TEMP->INTENSET = 1;
  NRF_RADIO->TXPOWER = 8;

  sendTime = loadState(200);
  if (sendTime > 250) {
    sendTime = 30;
    saveState(200, sendTime);
  }
  //sendTime = 30; //for test

  SLEEP_TIME = SLEEP_TIME_TEMP * sendTime;
  C_BATT_TIME = BATT_TIME / SLEEP_TIME;
  SLEEP_TIME_W = SLEEP_TIME;

  wait(50);
  batteryVoltage = hwCPUVoltage();
  wait(5);

  blinky(1, 1, BLUE_LED);
  wait(200);
  blinky(1, 1, RED_LED);
  wait(200);
  blinky(1, 1, GREEN_LED);
}


void blinky(uint8_t pulses, uint8_t repit, uint8_t ledColor) {
  for (int x = 0; x < repit; x++) {
    if (x > 0) {
      sleep(100);
    }
    for (int i = 0; i < pulses; i++) {
      if (i > 0) {
        sleep(40);
      }
      digitalWrite(ledColor, LOW);
      sleep(10);
      digitalWrite(ledColor, HIGH);
    }
  }
}


void ledsOff() {
  digitalWrite(RED_LED, HIGH);
  digitalWrite(GREEN_LED, HIGH);
  digitalWrite(BLUE_LED, HIGH);
}


void happy_init() {
  //hwWriteConfig(EEPROM_NODE_ID_ADDRESS, 255); // ******************** checking the node config reset *************************

  if (hwReadConfig(EEPROM_NODE_ID_ADDRESS) == 0) {
    hwWriteConfig(EEPROM_NODE_ID_ADDRESS, 255);
  }
  if (loadState(100) == 0) {
    saveState(100, 255);
  }
  CORE_DEBUG(PSTR("EEPROM NODE ID: %d\n"), hwReadConfig(EEPROM_NODE_ID_ADDRESS));
  CORE_DEBUG(PSTR("USER MEMORY SECTOR NODE ID: %d\n"), loadState(100));

  if (hwReadConfig(EEPROM_NODE_ID_ADDRESS) == 255) {
    mtwr = 0;
  } else {
    mtwr = 10000;
    no_present();
  }
  CORE_DEBUG(PSTR("MY_TRANSPORT_WAIT_MS: %d\n"), mtwr);
}


void interrupt_Init() {
  //***
  //SET
  //NRF_GPIO_PIN_NOPULL
  //NRF_GPIO_PIN_PULLUP
  //NRF_GPIO_PIN_PULLDOWN
  //***
  nrf_gpio_cfg_input(PIN_BUTTON, NRF_GPIO_PIN_NOPULL);
  APP_GPIOTE_INIT(APP_GPIOTE_MAX_USERS);
  PIN_BUTTON_MASK = 1 << PIN_BUTTON;
  app_gpiote_user_register(&m_gpiote_user_id, PIN_BUTTON_MASK, PIN_BUTTON_MASK, gpiote_event_handler);
  app_gpiote_user_enable(m_gpiote_user_id);
  buttIntStatus = 0;
}



void gpiote_event_handler(uint32_t event_pins_low_to_high, uint32_t event_pins_high_to_low)
{
  MY_HW_RTC->CC[0] = (MY_HW_RTC->COUNTER + 2);

  if (PIN_BUTTON_MASK & event_pins_high_to_low) {
    if (buttIntStatus == 0) {
      buttIntStatus = PIN_BUTTON;
    }
  }
}


void sensors_Init() {
  if (flag_nogateway_mode == 0) {

    sendBatteryStatus(0);

  } else {
    blinky(5, 3, RED_LED);
  }
}



void config_Happy_node() {
  if (mtwr == 0) {
    myid = getNodeId();
    saveState(100, myid);
    mypar = _transportConfig.parentNodeId;
    old_mypar = mypar;
    master_id = 0; // *************************** master slave mode is not initialized in this example, ..stub *******************************
    saveState(101, mypar);
    saveState(102, _transportConfig.distanceGW);
  }
  if (mtwr != 0) {
    myid = getNodeId();
    if (myid != loadState(100)) {
      saveState(100, myid);
    }
    if (isTransportReady() == true) {
      mypar = _transportConfig.parentNodeId;
      master_id = 0; // *************************** master slave mode is not initialized in this example, ..stub *******************************
      if (mypar != loadState(101)) {
        saveState(101, mypar);
      }
      if (_transportConfig.distanceGW != loadState(102)) {
        saveState(102, _transportConfig.distanceGW);
      }
      present_only_parent();
    }
    if (isTransportReady() == false)
    {
      no_present();
      flag_fcount = 1;
      err_delivery_beat = 5;
      _transportConfig.nodeId = myid;
      _transportConfig.parentNodeId = loadState(101);
      _transportConfig.distanceGW = loadState(102);
      mypar = _transportConfig.parentNodeId;
      happy_node_mode();
      gateway_fail();
    }
  }
}


void new_device() {
  hwWriteConfig(EEPROM_NODE_ID_ADDRESS, 255);
  saveState(100, 255);
  hwReboot();
}


void sendBatteryStatus(bool start) {
  batt_cap = battery_level_in_percent(batteryVoltage);
  if (start == 1) {
    //if (batt_cap < old_batt_cap) {
    check = sendBatteryLevel(battery_level_in_percent(batteryVoltage));
    old_batt_cap = batt_cap;
    if (!check) {
      _transportSM.failedUplinkTransmissions = 0;
      wait(20);
      check = sendBatteryLevel(battery_level_in_percent(batteryVoltage));
      if (!check) {
        _transportSM.failedUplinkTransmissions = 0;
        wait(40);
        check = sendBatteryLevel(battery_level_in_percent(batteryVoltage));
      }
    }
    //}
  } else {
    check = sendBatteryLevel(battery_level_in_percent(batteryVoltage));
    old_batt_cap = batt_cap;
    if (!check) {
      _transportSM.failedUplinkTransmissions = 0;
      wait(20);
      check = sendBatteryLevel(battery_level_in_percent(batteryVoltage));
      if (!check) {
        _transportSM.failedUplinkTransmissions = 0;
        wait(40);
        check = sendBatteryLevel(battery_level_in_percent(batteryVoltage));
      }
    }
  }
}


void no_present() {
  _coreConfig.presentationSent = true;
  _coreConfig.nodeRegistered = true;
}


void update_Happy_transport() {
  CORE_DEBUG(PSTR("MyS: UPDATE TRANSPORT CONFIGURATION\n"));
  mypar = _transportConfig.parentNodeId;
  master_id = 0; // *************************** master slave mode is not initialized in this example, ..stub *******************************
  if (mypar != loadState(101))
  {
    saveState(101, mypar);
  }
  if (_transportConfig.distanceGW != loadState(102))
  {
    saveState(102, _transportConfig.distanceGW);
  }
  present_only_parent();
  wait(50);
  nosleep = 0;
  flag_update_transport_param = 0;
}


void present_only_parent() {
  if (old_mypar != mypar) {
    CORE_DEBUG(PSTR("MyS: SEND LITTLE PRESENT:) WITH PARENT ID\n"));
    if (_sendRoute(build(_msgTmp, 0, NODE_SENSOR_ID, C_INTERNAL, 6).set(mypar))) {
      flag_sendRoute_parent = 0;
      old_mypar = mypar;
    } else {
      flag_sendRoute_parent = 1;
    }
  }
}


void happy_node_mode() {
  _transportSM.findingParentNode = false;
  _transportSM.transportActive = true;
  _transportSM.uplinkOk = true;
  _transportSM.pingActive = false;
  _transportSM.failureCounter = 0;
  _transportSM.uplinkOk = true;
  _transportSM.failureCounter = 0u;
  _transportSM.failedUplinkTransmissions = 0u;
  transportSwitchSM(stReady);
  CORE_DEBUG(PSTR("TRANSPORT: %d\n"), isTransportReady());
}


void gateway_fail() {
  flag_nogateway_mode = 1;
  flag_update_transport_param = 0;
  //SLEEP_TIME_W = SLEEP_NOGW;
  interrupt_Init();
}


void check_parent() {
  _transportSM.findingParentNode = true;
  CORE_DEBUG(PSTR("MyS: SEND FIND PARENT REQUEST, WAIT RESPONSE\n"));
  _sendRoute(build(_msg, 255, NODE_SENSOR_ID, C_INTERNAL, 7).set(""));
  wait(600, C_INTERNAL, 8);
  if (_msg.sensor == 255) {
    if (mGetCommand(_msg) == 3) {
      if (_msg.type == 8) {
        Ack_FP = 1;
        CORE_DEBUG(PSTR("MyS: PARENT RESPONSE FOUND\n"));
      }
    }
  }
  if (Ack_FP == 1) {
    CORE_DEBUG(PSTR("MyS: FIND PARENT PROCESS\n"));
    Ack_FP = 0;
    transportSwitchSM(stParent);
    flag_nogateway_mode = 0;
    flag_find_parent_process = 1;
    problem_mode_count = 0;
  } else {
    _transportSM.findingParentNode = false;
    CORE_DEBUG(PSTR("MyS: PARENT RESPONSE NOT FOUND\n"));
    _transportSM.failedUplinkTransmissions = 0;
    CORE_DEBUG(PSTR("TRANSPORT: %d\n"), isTransportReady());
    nosleep = 0;
    if (problem_mode_count < 3) {
      CORE_DEBUG(PSTR("PROBLEM MODE COUNTER: %d\n"), problem_mode_count);
      problem_mode_count++;
      SLEEP_TIME_W = SLEEP_TIME_W + SLEEP_TIME_W;
    }
  }
}


void find_parent_process() {
  flag_update_transport_param = 1;
  flag_find_parent_process = 0;
  CORE_DEBUG(PSTR("MyS: STANDART TRANSPORT MODE IS RESTORED\n"));
  err_delivery_beat = 0;
  SLEEP_TIME_W = SLEEP_TIME;
  nosleep = 0;
}


void receive(const MyMessage & message)
{
  if (message.sensor == SEND_TIME_CHILD_ID) {
    if (message.type == V_VAR1) {
      sendTime = message.getByte();
      saveState(200, sendTime);
      wait(50);
      send(conf_stMsg.set(sendTime));
      wait(50);
      blinky(3, 3, GREEN_LED);
      configMode = 0;
      nosleep = 0;
    }
  }
}
