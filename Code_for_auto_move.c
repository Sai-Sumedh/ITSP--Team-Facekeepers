/* 
  
*/
#include<L298N.h>
#include<ArduinoSTL.h>

#define ENL 4
#define IN1L 5
#define IN2L 6

#define ENR 8
#define IN1R 9
#define IN2R 10

L298N motorL(ENL,IN1L,IN2L);
L298N motorR(ENR,IN1R,IN2R);

bool right=0,left=1;
int t_for_90=60 ;		//rough estimate by calculation-assuming wheel speed as 900*(200/255)rpm,wheel radius-5cm,
				//so speed=0.05*900*(40/51)*2(pi)/60, which is approx.4m/s.
				//So,omega about center(while rotating) is 4/(0.15),
				//thus t=(theta)/(omega)=(pi)/(2*25) which is approx. 0.06s--60ms.
				//Actual value needs to be set after experiment.
bool forward=1,backward=0;
int i,j,time_onestep;		//time_onestep is time taken by the bot to complete one step-i.e. move from one coordinate(cell) to another(needs to be set after experiment)
int i_init=0,j_init=500;  //initial position of the bot-to account for any protrusions towards the left side.
String state;           

std::vector< std::vector<String> > room(1000,std::vector<String>(1000,"nc")) ;	//room is divided into cells(40cm*40cm-about the bot's dimensions-to assign a unique position to the bot)-each cell represented by a coordinate
                                                                          //maximum area cleanable is 10^6 *0.16= 1.6 lakh m^2(actually depends on battery durability)

void update(String &state,bool direction){   //used to update the orientation of the bot ONLY
  if(direction == left){
    if(state == "fr")
	state = "bc";
else if(state == "br")
	state ="fc";
else if(state == "fc")
state = "fr";
else state =  "br";		//state was bc
  }
  else{               //direction==right
    if(state == "fr")
	state = "fc";
else if(state == "br")
	state ="bc";
else if(state == "fc")
state = "br";
else state =  "fr";		//state was bc
  }
}

void turn(bool direction){
  if(direction == right){
    motorL.forwardFor(t_for_90);
    motorR.backwardFor(t_for_90);
    update(state,right);
    
  }
  else{                                   //direction== left
     motorL.backwardFor(t_for_90);
    motorR.forwardFor(t_for_90);
    update(state,left);
  }
}
void straight(bool x){
  if( x == forward ){
    motorL.forwardFor(time_onestep);
    motorR.forwardFor(time_onestep);
  
     if(state == "fr"){
  	  j++;room[i-1][j]="w";}
else if(state == "br"){
	    j--;room[i+1][j]="w";}
else if(state == "fc"){
      i++;room[i][j+1]="w";}
else  {i--;room[i][j-1]="w";}  //state was bc
  room[i][j]="c";
  
  }
  else{                                   //x== backward
    motorL.backwardFor(time_onestep);
    motorR.backwardFor(time_onestep);   //till the bot comes back one step                               

     if(state == "fr"){
  	  room[i+1][j]="w";j--;}
else if(state == "br"){
	    room[i-1][j]="w";j++;}
else if(state == "fc"){
      room[i][j-1]="w";i--;}
else  {room[i][j+1]="w";i++;}  //state was bc
  room[i][j]="c";
  }
}
void stop(){
  motorL.stop();
  motorR.stop();
}

#include<NewPing.h>
#define SONAR_NUM     8 // Number of sensors.
#define MAX_DISTANCE 200 // Maximum distance (in cm) to ping.
#define PING_INTERVAL 33 // Milliseconds between sensor pings (29ms is about the min to avoid cross-sensor echo).
#define min_distance 40
bool R=0,L=0,F=0;
bool c[4]={false};

unsigned long pingTimer[SONAR_NUM]; // Holds the times when the next ping should happen for each sensor.
unsigned int cm[SONAR_NUM];         // Where the ping distances are stored.
uint8_t currentSensor = 0;          // Keeps track of which sensor is active.

