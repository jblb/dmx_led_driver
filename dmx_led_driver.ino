

#include "Common.h"
#include "MessageLabels.h"
#include "RDMHandlers.h"
#include "UsbProReceiver.h"
#include "UsbProSender.h"
#include "WidgetSettings.h"
#include "WidgetSettings.h"
#include "Tlc5940.h"s


// Define the variables from Common.h
char DEVICE_NAME[] = "Arduino RGB driver";
byte DEVICE_NAME_SIZE = sizeof(DEVICE_NAME);
char MANUFACTURER_NAME[] = "Open Lighting & Haum";
byte MANUFACTURER_NAME_SIZE = sizeof(MANUFACTURER_NAME);

UsbProSender sender;
RDMHandler rdm_handler(&sender);

// Pin constants
const byte LED_PIN = 2;

// device setting
const byte DEVICE_PARAMS[] = {0, 1, 0, 0, 40};
const byte DEVICE_ID[] = {1, 0};

// global state
byte led_state = LOW;  // flash the led when we get data.

/**
 * Send the Serial Number response
 */
void SendSerialNumberResponse() {
  long serial = WidgetSettings.SerialNumber();
  sender.SendMessageHeader(SERIAL_NUMBER_LABEL, sizeof(serial));
  sender.Write((byte*) &serial, sizeof(serial));
  sender.SendMessageFooter();
}

/**
 * Send the device id / name response
 */
void SendDeviceResponse() {
  sender.SendMessageHeader(NAME_LABEL,
                           sizeof(DEVICE_ID) + sizeof(DEVICE_NAME));
  sender.Write(DEVICE_ID, sizeof(DEVICE_ID));
  sender.Write((byte*) DEVICE_NAME, sizeof(DEVICE_NAME));
  sender.SendMessageFooter();
}
/**
 * Send the manufacturer id / name response
 */
void SendManufacturerResponse() {
  int esta_id = WidgetSettings.EstaId();
  sender.SendMessageHeader(MANUFACTURER_LABEL,
                           sizeof(esta_id) + sizeof(MANUFACTURER_NAME));
  // ESTA ID is sent in little endian format
  sender.Write(esta_id);
  sender.Write(esta_id >> 8);
  sender.Write((byte*) MANUFACTURER_NAME, sizeof(MANUFACTURER_NAME));
  sender.SendMessageFooter();
}

/**
 * Write the DMX values to the PWM pins.
 * @param data the dmx data buffer.
 * @param size the size of the dmx buffer.
 */
void SetPWM(const byte data[], unsigned int size) {
  unsigned int start_address = WidgetSettings.StartAddress() - 1;
  byte personality = WidgetSettings.Personality();

  for (byte i = 0; i < 15 ; ++i) {
    byte value = data[start_address + i];
    long valueTLC = value << 4;
    Tlc.set(i, valueTLC);
  }
}

/**
 * Called when there is no serial data
 */
void Idle() {
  if (WidgetSettings.PerformWrite()) {
    rdm_handler.QueueSetDeviceLabel();
  }
}

/*
 * Called when a full message is recieved from the host.
 * @param label the message label.
 * @param message the array of bytes that make up the message.
 * @param message_size the size of the message.
 */
void TakeAction(byte label, const byte *message, unsigned int message_size) {
  switch (label) {
    case PARAMETERS_LABEL:
      // Widget Parameters request
      sender.WriteMessage(PARAMETERS_LABEL,
                          sizeof(DEVICE_PARAMS),
                          DEVICE_PARAMS);
      break;
    case DMX_DATA_LABEL:
      if (message[0] == 0) {
        // 0 start code
        led_state = !led_state;
        digitalWrite(LED_PIN, led_state);
        SetPWM(&message[1], message_size);
       }
      break;
    case SERIAL_NUMBER_LABEL:
      SendSerialNumberResponse();
      break;
    case NAME_LABEL:
      SendDeviceResponse();
      break;
    case MANUFACTURER_LABEL:
      SendManufacturerResponse();
      break;
     case RDM_LABEL:
      // HandleRDMMessage(message, message_size);
      break;
  }
}



void setup() {
	Tlc.init();
	WidgetSettings.Init();
	byte personality = WidgetSettings.Personality();
	
	pinMode(LED_PIN, OUTPUT);
	digitalWrite(LED_PIN, led_state);
	// SetupRDMHandling();

	}
	
void loop() {
	UsbProReceiver receiver(TakeAction, Idle);
	receiver.Read();
	}
