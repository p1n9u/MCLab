/* include library */
#include <Arduino.h>
#include "BG96.h"
#include "ECG.h"
#include <swRTC.h>

/* buf */
element buf[BUF_SIZE];

BG96 BG96(M1Serial, DebugSerial, PWR_PIN, STAT_PIN);
swRTC rtc;

void setup() {
    rtc.stopRTC();
    rtc.setTime(19,57,00); // hh, mm, ss
    rtc.setDate(20,8,2022); // DD, MM, YY 
    rtc.startRTC(); 
  
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



/* loop */
int ecg_data, i = 0;
ECG v;
TIMESTAMP thr, tmn, tsc, tmm;

void loop() {
    /* data processing v(2)+thr(1)+tmn(1)+tsc(1)+tmm(1) = 6 bytes */
    v.value = analogRead(A0);
    //v.value = i;
    thr.value = rtc.getHours();
    tmn.value = rtc.getMinutes();
    tsc.value = rtc.getSeconds();
    tmm.value = millis()%100;

    buf[i*6] = thr.data[0];
    buf[i*6+1] = tmn.data[0];
    buf[i*6+2] = tsc.data[0];
    buf[i*6+3] = tmm.data[0];
    buf[i*6+4] = v.data[0];
    buf[i*6+5] = v.data[1];

    DebugSerial.print("[value] : ");
    DebugSerial.print(v.value);
    DebugSerial.print(", [TimeStamp] : ");
    DebugSerial.print(thr.value);
    DebugSerial.print(":");
    DebugSerial.print(tmn.value);
    DebugSerial.print(":");
    DebugSerial.print(tsc.value);
    DebugSerial.print(".");
    DebugSerial.println(tmm.value);

    i++;
   
    if ( i == 80 ) {
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
