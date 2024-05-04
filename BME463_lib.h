/* This is the header file for the BME463 library. It contains three functions
 * that are useful for implementing filters to be used in multiple lab 
 * activities! These functions need to be defined in a .cpp file with the same
 * name as this header file. You may implement additional functions if you 
 * wish.
 *
 * To use this library in your main.cpp file you must add the line
 * #include "BME463_lib.h" after #include "mbed.h". 
 */ 


#ifndef _BME463_lib
#define _BME463_lib

/* DEFINE THIS IN LAB 4:
 * Moves all elements in an array of floats to the next position by one. Copies
 * the first element to the second position, the second element to the third 
 * position, and so on.
 * 
 * in := the array to be shifted by one.
 * n  := Number of elements in the array.
 */
void shift_right(float *in, int const n);

/* DEFINE THIS IN LAB 4:
 * This function implements an IIR filter. It is assumed that the input arrays
 * and the coefficient arrays are of the same size. Returns the output value.
 *
 * a  := The attenuation factor.
 * inx := The input numerator array.
 * cx  := The coefficients to dot product with inx.
 * nx  := The number of coefficients in cx.
 * iny := The input denominator array.
 * cy  := The coefficients to dot product with iny.
 * ny  := The number of coefficients in cy.
 */
float filter_IIR(float const a, float const* inx, float const* cx, int const nx, float const* iny, float const* cy, int const ny);

/* DEFINE THIS IN LAB 5:
 * This function implements an FIR filter. It is assumed that the input arrays
 * and the coefficient arrays are of the same size. Returns the output value.
 *
 * a  := The attenuation factor.
 * in := The input x array.
 * c  := The coefficients to dot product with inx.
 * n  := The number of coefficients in cx.
 */
float filter_FIR(float const a, float const* in, float const* c, int const n);

/**
 * @brief Implements the filtering stage of the Pan-Tompkins QRS Detection algorithm.
 * 
 * This function takes an analog input signal Ain and modifies the output yOut according to the signal processing algorithm.
 * 
 * @param Ain ADC input signal.
 * @param yOut Output QRS levels.
 * @return float The filtered value of the input signal.
 */
float pan_T_Filter(float Ain, float *yOut);

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
bool pan_T_Threshold(float *yOut, float *thresholdi1, float *spki, float *npki, float *spki_array, float *npki_array) ;

/**
 * Calculates the running average of an array with a new input value.
 * 
 * This function updates an array of floats by shifting its elements to the right,
 * adds a new input value at the beginning, and then calculates the running average 
 * of the updated array.
 * 
 * @param input_array Pointer to the array of floats.
 * @param length      The number of elements in the array.
 * @param input       The new input value to add to the array.
 * @return            The running average of the updated array.
 */
float array_running_avg(float *input_array, int length, float input);

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
float std_dev(float *input_array, int length);

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
float array_average(float arr[], int size);

/**
 * @brief Copies the elements of one array to another.
 * 
 * This function copies the elements of the original array to a destination array.
 * 
 * @param arr_orig Pointer to the original array.
 * @param arr_copy Pointer to the destination array where elements will be copied.
 * @param size Size of the arrays.
 * @return void
 */
void save_array(float *arr_orig, float *arr_copy, int size);
#endif