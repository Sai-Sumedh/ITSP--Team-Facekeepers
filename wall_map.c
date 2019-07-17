/*
*/
#include<AFMotor.h>
#include<ArduinoSTL.h>
#include<NewPing.h>

#define SONAR_NUM 6
#deine MAX_DISTANCE 200

AF_DCMotor motorL(1);
AF_DCMotor motorR(4);

const bool turn_right = false, turn_left = true, move_backward = false, move_forward = true;
const int inc_col_index = 1, dec_col_index = -1, inc_row_index = 2, dec_row_index = -2;
const int tile_clean = 10, tile_not_clean = -10, wall = 11, obstacle = 15;
const int row_index_init = 1, col_index_init = 1;
const int t_for_90 = 2000; //consider battery voltage change
const int time_onestep = 100; //set correct val

int state = inc_col_index; //default, redefine in setup and pass
int row_index, col_index;
int &i = row_index, &j = col_index;
std::vector< std::vector<int> > room_map(100, std::vector<int>(100, 0));

void update_after_turn(int &state, const bool direction){
  if(direction == turn_left){
    if(state == inc_col_index){
      state = dec_row_index;
    }
    else if (state == dec_col_index) {
      state = inc_row_index;
    }
    else if (state == inc_row_index) {
      state = inc_col_index;
    }
    else{
      state = dec_col_index;
    }
  }
  else{
    if(state == inc_col_index){
      state = inc_row_index;
    }
    else if(state == dec_col_index){
      state = dec_row_index;
    }
    else if(state == inc_row_index){
      state = dec_col_index;
    }
    else{
      state = inc_row_index;
    }
  }
}

void turn(const bool direction){
  if(direction == turn_left){
    motorL.run(BACKWARD);
    motorR.run(FORWARD);
    delay(t_for_90);
    update_after_turn(state, turn_left);
  }
  else{
    motorL.run(FORWARD);
    motorR.run(FORWARD);
    delay(t_for_90);
    update_after_turn(state, turn_right);
  }
}

void update_after_straight(const int state, const bool direction){
  if( direction == move_forward ){
    if(state == inc_col_index){
      j++;
      room_map[i-1][j] = wall;
    }
    else if(state == dec_col_index){
      j--;
      room_map[i+1][j] = wall;
    }
    else if(state == inc_row_index){
      i++;
      room_map[i][j-1] = wall;
    }
    else{
      i--;
      room_map[i][j-1] = wall;
    }
  }
  else{
    if(state == inc_col_index){
      room_map[i+1][j] = wall;
      j--;
    }
    else if(state == dec_col_index){
      room_map[i-1][j] = wall;
      j++;
    }
    else if(state == inc_row_index){
      room_map[i][j-1] = wall;
      i--;
    }
    else{
      room_map[i][j+1] = wall;
      i++;
    }
  }
  room_map[i][j] = tile_clean;
}

void straight(const bool direction){
  if(direction == move_forward){
    motorL.run(FORWARD);
    motorR.run(FORWARD);
    delay(time_onestep);
    update_after_straight(state, direction);
  }
  else{
    motorL.run(BACKWARD);
    motorR.run(BACKWARD);
    delay(time_onestep);
    update_after_straight(state, direction);
  }
}

void stop(){
  motorL.run(RELEASE);
  motorR.run(RELEASE);
  delay(100);
}

NewPing sonar[SONAR_NUM] = {
  NewPing(2, 2, MAX_DISTANCE),
  NewPing(3, 3, MAX_DISTANCE),
  NewPing(9, 9, MAX_DISTANCE),
  NewPing(5, 5, MAX_DISTANCE),
  NewPing(0, 0, MAX_DISTANCE),
  NewPing(10, 10, MAX_DISTANCE)
};

unsigned int cm[SONAR_NUM]
