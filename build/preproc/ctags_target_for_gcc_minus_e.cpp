# 1 "c:\\Users\\marti\\Documents\\Arduino\\udpboard2\\udpboard2.ino"
# 2 "c:\\Users\\marti\\Documents\\Arduino\\udpboard2\\udpboard2.ino" 2
# 3 "c:\\Users\\marti\\Documents\\Arduino\\udpboard2\\udpboard2.ino" 2
//#include <WebSockets2_Generic.h>
# 5 "c:\\Users\\marti\\Documents\\Arduino\\udpboard2\\udpboard2.ino" 2
# 6 "c:\\Users\\marti\\Documents\\Arduino\\udpboard2\\udpboard2.ino" 2

const char * ssid_softAP = "ESPap";
const char * password_softAP = "thereisnospoon";

IPAddress staticIP(192,168,4,123);
IPAddress gateway(192,168,4,109);
IPAddress subnet(255,255,255,0);


//Output values
const byte SPEED_R = 5; //, frem
const byte SPEED_F = 0; //, bak
const byte SERVO_OUT = 15; //D4, Rorudlæget til styremaskinen, flyttet fra 
const int RORBEGRAENSNING = 30;
const unsigned long TIMEOUT = 20000; //20,0ms

//Input values
const byte ROR_AUTO = 4; //D2  
const byte ROR_MAN = 14; //D5 
const byte FART = 12; //D6
const byte MODE = 13; //D7
// bool fjernbetjeningFlag = false;
// int rorUdlaeg = 90; //default val
bool autoMode = false; //default val
uint32_t t1;
int udl=90;
int marshSpeed = 210;
unsigned long timeout=0;
Servo servo1;
float mean = 90;
bool RADIO = true;

unsigned int localUdpPort = 4210; // local port to listen on
int incomingPacketLength = 40;
char incomingPacket[40]; // buffer for incoming packets



/* initialiserer moduler */
WiFiUDP Udp;

void setup(){
    // int * test_ptr = new int(42); //Opretter en pointer til en int = 42 på heapen
    // delete test_ptr; //deallokerer pointeren manulelt
    Serial.begin(115200);
    while (!Serial) { ;}

    Serial.println("Starter");

    //nyt
    Serial.printf("Connecting to %s ", ssid_softAP);
    WiFi.config(staticIP, gateway, subnet);
    WiFi.begin(ssid_softAP, password_softAP);

    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }
    Serial.println(" connected");

    Udp.begin(localUdpPort);//lytter på port 4210

    // bool conn = initConnectToWifi();
    // Serial.print("connected to Wifi: ");
    // Serial.println(conn);
    //nyt slut

    // client.onMessage(onMessageCallback);
 // client.onEvent(onEventsCallback);

    // if(true){

    //     while(!client.connect("ws://192.168.4.1:8000/ws")){
    //    // while(!client.connect("192.168.137.176",8000,"/ws")){

    //         // client.connect("192.168.137.176",8000,"/ws");
    //         Serial.println("Try to upgrade");
    //         delay(500);
    //     // clientPC.connect("192.168.137.1", 8000, "/ws3");
    //     };

    // }

    // delay(25000);// venter 25 sek ter til vi har kontakt til serielporten

    // pinMode(ROR_AUTO, INPUT);
    pinMode(ROR_MAN, 0x00);
    pinMode(FART, 0x00);
    pinMode(MODE, 0x00);

    servo1.attach(SERVO_OUT);
    analogWrite(SPEED_F,100);
    analogWrite(SPEED_R,100);
    servo1.write(175);
    delay(1000);
    analogWrite(SPEED_F,0);
    analogWrite(SPEED_R,0);
    servo1.write(45);
    delay(1000);
     analogWrite(SPEED_F,0);
    analogWrite(SPEED_R,100);
    servo1.write(90);
    delay(1000);
    analogWrite(SPEED_F,0);
    analogWrite(SPEED_R,0);
}

