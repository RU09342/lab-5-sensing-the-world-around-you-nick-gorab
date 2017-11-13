# Sensors and Signal Conditioning
## System Overview
In order for our MSP boards to record data from external phenomenon, sensor communication must be implemented into its code. There are three transducer sensors which are used in this section; voltage, current, and resistance. In order for this signal to be recognized by the microcontroller, there are several stages of signal conditioning it needs to pass through beforehand. The stages are as follows:
1. Turns sensor output into voltage
2. Filter noise from signal
3. Amplify signal to ADC range

Once the signal passes through these three stages, it still needs to be turned from an analog signal into a digital signal. This is done through the use of the MSP430's on-board ADC. Once the signal goes through the ADC, it can then be used by the microcontroller as an input. 

## Hardware
There are three types of circuits which needed to be created in order to use the sensors with the ADC. The main purpose of the hardware is to extract a voltage from the sensor, filter the singal and then amplify it within the ADC's range. For each of the different sensors, a different circuit needed to be constructed.

### Voltage

For the voltage based sensor, an LM35 temperature sensor was used. Because it outputs 10mV/°C, a non-inverting Op-Amp was used to amplify the sensor's output by 2.5, extending the sensor to the full range of the ADC.

![TempSensor](https://github.com/RU09342/lab-5-sensing-the-world-around-you-nick-gorab/Sensors%20and%20Signal%20Conditioning/Schematics/Temperature%20Sensor%20Schematic.png)

### Current
For the current based sensor, a phototransistor was used and acts as a normally closed switch. When it is exposed to light, the transistor is open and all of the signal goes through the 100k Resistor. But when the transistor detects no light, it closes and only has a resistance of about 15k so the majority of the signal runs to ground. The code is edited so that if the ADC is not reading a certain value, then the desired function will not initiate.

![PhotoTransistor](https://github.com/RU09342/lab-5-sensing-the-world-around-you-nick-gorab/Sensors%20and%20Signal%20Conditioning/Schematics/PhotoTransistor%20Schematic.png)

### Resistance
For the resistance based sensor, a thermistor was used in a voltage divider with a 10k resistor. This value is significant because at room temperature (25°C) the thermistor reads about 10k ohms. This gives the thermistor circuit max sensitvity and outputs the maximum range, ideal for ADC applications.

![Thermistor](https://github.com/RU09342/lab-5-sensing-the-world-around-you-nick-gorab/Sensors%20and%20Signal%20Conditioning/Schematics/Thermistor%20Schematic.png)

## Software
### ADC
In order for this signal to be utilized by the MSP430, the ADC needs to be initialized into the code. There are 2 main types of ADCs in this lab, 10-bit and 12-bit resolution. The process of setting up each of these ADC types are mostly the same, with some minor differences which will be explained.

The first step of initializing the ADC is to determine which pin the reading will be taken on. This is done using the following line of code, however the Pin assignment will change with each board. The following code is for the 5529.

```c
P6SEL |= BIT0;
```
 For reference, the ADC pins used in this lab are as follows:
 
|     Board    |   Pin   |
|--------------|---------|
| MSP430FR5994 | PIN 1.2 |
| MSP430FR2311 | PIN 1.1 |
| MSP430FR6989 | PIN 8.7 |
| MSP430F5529  | PIN 6.0 |
| MSP430G2553  | PIN 1.0 |

There are 3 main registers which need to be initialize for ADC useage. The first register is the `ADC1xCTL0` register. The first use of this register is responsible for turning on the ADC, setting the sampling time, and then enabling multiple sampling coversion. This is done in the following lines of code, which belongs to the 5592 code initialization. 

```c
ADC12CTL0  = ADC12ON        // Turns on ADC
           + ADC12SHT0_8    // Sampling time of 256 clk cycles
           + ADC12MSC;      // Multiple conversions
```

The next register initialized is `ADC1xCTL1`, which is responsible for the sampling timer and reading type. This lab uses SMCLK as the sample timer and "repeat single channel" reading mode. This is carried out in the following lines of code.

```c
ADC12CTL1  = ADC12SHP       // Signal comes from sampling timer instead of input
           + ADC12CONSEQ_2; // Repeats reading on single channel
```

Some of the boards use the second, or third ADC pin. Because of this, the ADC needs to be told which pin to take the reading on. This is done through the `ADC1xMCTL0` register. The code for pin selection can be found below.

```c
ADC12MCTL0 = ADC12INCH_2;   // Selects input channel 2 (A2)
```

The final step for the ADC initialization is getting the ADC started. For this, we return to the `ADC1xCTL0` register with the following lines of code, which are responsible for enabling and starting conversions.

```c
ADC12CTL0 |= ADC12ENC       // Enables conversions
          |  ADC12SC;       // Starts conversions
```

Once the ADC is setup, data can be read from the `ADC1xMEM0` register.

## UART
The data which is recorded by the board is sent through UART to a computer. This was done at a specified BAUD rate of 9600, with one reading sent per second. The code for initializing UART on the boards is mostly the same. For an example, the UART initialization code the MSP403F5529 is used. 

The first section of the code designates which pins to use for UART. For the 5529, these are P3.3 and P3.4. The pin select code is found below.
```c
P3SEL    |=  BIT3       // UART TX
         |   BIT4;      // UART RX
```

The UART state machine is then put to rest so the BAUD and clock source can be selected. The reset and clock source is set in the following lines of code:
```c
UCA0CTL1 |=  UCSWRST    // Resets state machine
         |   UCSSEL_2;  // SMCLK
```

After this, a BAUD rate of 9600 was done using the following lines. The values for these registers came from the datasheet provided by TI. 

```c
UCA0BR0   =  6;         // 9600 Baud Rate
UCA0BR1   =  0;         // 9600 Baud Rate
UCA0MCTL |=  UCBRS_0    // Modulation
         |   UCBRF_13   // Modulation
         |   UCOS16;    // Modulation
```
For UART to run the state machine needs to be taken out of rest mode, which is done in the following line of code:
```c
UCA0CTL1 &= ~UCSWRST;   // Initializes the state machine
```
## Timer
In order to communicate 1 reading per second, the timer module was used with interrupts. The clock was set to run in up-mode, with a corresponding CCR value which will interrupt at rougly 1 time per second. The code for clock initialization can be found below.

```c
TA0CCTL0 = CCIE;        // Emables Timer_A interrupts
TA0CTL   = TASSEL_1     // Uses SMCLK
         + MC_1;        // Counts in Up-Mode
TA0CCR0  = 32700;       // Samples ~ every second
```

# Data Conditioning
The ADC transmits a 10/12 bit data signal which needs to be sent over UART. This proposes an issue, as UART communication only sends 1 byte at a time, and if you were to send all of the ADC data at once, it would lead to a truncation error and the singal will not send correctly. This issue is avoided by splitting up the ADC data into 2 separate bytes, as seen in the function `formatAndSend();`. 

The function is simple, and the code is pretty straightforward. It separates the MSB and LSB and then sends them over UART one after the other. The code inside of the function is found below. 
```c
MSB       = value >> 8;      // Bit Shifts 12 bits to the right by 8
LSB       = value & 0xFF;    // ANDs the 12 bit value with 11111111, returning the LSB
UCA0TXBUF = MSB;             // Transmits the MSB first
while(!(UCA0IFG & UCTXIFG)); // Waits for the TX buffer to be cleared
UCA0TXBUF = LSB;             // Transmits the LSB second
```

















