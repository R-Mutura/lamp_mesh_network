
#include "painlessMesh.h"
#include <Arduino_JSON.h>

#include "variables.h"
JSONVar myVar;
//////////////////////////////////////////////////////////////
//pin definitions
#define radarMotionPin  15
#define lampTrigger     32
#define dbgLed          33
//#define connectionLed   25
//#define lampSwitch      4

int onFlag       = 0;
long timecount   = 0;
int waitduration = 100; //100 milisecond duration

String lampState= "OFF";

Scheduler userScheduler; // to control your personal task
painlessMesh  mesh;

// User stub
void sendMessage() ; // Prototype so PlatformIO doesn't complain
String construnct_json();

Task taskSendMessage( TASK_SECOND * 0.1 , TASK_FOREVER, &sendMessage );

void sendMessage() {
    if(onFlag==1){//only send if the radar is on and stays on for a few miliseconds
    String msg = construnct_json(); //construct json string 
    
    mesh.sendBroadcast( msg ); //send the data containig myNodeID and state
    onFlag = 0;//reset flag so message will be sent only once per trigger
    taskSendMessage.setInterval( random( TASK_SECOND * 0.1, TASK_SECOND * 0.5 ));
    }
     
    else{
      return;
    }  
}
   
   

// Needed for painless library
void receivedCallback( uint32_t from, String &msg ) {
  Serial.printf("Message: Received from %u msg=%s\n", from, msg.c_str());
  //TODO: Deserialize the data to het status and node number
        // compare numbers to see if its within range set on flag to 1 and start a timer for 1-second over flow to be used in the main loop
     StaticJsonDocument<256> doc;
     DeserializationError err = deserializeJson(doc, msg);
    if(err){
      Serial.print("ERROR: ");
      Serial.println(err.c_str());
      return;
      }  
   //obtaining the received data
    const char* state = doc["state"] ;
    lampState = String(state);
    int receivedID = doc["ID"];

     Serial.print("Lamp node: ");Serial.print(receivedID);
     Serial.print(" is");Serial.println(lampState);
     //call the calc function and turn on the laps where necessary
     if(lampState.equals("ON")){
      if(calc(receivedID)){ //check if we need to turn on this lamp with calc function
        //if true is returned here then turn on the light
          digitalWrite(lampTrigger, HIGH);
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
 void IRAM_ATTR isr() {
     onFlag = 1;
     digitalWrite(lampTrigger, HIGH);
     timecount = millis();//for time keeping of on duration
 }

void setup() {
  Serial.begin(115200);
  //attach interrupt
  pinMode(radarMotionPin, INPUT);
  attachInterrupt(radarMotionPin, isr, RISING);
  
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
}

void loop() {
  // it will run the user scheduler as well
  mesh.update();
  if(((millis() - timecount) >= waitduration))
      {
        //WAIT DURATION IS 100MS
        //time count is update in the "receivedCallback" function and
        //onFlag is update the isr routine with RISING action.
        
        digitalWrite(lampTrigger, LOW);
      }
    

   
}

String construnct_json(){
  if( onFlag == 1 && (digitalRead(radarMotionPin))){
       myVar["ID"] = myNodeID;
       myVar["state"] = "ON";

       return JSON.stringify(myVar);
  }
  return "0";
}
