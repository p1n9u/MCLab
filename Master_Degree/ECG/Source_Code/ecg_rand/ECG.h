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
