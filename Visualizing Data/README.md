# Visualizing Data
This section of the lab took the value produced by the ADC and _did something with it_. There are various different ways of displaying this data, but for this section only three avenues were explored. 

## LCD Display

Using everybody's favorite board, the MSP430FR6989, the on-board LCD screen was used in order to display the temperature data recorded from a LM35 temperature sensor. 

### Dependancies
In order to use the LCD display, a driver was required. This was graciously provided to us by Russel Trafford and implemented into the code. We were provided with both a .h and .c file for the LCD Display. The implementation is found in the very first couple lines of code, which are found below.
```c
#include <msp430.h>
#include "LCDDriver.h"
``` 
### Functions
The only major difference between this code and the previous code found in the Sensors and Signal Conditioning section is the function `formatAndDisplay`. This is responsible for displaying the temperature data on the LCD screen, and replaces the `formatAndSend` function found in the other code. 

The function has to be able to format the temperature data and then separate it into the 1's, 10's and 100's place in order to be sent to the display. In order to do this, the function needs an input value. It is called in the fashion shown below.
```c
formatAndDisplay(ADC_Data);
```

In order to account for a gain of 2.5 produced from the external circuitry, the ADC value was divided by 34, the same value as in the previous section. The lines of code below are responsible for separating out the digits of the display, storing values in their respective variables. It takes in a raw value (sauce) and then refines it to something useful.

```c
Ones     = (rawSauce % 10);        // Determines ones place
Tens     = ((rawSauce/10) % 10);   // Determines tens place
Hundreds = ((rawSauce/100) % 10);  // Determines hundreds place
```

After this is done the LCD Driver functions are used in order to display the number on the screen. This is done in the following lines of code which can be seen below. 
```c
showChar(Ones, 0);                 // Displays ones digit
showChar(Tens, 1);                 // Displats tens digit
showChar(Hundreds, 2);             // Displats hundreds digit
```

This function will take in the varibale to be displayed, and then its respective position on the LCD screen. In the code above, the Ones variable will be placed in the first spot of the screen, tens in the second, and hundreds in the third. Just like real numbers. How exciting.

Besides this function, the rest of the code is pretty much identical to the previous sections.

### PC Visualization 
For this section, the value recorded from the ADC is to be displayed and graphed by a computer. In order to complete this section, Code Composer Studio's Graph function was used. It was set to scan for the `TX_Data` variable. This value is then plotted. 

For this section a distance sensor was used in order to show a relativly fast change in ADC value. The figure below shows the change in ADC value over time as I moved my hand closer and further from the distance sensor. 

[SHOW GRAPH]

The code which was used in this section is identical to the code used for the Sensors and Signal Conditioning section. 

