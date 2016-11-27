#include <IRremote.h>

#define MASK1 B110000
#define MASK2 B001100
#define MASK3 B000011
#define PLAYER1 B010101
#define PLAYER2 B101010

#define WHITE_BG 60
#define RED_BG 61
#define GREEN_BG 62

#define LEFT_RIGHT 100
#define RIGHT_LEFT 101
#define LEFTCOL 200
#define MIDCOL 201
#define RIGHTCOL 202
#define TOPROW 250
#define MIDROW 251
#define BOTROW 252

int RECV_PIN = 4;
IRrecv irrecv(RECV_PIN);
decode_results results;
byte player = 1;
byte taken[3];
byte masks[3] = {MASK1,MASK2,MASK3};
byte turns;

void setup()
{
  Serial.begin(9600);
  irrecv.enableIRIn();
  // Sets pins 3,5,6 and 7 to output mode
  DDRD |= B11101000;
  //Pins 5,6, and 7  represent each row's ground
  //initially set to High so no rows light up
  PORTD |= B11100000;
  // Sets pins 8,9,10,11,12,13 to output mode
  DDRB |= B111111;
}

void loop() {
  if (irrecv.decode(&results)) {
    Serial.println(results.value, HEX);
    switch(results.value){
      //button 1 was pressed
      case 0xff30cf:
        // if the two bits that are not masked contain a value
        if (!(taken[0] & MASK1) ){
          //Puts the two bits representing a player to the column position corresponding to the button pressed
          taken[0] |= player << 4;
          // switches who's turn it is
		  Serial.write(if (player == 1){1}else{11});
          player ^= 3;
          turns++;
        }break;

      //button 2 was pressed
      case 0xff18e7:
        if (!(taken[0] & MASK2) ){
          taken[0] |= player << 2;
		  Serial.write(if (player == 1){2}else{12});
          player ^= 3;
          turns++;
        }break;

      //button 3 was pressed
      case 0xff7a85:      
        if (!(taken[0] & MASK3) ){
          taken[0] |= player;
		  Serial.write(if (player == 1){3}else{13});
          player ^= 3;
          turns++;
        }break;

      //button 4 was pressed
      case 0xff10ef:
        if (!(taken[1] & MASK1) ){
          taken[1] |= player << 4;
		  Serial.write(if (player == 1){4}else{14});
          player ^= 3;
          turns++;
        }break;

      //button 5 was pressed
      case 0xff38c7:
        if (!(taken[1] & MASK2) ){
          taken[1] |= player << 2;
		  Serial.write(if (player == 1){5}else{15});
          player ^= 3;
          turns++;
        }break;

      //button 6 was pressed
      case 0xff5aa5:
        if (!(taken[1] & MASK3) ){
          taken[1] |= player;
		  Serial.write(if (player == 1){6}else{16});
          player ^= 3;
          turns++;
        }break;

      //button 7 was pressed
      case 0xff42bd:
        if (!(taken[2] & MASK1) ){
          taken[2] |= player << 4;
		  Serial.write(if (player == 1){7}else{17});
          player ^= 3;
          turns++;
        }break;

      //button 8 was pressed
      case 0xff4ab5:
        if (!(taken[2] & MASK2) ){
          taken[2] |= player << 2;
		  Serial.write(if (player == 1){8}else{18});
          player ^= 3;
          turns++;
        }break;

      //button 9 was pressed
      case 0xff52ad:
        if (!(taken[2] & MASK3) ){
          taken[2] |= player;
		  Serial.write(if (player == 1){9}else{19});
          player ^= 3;
          turns++;
        }break;

      //button 0 was pressed
      case 0xff6897:
        reset();
        break;
    }
    check();
    irrecv.resume(); // Receive the next value
    
  }
  // Display the current places filled using leds
  for(int i=0; i < 3; i++){
    // sets PORTB to the current row
    PORTB = taken[i];
    byte rowbit = 1;
    rowbit <<= i+5;
    // Sets the bit representing the ground for the current row to low
    PORTD ^= rowbit;
    delay(1);
    PORTD |= rowbit;
  }
}

void check(){
  Serial.println(turns);
  int columns = B111111;
  byte leftdiagonal = 0;
  byte rightdiagonal = 0;
  
  for(int i=0; i < 3; i++){

    // ANDs all the rows together
    columns &= taken[i];
    
    // creates a single byte representing the left diagonal
    leftdiagonal |= taken[i] & masks[i];
    // creates a single byte representing the right diagonal
    rightdiagonal |= taken[i] & masks[2-i];

    //checks if a player one on a row
    if(taken[i] == PLAYER1){
        PORTB= PLAYER1; 
		Serial.write(if i == 0) {TOPROW}else if (if i == 1){MIDROW}else{BOTROW});
        win();         
      }
    else if(taken[i] == PLAYER2){  
        PORTB= PLAYER2;
		Serial.write(if i == 0) {TOPROW}else if (if i == 1){MIDROW}else{BOTROW});		
        win();        
      } 
  }
  
  //checks if a player one on a column
   if(columns){
      // log(columns)/log(2) (i.e. log to the base 2) gets the index of the 1 bit in columns
      // if it's even Player 1 wins, if odd Player 2 wins 
      if( ((int)(log(columns)/log(2)) %2) ==  0 ){
          PORTB= PLAYER1; 
          win();
       }else {
          Serial.println("Player Two");
          PORTB = PLAYER2;
          win();
       }
   }

   //checks if a player one on a diagonal
   if(leftdiagonal == PLAYER1 || rightdiagonal == PLAYER1){
      PORTB = PLAYER1;
	  Serial.write(if (leftdiagonal == PLAYER1){LEFT_RIGHT}else{RIGHT_LEFT});
      win();
   }else if(leftdiagonal == PLAYER2 || rightdiagonal == PLAYER2){
      PORTB = PLAYER2; 
	  Serial.write(if (leftdiagonal == PLAYER1){LEFT_RIGHT}else{RIGHT_LEFT});
      win();
   }

   // checks if there has been a draw
   if ( turns == 9){
      PORTB= B111111; 
      win();
    }
}

void win(){ 
      delay(50);
      for (int i =0; i < 10; i++){
        PORTD |= B00001000;
        delay(200);
        PORTD ^= B11101000;
      }
      reset();
}
void reset(){
  
  for (int i = 0; i < 3; i++) {
    taken[i] = 0;
  }
  turns = 0;
  player = 1;

}
