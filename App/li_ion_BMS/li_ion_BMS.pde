import processing.serial.*;
import static javax.swing.JOptionPane.*;
/*****************************************
      Variables
*****************************************/
  String dataBuffer;
  int index;
  int s_index;
  int cellNumber;
  String[] cellVoltage = new String[10];
  String[] cellBalance = new String[10];
  String packCurrent;
  String charger;
  int[] charge = new int[10];
/*****************************************/
  //Square_1
  int s1_X1 = 50;
  int s1_X2 = 320;
  int s1_Y1 = 100;
  int s1_Y2 = 430;
  //Square_2
  int s2_X1 = 350;
  int s2_X2 = 590;
  int s2_Y1 = 100;
  int s2_Y2 = 430;
  //Square_3
  int s3_X1 = 215;
  int s3_X2 = 285;
  int s3_Y1 = 60;
  int s3_Y2 = 90;
  //Squares_4
  int s4_X = 170;
  int s4_Y = 133;
  int s4_dY = 20;
  int s4_L = 55;
  int s4_H = 15;
  //Squares_5
  int s5_X = 238;
  int s5_Y = 133;
  int s5_dY = 20;
  int s5_L = 55;
  int s5_H = 15;
  //Text_1
  int t1_X = 70;
  int t1_Y = 120;
  //Text_2
  int t2_Y = 40;
  //Text_3
  int t3_X = 80;
  int t3_Y = 145;
  //Text_4
  int t4_X = 450;
  int t4_Y = 200;
  //Colors
  color s_B1 = color(225);
  color s_B2 = color(127);
  color black = color(0);
  color red = color(128, 0, 0);
  color green = color(34, 177, 76);
  //Fonts
  PFont Calibri_Bold;
  PFont Calibri;
  //Serial
  Serial myPort;
  final boolean debug = true;
 /*****************************************/
 
void setup() {
  size(640, 480);
  Calibri_Bold = createFont("Calibri Bold", 20, true);
  Calibri = createFont("Calibri", 16, true);
  String COMx, COMlist = "";
  try {
    if(debug) printArray(Serial.list());
    int i = Serial.list().length;
    if (i != 0) {
      if (i >= 2) {
        // need to check which port the inst uses -
        // for now we'll just let the user decide
        for (int j = 0; j < i;) {
          COMlist += char(j+'a') + " = " + Serial.list()[j];
          if (++j < i) COMlist += ",  ";
        }
        COMx = showInputDialog("Which COM port is correct? (a,b,..):\n"+COMlist);
        if (COMx == null) exit();
        if (COMx.isEmpty()) exit();
        i = int(COMx.toLowerCase().charAt(0) - 'a') + 1;
      }
      String portName = Serial.list()[i-1];
      if(debug) println(portName);
      myPort = new Serial(this, portName, 9600); // change baud rate to your liking
      myPort.bufferUntil('\n'); // buffer until CR/LF appears, but not required..
    }
    else {
      showMessageDialog(frame,"Device is not connected to the PC");
      exit();
    }
  }
  catch (Exception e)
  { //Print the type of error
    showMessageDialog(frame,"COM port is not available (may\nbe in use by another program)");
    exit();
  }
  //Initializing variables
  index = 0;
  s_index = 0;
  cellNumber = 0;
  packCurrent = "0";
  charger = "0";
  for (int x = 0; x < 10; x++)
  {
    cellVoltage[x] = "0";
    cellBalance[x] = "0";
    charge[x] = 0;
  }
}
 
void draw() {
 /*****************************************
               Test
  *****************************************
    cellNumber = int(random(0, 11));
    //cellNumber = 5;
    charger = int(random(0, 2));
    packCurrent = nf(random(0, 0.5), 1, 2).replace(',', '.');
    cellVoltage[0] = nf(random(3.0, 4.2), 1, 2).replace(',', '.');
    cellVoltage[1] = nf(random(3.0, 4.2), 1, 2).replace(',', '.');
    cellVoltage[2] = nf(random(3.0, 4.2), 1, 2).replace(',', '.');
    cellVoltage[3] = nf(random(3.0, 4.2), 1, 2).replace(',', '.');
    cellVoltage[4] = nf(random(3.0, 4.2), 1, 2).replace(',', '.');
    cellVoltage[5] = nf(random(3.0, 4.2), 1, 2).replace(',', '.');
    cellVoltage[6] = nf(random(3.0, 4.2), 1, 2).replace(',', '.');
    cellVoltage[7] = nf(random(3.0, 4.2), 1, 2).replace(',', '.');
    cellVoltage[8] = nf(random(3.0, 4.2), 1, 2).replace(',', '.');
    cellVoltage[9] = nf(random(3.0, 4.2), 1, 2).replace(',', '.');
    
    cellBalance[0] = int(random(0, 2));
    cellBalance[1] = int(random(0, 2));
    cellBalance[2] = int(random(0, 2));
    cellBalance[3] = int(random(0, 2));
    cellBalance[4] = int(random(0, 2));
    cellBalance[5] = int(random(0, 2));
    cellBalance[6] = int(random(0, 2));
    cellBalance[7] = int(random(0, 2));
    cellBalance[8] = int(random(0, 2));
    cellBalance[9] = int(random(0, 2));
    
 *****************************************/
 noLoop();
 init();
 cell_number();
 charging_field();
 cell_output();
 charge_fill();
 charge_init();
 balance_fill();
 pack_output();
}