NewPing sonar[SONAR_NUM] = {     // Sensor object array, 0,1,2,3-front sensors;4,5-left(back,front) sensors;6,7-right(back,front) sensors.
  NewPing(41, 41, MAX_DISTANCE), // Each sensor's trigger pin, echo pin, and max distance to ping.
  NewPing(43, 43, MAX_DISTANCE),
  NewPing(45, 25, MAX_DISTANCE),
  NewPing(21, 21, MAX_DISTANCE),
  NewPing(23, 23, MAX_DISTANCE),
  NewPing(25, 25, MAX_DISTANCE),
  NewPing(27, 27, MAX_DISTANCE),
  NewPing(29, 29, MAX_DISTANCE),
 };

void setup() {
  motorL.setSpeed(200);
  motorR.setSpeed(200);				// on a scale 0 to 255, where 0-minimum and 255-maximum.
  
  Serial.begin(115200);
  pingTimer[0] = millis() + 75;           // First ping starts at 75ms, gives time for the Arduino to chill before starting.
  for (uint8_t i = 1; i < SONAR_NUM; i++) // Set the starting time for each sensor.
    pingTimer[i] = pingTimer[i - 1] + PING_INTERVAL;
    
    i=i_init;
    j=j_init;
                                                      //wall mapping
   while(!( state =="bc" && i==i_init+1 && j==j_init)){
  for (uint8_t i = 0; i < SONAR_NUM; i++) { // Loop through all the sensors.
    if (millis() >= pingTimer[i]) {         // Is it this sensor's time to ping?
      pingTimer[i] += PING_INTERVAL * SONAR_NUM;  // Set next time this sensor will be pinged.
      if (i == 0 && currentSensor == SONAR_NUM - 1) oneSensorCycle(); // Sensor ping cycle complete, do something with the results.
      sonar[currentSensor].timer_stop();          // Make sure previous timer is canceled before starting a new ping (insurance).
      currentSensor = i;                          // Sensor being accessed.
      cm[currentSensor] = 0;                      // Make distance zero in c there's no ping echo for this sensor.
      sonar[currentSensor].ping_timer(echoCheck); // Do the ping (processing continues, interrupt will call echoCheck to look for echo).
    }
  }
  // Other code that *DOESN'T* analyze ping results 
  decide();
  action(case_no(c));
   }
   turn(right);   //state was bc, i is i_init+1, j is j_init after turning right, state is now fr on i=i_init+1
   
    
}

void loop() {                 
                                                //taking sensor input

   for (uint8_t i = 0; i < SONAR_NUM; i++) { // Loop through all the sensors.
    if (millis() >= pingTimer[i]) {         // Is it this sensor's time to ping?
      pingTimer[i] += PING_INTERVAL * SONAR_NUM;  // Set next time this sensor will be pinged.
      if (i == 0 && currentSensor == SONAR_NUM - 1) oneSensorCycle(); // Sensor ping cycle complete, do something with the results.
      sonar[currentSensor].timer_stop();          // Make sure previous timer is canceled before starting a new ping (insurance).
      currentSensor = i;                          // Sensor being accessed.
      cm[currentSensor] = 0;                      // Make distance zero in case there's no ping echo for this sensor.
      sonar[currentSensor].ping_timer(echoCheck); // Do the ping (processing continues, interrupt will call echoCheck to look for echo).
    }
  }
  clean_row(i);
   
 if(room[i-1][j]=="c" && room[i+1][j]=="c"){    //termination condition.
 if(state=="fr" && room[i][j+1]=="c") stop();
 else if(state=="br" && room[i][j-1]=="c") stop();
   }
  
}

void echoCheck() { // If ping received, set the sensor distance to array.
  if (sonar[currentSensor].check_timer())
    cm[currentSensor] = sonar[currentSensor].ping_result / US_ROUNDTRIP_CM;
}

void oneSensorCycle() { // Sensor ping cycle complete, do something with the results.
  // The following code would be replaced with your code that does something with the ping results.
  for (uint8_t i = 0; i <=3; i++) {
   if(cm[i] <= min_distance && cm[i] !=0 ) ::F=1;
   
  }
  if((cm[4] <= min_distance && cm[4]!=0) || (cm[5] <= min_distance && cm[5]!=0)) ::L=1;
  if((cm[6] <= min_distance && cm[6]!=0) || (cm[7] <= min_distance && cm[7]!=0)) ::R=1;
  
}

