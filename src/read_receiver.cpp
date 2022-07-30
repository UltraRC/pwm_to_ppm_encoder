#include <Arduino.h>
#include <stdint.h>
#include "read_receiver.h"

#define THR_PIN 36
#define AIL_PIN 39
#define ELE_PIN 34
#define RUD_PIN 35
#define GEA_PIN 32
#define AUX_PIN 33

#define MIN_PULSEWIDTH 500      // uS
#define MAX_PULSEWIDTH 2500

#define MIN_NORM_INPUT -1000    // Range of channel signal
#define MAX_NORM_INPUT 1000

uint32_t receiverPulsewidth[NUM_CHANNELS];  // Holds pulsewidths (500,2500) uS for each channel
volatile uint32_t receiverPulsewidthBuffer[NUM_CHANNELS];

void receiver_init(void)
{
    pinMode(THR_PIN, INPUT);        // Set PWM-pins to input-mode
    pinMode(AIL_PIN, INPUT);
    pinMode(ELE_PIN, INPUT);
    pinMode(RUD_PIN, INPUT);
    pinMode(GEA_PIN, INPUT);
    pinMode(AUX_PIN, INPUT);
    
    initPulsewidthArray();          // Give the pwm-array initial values

    attachInterrupt(digitalPinToInterrupt(THR_PIN), thrInterrupt, CHANGE);
    attachInterrupt(digitalPinToInterrupt(AIL_PIN), ailInterrupt, CHANGE);
    attachInterrupt(digitalPinToInterrupt(ELE_PIN), eleInterrupt, CHANGE);
    attachInterrupt(digitalPinToInterrupt(RUD_PIN), rudInterrupt, CHANGE);
    attachInterrupt(digitalPinToInterrupt(GEA_PIN), geaInterrupt, CHANGE);
    attachInterrupt(digitalPinToInterrupt(AUX_PIN), auxInterrupt, CHANGE);
}

void receiver_update()
{
    noInterrupts();
    memcpy(receiverPulsewidth, (const void *)receiverPulsewidthBuffer, sizeof(receiverPulsewidthBuffer));
    interrupts();
}

int32_t getChannel(Channel_t channel)
{
    return pulsewidthNormalize(channel);
}

uint32_t getChannelPulsewidth(Channel_t channel)
{
    return receiverPulsewidth[channel];
}

void measurePulseWidth(uint8_t pin, Channel_t channel, uint32_t* pulseStart)
{
    if(digitalRead(pin)) {
    *pulseStart = micros();
    } else {
    receiverPulsewidthBuffer[channel] = micros() - *pulseStart;
    }
}

int32_t pulsewidthNormalize(Channel_t channel)
{
    int32_t pw = getChannelPulsewidth(channel); // pw ==> Pulsewidth
    pw = map(pw, MIN_PULSEWIDTH, MAX_PULSEWIDTH, MIN_NORM_INPUT, MAX_NORM_INPUT);
    return constrain(pw, MIN_NORM_INPUT, MAX_NORM_INPUT);
}

void initPulsewidthArray(void)
{
    for(uint8_t i=THR; i<=AUX; i++) {
        receiverPulsewidthBuffer[i] = (MAX_PULSEWIDTH + MIN_PULSEWIDTH) / 2;
    }
}

// ----------------------- Interrupt functions ------------------------

void IRAM_ATTR thrInterrupt(void)
{
    static uint32_t pulseStart = 0;
    measurePulseWidth(THR_PIN, THR, &pulseStart);
}

void IRAM_ATTR ailInterrupt(void)
{
    static uint32_t pulseStart = 0;
    measurePulseWidth(AIL_PIN, AIL, &pulseStart);
}

void IRAM_ATTR eleInterrupt(void)
{
    static uint32_t pulseStart = 0;
    measurePulseWidth(ELE_PIN, ELE, &pulseStart);
}

void IRAM_ATTR rudInterrupt(void)
{
    static uint32_t pulseStart = 0;
    measurePulseWidth(RUD_PIN, RUD, &pulseStart);
}

void IRAM_ATTR geaInterrupt(void){
    static uint32_t pulseStart = 0;
    measurePulseWidth(GEA_PIN, GEA, &pulseStart);
}

void IRAM_ATTR auxInterrupt(void)
{
    static uint32_t pulseStart = 0;
    measurePulseWidth(AUX_PIN, AUX, &pulseStart);
}