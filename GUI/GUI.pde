import controlP5.*;
import meter.*;
import processing.serial.*;
Serial myPort;  // Create object from Serial class
String val;     // Data received from the serial port
Float val_float;

// https://www.sojamo.de/libraries/controlP5/

Meter m;
ControlP5 cp5;
Knob P_knob;
Knob I_knob;
Knob D_knob;
Slider slider;

void setup()
{
  //------------------------------------
  // Setup serial read and write
  //------------------------------------
  
  // I know that the first port in the serial list on my mac
  // is Serial.list()[0].
  // On Windows machines, this generally opens COM1.
  // Open whatever port is the one you're using.
  String portName = Serial.list()[2]; //change the 0 to a 1 or 2 etc. to match your port
  myPort = new Serial(this, portName, 115200);
  
  //------------------------------------
  // GUI
  //------------------------------------
  size(800,400);
  
  // Meter
  m = new Meter(this, 25, 50);
  String[] scalelabels = {"-40", "-30", "-20", "-10", "0", "10", "20", "30", "40"}; 
  m.setScaleLabels(scalelabels);
  //m.setScaleFontName("Times new roman");
  m.setDisplayDigitalMeterValue(true);
  m.setMinScaleValue(-40);
  m.setMaxScaleValue(40);
  m.setMinInputSignal(-40);
  m.setMaxInputSignal(40);
  m.setTitle("Angle");
  
  // ControlP5
  cp5  = new ControlP5(this);
  fill(0,0,0);
  text("PID Controller", 600, 50);
  
  P_knob = cp5
    .addKnob("P_Knob")
    .setRange(0,40)
    .setValue(25)
    .setPosition(500,70)
    .setRadius(30)
    .setDragDirection(Knob.VERTICAL)
    .setNumberOfTickMarks(10)
    .snapToTickMarks(true);
  I_knob = cp5
    .addKnob("I_Knob")
    .setRange(0,40)
    .setValue(0)
    .setPosition(600,70)
    .setRadius(30)
    .setDragDirection(Knob.VERTICAL)
    .setNumberOfTickMarks(10)
    .snapToTickMarks(true);
   D_knob = cp5
    .addKnob("D_Knob")
    .setRange(0,40)
    .setValue(20)
    .setPosition(700,70)
    .setRadius(30)
    .setDragDirection(Knob.VERTICAL)
    .setNumberOfTickMarks(10)
    .snapToTickMarks(true);
    
    fill(0,0,0);
    text("Time delay", 600, 200);
    slider = cp5
     .addSlider("slider")
     .setPosition(620,230)
     .setSize(20,100)
     .setRange(0,1000)
     .setNumberOfTickMarks(5);
}

void draw()
{
  if ( myPort.available() > 0) 
  {  // If data is available,
    val = myPort.readStringUntil('\n');
    
    try {
      val_float = float(val);
      if(val_float > 40){
        m.updateMeter(int(40));
      }
      else if(val_float < -40){
        m.updateMeter(int(-40));
      }
      else {
        m.updateMeter(int(val_float));
      }
      
      if(val_float.isNaN()){
        //println("Read NAN");
        println(val);
      }else{
        //println("Reading input: " + val_float);
      }  
  } catch (Exception  e) {
      //println("Can not convert");
      //println(val);
    }
  } 
}

void P_Knob(int v) {
  if(v < 10){
    myPort.write("P0"+v);
  }
  myPort.write("P"+v);
}

void I_Knob(int v) {
  if(v < 10){
    myPort.write("I0"+v);
  }
  myPort.write("I"+v);
}

void D_Knob(int v) {
  if(v < 10){
    myPort.write("D0"+v);
  }
  myPort.write("D"+v);
}

void slider(int v) {
  if(v < 10){
    myPort.write("T0"+v);
  }
  myPort.write("T"+v);
}