void decide() {
  
  if (L==0) c[0]=1;// no wall to the left- need to get wall back on left side.
  if (L==1 && F==0) c[1]=1;        // R doesnt matter- one obstacle c(wall on one side).
  if (L==1 && R==0 && F==1) c[2]=1;// 2 obstacle c-need to take a right.
  if (L==1 && R==1 && F==1) c[3]=1;// 3 obstacle c-need to go back.
}

int case_no(bool c[]){
  for(int i=0;i<4;i++){
    if(c[i]==1) {
      return i;
      break;
    }
      
    }
}

void action(int case_number){
  if(case_number==0){
   turn(left);
  straight(forward);
   
  }
  else if(case_number==1){
    straight(forward);
    
  }
  else if(case_number==2){ // current location is room[i][j]
    stop();
     if(state == "fr"){
  	  room[i-1][j]="w";j++;}
else if(state == "br"){
	    room[i+1][j]="w";j--;}
else if(state == "fc"){
      room[i][j+1]="w";i++;}
else  {room[i][j-1]="w";i--;}  //state was bc
  room[i][j]="w";
    turn(right);
    
  }
  else{                 //case_number==3
     stop();
     if(state == "fr"){
  	  room[i-1][j]="w";room[i][j+1]="w";}
else if(state == "br"){
	    room[i+1][j]="w";room[i][j-1]="w";}

else if(state == "fc"){
      room[i][j+1]="w";room[i+1][j]="w";}

else  {room[i][j-1]="w";room[i-1][j]="w";}  //state was bc
  
    
    straight(backward);
    
  }
}

void clean_row(int i){
  while(F==0){      // no obstacle in front
  
      if(room[i-1][j]=="c" ){      //cell to the (left, in case of fr)side has been cleaned, so there is no obstacle to the left 
  
  if(state == "fr"){  // bot's position: room[i][j], going forward along row
    
    if(room[i][j+1]=="nc"){     //cell in front is yet to be cleaned
    straight(forward);
  }
    else{            // room[i][j+1]=="c", since it cannot be w( as a layer just inside w has already been cleaned)

     if(room[i+1][j]=="c" || R==1) stop();
     else switch_rows("fr");
    }
  
}
  else if (state=="br"){       
    if(room[i][j-1]=="nc"){     //cell in front is yet to be cleaned
    straight(forward);
  }
    else{            // room[i][j+1]=="c", since it cannot be w( as a layer just inside w has already been cleaned)

       if(room[i+1][j]=="c" || L==1) stop();
     else switch_rows("fr");
    }
  }
}
  else if(room[i-1][j]=="nc"){
    if(state=="fr"){
      if(L==1) {room[i-1][j]="ob";straight(forward);}
      else { // no obstacle to the left,floor not yet cleaned
    
        turn(left);
        straight(forward);
        while(L==1 && F==0){
          if(room[i-1][j]=="nc") straight(forward);
          else if(room[i-1][j]=="c") {
            stop();
            turn(right);    //now bot is back along a row
            clean_row(i);
          }
        }
      } 
  }
  else if(state=="br"){
      if(R==1) {room[i-1][j]="ob";straight(forward);}
      else { // no obstacle to the right,not yet cleaned floor
    
        turn(right);
        straight(forward);
        while((R==1 || room[i][j+1]=="c") && F==0){
          if(room[i-1][j]=="nc") straight(forward);
          else if(room[i-1][j]=="c") {
            stop();
            turn(left);    //now bot is back along a row
            clean_row(i);
          }
        }
      } 
  }
}
}             
                                                  // since outside the while loop => F==1
if(room[i-1][j]=="c" && room[i+1][j]=="c") stop();  //termination condition

  else   switch_rows(state);
}

void switch_rows(String state){
  if(state=="fr"){
    turn(right);
    straight(forward);
    turn(right);
  }
  else{       //state=="br" since switch_rows is to be used only between 2 rows.
    turn(left);
    straight(forward);
    turn(left);
  }
}
