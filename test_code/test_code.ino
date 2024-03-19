//configurations
//ultrasonic
#define trigpin 2
#define echopin 3

#define buzzer 4
//leds
#define greenLed 12
#define redLed 13
#define yellowLed 15
//switches
#define sw1 0
#define sw2 1

void setup() {
  
  pinMode(greenLed, OUTPUT);
  pinMode(redLed, OUTPUT);
  pinMode(yellowLed, OUTPUT);
  pinMode(buzzer, OUTPUT);
  pinMode(sw1, INPUT);
  pinMode(sw2, INPUT);
  Serial.begin(9600);
}

// the loop function runs over and over again forever
void loop() {
    int x,y;
    x= digitalRead(sw1);
    y= digitalRead(sw2);
    Serial.println(x);
    Serial.println(y);
    // digitalWrite(greenLed, HIGH);   // turn the LED on (HIGH is the voltage level)
    // delay(1000);                       // wait for a second
    // digitalWrite(greenLed, LOW);    // turn the LED off by making the voltage LOW
    // delay(1000);
    // digitalWrite(redLed, HIGH);   // turn the LED on (HIGH is the voltage level)
    // delay(1000);                       // wait for a second
    // digitalWrite(redLed, LOW);    // turn the LED off by making the voltage LOW
    // delay(1000);                      // wait for a second
    // digitalWrite(yellowLed, HIGH);   // turn the LED on (HIGH is the voltage level)
    // delay(1000);                       // wait for a second
    // digitalWrite(yellowLed, LOW);    // turn the LED off by making the voltage LOW
    // delay(1000);// wait for a second
    // digitalWrite(buzzer, HIGH);   // turn the LED on (HIGH is the voltage level)
    // delay(500);                       // wait for a second
    // digitalWrite(buzzer, LOW);    // turn the LED off by making the voltage LOW
    // delay(500);                      // wait for a second

    digitalWrite(greenLed, sw1);
    digitalWrite(redLed, sw2);
    digitalWrite(yellowLed, (sw1 && sw2));
    if (sw1 && sw2){
        digitalWrite(buzzer, HIGH);
        delay(1000);
        digitalWrite(buzzer, LOW);
         delay(1000);
    }
}
