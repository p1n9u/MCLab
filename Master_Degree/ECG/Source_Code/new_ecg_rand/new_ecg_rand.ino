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
#define QUEUE_SIZE      2048
#define DELIMITER_C     '/'
#define DELIMITER_S     "/"
#define DATA_CNT        128
#define TCP_IP          "166.104.185.207"
#define TCP_PORT        8080;

/* define data type */
typedef char element;

/* declare global */
element buf[BUF_SIZE];

/* implement queue */
typedef struct QueueType {
    element queue[QUEUE_SIZE];
    int front, rear, cnt;
} QueueType;

void init(QueueType *Q) {
    Q->front = Q->rear = Q->cnt = 0;
}

QueueType *create_queue(void) {
    QueueType *new_queue = (QueueType *)malloc(sizeof(QueueType));
    if ( new_queue == NULL ) {
        DebugSerial.println("create queue err");
    } else {
        init(new_queue);
        DebugSerial.println("create queue success");
        return new_queue;
    }
}

int is_empty(QueueType *Q) {
    return (Q->front == Q->rear);
}

int is_full(QueueType *Q) {
    return ((Q->front)%QUEUE_SIZE == (Q->rear+1)%QUEUE_SIZE);
}

void enqueue(QueueType *Q, element item) {
    if ( is_full(Q) ) {
        DebugSerial.println("queue overflow");
    } else {
        Q->rear = (Q->rear+1)%QUEUE_SIZE;
        Q->queue[Q->rear] = item;
    }
}

element dequeue(QueueType *Q) {
    if ( is_empty(Q) ) {
        DebugSerial.println("queue underflow");
    } else {
        Q->front = (Q->front+1)%QUEUE_SIZE;
        return Q->queue[Q->front];
    }
}

void push_data(QueueType *Q, char *str) {
    while ( *str != '\0' ) {
        enqueue(Q, *str);
        Q->cnt++;
        *str++;
    } 
}

void pop_data(QueueType *Q) {
    int i=0, cnt=0;
    element e;
    while ( cnt < DATA_CNT ) {
        e = dequeue(Q);
        Q->cnt--;
        if ( e == DELIMITER_C ) {
            cnt++;
        }
        buf[i++] = e;
    }
} 

QueueType *queue;
BG96 BG96(M1Serial, DebugSerial, PWR_PIN, STAT_PIN);

void setup() {
    M1Serial.begin(115200);
    DebugSerial.begin(115200);
    ECGSerial.begin(115200);
    pinMode(10, INPUT); // Setup for leads off detection LO +
    pinMode(11, INPUT); // Setup for leads off detection LO -
    queue = create_queue();
    
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
    DebugSerial.println("Done");
    delay(10000);   
}

int loop_cnt = 1;
long ekg_data;

void loop() {
    if ( loop_cnt == 2001 ) {
        loop_cnt = 1;
    }
    String data = "";
    char ch[BUF_SIZE];

    data += String(loop_cnt) + DELIMITER_S;
    ekg_data = random(400, 600);
    data += String(ekg_data) + DELIMITER_S;
    
    data.toCharArray(ch, data.length()+1);
    DebugSerial.print("[push data] : ");
    DebugSerial.println(ch);
       
    push_data(queue, ch);
    
    if ( queue->cnt > 1024 ) {
        pop_data(queue);
        if ( BG96.socketSend( buf, strlen(buf)+1 ) == 0 ) {
            DebugSerial.println("[TCP Send] : ");
            DebugSerial.println(buf);
            DebugSerial.print("[buf len] : ");
            DebugSerial.println(strlen(buf)+1);
        } else {
            DebugSerial.println("Send Fail!!!");
        }
    }

    loop_cnt++;
    delay(10);
}
