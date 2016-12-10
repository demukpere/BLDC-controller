int speedBuf, speedCMD, phase;


#define HALL_SENSOR_MASK B11100000
#define DRIVE_MASK B00111111

#define ROTOR_DIRECTION_PIN 7	//CAUTION! Use with arduino API only
#define SPEED_CONTROL_PIN A0	//CAUTION! Use with arduino API only

//Hall sensor pin on Arduino board
#define HALL_SENSOR_A_INO 5
#define HALL_SENSOR_B_INO 6
#define HALL_SENSOR_C_INO 7

//Hall sensor pin on ATMEGA328P
#define HALL_SENSOR_A_AVR PORTD5
#define HALL_SENSOR_B_AVR PORTD6
#define HALL_SENSOR_C_AVR PORTD7

//Drive pins on Arduino board
#define PHASE_C_LOW_INO 13
#define PHASE_B_LOW_INO 12
#define PHASE_C_HIGH_INO 11
#define PHASE_B_HIGH_INO 10
#define PHASE_A_HIGH_INO 9
#define PHASE_A_LOW_INO 8

//Drive pins on ATMEGA328P board
#define PHASE_C_LOW_AVR PORTB5
#define PHASE_B_LOW_AVR PORTB4
#define PHASE_C_HIGH_AVR PORTB3
#define PHASE_B_HIGH_AVR PORTB2
#define PHASE_A_HIGH_AVR PORTB1
#define PHASE_A_LOW_AVR PORTB0

//hall_sensor commutation sequence
#define PHASE_1_HALL_SENSOR ((1 << HALL_SENSOR_C_AVR) | (1 << HALL_SENSOR_A_AVR))
#define PHASE_2_HALL_SENSOR (1 << HALL_SENSOR_C_AVR)
#define PHASE_3_HALL_SENSOR ((1 << HALL_SENSOR_C_AVR) | (1 << HALL_SENSOR_B_AVR))
#define PHASE_4_HALL_SENSOR (1 << HALL_SENSOR_B_AVR)
#define PHASE_5_HALL_SENSOR ((1 << HALL_SENSOR_B_AVR) | (1 << HALL_SENSOR_A_AVR))
#define PHASE_6_HALL_SENSOR (1 << HALL_SENSOR_A_AVR)

//Clockwise commutation sequence
#define PHASE_1_DRIVE_CW ((1 << PHASE_A_HIGH_AVR) | (1 << PHASE_B_LOW_AVR))
#define PHASE_2_DRIVE_CW ((1 << PHASE_C_HIGH_AVR) | (1 << PHASE_B_LOW_AVR))
#define PHASE_3_DRIVE_CW ((1 << PHASE_C_HIGH_AVR) | (1 << PHASE_A_LOW_AVR))
#define PHASE_4_DRIVE_CW ((1 << PHASE_B_HIGH_AVR) | (1 << PHASE_A_LOW_AVR))
#define PHASE_5_DRIVE_CW ((1 << PHASE_B_HIGH_AVR) | (1 << PHASE_C_LOW_AVR))
#define PHASE_6_DRIVE_CW ((1 << PHASE_A_HIGH_AVR) | (1 << PHASE_C_LOW_AVR))

//Counter-clockwise commutation sequence
#define PHASE_1_DRIVE_CCW ((1 << PHASE_B_HIGH_AVR) | (1 << PHASE_A_LOW_AVR))
#define PHASE_2_DRIVE_CCW ((1 << PHASE_B_HIGH_AVR) | (1 << PHASE_C_LOW_AVR))
#define PHASE_3_DRIVE_CCW ((1 << PHASE_A_HIGH_AVR) | (1 << PHASE_C_LOW_AVR))
#define PHASE_4_DRIVE_CCW ((1 << PHASE_A_HIGH_AVR) | (1 << PHASE_B_LOW_AVR))
#define PHASE_5_DRIVE_CCW ((1 << PHASE_C_HIGH_AVR) | (1 << PHASE_B_LOW_AVR))
#define PHASE_6_DRIVE_CCW ((1 << PHASE_C_HIGH_AVR) | (1 << PHASE_A_LOW_AVR))

