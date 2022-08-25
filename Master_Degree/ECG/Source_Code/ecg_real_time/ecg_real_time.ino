/* include library */
#include <Arduino.h>
#include "BG96.h"
#include "ECG.h"

/* buf */
element buf[BUF_SIZE];

BG96 BG96(M1Serial, DebugSerial, PWR_PIN, STAT_PIN);

void setup() {

    M1Serial.begin(115200);
    DebugSerial.begin(115200);
    ECGSerial.begin(115200);
    pinMode(10, INPUT); // Setup for leads off detection LO +
    pinMode(11, INPUT); // Setup for leads off detection LO -
    
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

    /* first arg 0 : UDP, 1 : TCP */
    if ( BG96.socketCreate(0, _IP, _PORT) == 0 ) {
        DebugSerial.println("Socket Create!!!");
    }

    memset(buf, '\0', BUF_SIZE*sizeof(char));
    DebugSerial.println("Setup Done!!!");
    delay(1000);   
}



/* loop arguments */
int i = 0;
ECG e;
TIMESTAMP mn, sc;
MS ms;
unsigned long t;

void loop() {
    /* data processing e(2) + mn(1) + sc(1) + ms(2) = 6 bytes */
    e.value = analogRead(A0);
    //e.value = i;

    /* minutes : mn, seconds : sc, millis : ms */
    t = millis();
    ms.value = t%1000;
    sc.value = (t/1000)%60;
    mn.value = (t/1000)/60;

    /* fill buffer */
    buf[i*6] = mn.data[0];
    buf[i*6+1] = sc.data[0];
    buf[i*6+2] = ms.data[0];
    buf[i*6+3] = ms.data[1];
    buf[i*6+4] = e.data[0];
    buf[i*7+5] = e.data[1];

    /* debug serial */
    DebugSerial.print("[ECG value] : ");
    DebugSerial.print(e.value);
    DebugSerial.print(", [TimeStamp] : ");
    DebugSerial.print(mn.value);
    DebugSerial.print(":");
    DebugSerial.print(sc.value);
    DebugSerial.print(":");
    DebugSerial.println(ms.value);

    i++;

    /* send buffer */
    if ( i == 90 ) {
        DebugSerial.print("[count] :");
        DebugSerial.print(i);
        DebugSerial.println(", Buf filled!!!");
        if ( BG96.socketSend( buf, sizeof(buf) ) == 0 ) {
            DebugSerial.println("Send Success!!!");
        } else {
            DebugSerial.println("Send Fail!!!");
        }
        i = 0;
        DebugSerial.println("clear buf");
        memset(buf, '\0', BUF_SIZE*sizeof(char));
    }

    delay(1);
}
