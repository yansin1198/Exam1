#include "mbed.h"
#include "uLCD_4DGL.h"
using namespace std::chrono;

Timer debounce;                  //define debounce timer
InterruptIn button(USER_BUTTON); //Interrupt on digital pushbutton input SW2
DigitalOut led1(LED1);

InterruptIn buttonUp(D10);
InterruptIn buttonDown(D11);
InterruptIn buttonConfirm(D12);

uLCD_4DGL uLCD(D1, D0, D2); // serial tx, serial rx, reset pin;
AnalogOut aout(PA_4); //D7
AnalogIn ain(A0);

void getSample();

float ADCdata[100];
int p; //samplePoint
int sampleIndex; //ADCdata Index
int countTimes;
float T = 100;
int frequency = 10;
int start = 0;
int currentRow = 6;
int prePosition = 6;
int rate = 1;
EventQueue queue(32 * EVENTS_EVENT_SIZE);
Thread t;

void waveGenerate() {
    uLCD.locate(3, 14);
    uLCD.printf("rate start!\n");
    float T = 0;
    while (1) {
        if (rate == 1) {
            T = 100000 / 8.4f;
            float peakHeight = 3.3f / 3.0f; 

            // divide the waveform into 240 parts. 
            if (p >= 240) {
                p = 0;
            }
            else if (p > 0 && p <= 80) {
                aout = p / 80.0f / peakHeight; // aout * 3.3 = 3
            }
            else if (p > 80 && p < 160) { 
                aout = 0.9f; // aout * 3.3 = 3
            }
            else if (p >= 160 && p < 240) {
                aout = (240 - p) / 80.0f / peakHeight; // aout * 3.3 = 3
            }
            p++;

            /*if (sampleIndex >= 100) {
                sampleIndex = 0;
                countTimes = 0;
            } else if (sampleIndex < 100 && (countTimes % (frequency/2) == 0)) {
                ADCdata[sampleIndex] = ain;
                sampleIndex++;
            }
            countTimes++;*/
            wait_us(T/25);
        } else if (rate == 2) {
            T = 100000 / 8.4f;
            float peakHeight = 3.3f / 3.0f; 

            // divide the waveform into 240 parts. 
            if (p >= 240) {
                p = 0;
            }
            else if (p > 0 && p <= 40) {
                aout = p / 40.0f / peakHeight; // aout * 3.3 = 3
            }
            else if (p > 40 && p < 200) { 
                aout = 0.9f; // aout * 3.3 = 3
            }
            else if (p >= 200 && p < 240) {
                aout = (240 - p) / 40.0f / peakHeight; // aout * 3.3 = 3
            }
            p++;

            /*if (sampleIndex >= 100) {
                sampleIndex = 0;
                countTimes = 0;
            } else if (sampleIndex < 100 && (countTimes % (frequency/2) == 0)) {
                ADCdata[sampleIndex] = ain;
                sampleIndex++;
            }
            countTimes++;*/
            wait_us(T/25);
        } else if (rate == 3) {
            T = 100000 / 8.4f;
            float peakHeight = 3.3f / 3.0f; 

            // divide the waveform into 240 parts.
            if (p >= 240) {
                p = 0;
            }
            else if (p > 0 && p <= 20) {
                aout = p / 20.0f / peakHeight; // aout * 3.3 = 3
            }
            else if (p > 20 && p < 220) { 
                aout = 0.9f; // aout * 3.3 = 3
            }
            else if (p >= 220 && p < 240) {
                aout = (240 - p) / 20.0f / peakHeight; // aout * 3.3 = 3
            }
            p++;

            /*if (sampleIndex >= 100) {
                sampleIndex = 0;
                countTimes = 0;
            } else if (sampleIndex < 100 && (countTimes % (frequency/2) == 0)) {
                ADCdata[sampleIndex] = ain;
                sampleIndex++;
            }
            countTimes++;*/
            wait_us(T/25);
        } else if (rate == 4) {
            T = 100000 / 8.4f;
            float peakHeight = 3.3f / 3.0f; 

            // divide the waveform into 240 parts.
            if (p >= 240) {
                p = 0;
            }
            else if (p > 0 && p <= 10) {
                aout = p / 10.0f / peakHeight; // aout * 3.3 = 3
            }
            else if (p > 10 && p < 230) { 
                aout = 0.9f; // aout * 3.3 = 3
            }
            else if (p >= 230 && p < 240) {
                aout = (240 - p) / 10.0f / peakHeight; // aout * 3.3 = 3
            }
            p++;

            /*if (sampleIndex >= 100) {
                sampleIndex = 0;
                countTimes = 0;
            } else if (sampleIndex < 100 && (countTimes % (frequency/2) == 0)) {
                ADCdata[sampleIndex] = ain;
                sampleIndex++;
            }
            countTimes++;*/
            wait_us(T/25);
        }
    }
}

void Selection() {
    uLCD.locate(3, 10);
    uLCD.printf("Confirm\n");
    if (currentRow == 6) {
        uLCD.locate(3, 11);
        uLCD.printf("1   rate\n");
        rate = 1;
    } else if (currentRow == 7) {
        uLCD.locate(3, 11);
        uLCD.printf("1/2 rate\n");
        rate = 2;
    } else if (currentRow == 8) {
        uLCD.locate(3, 11);
        uLCD.printf("1/4 rate\n");
        rate = 3;
    } else if (currentRow == 9) {
        uLCD.locate(3, 11);
        uLCD.printf("1/8 rate\n");
        rate = 4;
    }
}

void Info() {
   // Note that printf is deferred with a call in the queue
   // It is not executed in the interrupt context
   uLCD.locate(2, prePosition);
   uLCD.printf(" ");
   uLCD.locate(2, currentRow);
   uLCD.printf(">");
}

void ISR1() {
   if (duration_cast<milliseconds>(debounce.elapsed_time()).count() > 500) {
        prePosition = currentRow;
        currentRow -= 1; 
        if (currentRow <= 6) {
            currentRow = 6;
        }
        queue.call(Info);
        debounce.reset(); //restart timer when the toggle is performed
   }
}

void ISR2() {
   if (duration_cast<milliseconds>(debounce.elapsed_time()).count() > 500) {
        prePosition = currentRow;
        currentRow += 1; 
        if (currentRow >= 9) {
            currentRow = 9;
        }
        queue.call(Info);
        debounce.reset(); //restart timer when the toggle is performed
   }
}

void ISR3() {
   if (duration_cast<milliseconds>(debounce.elapsed_time()).count() > 500) {
        queue.call(Selection);
        queue.call(waveGenerate);
        debounce.reset();
   }
}

int main() {
    
    uLCD.background_color(0xFFFFFF);
    uLCD.textbackground_color(0xFFFFFF);
    uLCD.color(BLUE);
    uLCD.locate(2, currentRow);
    uLCD.printf(">");
    uLCD.locate(3, 6);
    uLCD.printf("1\n");
    uLCD.locate(3, 7);
    uLCD.printf("1/2\n");
    uLCD.locate(3, 8);
    uLCD.printf("1/4\n");
    uLCD.locate(3, 9);
    uLCD.printf("1/8\n");

    t.start(callback(&queue, &EventQueue::dispatch_forever));
    debounce.start();
    buttonUp.rise(&ISR1); // attach the address of the toggle
    buttonDown.rise(&ISR2);
    buttonConfirm.rise(&ISR3);

   while (1)
      ;
}

