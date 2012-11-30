// File name: led_gui.pde
// Author: jks Liu
// Some rights reserved: GPLv3 or later

import processing.serial.*;

Serial myPort;      // The serial port
int inByte = -1;    // Incoming serial data

// Reset GUI as write circle and blue line
void gui_reset() {  
  fill(255);
  for (int i = 0; i < 64; ++i) {
    for (int j = 0; j < 32; ++j) {
      ellipse(10 * i + 5, 10 * j + 5, 8, 8);
    }
  }
  
  stroke(12, 47, 234); // Blue
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

// Serial send data
void serial_send_data() {
  // mouseX and mouseY are volatile.
  int tmp_x = mouseX / 10;
  int tmp_y = mouseY / 10;
  
  if (tmp_y >= 32) {
    myPort.write('c');
    gui_reset();
  } else {
   myPort.write('x');
   myPort.write(tmp_x);
   myPort.write('y');
   myPort.write(tmp_y);
   fill(250, 8, 28);
   ellipse(10 * tmp_x + 5, 10 * tmp_y + 5, 8, 8);
  }
}
  
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
  
  gui_reset();
}

void draw() {
  text("Powered by jks Liu", 10, 400);
}

void serialEvent(Serial myPort) {
  inByte = myPort.read();
}

void mouseClicked() {
  serial_send_data();
}

void mouseDragged() {
  serial_send_data();
}



