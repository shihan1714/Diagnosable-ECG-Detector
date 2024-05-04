#include "BME463_lib.h"
#include <stdio.h>
#include <string.h> 
#include <math.h>


/* DEFINE THIS IN LAB 4:
 * Moves all elements in an array of floats to the next position by one. Copies
 * the first element to the second position, the second element to the third 
 * position, and so on.
 * 
 * in := the array to be shifted by one.
 * n  := Number of elements in the array.
 */
void shift_right(float *in, const int n){
    if (n > 0) {
        float temp = in[n-1];
        for(int i = n-1; i > 0; i--){
            in[i] = in[i-1];
        }
        in[0] = 0;
    }
}

/* DEFINE THIS IN LAB 4:
 * This function implements an IIR filter. It is assumed that the input arrays
 * and the coefficient arrays are of the same size. Returns the output value.
 *
 * a  := The attenuation factor.
 * inx := The input numerator array. [ARRAY]
 * cx  := The coefficients to dot product with inx. [ARRAY]
 * nx  := The number of coefficients in cx.
 * iny := The input denominator array.
 * cy  := The coefficients to dot product with iny.
 * ny  := The number of coefficients in cy.
 */

float filter_IIR(const float a, const float *inx, const float *cx, const int nx, const float *iny, const float *cy, const int ny){
    float output;
    float sumNum = 0.0; // Numerator array summation, output
    float sumDenom = 0.0; // Denominator array summation, input
    for(int x = 0; x < nx; x++){ 
        sumNum += inx[x]*cx[x]; // Numerator array summation, output
    }
    for(int x = 0; x < ny; x++){
        sumDenom -= iny[x]*cy[x]; // Denominator array summation, input
    }
    // Attenuation factor on numerator array.
    output = sumDenom + a*sumNum;
    return output;
}

/*
 * a  := The attenuation factor.
 * in := The input x array.
 * c  := The coefficients to dot product with inx.
 * n  := The number of coefficients in c.
 */
float filter_FIR(const float a, const float *in, const float *c, const int n){
    float sumNum = 0.0;
    for(int x = 0; x < n; x++){
        sumNum += in[x]*c[x];
    } 
    return a*sumNum;
}

/**
 * @brief Implements the filtering stage of the Pan-Tompkins QRS Detection algorithm.
 * 
 * This function takes an analog input signal Ain and modifies the output yOut according to the signal processing algorithm.
 * 
 * @param Ain ADC input signal.
 * @param yOut Output QRS levels.
 * @return float The filtered value of the input signal.
 */