#define ROTOR_DIRECTION_PIN 7	//CAUTION! Use with arduino API only
#define SPEED_CONTROL_PIN A0	//CAUTION! Use with arduino API only
#define CURRENT_MONITOR_PIN A1	//CAUTION! Use with arduino API only
#define MAX_CURRENT 20	// in amperes
#define SHUNT_RESISTANCE 0.05 //in ohms
#define MAX_VOLTAGE (MAX_CURRENT * SHUNT_RESISTANCE)	//in volts
#define MAX_CURRENT_ADC (MAX_VOLTAGE / 5 * 1024)

//Commutation table cloumn index
#define ROTOR_POSITION 0
#define CW_ROTATION 1
#define CCW_ROTATION 2

#define PHASE_C_HIGH_ON   ((1<<COM2A1)|(0<<COM2A0))
#define PHASE_C_HIGH_OFF   ((0<<COM2A1)|(0<<COM2A0))
#define PHASE_B_HIGH_ON   ((1<<COM1B1)|(0<<COM1B0))
#define PHASE_B_HIGH_OFF   ((0<<COM1B1)|(0<<COM1B0))
#define PHASE_A_HIGH_ON	((1<<COM1A1)|(0<<COM1A0))
#define PHASE_A_HIGH_OFF   ((0<<COM1A1)|(0<<COM1A0))

static const int  commutation_table[][6]= {
  { PHASE_1_HALL_SENSOR, PHASE_2_HALL_SENSOR, PHASE_3_HALL_SENSOR, PHASE_4_HALL_SENSOR, PHASE_5_HALL_SENSOR, PHASE_6_HALL_SENSOR},
  { PHASE_1_DRIVE_CW, PHASE_2_DRIVE_CW, PHASE_3_DRIVE_CW, PHASE_4_DRIVE_CW, PHASE_5_DRIVE_CW, PHASE_6_DRIVE_CW},
  { PHASE_1_DRIVE_CCW, PHASE_2_DRIVE_CCW, PHASE_3_DRIVE_CCW, PHASE_4_DRIVE_CCW, PHASE_5_DRIVE_CCW, PHASE_6_DRIVE_CCW}
};

void set_Speed(){
  OCR2B= speedBuf;
}

void setup()
{
  pinMode(ROTOR_DIRECTION_PIN, INPUT);
  pinMode(3, OUTPUT);
  
  /* add setup code here */
  DDRB = (DDRB & ~DRIVE_MASK) | DRIVE_MASK;
  DDRD = (DDRD | HALL_SENSOR_MASK) & ~HALL_SENSOR_MASK;
  
  TCCR2A = (TCCR2A & B00001100) | B00110001;
  TCCR2B = (TCCR2B & B11110000) | B00000010;
  
  OCR2B = 0;
  Serial.begin(9600);

}



void loop(){
    
  for(phase = 0; phase < 6; phase++){
    PORTB = (PORTB & ~DRIVE_MASK) | commutation_table[CW_ROTATION][phase];

    set_Speed();
  if (analogRead(CURRENT_MONITOR_PIN) > MAX_CURRENT_ADC ){ // If current consumption is too high, limit current
    if( speedBuf >= 2 ){
      speedBuf /= 2;
      }// Slow down if too fast.
    //Serial.print(analogRead(CURRENT_MONITOR_PIN)/1024.0*5/0.05);
    //Serial.print(" ");
    //Serial.println(speedBuf/255.0);
  }else{
    //if (Serial.available() > 0) speedCMD= 2* (Serial.read()-32);
    speedCMD = analogRead(SPEED_CONTROL_PIN);
    // Incrementally approach commanded speed.(ramp instead of step)
    if( speedCMD > speedBuf ) speedBuf++;
    else if( speedCMD < speedBuf ) speedBuf--;
  }
  delay(10);
}