void loop(){

    int packetSize = Udp.parsePacket();
    if (packetSize)
    {
        // receive incoming UDP packets

        int len = Udp.read(incomingPacket, incomingPacketLength);

        if (len > 0)
        {
        incomingPacket[len] = '\0';
        }
        Serial.printf("UDP packet contents: %s, %d\n", incomingPacket,len);

        udl = process(incomingPacket, len);
        // // send back a reply, to the IP address and port we got the packet from
        // Udp.beginPacket(Udp.remoteIP(), Udp.remotePort());
        // Udp.write(replyPacket);
        // Udp.endPacket();
    }


 if(!sleep(50)){//opdaterer hvert 50 ms - kører hvert loop
    if(RADIO){
        timeout = pulseIn(MODE,0x1,TIMEOUT);
    }//venter TIMEOUT = 20000 microseconds =20 ms to wait for the pulse to be completed: the function returns 0 if no complete pulse was received within the timeout
    // Serial.print("------------------------------------------------------------------------>");  
        // Serial.print("timeout: ");
        // Serial.println(timeout);
        // Serial.print(udl);
        // Serial.print(" , ");
        // Serial.print(map(pulseIn(ROR_MAN,HIGH),955,2040,45,135));
        // Serial.print(" , ");
        // Serial.print(map(pulseIn(FART,HIGH),1002,2004,-200,200));
        // Serial.print(" , ");
        // Serial.println(pulseIn(MODE,HIGH)>1500);

        if(timeout < 10 ){// ikke tændt for fjernbetjening, da der ikke er registreret en pulseIn fra fjernbetjeningen
            //udl = pulseIn(ROR_AUTO,HIGH)/10 + 45 ;


                Serial.println(udl);
                // client.send(String(udl,1));
                // servo1.write(udl-15);

        }else if(timeout > 1500){// auto mode - da pulseIn(MODE HIGH)= ca. 2000 micro sek = ca. 2 ms (the length of the pulse (in microseconds) or 0 if no pulse is completed before the timeout)
                // udl = pulseIn(ROR_AUTO,HIGH)/10 + 45;//input fra ROR_AUTO pin
                Serial.print(" ##############################, ");
                // Serial.println(udl);
                servo1.write(udl);
                if(marshSpeed>0){
                    analogWrite(SPEED_F,marshSpeed);//marsh hast
                    analogWrite(SPEED_R,0);//marsh hast
                }else {
                    analogWrite(SPEED_F,0);//marsh hast
                    analogWrite(SPEED_R,-marshSpeed);//marsh hast
                }

        }else if(timeout < 1500){//manuel mode

            float k = map(pulseIn(ROR_MAN,0x1),955,2040,45,135);
            float s = map(pulseIn(FART,0x1),1000,2000,-250,250);
            Serial.print(" ****************************, ");
            Serial.println(k);
            mean = 0.7*mean + 0.3*k;

            servo1.write(mean-15);
            if (s<40){
                analogWrite(SPEED_F,-s);
                analogWrite(SPEED_R,0);
            }else if(s>40){
                analogWrite(SPEED_R,s);
                analogWrite(SPEED_F,0);
            }else{
                analogWrite(SPEED_R,0);
                analogWrite(SPEED_F,0);
            }
        }
    }
}

bool sleep(int t){

    uint32_t t0 = millis(); //opdaterer timer ved nyt tik
    if (t0 - t1 <t) {
        return true;
    }
    t1 = t0;//opdaterer closure 
    return false;
}

float process(char *dat, int len)
{

 int startBit = 1;

 int intVal = atoi(&dat[startBit]);
 // Serial.printf("process linje 212 %f %i \n", intVal, len);

return intVal;
}

////////////////////// WiFi + websoket2 //////////////////////////
// bool initConnectToWifi(){
// 	// ESP8266 Connect to wifi
//     WiFi.mode(WIFI_STA);
// 	for (byte j = 0; j < 15; j++) {
// 	delay(1000);

// 		WiFi.begin("ESPap","thereisnospoon");
//         Serial.println("Prøver at forbinde ESP AP");


// 		for (int i = 0; i < 10 && WiFi.status() != WL_CONNECTED; i++) 
// 		{
//         Serial.print(".");
// 			delay(1000);
// 		}
// 		if (WiFi.status() == WL_CONNECTED) {
//             // WiFi.setAutoReconnect(false);
// 	        // WiFi.persistent(false);
//             Serial.println("Succesfuld forbindelse til WiFi");
// 			return true;
// 			}
// 	}
// 	// Check if connected to wifi
// 	if (WiFi.status() != WL_CONNECTED) 
// 	{
//         Serial.println("No Wifi!");
// 		return false;
// 	}
// }

// void onMessageCallback(WebsocketsMessage message) 
// {
//     String besked = message.data();
// 	// float misvisning = 0; //
// 	char nr = besked.charAt(0);
//     switch (nr)
//     {
//     case 'a':
//         Serial.print("a:");
//         Serial.println(besked.substring(1, besked.length()).toInt());
//         udl = besked.substring(1, besked.length()).toInt();

//         break;
//     case 'b':
//         Serial.print("b:");
//         Serial.println(besked.substring(1, besked.length()).toInt());
//         marshSpeed = besked.substring(1, besked.length()).toInt();
//         break;
//     default:
//     Serial.print("default:");
//     Serial.println(message.data());
//         break;
//     }
// }
// void onEventsCallback(WebsocketsEvent event, String data)
// {
//   if (event == WebsocketsEvent::ConnectionOpened)
//   {
//     Serial.println("Connnection Opened");
//   }
//   else if (event == WebsocketsEvent::ConnectionClosed)
//   {
//     Serial.println("Connnection Closed");
//   }
//   else if (event == WebsocketsEvent::GotPing)
//   {
//     Serial.println("Got a Ping!");
//   }
//   else if (event == WebsocketsEvent::GotPong)
//   {
//     Serial.println("Got a Pong!");
//   }
// }
