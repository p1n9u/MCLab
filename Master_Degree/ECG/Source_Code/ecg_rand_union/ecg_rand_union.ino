/* include library */
#include <Arduino.h>
#include "BG96.h"

/* define macro */
#define DebugSerial     Serial
#define M1Serial        Serial1
#define ECGSerial       Serial2
#define PWR_PIN         2
#define STAT_PIN        3
#define BUF_SIZE        1024
#define TCP_IP          "166.104.185.207"
#define TCP_PORT        8080;

/* define data type */
typedef char element;

typedef union {
    unsigned value:10;
    char data[2];
} ECG;

/* declare global */
element buf[BUF_SIZE];

BG96 BG96(M1Serial, DebugSerial, PWR_PIN, STAT_PIN);

void setup() {
    M1Serial.begin(115200);
    DebugSerial.begin(115200);
    ECGSerial.begin(115200);
    pinMode(10, INPUT); // Setup for leads off detection LO +
    pinMode(11, INPUT); // Setup for leads off detection LO -
    
    /* debug ekg data if not use comment */
    randomSeed(analogRead(0));

    /* BG96 Power On Sequence */
    if ( BG96.isPwrON() ) {
        DebugSerial.println("BG96 Power ON Status");
        if ( BG96.pwrOFF() ) {
            DebugSerial.println("BG96 Power Off Error");
        } else {
            DebugSerial.println("BG96 Power Off Success");
            DebugSerial.println("Module Power ON Sequence Start");
            if ( BG96.pwrON() ) {
                DebugSerial.println("BG96 Power ON Error");
            } else {
                DebugSerial.println("BG96 Power ON Success");
            }
        }
    } else {
        DebugSerial.println("BG96 Power OFF Status");
        if ( BG96.pwrON() ) {
            DebugSerial.println("BG96 Power ON Error");
        } else {
            DebugSerial.println("BG96 Power ON Success");
        }
    }

    /* BG96 Module Initialization */
    if ( BG96.init() ) {
        DebugSerial.println("BG96 Module Error!!!");
    }

    /* BG96 Module Power Saving Mode Disable */
    if ( BG96.disablePSM() ) {
        DebugSerial.println("BG96 PSM Disable Error!!!");
    }

    /* Network Regsistraiton Check */
    while ( BG96.canConnect() != 0 ) {
        DebugSerial.println("Network not Ready !!!");
        delay(2000);
    }

    DebugSerial.println("BG96 Module Ready!!!");

    char _IP[] = TCP_IP;
    int  _PORT = TCP_PORT;

    if ( BG96.actPDP() == 0 ) {
        DebugSerial.println("BG96 PDP Activation!!!");
    }

    if ( BG96.socketCreate(1, _IP, _PORT) == 0 ) {
        DebugSerial.println("TCP Socket Create!!!");
    }

    memset(buf, '\0', BUF_SIZE*sizeof(char));
    DebugSerial.println("Setup Done!!!");
    delay(1000);   
}

int loop_cnt = 0;

/* debug ekg data if not use comment */
int ecg_data;

void loop() {
    
    /// ecg_data = random(0, 1023);
    DebugSerial.print("[data test] : ");
    DebugSerial.println(loop_cnt);

    ECG v;
    v.value = loop_cnt;
    buf[loop_cnt*2] = v.data[0];
    buf[loop_cnt*2+1] = v.data[1];
    loop_cnt++;
   
    if ( loop_cnt == 512 ) {
        DebugSerial.print("[loop cnt] :");
        DebugSerial.println(loop_cnt);
        DebugSerial.println("Buf filled!!!");
        if ( BG96.socketSend( buf, sizeof(buf) ) == 0 ) {
            DebugSerial.println("Send Success!!!");
        } else {
            DebugSerial.println("Send Fail!!!");
        }
        loop_cnt = 0;
        DebugSerial.print("[loop cnt] :");
        DebugSerial.println(loop_cnt);
        memset(buf, '\0', BUF_SIZE*sizeof(char));
    }

    delay(1);
}
