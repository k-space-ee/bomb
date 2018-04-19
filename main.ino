#define UNOPENED 1
#define RUNNING 2
#define RUNNING_FAST 3
#define EXPLODE 4
#define SUCCESS 5
#define TIMEOUT 6
#define CHECK 7

#define SOUND_PIN 53

#define LIGHT_SENSOR A7

#define A11_MIN 20
#define A11_MAX 100

#define A12_MIN 900
#define A12_MAX 1000

#define A13_MIN 200
#define A13_MAX 300

#define A14_MIN 550
#define A14_MAX 650

#define A15_MIN 140
#define A15_MAX 250


// include the library code:
#include <LiquidCrystal.h>

// initialize the library with the numbers of the interface pins
//LiquidCrystal lcd(12, 11, 5, 4, 3, 2);
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);

long seconds = 180;
long fastStartTime = 0;
long startTime = 0;
long lasted = 0;
long lastLasted = 0;

int state = CHECK;
int value = 0;

int mistakeNumber = 0;
String message = "";

void setup() {
        pinMode(SOUND_PIN, OUTPUT);
        digitalWrite(SOUND_PIN, LOW);
        
        
        // set up the LCD's number of columns and rows: 
        lcd.begin(16, 2);
        //lcd.clear();
        
        // set pullup resistors on inputs
//        digitalWrite(A15, HIGH);
//        digitalWrite(A14, HIGH);
//        digitalWrite(A13, HIGH);
//        //digitalWrite(A12, HIGH); // cannot use pullup on A12, because it reads 1023 then
//        digitalWrite(A11, HIGH);
        
}