void init() {
  background(192);
  hint(ENABLE_STROKE_PURE);
  textFont(Calibri_Bold);
  fill(black);
  text("Battery Management System", 200, 30);
  strokeWeight(2);
  //Square_1
  stroke(s_B1);
  line(s1_X1, s1_Y1, s1_X1, s1_Y2);
  line(s1_X1, s1_Y1, s1_X2, s1_Y1);
  stroke(s_B2);
  line(s1_X1, s1_Y2, s1_X2, s1_Y2);
  line(s1_X2, s1_Y1, s1_X2, s1_Y2);
  //Square_2
  stroke(s_B1);
  line(s2_X1, s2_Y1, s2_X2, s2_Y1);
  line(s2_X1, s2_Y1, s2_X1, s2_Y2);
  stroke(s_B2);
  line(s2_X1, s2_Y2, s2_X2, s2_Y2);
  line(s2_X2, s2_Y1, s2_X2, s2_Y2);
  //Text_1
  textFont(Calibri);
  text("Connected Cells:", t1_X, t1_Y-t2_Y);
  //Text_2
  fill(black);
  text("Cell", t1_X, t1_Y);
  text("Voltage", t1_X+40, t1_Y);
  text("Charge", t1_X+105, t1_Y);
  text("Balance", t1_X+170, t1_Y);
  //Text_4
  fill(black);
  textFont(Calibri, 20);
  text("Pack", t4_X, t1_Y);
  textLeading(60);
  text("Voltage:\nCurrent:", t4_X-80, t4_Y);
  text("V\nA", t4_X+45, t4_Y);
}

void cell_number() {
  textFont(Calibri);
  text(cellNumber, t1_X+115, t1_Y-t2_Y);
}

void charging_field() {
  //Square_3
  hint(ENABLE_STROKE_PURE);
  strokeWeight(2);
  noStroke();
  if (charger.equals("1\n")) fill(green);
  else fill(red);
  rect(s3_X1, s3_Y1, s3_X2-s3_X1, s3_Y2-s3_Y1);
  stroke(black);
  line(s3_X1, s3_Y1, s3_X2, s3_Y1);
  line(s3_X1, s3_Y1, s3_X1, s3_Y2);
  stroke(s_B2);
  line(s3_X2, s3_Y1, s3_X2, s3_Y2);
  line(s3_X1, s3_Y2, s3_X2, s3_Y2);
  fill(black);
  text("Charging", t1_X+150,t1_Y-t2_Y);
}

void cell_output() {
  for (int x = 0; x < cellNumber; x++) {
    text(x+1, t3_X, t3_Y+x*20);
    text(cellVoltage[x], t3_X+35, t3_Y+x*20);
    text("V", t3_X+65, t3_Y+x*20);
  }
}

void charge_init() {
  //Squares_4
  hint(DISABLE_STROKE_PURE);
  stroke(black);
  noFill();
  strokeWeight(1);
  for (int x = 0; x < cellNumber; x++) {
    rect(s4_X, s4_Y+x*s4_dY, s4_L, s4_H);
    fill(black);
    rect(s4_X, s4_Y+x*s4_dY, charge[x], s4_H);
    noFill();
  }
}

void balance_fill() {
  //Squares_5
  hint(DISABLE_STROKE_PURE);
  stroke(black);
  noFill();
  strokeWeight(1);
  for (int x = 0; x < cellNumber; x++) {
    rect(s5_X, s5_Y+x*s5_dY, s5_L, s5_H);
    if (cellBalance[x].equals("1\n")) {
      fill(black);
      rect(s5_X, s5_Y+x*s5_dY, s5_L, s5_H);
      noFill();
    }
  }
}

void pack_output() {
  fill(black);
  textFont(Calibri, 20);
  text(pack_voltage(), t4_X-5, t4_Y);
  text(packCurrent, t4_X+5, t4_Y+60);
}

void charge_fill() {
  for (int x = 0; x < cellNumber ;x++) charge[x] = int(((float(cellVoltage[x]))-3)*55/1.2);
}

String pack_voltage() {
  float packVoltage = 0;
  for (int x = 0; x < cellNumber ;x++) packVoltage += float(cellVoltage[x]);
  return nf(packVoltage, 2, 2).replace(',', '.');
}

void serialEvent (Serial myPort) {
  dataBuffer = myPort.readStringUntil('\n');
  if ((dataBuffer.equals("Start\n")) || (index > 0) ) {
    switch (index) {
      case 0:
        index++;
      break;
      case 1:
        cellNumber = int(trim(dataBuffer));
        index++;
      break;
      case 2:
        if (s_index < (cellNumber-1)) {
          cellVoltage[s_index] = dataBuffer;
          s_index++;
        }
        else {
          cellVoltage[s_index] = dataBuffer;
          index++;
          s_index = 0;
        }
      break;
      case 3:
        if (s_index < (cellNumber-1)) {
          cellBalance[s_index] = dataBuffer;
          s_index++;
        }
        else {
          cellBalance[s_index] = dataBuffer;
          index++;
          s_index = 0;
        }
      break;
      case 4:
        packCurrent = dataBuffer;
        index++;
      break;
      case 5:
        charger = dataBuffer;
        index = 0;
        loop();
      break;
    }
  }
  else myPort.clear();
}