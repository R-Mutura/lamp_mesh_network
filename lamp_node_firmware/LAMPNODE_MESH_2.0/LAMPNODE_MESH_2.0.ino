
#include "painlessMesh.h"
#include <Arduino_JSON.h>

#include "variables.h"
JSONVar myVar;
//////////////////////////////////////////////////////////////
     #define speedThreshold 10 //speed threshold for sensing is 10km/h
     #define AVERAGE 4
     unsigned int doppler_div = 44;
     unsigned int mysamples[AVERAGE];
////////////////////////
#define mydelay 50 // in miliseconds
int mytime_now = 0; // will be used to calc delat for the send cycle
//pin definitions
#define radarMotionPin  15
#define lampTrigger     32
#define dbgLed          33
//#define connectionLed   25
//#define lampSwitch      4
bool calc_delay = false;
int onFlag       = 0;
long timecount   = 0;
int waitduration = 5000; //100 milisecond duration

String lampState= "OFF";

Scheduler userScheduler; // to control your personal task
painlessMesh  mesh;

// User stub
void sendMessage() ; // Prototype so Platform-IO doesn't complain
String construnct_json();

Task taskSendMessage( TASK_SECOND * 1 , TASK_FOREVER, &sendMessage );

void sendMessage() {
     
    if((onFlag == 1) || (calc_speed(radarMotionPin)>speedThreshold)){//only send if the radar is on and stays on for a few miliseconds
    String msg = construnct_json(); //construct json string 
    Serial.print("sending mesage = ");
    Serial.println(msg);
    bool sendingstatus = mesh.sendBroadcast( msg ); //send the data containig myNodeID and state
    Serial.print("sending status = " );
    Serial.println(sendingstatus);
    onFlag = 0;//reset flag so message will be sent only once per trigger
    //taskSendMessage.setInterval( random( TASK_SECOND * 0.01, TASK_SECOND * 0.05 ));
    }
     
    
}
   
   

// Needed for painless library
void receivedCallback( uint32_t from, String &msg ) {
 //vars to be used in this function
   String state;
  int receivedID;
  Serial.printf("Message: Received from %u msg=%s\n", from, msg.c_str());
  //TODO: Deserialize the data to het status and node number
        // compare numbers to see if its within range set on flag to 1 and start a timer for 1-second over flow to be used in the main loop
 
     //desirializing the json data here
     JSONVar myObject = JSON.parse(msg.c_str());
          
     if (JSON.typeof(myObject) == "undefined") 
     {
    Serial.println("Parsing input failed!");
    return;
     } 

       if (myObject.hasOwnProperty("state")) {
                Serial.print("myObject[\"state\"] = ");
             state = (const char*) (myObject["state"]);
                Serial.println(state);
         }
       if (myObject.hasOwnProperty("ID")) {
                Serial.print("myObject[\"ID\"] = ");
             receivedID = (int) myObject["ID"];
                Serial.println(receivedID);
         }  


     
     //call the calc function and turn on the laps where necessary
     if(state.equals("ON")){
      int lampStatus = calc(receivedID);
      if(lampStatus == 1){ //check if we need to turn on this lamp with calc function
        //if true is returned here then turn on the light
          digitalWrite(lampTrigger, HIGH);
          digitalWrite(dbgLed , HIGH);
          Serial.println("turning on via mesh network");
          lampState = "OFF";
          timecount = millis();
          
        }
        
      }
     
}

void newConnectionCallback(uint32_t nodeId) {
    Serial.printf("--> startHere: New Connection, nodeId = %u\n", nodeId);
}

void changedConnectionCallback() {
  Serial.printf("Changed connections\n");
}

void nodeTimeAdjustedCallback(int32_t offset) {
    Serial.printf("Adjusted time %u. Offset = %d\n", mesh.getNodeTime(),offset);
}
//void IRAM_ATTR isr() {
  //   onFlag = 1;
    // digitalWrite(lampTrigger, HIGH);
     //digitalWrite(dbgLed,HIGH);
    // timecount = millis();//for time keeping of on duration
 //}

