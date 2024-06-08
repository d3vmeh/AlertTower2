#include <RadioLib.h>



// SX1262 has the following connections:
// NSS pin:   10
// DIO1 pin:  2
// NRST pin:  3
// BUSY pin:  9
SX1262 radio = new Module(10, 2, 3, 9);




int transmissionState = RADIOLIB_ERR_NONE;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);

  // initialize SX1262 with default settings
  Serial.print(F("[SX1262] Initializing ... "));
  int state = radio.begin();
  if (state == RADIOLIB_ERR_NONE) {
    Serial.println(F("success!"));
  } else {
    Serial.print(F("failed, code "));
    Serial.println(state);
    while (true);
  }

  // set the function that will be called
  // when packet transmission is finished
  radio.setPacketSentAction(setFlag);

  // start transmitting the first packet
  Serial.print(F("[SX1262] Sending first packet ... "));

  // you can transmit C-string or Arduino string up to
  // 256 characters long
  transmissionState = radio.startTransmit("Hello World!");

}

volatile bool transmittedFlag = false;

// this function is called when a complete packet
// is transmitted by the module
// IMPORTANT: this function MUST be 'void' type
//            and MUST NOT have any arguments!
#if defined(ESP8266) || defined(ESP32)
  ICACHE_RAM_ATTR
#endif
void setFlag(void) {
  // we sent a packet, set the flag
  transmittedFlag = true;
}

// counter to keep track of transmitted packets
int count = 0;

void loop() {
    // put your main code here, to run repeatedly:
  if(transmittedFlag) {
      // reset flag
      transmittedFlag = false;
 
      if (transmissionState == RADIOLIB_ERR_NONE) {
        // packet was successfully sent
        Serial.println(F("transmission finished!"));

        // NOTE: when using interrupt-driven transmit method,
        //       it is not possible to automatically measure
        //       transmission data rate using getDataRate()

      } else {
        Serial.print(F("failed, code "));
        Serial.println(transmissionState);

      }

      // clean up after transmission is finished
      // this will ensure transmitter is disabled,
      // RF switch is powered down etc.
      radio.finishTransmit();

      // wait a second before transmitting again
      delay(1000);

      // send another one
      Serial.print(F("[SX1262] Sending another packet ... "));

      // you can transmit C-string or Arduino string up to
      // 256 characters long
      String str = "Hello World! #" + String(count++);
      transmissionState = radio.startTransmit(str);
  }

}
