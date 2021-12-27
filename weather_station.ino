#include "ModbusMaster.h" //https://github.com/4-20ma/ModbusMaster

#define MAX485_RE_NEG  4 //D4 RS485 has a enable/disable pin to transmit or receive data. Arduino Digital Pin 2 = Rx/Tx 'Enable'; High to Transmit, Low to Receive
#define RX_PIN      33 //RX2 
#define TX_PIN      2  //TX2

// Configuration for Atmosheric temperature and humidity
#define ATMOS_TH 1

// Configuration for Wind speed sensor
#define WIND_SPEED 2

// Configuration for the Wind direction sensor
#define WIND_DIRECTION 3


// Fall-back call
#define Slave_ID 0

// Start address to read from
int DataAdd = 0000;



// instantiate ModbusMaster object
ModbusMaster modbus;

void preTransmission() {
  digitalWrite(MAX485_RE_NEG, HIGH); //Switch to transmit data
}

void postTransmission() {
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
  String windDirection = readDevice(WIND_DIRECTION, 1, false);
  String humidTemperature = readDevice(ATMOS_TH, 2, true);

  delay(3000);
}

String readDevice(int slaveID, int bufferSize, bool dataAddIncrement) {
  uint8_t result;
  String value;
  modbus.begin(slaveID, Serial2);
  delay(100);

  modbus.preTransmission(preTransmission);
  modbus.readHoldingRegisters(DataAdd, bufferSize);
  modbus.postTransmission(postTransmission);

  // Validating Response
  if (getResultMsg(&modbus, result) == "Success") {
    if (bufferSize == 1){
        value = String(modbus.getResponseBuffer(0));
        Serial.println("INFO | SUCCESS | Device " + String(slaveID) + " : " + value + " Reg address " + String(DataAdd));
    } else{
        String temperature = String(modbus.getResponseBuffer(0));
        String humidity = String(modbus.getResponseBuffer(1));
        value = temperature + "," + humidity;
        Serial.println("INFO | SUCCESS | Device " + String(slaveID) + " : " + value + " Combined param");
      }
  
  }
  else {
    value = getResultMsg(&modbus, result);
    Serial.println("ERROR | FAILED | Device " + String(slaveID) + " : " + value + "R eg address " + String(DataAdd));
  }
  delay(500);
  return value;
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