void setup() {
  Serial.begin(115200);
  //attach interrupt
  pinMode(radarMotionPin, INPUT);
  //attachInterrupt(radarMotionPin, isr, RISING);
  
  //pinMode(controllerInput, INPUT);
  pinMode(dbgLed, OUTPUT);
  //pinMode(connectionLed, OUTPUT);
  pinMode(lampTrigger, OUTPUT);
  
//mesh.setDebugMsgTypes( ERROR | MESH_STATUS | CONNECTION | SYNC | COMMUNICATION | GENERAL | MSG_TYPES | REMOTE ); // all types on
  mesh.setDebugMsgTypes( ERROR | STARTUP );  // set before init() so that you can see startup messages
  //Serial.println(a);
  mesh.init( MESH_PREFIX, MESH_PASSWORD, &userScheduler, MESH_PORT );
  mesh.onReceive(&receivedCallback);
  mesh.onNewConnection(&newConnectionCallback);
  mesh.onChangedConnections(&changedConnectionCallback);
  mesh.onNodeTimeAdjusted(&nodeTimeAdjustedCallback);

  userScheduler.addTask( taskSendMessage );
  taskSendMessage.enable();
  
  digitalWrite(dbgLed, HIGH);
  delay(1000);
  digitalWrite(dbgLed, LOW);
  
}

void loop() {
  // it will run the user scheduler as well
  mesh.update();

  float speed = calc_speed(radarMotionPin);
      if(speed >= speedThreshold)//check if the sensed speed is greater than 5kmh
         {
            onFlag = 1;
            digitalWrite(lampTrigger, HIGH);
            digitalWrite(dbgLed,HIGH);
            timecount = millis();
         } 

      
  if(((millis() - timecount) >= waitduration) && (calc_speed(radarMotionPin)<speedThreshold))
      {
        //WAIT DURATION IS 100MS
        //time count is update in the "receivedCallback" function and
        //onFlag is update the isr routine with RISING action.
        
        digitalWrite(lampTrigger, LOW);
        digitalWrite(dbgLed , LOW);
        onFlag = 0; //set the on flag low if all is low
      }
    

   
}

String construnct_json(){
  //we need to create a json object and serialize it
  JSONVar myObject;
  if( onFlag == 1 || (calc_speed(radarMotionPin)>=speedThreshold)){
       myObject["ID"] = myNodeID;
       myObject["state"] = "ON";
       Serial.print("my keys = ");
        Serial.println(myObject.keys());
        
         String jsonString = JSON.stringify(myObject);
         Serial.print("JSON.stringify(myObject) = ");
         Serial.println(jsonString);
         
       return (jsonString);
  }
  else{
  return "0";
  }
}


float calc_speed(int PIN_NUMBER)
{

     unsigned int x;
     

    
     pulseIn(PIN_NUMBER, HIGH);
      unsigned int pulse_length = 0;
      for (x = 0; x < AVERAGE; x++)
       {
          pulse_length = pulseIn(PIN_NUMBER, HIGH); 
          pulse_length += pulseIn(PIN_NUMBER, LOW);    
          mysamples[x] =  pulse_length;
       }
      
      bool mysamples_ok = true;
      unsigned int nbPulsesTime = mysamples[0];
        for (x = 1; x < AVERAGE; x++)
        {
          nbPulsesTime += mysamples[x];
          if ((mysamples[x] > mysamples[0] * 2) || (mysamples[x] < mysamples[0] / 2))
          {
            mysamples_ok = false;
          }
        }

    if (mysamples_ok)
    {
      unsigned int Ttime = nbPulsesTime / AVERAGE;
      unsigned int Freq = 1000000 / Ttime;
      Serial.print("\r\n");
      Serial.print(Freq);
      Serial.print("Hz : ");
      Serial.print(Freq/doppler_div);
      Serial.print("km/h\r\n");

        return (Freq/doppler_div);

     }      
    else{
         Serial.print(".");      
    }      
     return 0;
   
}