float pan_T_Filter(float Ain, float *yOut){
    float value = 0.0;
    // Initializing LP Filter
    static float a1[13] = {1, 0, 0, 0, 0, 0, -2, 0, 0, 0, 0, 0, 1};
    static float x1[13] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    static float b1[3] = {1, -2, 1};
    static float y1[3] = {0, 0, 0};
    static float g1 = 1;
    static int nx1 = sizeof(a1) / sizeof(a1[0]);
    static int ny1 = sizeof(b1) / sizeof(b1[0]);

    // Initializing HP Filter
    static float a2[34] = {-1, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 32.0, -32.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1};
    static float x2[34] = {0, 0, 0, 0, 0, 0, 0, 0 ,0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    static float b2[2] = {1, -1};
    static float y2[2] = {0, 0,};
    static float g2 = 1.0/32.0;
    static int nx2 = sizeof(a2) / sizeof(a2[0]);
    static int ny2 = sizeof(b2) / sizeof(b2[0]);

    // Initializing deriv 2 Numerators and configs
    static float a3[5] = {2, 1, 0, -1, -2}; // deriv2 coefficients
    static float x3[5] = {0, 0, 0, 0, 0}; // x[0] = x[nT], x[1] = x(nT - T), x[3] = x(nT - 3T), x[4] = x(nT - 4T)
    static float y3[1] = {0}; // y[0] = y(nT) is the output produced for each input, x[0] = x(nT)
    static float g3 = 1.0/8.0;
    static int const nx3 = sizeof(a3) / sizeof(a3[0]);

    // Initializing MWI Numerators and configs
    static float a4[32] = {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1};
    static float x4[32] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    static float z4[1] = {0};
    static float g4 = 0.03125;
    static int const nx4 = sizeof(a4) / sizeof(a4[0]);
    static int const ny4 = 3;

    shift_right(x1, nx1); // Shifting lp filter
    shift_right(y1, ny1);
    shift_right(y2, ny2); // Shifting hp filter
    shift_right(x2, nx2);
    shift_right(x3, nx3); // Shifting Derivative filter
    shift_right(x4, nx4); // Shifting MWI Filter
    shift_right(yOut, ny4); // Shift MWI output for edge detection

    /* LP filter */
    x1[0] = Ain;
    y1[0] = filter_IIR(g1, x1, a1, nx1, y1, b1, ny1);

    /* HP Filter */
    x2[0] = y1[0];
    y2[0] = filter_IIR(g2, x2, a2, nx2, y2, b2, ny2);
    value = y2[0];

    /* Deriv 2 Filter */
    x3[0] = y2[0];
    y3[0] = filter_FIR(g3, x3, a3, nx3);

    /* Squaring Filter */
    y3[0] = y3[0]*y3[0];

    /* Moving Integral Filter */
    x4[0] = y3[0];
    yOut[0] = filter_FIR(g4, x4, a4, nx4);
    return value;
}

/**
 * @brief Implements the moving threshold stage of the Pan-Tompkins QRS detection algorithm.
 * 
 * This function updates the threshold detection values based on the output QRS levels from the last three values.
 * 
 * @param yOut Pointer to the array containing the output of the QRS algorithm for the last three values.
 * @param thresholdi1 Pointer to the threshold detection value.
 * @param spki Pointer to the peak value of the signal (QRS complex).
 * @param npki Pointer to the peak value of the noise.
 * @param spki_array Array to store recent peak values of the signal.
 * @param npki_array Array to store recent peak values of the noise.
 * @return bool True if a QRS complex is detected, false otherwise.
 */
bool pan_T_Threshold(float *yOut, float *thresholdi1, float *spki, float *npki, float *spki_array, float *npki_array) {
    static float peakt = 0.0;
    static float peaki = 0.0;
    static bool QRS_detected = false;
    
    if (yOut[0] > yOut[2] && yOut[0] > peakt) {
        peakt = yOut[0];
    }
    if (peakt > *thresholdi1) { // Compare with the value pointed to by thresholdi1
        QRS_detected = true;
    }
    if (yOut[0] <= yOut[2] && yOut[0] < 0.5f * peakt) {
        peaki = peakt; // Peakt is a local max
        QRS_detected = false;
        if (peaki > *thresholdi1) { // Compare with the value pointed to by thresholdi1
            //*spki = 0.125f * peaki + 0.875f * (*spki); // Update the value pointed to by spki
            *spki = array_running_avg(spki_array, 8, peaki);
        } else {    // Local max is a noise peak.
            //*npki = 0.125f * peaki + 0.875f * (*npki); // Update the value pointed to by npki
            *npki = array_running_avg(npki_array, 8, peaki);
        }
        *thresholdi1 = *npki + 0.25f * (*spki - *npki); // Update the value pointed to by thresholdi1
        peakt = 0; // Reset peakt for polling local max
    }    
    return QRS_detected;
}

/**
 * @brief Queues the most recent value into an array and calculates the average of the input array.
 * 
 * This function inserts the most recent value (input) into the input_array, shifts the existing values to the right,
 * and then calculates the average of the input_array.
 * 
 * @param input_array Pointer to the input array.
 * @param length Length of the input array.
 * @param input The most recent value to be inserted into the input_array.
 * @return float The average of the input_array.
 */
float array_running_avg(float *input_array, int length, float input){
    shift_right(input_array, length);
    input_array[0] = input;
    float average = 0.0;
    for(int i = 0; i < length; i++){
        average += input_array[i]*0.125;
    }
    return average;
}

/**
 * Calculates the standard deviation of an array of floats.
 * 
 * This function takes an array of floats and its length as input,
 * and returns the standard deviation of the values in the array.
 * 
 * @param input_array Pointer to the array of floats.
 * @param length      The number of elements in the array.
 * @return            The standard deviation of the values in the array.
 */
float std_dev(float *input_array, int length) {
    float mean = 0.0, sum_deviation = 0.0;
    
    // Calculate mean 
    for(int i = 0; i < length; i++) {
        mean += input_array[i];
    }
    mean /= length;
    
    // Calculate sum of squared differences
    for(int i = 0; i < length; i++) {
        sum_deviation += (input_array[i] - mean) * (input_array[i] - mean);
    }
    
    // Calculate standard deviation
    float standard_deviation = 0.0;
    if (length > 1) {
        standard_deviation = sqrt(sum_deviation / (length - 1));
    }
    
    return standard_deviation;
}

/**
 * @brief Calculates the average of floats in an input array.
 * 
 * This function takes an array of floats and its size as input
 * and returns the average of the elements in the array.
 * 
 * @param arr Pointer to the array of floats.
 * @param size Size of the array.
 * @return float The average of floats in the array.
 */
float array_average(float arr[], int size){
    float sum = 0.0;
    
    // Calculate the sum of all elements in the array
    for (int i = 0; i < size; i++) {
        sum += arr[i];
    }
    
    // Calculate the average
    float average = sum/size;
    
    return average;
}

/*
 * Copies the elements of one array to another.
 * 
 * Parameters:
 * arr_orig : Pointer to the original array
 * arr_copy : Pointer to the destination array where elements will be copied
 * size     : Size of the arrays
 * 
 * Returns:
 * Void
 */
void save_array(float *arr_orig, float *arr_copy, int size){
    for(int i = 0; i < size; i++){
        arr_copy[i] = arr_orig[i];
    }
}