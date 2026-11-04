#include <Wire.h>
#include "MAX30105.h"
#include "spo2_algorithm.h"

#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <BLE2902.h>

#define SERVICE_UUID        "e9cc598a-5c4a-458b-a727-e7f0b3ad8d21"
#define CHARACTERISTIC_UUID "688ba9f0-5c8d-41ea-85e5-5aa7f3abad2f"

// BLE
BLECharacteristic *pCharacteristic;
bool deviceConnected = false;

class MyServerCallbacks : public BLEServerCallbacks {

    void onConnect(BLEServer* pServer) override {
        deviceConnected = true;
        Serial.println("Phone connected!");
    }

    void onDisconnect(BLEServer* pServer) override {
        deviceConnected = false;

        Serial.println("Phone disconnected!");

        pServer->getAdvertising()->start();
    }
};

// MAX30102
MAX30105 Sensor;

#define No_Samples 100
#define New_Samples 50

uint32_t ir[No_Samples];
uint32_t red[No_Samples];

// Calculated measurements
int32_t spo2;
int8_t validSPO2;

int32_t heartRate;
int8_t validHeartRate;


// Read one sensor sample
void readSensorSample(uint32_t &redValue, uint32_t &irValue) {

    while (Sensor.available() == false) {
        Sensor.check();
    }

    redValue = Sensor.getRed();
    irValue = Sensor.getIR();

    Sensor.nextSample();
}


// Calculate heart rate and SpO2
void calculateMeasurements() {

    maxim_heart_rate_and_oxygen_saturation(
        ir,
        No_Samples,
        red,
        &spo2,
        &validSPO2,
        &heartRate,
        &validHeartRate
    );
}


void setup() {

    Serial.begin(115200);
    Wire.begin();

    // MAX30102 initialization
    if (!Sensor.begin(Wire, I2C_SPEED_FAST)) {

        Serial.println("sensor not found");

        while (1);
    }

    Serial.println("sensor found");

    // Sensor configuration, not validated cause of noise errors
    byte ledBrightness = 60;
    byte sampleAverage = 4;
    byte ledMode = 2;
    byte sampleRate = 100;
    int pulseWidth = 411;
    int adcRange = 4096;

    Sensor.setup(
        ledBrightness,
        sampleAverage,
        ledMode,
        sampleRate,
        pulseWidth,
        adcRange
    );

    Sensor.setPulseAmplitudeRed(0x0A);
    Sensor.setPulseAmplitudeGreen(0);


    // initial collection of samples. the first value of both heart rate and oxygen level will take time cause of latency 
    Serial.println("Collecting initial samples...");

    for (int i = 0; i < No_Samples; i++) {

        readSensorSample(red[i], ir[i]);
    }

    calculateMeasurements();


    

    BLEDevice::init("test_pulse_xiao");

    BLEServer *pServer = BLEDevice::createServer();

    pServer->setCallbacks(new MyServerCallbacks());

    BLEService *pService = pServer->createService(SERVICE_UUID);

    pCharacteristic = pService->createCharacteristic(
        CHARACTERISTIC_UUID,
        BLECharacteristic::PROPERTY_READ |
        BLECharacteristic::PROPERTY_NOTIFY
    );

    pCharacteristic->addDescriptor(new BLE2902());

    pService->start();


   //BLE advertising
    BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();

    pAdvertising->addServiceUUID(SERVICE_UUID);

    pAdvertising->setScanResponse(true);

    pAdvertising->setMinPreferred(0x06);
    pAdvertising->setMinPreferred(0x12);

    BLEDevice::startAdvertising();

    Serial.println("BLE initialized!");
    Serial.println("Device name: test_pulse_xiao");
    Serial.println("Waiting for phone connection...");
}


void loop() {

    // Shift previous 50 samples, this used to delay latency and 50 samples is used such that the previous reading will have only a little effect on the new reading, in case of error.
    

    for (int i = New_Samples; i < No_Samples; i++) {

        red[i - New_Samples] = red[i];
        ir[i - New_Samples] = ir[i];
    }


    // Collect 50 new samples

    for (int i = New_Samples; i < No_Samples; i++) {

        readSensorSample(red[i], ir[i]);
    }


    // Calculate updated measurements
    calculateMeasurements();


    // Display heart rate
    if (validHeartRate) {

        Serial.print("Heart Rate: ");
        Serial.print(heartRate);
        Serial.println(" BPM");

    } else {

        Serial.println("Heart Rate: Invalid reading");
    }


    // Display SpO2
    if (validSPO2) {

        Serial.print("SpO2: ");
        Serial.print(spo2);
        Serial.println(" %");

    } else {

        Serial.println("SpO2: Invalid reading");
    }


    // Display latest sensor values
    Serial.print("IR Value: ");
    Serial.println(ir[No_Samples]);

    Serial.print("RED Value: ");
    Serial.println(red[No_Samples]);


    // BLE DATA TRANSMISSION(used ai to validate the ble code)

    if (deviceConnected) {

        uint8_t dataPackage[2];

        if (validHeartRate && heartRate > 0 && heartRate <= 255) {

            dataPackage[0] = (uint8_t)heartRate;

        } else {

            dataPackage[0] = 0;
        }

        if (validSPO2 && spo2 > 0 && spo2 <= 100) {

            dataPackage[1] = (uint8_t)spo2;

        } else {

            dataPackage[1] = 0;
        }

        pCharacteristic->setValue(dataPackage, sizeof(dataPackage));

        pCharacteristic->notify();


        Serial.print("BLE Packet Sent: ");

        Serial.print(dataPackage[0]);

        Serial.print(", ");

        Serial.println(dataPackage[1]);
    }


}
