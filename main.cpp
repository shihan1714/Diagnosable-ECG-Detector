/* ECG Simulator from SD Card - Receiver
 * 
 * This program uses a Nucleo F303K8 to receive a short from another
 * F303K8 and convert it to a float value from 0 - 1 for output from an analog
 * output pin. The short sent from the other Nucleo (referred to as Sender) is 
 * divided by 2048 because it was obtained via an 11-bit ADC.
 * 
 * Modified heavily from: https://forums.mbed.com/t/two-nucleo-serial-communication-via-tx-and-rx-and-vice-versa/8131
 * 
 * Authors:    Amit J. Nimunkar and Lucas N. Ratajczyk
 * Date:       05-24-2021
 * 
 * Modified by Royal Oakes, 02-02-2022.
 * Modified by Andrew Shih, 05-04-2024
 */



#include "mbed.h"
#include "BME463_lib.h"
#include <cstdio>
#include <cstring>

#define SNR_THRESHOLD 0.09

//**************************************************************************
/* Pin Declarations */
//**************************************************************************
//Serial      pc(USBTX,USBRX);    // Optionally - Set up serial communication with the host PC for printing statement to console
Serial      sender(D1,D0);      // Set up serial communication with the Nucleo which is sending the signal to us (Sender)
AnalogIn    Ain(A0);        // Input 
DigitalOut  D11_Out(D11);   // Test sig out
DigitalOut  D12_Out(D12);   // cur_noise_state flag
DigitalOut  D13_LED(D13);   // QRS detection output
AnalogOut   ADC3(A3);       // signal input
AnalogOut   ADC4(A4);       // conigurable ADC output. Currently set to SPKI value
AnalogOut   ADC5(A5);       // Configurable ADC output. Currently not used. 

//**************************************************************************
/* SEND/RECEIVE VARIABLES */
//**************************************************************************
// This union is used to recieve data from the sender. Use data.s to access the
// data as a short, and use data.h to access the individual bytes of data.
typedef union _data {
    short s;
    char h[sizeof(short)];
} myShort;

myShort data;

char d;         // Variable to hold the current byte extracted from Sender
int num;        // Integer version of the myShort value
int i = 0;      // Index counter for number of bytes received
float samp_rate = 360.0f;           // Sample rate of the ISR

// Ticker for the ISR
Ticker sampTick; 

// Prototypes
void ISRfxn();

//**************************************************************************
/* GLOBAL VARIABLES */
//************************************************************************** 76 cols
/* State Variables */ 
bool FILTERING_FLAG = false;

/* DSP Relevant Variables */ 
float input = 0.0;

//**************************************************************************
/* Main Application */
//**************************************************************************
int main() {
//**************************************************************************
    /* Main App, Local Variables */ 
    float output[3] = {0.0, 0.0, 0.0};
    float NSR = 0.0;
    float npki = 0.0;
    float spki = 0.0;
    float npki_clean = 0.0;
    float spki_clean = 0.0;
    float npki_array[8] = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
    float spki_array[8] = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
    float spki_array_clean[8] = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
    float npki_array_clean[8] = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
    float thresholdi1 = 0.0;
    bool QRS_detected = false;
    bool cur_noise_state = false;
    bool prev_noise_state = false;
    float filter_splice = 0.0;

//**************************************************************************
    // Set up serial communication
    sender.baud(115200);
    //pc.baud(115200); // Optional debugging.
    
    // Sample num at a fixed rate
    sampTick.attach(&ISRfxn, 1.0f/samp_rate);
    // Get data from sender
//**************************************************************************
    while (1) { // MAIN LOOP //
//**************************************************************************
        /* Sender Code SPI Processing. DO NOT TOUCH */ 
        // Get the current character from Sender
        d = sender.getc();
        
        // If the byte we got was a '\0', it is possibly the terminator
        if (d == '\0' && i >= sizeof(short)){
            i = 0;                          // Reset index counter.
            num = (int) data.s;             // Convert the short to an int.
        } else if (i < sizeof(short)) {     // If 2 bytes haven't been received, 
            data.h[i++] = d;                // then the byte is added to data
        }
        //******************************************************************

        if(FILTERING_FLAG){
            filter_splice = pan_T_Filter(input, output);
            QRS_detected = pan_T_Threshold(output, &thresholdi1, &spki, &npki, spki_array, npki_array);

            NSR = npki/sqrt(npki*npki + spki*spki);
            cur_noise_state = NSR > SNR_THRESHOLD;
            
            if(!cur_noise_state){
                // Acceptable amount of noise, save copy to clean array
                save_array(npki_array, npki_array_clean, 8);
                save_array(spki_array, spki_array_clean, 8);
                spki_clean = spki;
                npki_clean = npki;
            }          
            
            if(!cur_noise_state && prev_noise_state){    
                // If state transitions from noisy to clean, restore spki and npki arrays to clean state 
                save_array(npki_array_clean, npki_array, 8);
                save_array(spki_array_clean, spki_array, 8);
                spki = array_average(spki_array, 8);
                npki = array_average(npki_array, 8);
            }
            
            prev_noise_state = cur_noise_state;
            
            ADC3 = input; 
            ADC4 = spki;

            D12_Out = cur_noise_state;
            D11_Out = QRS_detected;
            if(D11_Out){
                D13_LED = 1;
            }
            
            FILTERING_FLAG = false;
        }
    }
}


/* Interrupt function. Computes the ADC value of num and outputs the voltage. */
void ISRfxn() {
    float fnum = (float) num/2048.0f;
    input = fnum;
    FILTERING_FLAG = true;
}
