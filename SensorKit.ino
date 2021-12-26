#include "ModbusMaster.h" //https://github.com/4-20ma/ModbusMaster
/*!
  We're using a MAX485-compatible RS485 Transceiver.
  Rx/Tx is hooked up to the hardware serial port at 'Serial'.
  The Data Enable (DE) and Receiver Enable (RE) pins are hooked up as follows:
*/
#define MAX485_RE_NEG  4 //D4 RS485 has a enable/disable pin to transmit or receive data. Arduino Digital Pin 2 = Rx/Tx 'Enable'; High to Transmit, Low to Receive
#define Slave_ID       1
#define RX_PIN      33 //RX2 
#define TX_PIN      2  //TX2

//----------------- Insert New Slave Address here! -------------
int StartAdd = 0000;
int StopAdd = 0001;

// instantiate ModbusMaster object
ModbusMaster modbus;
void preTransmission()
{
  digitalWrite(MAX485_RE_NEG, HIGH); //Switch to transmit data
}
void postTransmission()
{
  digitalWrite(MAX485_RE_NEG, LOW); //Switch to receive data
}
void setup()
{
  
  pinMode(MAX485_RE_NEG, OUTPUT);
  // Init in receive mode
  digitalWrite(MAX485_RE_NEG, LOW);
  // Modbus communication runs at 9600 baud
  Serial.begin(9600, SERIAL_8N1);
  Serial2.begin(9600, SERIAL_8N1, RX_PIN, TX_PIN);
  modbus.begin(Slave_ID, Serial2);
  // Callbacks allow us to configure the RS485 transceiver correctly
  modbus.preTransmission(preTransmission);
  modbus.postTransmission(postTransmission);
  
}

void loop() 
{
  uint8_t result;
  String Val;
  for (int reg=StartAdd; reg<StopAdd; reg++){ 
    modbus.preTransmission(preTransmission); 
    modbus.readHoldingRegisters(reg, 2);
    modbus.postTransmission(postTransmission);
    if (getResultMsg(&modbus, result) == "Success"){
      Val = String(modbus.getResponseBuffer(1));
    }
    else{
      Val = getResultMsg(&modbus, result);
    }
    Serial.println(String(reg)+" : "+Val);
    delay(500);
  }
  delay(3000);
}  
String getResultMsg(ModbusMaster *node, uint8_t result) 
{
  String tmpstr2;
  switch (result) 
  {
  case node->ku8MBSuccess:
    tmpstr2 = "Success";
    break;
  case node->ku8MBIllegalFunction:
    tmpstr2 = "Illegal Function";
    break;
  case node->ku8MBIllegalDataAddress:
    tmpstr2 = "Illegal Data Address";
    break;
  case node->ku8MBIllegalDataValue:
    tmpstr2 = "Illegal Data Value";
    break;
  case node->ku8MBSlaveDeviceFailure:
    tmpstr2 = "Slave Device Failure";
    break;
  case node->ku8MBInvalidSlaveID:
    tmpstr2 = "Invalid Slave ID";
    break;
  case node->ku8MBInvalidFunction:
    tmpstr2 = "Invalid Function";
    break;
  case node->ku8MBResponseTimedOut:
    tmpstr2 = "Response Timed Out";
    break;
  case node->ku8MBInvalidCRC:
    tmpstr2 = "Invalid CRC";
    break;
  default:
    tmpstr2 = "Unknown error: " + String(result);
    break;
  }
  return tmpstr2;
}
