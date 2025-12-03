#include <Servo.h>

#include <EEPROM.h>

#include <stdio.h>
Servo sv1;
Servo sv2;
Servo sv3;
Servo sv4;
Servo sv5;
Servo sv6;
static volatile int8_t run_servo = 0;
uint16_t startAddress = 0;
static volatile uint16_t i_eeprom = 0;
static volatile uint16_t read_eeprom = 0;
uint8_t itr_read_ep = 0;
static volatile bool del_eeprom = 0;
static String pos = "";
static String l_pos = "";
const int numServos = 5;  // Số lượng servo

Servo sv[numServos];

void setup() {
  Serial.begin(9600);
  pinMode(2, INPUT_PULLUP);
  pinMode(3, INPUT_PULLUP);

  attachInterrupt(0, interrupt_1, FALLING);
  attachInterrupt(1, interrupt_2, FALLING);


  sv[0].attach(8);
  sv[1].attach(9);
  sv[2].attach(10);
  sv[3].attach(11);
  sv[4].attach(12);
  sv[5].attach(13);
}


void task_pot() {


  uint16_t angle_A[6];
  uint8_t analog_Read[6] = { 0, 1, 2, 3, 4, 5 };
  uint8_t _mP;
  String pos = "";
  pos = "H";

  for (uint8_t t = 0; t < 6; t++) {

    _mP = 180;
    angle_A[t] = map((uint16_t)analogRead(analog_Read[t]), 0, 1023, 0, _mP);

    _operation_servo(angle_A[t], sv[t]);
  }
  pos = pos + "H";

  Serial.println(pos);
  delay(200);
}

void _operation_servo(uint16_t angle_A, Servo &_sv_A) {

  _sv_A.write(angle_A);
  pos = pos + String(_sv_A.read()) + ";";
}


void task() {


  uint16_t add_[6];

  l_pos = "";
  if (read_eeprom < i_eeprom) {

    l_pos = String(l_pos) + "L";
    for (int i = 0; i < 6; i++) {
      EEPROM.get((read_eeprom + (i * sizeof(uint16_t))), add_[i]);
      //Serial.print( add_[i]);
      l_pos = l_pos + add_[i];
      if (i == 5) {
        continue;
      }
      l_pos = l_pos + ";";
    }

    l_pos = l_pos + "H/L";

    for (int i = 0; i < 6; i++) {
      _operation_servo(add_[i], sv[i]);
    }

    read_eeprom = read_eeprom + 12;
    if (read_eeprom == i_eeprom) {
      read_eeprom = 0;
    }



  } else {
    Serial.println(" none eeprom");
  }

  Serial.println(l_pos);
  delay(500);
}


void interrupt_2() {
  if (run_servo == 0) {
    run_servo = 1;

  } else {
    run_servo = 0;
    del_eeprom = 1;
  }

  Serial.println(" :da vao interrup2r");
}

void set_servo() {

  uint16_t abc = i_eeprom;
  // Serial.println(i_eeprom);
  uint16_t address_sv[6];
  uint16_t value_sv[6];


  delay(20);
  Serial.print("S");
  for (uint8_t i; i < 6; i++) {
    address_sv[i] = (abc + (i * sizeof(uint16_t)));

    value_sv[i] = sv[i].read();
    EEPROM.put(address_sv[i], value_sv[i]);
    delay(20);
    Serial.print(address_sv[i]);
    Serial.print(":");
    Serial.print(value_sv[i]);
    if (i == 5) {
      continue;
    }
    Serial.print(";");
  }
  //Serial.print("S");
  //Serial.print(abc);
  i_eeprom = abc + 12;
  Serial.println(i_eeprom);

  delay(100);
}
void interrupt_1() {
  run_servo = 2;
  Serial.println("  :da vao interrupt 1");
}

void dete_eeprom() {


  if (del_eeprom == 1) {

    for (int i = 0; i < 512; i++) {
      EEPROM.write(i, 0);
      delay(5);  //Phải có delay tối thiểu 5 mili giây giữa mối lần write
    }
    del_eeprom = 0;
  }
}
void loop() {

  dete_eeprom();
  if (Serial.available() > 0) {
    uint16_t virtual_servo[6];

    String read_serial = Serial.readString();
    String c = read_serial;
    String sub_str = c.substring(0, 1);

    char case_ = c[1];
    switch (case_) {
      case 'P':
        Serial.println("da tat dieu khien tay");
        break;
      case 'I':
        goto activate_servo;
        break;
      case 'O':
        task_pot();
        break;
      case 'U':
        set_servo();
        break;
      case 'S':
        task();
        break;
    }

activate_servo:
    char a[8] = { 'I', 'a', 'b', 'c', 'd', 'e', 'f' };
    // I1a2b3c4d5e6f char
    uint8_t b[6];
    for (int i = 0; i < 6; i++) {
      b[i] = c.indexOf(a[i]);
    }
    for (int i = 0; i < 6; i++) {

      _operation_servo(c.substring(b[i] + 1, b[i + 1]).toInt(), sv[i]);
    }



    delay(200);
  }
}
