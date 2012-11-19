import processing.serial.*;

Serial myPort;      // The serial port
int inByte = -1;    // Incoming serial data


void setup() {
  size(640, 480);
  // create a font with the third font available to the system:
  PFont myFont = createFont(PFont.list()[2], 14);
  textFont(myFont);

  // List all the available serial ports:
  println(Serial.list());

  // I know that the first port in the serial list on my mac
  // is always my  FTDI adaptor, so I open Serial.list()[0].
  // In Windows, this usually opens COM1.
  // Open whatever port is the one you're using.
  String portName = Serial.list()[4];
  myPort = new Serial(this, portName, 9600);
}

void draw() {
  background(100);
  line(0, 320, 640, 320);
  text("Last Received: " + inByte, 10, 400);
  
  if (mousePressed) {
    frameRate(40);
    if (mouseY > 320) {
      myPort.write('c');
    } else {
     myPort.write('x');
     myPort.write(mouseX / 10);
     myPort.write('y');
     myPort.write(mouseY / 10);
    }
  } else { 
    frameRate(5);
  } // End if mousePressed
}

void serialEvent(Serial myPort) {
  inByte = myPort.read();
}



