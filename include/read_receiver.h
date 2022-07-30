#ifndef READ_RECEIVER_H
#define READ_RECEIVER_H

#define NUM_CHANNELS 6

typedef enum {
    THR = 0,
    AIL,
    ELE,
    RUD,
    GEA,
    AUX
} Channel_t;

void receiver_init(void);
void receiver_update();
int32_t getChannel(Channel_t channel);
uint32_t getChannelPulsewidth(Channel_t channel);
void measurePulseWidth(uint8_t pin, Channel_t channel, uint32_t* pulseStart);
int32_t pulsewidthNormalize(Channel_t channel);
void initPulsewidthArray(void);

// -------- Interrupts --------
void IRAM_ATTR thrInterrupt(void);
void IRAM_ATTR ailInterrupt(void);
void IRAM_ATTR eleInterrupt(void);
void IRAM_ATTR rudInterrupt(void);
void IRAM_ATTR geaInterrupt(void);
void IRAM_ATTR auxInterrupt(void);


#endif // PWM_TO_PPM_H