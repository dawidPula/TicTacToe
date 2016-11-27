import processing.serial.*;

Serial myPort;

void redraw(int c){
  switch(c){
    case 20: background(255, 255, 255);
             break;
    case 30: background(255,0,0);
             break;
    case 62: background(0,255,0);
             break;
  }

  stroke(128,128,128);
  strokeWeight(4);
  line(200,50,200,350);
  line(400,50,400,350);
  line(100,150,500,150);
  line(100,250,500,250);
}


void setup() {
  myPort = new Serial(this, Serial.list()[0], 9600);
    size(640, 360);
    redraw(60);
}

int fillSquare(int s){
  if (s>=1 && s<=9){
    stroke(255,0,0);
    fill(255,0,0); //red
  }else if(s>=11 && s<=19){
       stroke(0,255,0);
       fill (0,255,0); //green
    }
  switch (s){
     case 1: 
     case 11: ellipse(150,100,30,30);
              break;
     case 2:
     case 12: ellipse(300,100,30,30);
              break;
     case 3:
     case 13: ellipse(450,100,30,30);
              break;
     case 4:
     case 14: ellipse(150,200,30,30);
              break;
     case 5:
     case 15: ellipse(300,200,30,30);
              break;
     case 6:
     case 16: ellipse(450,200,30,30);
              break;
     case 7:
     case 17: ellipse(150,300,30,30);
              break;
     case 8:
     case 18: ellipse(300,300,30,30);
              break;
     case 9: 
     case 19: ellipse(450,300,30,30);
              break;
              
     case 62: redraw(s);
  }
  return s;
} 

void win(int w){
  stroke(0,0, 0);
  strokeWeight(10);
  switch(w){
    case 120: line(150,100,450,300); /* \     */
              return w-100;
    case 130: line(150,100,450,300); /* \     */
              return w-100;
    case 121: line(150,300,450,100); /* /     */
              return w-101;
    case 131: line(150,300,450,100); /* /     */
              return w-101;
    case 220: line(150,100,150,300); /* |     */
              return w-200;
    case 230: line(150,100,150,300); /* |     */
              return w-200;
    case 221: line(300,100,300,300); /*  |    */
              return w-201;
    case 231: line(300,100,300,300); /*  |    */
              return w-201;
    case 222: line(450,100,450,300); /*   |   */
              return w-202;
    case 232: line(450,100,450,300); /*   |   */
              return w-202;
    case 270: line(150,100,450,100); /* ���   */
              return w-250;
    case 280: line(150,100,450,100); /* ���   */
              return w-250;
    case 271: line(150,200,450,200); /* ---   */
              return w-251;
    case 281: line(150,100,450,100); /* ���   */
              return w-251;
    case 272: line(150,300,450,300); /* ___   */
              return w-252;
    case 282: line(150,100,450,100); /* ���   */
              return w-252;
  }
} 

void draw(){
  while (myPort.available()>0){
    int val = myPort.read();
    win(fillSquare(val));
    redraw(val)
    println(val);
  }
}
