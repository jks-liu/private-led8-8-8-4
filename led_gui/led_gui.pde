import processing.serial.*;

Serial myPort;      // The serial port
int inByte = -1;    // Incoming serial data

void setup() {
  size(640, 480);
  // create a font with the third font available to the system:
  PFont myFont = createFont(PFont.list()[0], 16);
  textFont(myFont);

  println(Serial.list());
  println(PFont.list());
  
  String serial_config[] = loadStrings("serial.config");
  int  serial_baud_rate = Integer.parseInt(serial_config[1]);

  String portName = serial_config[0];
  myPort = new Serial(this, portName, serial_baud_rate);
  
  for (int i = 0; i < 64; ++i) {
    for (int j = 0; j < 32; ++j) {
      ellipse(10 * i + 5, 10 * j + 5, 8, 8);
    }
  }
  
  stroke(12, 47, 234);
  for (int i = 0; i < 64; i = i + 8) {
    line(i * 10, 0, i * 10, 320);
    line(i * 10 + 1, 0, i * 10 + 1, 320);
  }
  
  for (int i = 0; i < 32; i = i + 8) {
    line(0, i * 10, 640, i *10);
    line(0, i * 10 + 1, 640, i *10 + 1);
  }
  
  stroke(0);
  line(0, 320, 640, 320);
  line(0, 321, 640, 321);
}

void draw() {
  text("Last Received:          ", 10, 400);
  text("Last Received: " + inByte, 10, 400);
}

void serialEvent(Serial myPort) {
  inByte = myPort.read();
}

void mouseClicked() {
  if (mouseY > 320) {
    myPort.write('c');
  } else {
   myPort.write('x');
   myPort.write(mouseX / 10);
   myPort.write('y');
   myPort.write(mouseY / 10);
  }
}

void mouseDragged() {
  if (mouseY > 320) {
    myPort.write('c');
  } else {
   myPort.write('x');
   myPort.write(mouseX / 10);
   myPort.write('y');
   myPort.write(mouseY / 10);
  }
}



