#include <IRremote.h>
#include <EEPROM.h>

#define MASK1 B110000
#define MASK2 B001100
#define MASK3 B000011
#define PLAYER1 B010101
#define PLAYER2 B101010

#define WHITE_BG 60
#define LEFT_RIGHTG 120
#define LEFT_RIGHTR 130
#define RIGHT_LEFTG 121
#define RIGHT_LEFTR 131
#define LEFTCOLG 220
#define LEFTCOLR 230
#define MIDCOLG 221
#define MIDCOLR 231
#define RIGHTCOLG 222
#define RIGHTCOLR 232
#define TOPROWG 270
#define TOPROWR 280
#define MIDROWG 271
#define MIDROWR 281
#define BOTROWG 272
#define BOTROWR 282

int RECV_PIN = 4;
IRrecv irrecv(RECV_PIN);
decode_results results;
int player = 1;
byte taken[3];
byte masks[3] = {MASK1,MASK2,MASK3};
byte turns;

void setup()
{
  Serial.begin(9600);
  Serial.print("Player one won : ");
  Serial.println(EEPROM.read(1));
  Serial.print("Player two won : ");
  Serial.println(EEPROM.read(2));
  irrecv.enableIRIn();
  // Sets pins 3,5,6 and 7 to input mode
  DDRD |= B11101000;
  //Pins 5,6, and 7  represent each row's ground
  //initially set to High so no rows light up
  PORTD |= B11100000;
  // Sets pins 8,9,10,11,12,13 to input mode
  DDRB |= B111111;

}

void loop() {
  if (irrecv.decode(&results)) {
    switch(results.value){
      //button 1 was pressed
      case 0xff30cf:
        // if the two bits that are not masked contain a value
        if (!(taken[0] & MASK1) ){
          //Puts the two bits representing a player to the column position corresponding to the button pressed
          taken[0] |= player << 4;
          // switches who's turn it is
		  Serial.write( 1==player ? 1 : 11);
          player ^= 3;
          turns++;
        }break;

      //button 2 was pressed
      case 0xff18e7:
        if (!(taken[0] & MASK2) ){
          taken[0] |= player << 2;
		  Serial.write(1==player ? 2 : 12);
          player ^= 3;
          turns++;
        }break;

      //button 3 was pressed
      case 0xff7a85:      
        if (!(taken[0] & MASK3) ){
          taken[0] |= player;
		  Serial.write(1==player ? 3 : 13);
          player ^= 3;
          turns++;
        }break;

      //button 4 was pressed
      case 0xff10ef:
        if (!(taken[1] & MASK1) ){
          taken[1] |= player << 4;
		  Serial.write(1==player ? 4 : 14);
          player ^= 3;
          turns++;
        }break;

      //button 5 was pressed
      case 0xff38c7:
        if (!(taken[1] & MASK2) ){
          taken[1] |= player << 2;
		  Serial.write(1==player ? 5 : 15);
          player ^= 3;
          turns++;
        }break;

      //button 6 was pressed
      case 0xff5aa5:
        if (!(taken[1] & MASK3) ){
          taken[1] |= player;
		  Serial.write(1==player ? 6 : 16);
          player ^= 3;
          turns++;
        }break;

      //button 7 was pressed
      case 0xff42bd:
        if (!(taken[2] & MASK1) ){
          taken[2] |= player << 4;
		  Serial.write(1==player ? 7 : 17);
          player ^= 3;
          turns++;
        }break;

      //button 8 was pressed
      case 0xff4ab5:
        if (!(taken[2] & MASK2) ){
          taken[2] |= player << 2;
		  Serial.write(1==player ? 8 : 18);
          player ^= 3;
          turns++;
        }break;

      //button 9 was pressed
      case 0xff52ad:
        if (!(taken[2] & MASK3) ){
          taken[2] |= player;
		  Serial.write(1==player ? 9 : 19);
          player ^= 3;
          turns++;
        }break;

      //button 0 was pressed
      case 0xff6897:
        reset();
        Serial.write(WHITE_BG);
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
        Serial.write(0==i ? TOPROWG : i==1 ? MIDROWG : BOTROWG);
        win(PLAYER1);         
      }
    else if(taken[i] == PLAYER2){
        Serial.write(0==i ? TOPROWR : i==1 ? MIDROWR : BOTROWR); 
        win(PLAYER2);        
      } 
  }
  
  //checks if a player one on a column
   if(columns){
      // log(columns)/log(2) (i.e. log to the base 2) gets the index of the 1 bit in columns
      // if it's even Player 1 wins, if odd Player 2 wins
      int bitIndex = (int)(log(columns)/log(2)) ;
      if((bitIndex%2) ==  0 ){
          Serial.write(0==bitIndex ? LEFTCOLG : 2==bitIndex ? MIDCOLG : RIGHTCOLG);
          win(PLAYER1);
       }else {
          Serial.write(1==bitIndex ? LEFTCOLG : 3==bitIndex ? MIDCOLR : RIGHTCOLR);
          win(PLAYER2);
       }
   }

   //checks if a player one on a diagonal
   if(leftdiagonal == PLAYER1 || rightdiagonal == PLAYER1){
      Serial.write(leftdiagonal == PLAYER1 ? LEFT_RIGHTG : RIGHT_LEFTG);
      win(PLAYER1);
    }else if(leftdiagonal == PLAYER2 || rightdiagonal == PLAYER2){ 
      Serial.write(leftdiagonal == PLAYER1 ? LEFT_RIGHTR : RIGHT_LEFTR);
      win(PLAYER2);
      }

   // checks if there has been a draw
   if ( turns == 9){
      Serial.write(WHITE_BG);
      win(B111111);
    }
}

void win( byte winner){ 
      PORTB = winner;
      delay(50);
      for (int i =0; i < 10; i++){
        PORTD |= B00001000;
        delay(200);
        PORTD ^= B11101000;
      }
      if (winner != B111111);
        EEPROM.write(player ^ 3, EEPROM.read(player ^ 3) + 1);
      reset();
}
void reset(){
  
  for (int i = 0; i < 3; i++) {
    taken[i] = 0;
  }
  turns = 0;
  player = 1;
  Serial.print("Player one won : ");
  Serial.println(EEPROM.read(1));
  Serial.print("Player two won : ");
  Serial.println(EEPROM.read(2));

}
