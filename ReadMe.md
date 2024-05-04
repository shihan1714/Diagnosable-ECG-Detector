# Project Title

With the onset of mobile health technology, there arises the opportunity and need for phone based portable ECG systems to detect and discriminate diagnosable and undiagnosable ECG signals. This report details a real time ECG detection system that is able to classify whether an acquired ECG signal is adequate for diagnostic purposes or if another recording is needed. ECG recordings have been collected from MIT_BIH Noise Stress Test Database, in which 12 collections of 6 different signal-to-noise ratios. Recordings begin with five minutes of clean, unaltered ECG data before alternating between noisy and clean ECG data every two minutes. Signal processing is designed to run on the Nucleo F303K8 microcontroller, and output the determined classification to an oscilloscope.   

## Table of Contents

- [Introduction](#introduction)
- [How to Use](#how-to-use)
  - [Installation](#installation)
- [Test Inputs](#test-inputs)
- [Example Test Cases](#example-test-cases)
- [Testing Metrics](#testing-metrics)

## Introduction

This project operates on two STM32 Nucleo-F303K8 with a sim card for reading ECG data for testing.  The two Nucleo-F303K8 boards are based on a sender board and a receiver board, communicating via SPI. The sender board reads a sim card for ECG data and sends the data via SPI to pins D0 and D1 on the receiver board. The receiver board code then converts the serial data into a floating point value that is a single sample of ECG at 360 Hz. This floating point conversion is done in the ISR function "void ISRfxn(){}". At the end of "ISRfxn()", the ISR state flag "FILTERING_STATE" is asserted which starts the main application of thsi project.

After the serial data preprocessing, the floating point data is fed into the Pan-Tompkins QRS detection algorithm broken into two stages:
- float pan_T_Filter(float Ain, float *yOut);
- bool pan_T_Threshold(float *yOut, float *thresholdi1, float *spki, float *npki, float *spki_array, float *npki_array) ;

Details and documentation of these functions can be found in the "BME463_lib.h" file.

## How to Use

### Installation

Include any necessary steps to install and set up your code. This may include dependencies, environment setup, or configuration instructions.

## Test Inputs

List the software or hardware inputs required to test your code. This could include specific input files, signal generator settings, or hardware configurations.

## Example Test Cases

Provide example test cases along with the expected output. This helps reviewers understand how your code behaves under different scenarios.

- **Test Case 1:**
  - Input: [Describe the input data or parameters]
  - Expected Output: [Describe the expected outcome or result]

- **Test Case 2:**
  - Input: [Describe the input data or parameters]
  - Expected Output: [Describe the expected outcome or result]

- [Add more test cases as needed]

## Testing Metrics

Describe the metrics used to evaluate the functionality and accuracy of your algorithm. This could include performance benchmarks, accuracy percentages, or any other relevant measurements.

