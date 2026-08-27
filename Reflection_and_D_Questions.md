# Reflection and Distinction Follow-Up Questions

## System architecture and interrupt use

The system uses three digital sensor inputs on D8, D9 and D10. These pins belong to the same PORTB pin-change interrupt group on the Arduino Uno. One `PCINT0_vect` ISR manages all three inputs. The ISR compares the current PORTB register value with the previous value to identify which bit changed. When a falling edge is detected, the matching volatile sensor flag is set.

Timer1 runs separately from the sensor interrupt logic. It is configured in CTC mode with a 1024 prescaler and `OCR1A = 15624`, producing one compare-match event each second. The timer ISR sets a volatile flag. The main loop processes the flag, increments the heartbeat count, toggles the onboard LED and prints the timer status.

The ISRs are kept short. They do not use `delay()` and do not print to the Serial Monitor. Longer processing happens in modular functions called from `loop()`.

## If the timer frequency increased from once per second to 100 times per second, what impact would that have on the system?

Increasing the timer frequency from 1 Hz to 100 Hz changes the timer period from one second to 10 milliseconds. The processor would enter the Timer1 ISR 100 times each second instead of once each second.

The current timer ISR is short because it only sets `flag_timer`, so each interrupt would still finish quickly. The main impact would occur in the main loop. The program currently prints a heartbeat message every time the timer flag is processed. At 100 Hz, this would attempt to produce up to 100 heartbeat messages per second. At 9600 baud, the Serial Monitor would become a major bottleneck and sensor-event processing would become slower.

The onboard LED would also toggle too quickly for a normal visible heartbeat.

Another issue is the use of a Boolean timer flag. If several timer interrupts happen before `loop()` processes and clears the flag, they still appear as one pending event. Some timer events would therefore be lost. For a 100 Hz design, I would use a counter instead of a Boolean flag and reduce the rate of Serial output.

## What happens if the ISR has more logic?

If an ISR contains more logic, the processor spends longer inside the interrupt routine. While a normal Arduino AVR ISR is running, other interrupts do not run normally. A long ISR therefore increases response latency for the timer and the pin-change inputs.

If sensor events arrive quickly, a long ISR makes the system less responsive and increases the risk of missed or combined events. Long calculations, loops, `delay()` and Serial printing should not be placed inside an ISR.

Serial output is especially unsuitable inside an ISR because Arduino Serial transmission also depends on interrupt-driven buffering.

The safer design is to keep each ISR short. The ISR should record a flag, counter or changed-bit mask and return. The main loop should then perform Serial output, calculations and other longer processing.

## Demonstration access

Video:

https://drive.google.com/file/d/1QtW7P5DFdDCfbmkPxNT3EewparffDMZI/view?usp=drive_link
