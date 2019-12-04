#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>


char auth[] = "aAISQlGcE-5UZxxipP0qxafbXHSFBJEt";
char ssid[] = "Eleceasy";
char pass[] = "Bb20072536";
char server[] = "oasiskit.com";
unsigned port = 8080;

BlynkTimer timer;

BLYNK_CONNECTED(){
    Blynk.syncAll();
}

//ปุ่ม Blynk
#define Widget_Btn_00 V0    //ปุ่ม Auto/Manual
#define Widget_Btn_01 V1    //Terminal
#define Widget_Btn_Pump V2    //ปุ่ม เปิด-ปิด ปั๊มน้ำ
#define Widget_SoilValue V4 //แสดงค่าความชื่นในดิน

//สถานะปุ่ม Blynk
#define Widget_LED_Pump V3  //ไฟสถานะปั๊มน้ำ

WidgetLED LedBlynkPump(Widget_LED_Pump);

//Pin INPUT
#define SoilSensor_1 36
#define SoilSensor_2 39
#define SoilSensor_3 34
#define SoilSensor_4 35


//Pin OUTPUT
#define Relay1_pump  32

//ประกาศตัวแปร
int averagesoilsensorValue = ReadsoilsensorValue(); //ค่าเฉลี่ย soilsensor คิดเป็นเปอร์เซน
int SoilSensorValue;

void setup(){
    Serial.begin(115200);
    // WiFi.begin(ssid, pass);
    // while (WiFi.status() != WL_CONNECTED){
    //     delay(500);
    //     Serial.print(".");
    // }
    Serial.println("WiFi connected");
    Serial.print("Connecting to ");
    Serial.println(ssid);
    Blynk.begin(auth, ssid, pass, server, port);

    //pinMode INPUT
    pinMode(SoilSensor_1, INPUT);
    pinMode(SoilSensor_2, INPUT);
    pinMode(SoilSensor_3, INPUT);
    pinMode(SoilSensor_4, INPUT);

    //pinMode OUTPUT
    pinMode(Relay1_pump, OUTPUT);

    //เรียกใช้ Function
    timer.setInterval(2000, sendSensor);
    timer.setInterval(2000, AutoSoilWater);
    //timer.setInterval(2000, averagesoilsensorValue);
    

    
}

void loop(){
    ReadsoilsensorValue();
    Serial.print("SoilAverage : "); Serial.print(averagesoilsensorValue); Serial.println("%");
    
    Blynk.run();
    timer.run();
}


void sendSensor(){
    Blynk.virtualWrite(Widget_SoilValue, averagesoilsensorValue);
}




BLYNK_WRITE(Widget_Btn_Pump){
    int value = param.asInt();

    if(value== 1){
        digitalWrite(Relay1_pump, HIGH);
        LedBlynkPump.on();
    }else{
        digitalWrite(Relay1_pump, LOW);
        LedBlynkPump.off();
    }

}


int ReadsoilsensorValue(){
    static unsigned long timer1 = millis();
    const unsigned long period = 1000; 
    if(millis() - timer1 >= period){
        timer1 = millis();

        int SoilSensorValue_1 = analogRead(SoilSensor_1);
        int SoilSensorValue_2 = analogRead(SoilSensor_2);
        int SoilSensorValue_3 = analogRead(SoilSensor_3);
        int SoilSensorValue_4 = analogRead(SoilSensor_4);

        int mapSoilSensorValue_1 = map(SoilSensorValue_1, 0, 4095, 100, 0);
        int mapSoilSensorValue_2 = map(SoilSensorValue_2, 0, 4095, 100, 0);
        int mapSoilSensorValue_3 = map(SoilSensorValue_3, 0, 4095, 100, 0);
        int mapSoilSensorValue_4 = map(SoilSensorValue_4, 0, 4095, 100, 0);

        // Serial.print("Soil_1 :"); Serial.print(mapSoilSensorValue_1); Serial.println("%");
        // Serial.print("Soil_2 :"); Serial.print(mapSoilSensorValue_2); Serial.println("%");
        // Serial.print("Soil_3 :"); Serial.print(mapSoilSensorValue_3); Serial.println("%");
        // Serial.print("Soil_4 :"); Serial.print(mapSoilSensorValue_4); Serial.println("%");

        SoilSensorValue = 10 ;       
        //Serial.print("SoilAverage : "); Serial.print(SoilSensorValue); Serial.println("%");
        return SoilSensorValue;
    }
}

void AutoSoilWater(){
    if(averagesoilsensorValue < 65 ){
        if(Widget_Btn_Pump == 1){

            digitalWrite(Relay1_pump, HIGH);
            Blynk.virtualWrite(Widget_Btn_Pump, 1);

        }
        else if(averagesoilsensorValue > 70){

            digitalWrite(Relay1_pump, LOW);
            Blynk.virtualWrite(Widget_Btn_Pump, 0);

        }
    }
}