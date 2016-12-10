//Test Program for Motor Controller based on arduino
//Author: Emukpere David Temidayo

#define THROTTLE_PIN A4
#define NUMBER_OF_COMMUTATIONS 6
#define AL 13
#define AH 12
#define BL 11
#define BH 10
#define CL 9
#define CH 8
#define DRIVE_PORT  PORTB
#define PWM_PIN 3
#define DRIVE_PATTERN_1 ((1<<AL)|(0<<AH)|(0<<BL)|(1<<BH)|(0<<CL)|(0<<CH))
#define DRIVE_PATTERN_2 ((0<<AL)|(0<<AH)|(0<<BL)|(1<<BH)|(1<<CL)|(0<<CH))
#define DRIVE_PATTERN_3 ((0<<AL)|(1<<AH)|(0<<BL)|(0<<BH)|(1<<CL)|(0<<CH))
#define DRIVE_PATTERN_4 ((0<<AL)|(1<<AH)|(1<<BL)|(0<<BH)|(0<<CL)|(0<<CH))
#define DRIVE_PATTERN_5 ((0<<AL)|(0<<AH)|(1<<BL)|(0<<BH)|(0<<CL)|(1<<CH))
#define DRIVE_PATTERN_6 ((1<<AL)|(0<<AH)|(0<<BL)|(0<<BH)|(0<<CL)|(1<<CH))
#define DRIVE_MASK ((1<<AL)|(1<<AH)|(1<<BL)|(1<<BH)|(1<<CL)|(1<<CH))
static byte comm_table[6] = {
  DRIVE_PATTERN_1,
  DRIVE_PATTERN_2,
  DRIVE_PATTERN_3,
  DRIVE_PATTERN_4,
  DRIVE_PATTERN_5,
  DRIVE_PATTERN_6
};
byte speedSet, speedRef = 0;


void setup() {
  // put your setup code here, to run once:
  //setup data directions here
  pinMode(AL,OUTPUT);
  pinMode(AH,OUTPUT);
  pinMode(BL,OUTPUT);
  pinMode(BH,OUTPUT);
  pinMode(CL,OUTPUT);
  pinMode(CH,OUTPUT);
  pinMode(PWM_PIN, OUTPUT);
  DRIVE_PORT &= ~DRIVE_MASK; //initializing DRIVE PORT
  
  //setup PWM here 
  TCCR2A = (TCCR2A & B00001100)|B00110001;
  TCCR2B = (TCCR2A & B11110000)|B00000010;

  OCR2B = 0;  

}


void loop() {
  // put your main code here, to run repeatedly:

  for (int i =0; i < NUMBER_OF_COMMUTATIONS; i++)
  {
    DRIVE_PORT = comm_table[i];
    byte speedRef = analogRead(THROTTLE_PIN);
    if (speedRef > speedSet){
      ++speedSet;
    }
    else {
      --speedSet;
    }
    OCR2B = speedSet;
    
    delay(10);
  }

}