void loop() {

        switch (state) {
        case CHECK:
                writeLine(0, "Checking inputs ");
                writeLine(1, "       ...      ");
                
                for (value = 0; value < 20; value++) {
                
                        delay(100);
                        
                        if (checkValue(analogRead(11), A11_MIN, A11_MAX, CHECK)) {
                                setMistake(11, " check wire 11  ");
                                break;
                        }
                        else if (checkValue(analogRead(12), A12_MIN, A12_MAX, CHECK)) {
                                setMistake(12, " check wire 12  ");
                                break;
                        }
                        else if (checkValue(analogRead(13), A13_MIN, A13_MAX, CHECK)) {
                                setMistake(13, " check wire 13  ");
                                break;
                        }
                        else if (checkValue(analogRead(14), A14_MIN, A14_MAX, CHECK)) {
                                setMistake(14, " check wire 14  ");
                                break;
                        }
                        else if (checkValue(analogRead(15), A15_MIN, A15_MAX, CHECK)) {
                                setMistake(15, " check wire 15  ");
                                break;
                        }
                }
                if (mistakeNumber == 0) {
                        writeLine(0, "  CHECK PASSED  ");
                        writeLine(1, "                ");
                        delay(1000);
                }
                else {
                        writeLine(0, "  Cannot start  ");
                        writeLine(1, message);
                        while (true) {
                                // never start
                                delay(1000);
                        }
                }
                
                
                writeLine(0, "    Arming...   ");
                for (value = 9; value >= 0; value--) {
                        writeLine(1, "         for " + String(value) + "s");
                        delay(1000);
                }
                
                changeState(UNOPENED);
                lcd.clear();
                
                for (value = 0; value < 2; value++) {
                        digitalWrite(SOUND_PIN, HIGH);
                        delay(80);
                        digitalWrite(SOUND_PIN, LOW);
                        delay(120);
                }
                
                break;
        case UNOPENED:
                /*
                value = analogRead(LIGHT_SENSOR);
                printValue(value);
                */
                
                // changes state to RUNNING if value is less than 0 or more than 500
                checkValue(analogRead(LIGHT_SENSOR), 0, 400, RUNNING);
                break;
                
        case RUNNING:
        case RUNNING_FAST:
                if (lasted >= seconds) {
                        changeState(TIMEOUT);
                        break;
                }
                
                if (message == "") {
                        value = seconds - lasted;
                        switch (value) {
                        case 180: writeLine(0, "     Hello!     "); break;
                        case 175: writeLine(0, "   Time left    ");        break;
                        case 150: writeLine(0, "   Hurry up!    "); break;                        
                        case 145: writeLine(0, "   Time left    ");        break;
                        case 100: writeLine(0, "I'm watchin you!");        break;
                        case 90:  writeLine(0, "   Time left    ");        break;
                        case 60:  writeLine(0, "  Last minute!  ");        break;
                        case 55:  writeLine(0, "   Time left    "); break;
                        case 30:  writeLine(0, " tick tick tick ");        break;
                        case 23:  writeLine(0, "   Time left    "); break;
                        case 20:  writeLine(0, " Bwa ha ha haa! "); break;
                        case 8:   writeLine(0, "   Time left    ");        break;
                        }
                }
                
                
                /*
                if (lastLasted != lasted) {
                        lastLasted = lasted;
                        value = analogRead(A15);
                        printValue(value);
                }*/
                
                // A11 - 60 (default) / 220 (default with pullup)
                if (checkValue(analogRead(11), A11_MIN, A11_MAX, EXPLODE)) {
                        setMistake(11, " wrong wire 11  ");
                }
                // A13 - 240 / 580
                if (checkValue(analogRead(13), A13_MIN, A13_MAX, EXPLODE)) {
                        setMistake(13, " wrong wire 13  ");
                }
                // A14 - 590 / 770
                if (checkValue(analogRead(14), A14_MIN, A14_MAX, EXPLODE)) {
                        setMistake(14, " wrong wire 14  ");
                }
                
                if (checkValue(analogRead(12), A12_MIN, A12_MAX, EXPLODE)) {
                        setMistake(12, " wrong wire 12  ");
                }
                
                if (checkValue(analogRead(15), A15_MIN, A15_MAX, SUCCESS)) {
                        // SUCCESS
                        digitalWrite(SOUND_PIN, HIGH);
                        delay(80);
                        digitalWrite(SOUND_PIN, LOW);
                        delay(120);
                }

                /*
                // A15 - 190 / 510
                value = analogRead(15);
                if (value < 140 || value > 250) {        
                        // A12 - 940 / 1023 (cannot use pullup)
                        if (checkValue(analogRead(12), 900, 1000, SUCCESS)) {
                                // SUCCESS
                        }
                }
                // A12 - 940 / 1023 (cannot use pullup)
                else if (checkValue(analogRead(12), 900, 1000, RUNNING_FAST)) {
                        // wrong order
                        setMistake(12, "   Time left    ");
                }
                */
                
                
                // time ticking
                lcd.setCursor(5, 1);
                lcd.print(String(getTime()));
                break;
        case EXPLODE:
        case TIMEOUT:
                digitalWrite(SOUND_PIN, HIGH);
                // wrong wire
                if (lasted % 2 == 1) {
                        lcd.setCursor(0, 0);
                        lcd.print("    Kaboom!     ");
                        if (state == EXPLODE) {
                                lcd.setCursor(0, 1);
                                lcd.print(message);
                        } else {
                                // no more time!
                                lcd.setCursor(5, 1);
                                lcd.print("00:00");
                        }
                } else {
                        lcd.clear();
                }
                break;

        case SUCCESS:
                lcd.setCursor(0, 0);
                lcd.print(" Great success! ");
                lcd.setCursor(0, 1);
                lcd.print("    disarmed    ");
                break;
        }

        if (state == RUNNING_FAST) {
                lasted = (millis() - startTime) / 1000 + (millis() - fastStartTime) / 50;
        } else {
                lasted = (millis() - startTime) / 1000;
        }
}

void printValue(int v) {
        lcd.setCursor(0, 1);
        if (v < 1000) lcd.print(" ");
        if (v < 100) lcd.print(" ");
        if (v < 10) lcd.print(" ");
        lcd.print(String(v));
}

void setMistake(int mistake, String m) {
        if (0 == mistakeNumber) {
                mistakeNumber = mistake;
                message = m;
        }
        else if (mistake != mistakeNumber){
                changeState(EXPLODE);
        }
}

bool checkValue(int value, int min, int max, int newState) {
        if (value < min || value > max) {
                changeState(newState);
                return true;
        }
        return false;
}

void changeState(int newState) {
        switch (newState) {
        case RUNNING:
                if (state == RUNNING)
                        return;
                startTime = millis();
                lcd.setCursor(0, 0);
                lcd.print("   Time left    ");
                break;
        case RUNNING_FAST:
                if (state == EXPLODE || state == RUNNING_FAST)
                        return;
                fastStartTime = millis();
                break;
        }
        state = newState;
}

void writeLine(int line, String message) {
        lcd.setCursor(0, line);
        lcd.print(message);
}

String getTime() {
        int s = seconds - lasted;
        int minutes = s / 60;
        s = s % 60;

        return (minutes < 10 ? "0" : "") + String(minutes) + ":"
                        + (s < 10 ? "0" : "") + String(s);
}
