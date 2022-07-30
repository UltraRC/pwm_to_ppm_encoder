#include <Arduino.h>
#include "read_receiver.h"

#define PPM_OUT 14
#define SYNC 7500                       // Sync signal [us]
#define CHANNEL_START 500               // Signal goes low for 0.5ms before transmitting a channel
#define PPM_PERIOD 20000                // us
#define RECEIVER_POLLING_PERIOD 20000   // us ~ 50 Hz

typedef enum {
    sync_start = 0,
    channel_start,
    channel_pulse
} state_t;

uint32_t channels[NUM_CHANNELS] = {1500, 1500, 1500, 1500, 1500, 1500}; // Six channels
state_t state = sync_start;

void poll_receiver(void);

void setup()
{
    receiver_init();
    pinMode(PPM_OUT, OUTPUT);
}

void loop()
{
    static uint64_t last_time = 0;
    static uint64_t receiver_polling_timer = 0;
    uint64_t current_time = esp_timer_get_time();
    uint64_t delta_time = current_time - last_time;
    static uint8_t channel = 0;
    
    if(current_time-receiver_polling_timer >= RECEIVER_POLLING_PERIOD)
    {
        receiver_polling_timer = current_time;
        poll_receiver();
    }

    switch(state)
    {
        case sync_start:
            digitalWrite(PPM_OUT, HIGH);                    // Start sync pulse ~ 7.5ms
            if(delta_time >= SYNC)
            {
                state = channel_start;
                last_time = current_time;
            }
            break;

        case channel_start:
            digitalWrite(PPM_OUT, LOW);
            if(delta_time >= CHANNEL_START)
            {
                if(channel < NUM_CHANNELS)
                {
                    state = channel_pulse;
                }
                else
                {
                    state = sync_start;
                    channel = 0;
                }
                last_time = current_time;
            }
            break;

        case channel_pulse:
            digitalWrite(PPM_OUT, HIGH);
            if(delta_time >= channels[channel] - CHANNEL_START)
            {
                state = channel_start;
                last_time = current_time;
                channel++;
            }
            break;
    }
}

/**
 * @brief Update receiver channel values
 * 
 */
void poll_receiver(void)
{
    receiver_update();
    for(uint8_t i=0; i<NUM_CHANNELS; i++)
    {
        channels[i] = getChannelPulsewidth((Channel_t)i);
    }
}