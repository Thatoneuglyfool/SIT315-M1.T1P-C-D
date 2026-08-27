# SIT315 M1QP - Interrupt-Based Sense-Think-Act System

## Project overview

This project implements an Arduino Uno interrupt-driven system using three digital inputs and a Timer1 periodic task.

The demonstrated system uses:

- Sensor 1 on digital pin D8, PB0 / PCINT0
- Sensor 2 on digital pin D9, PB1 / PCINT1
- Sensor 3 on digital pin D10, PB2 / PCINT2
- Arduino onboard LED on D13 for the periodic heartbeat
- Pin Change Interrupts for the three sensor inputs
- Timer1 in CTC mode for the periodic task
- Serial Monitor output for traceable sensor and timer events

## Files

- `TaskM1.cpp` - final C++ source requested for submission
- `uninterrupted_cir1.ino` - Arduino/Tinkercad source used for the demonstrated circuit
- `Circuit_Diagram.png` - Tinkercad schematic
- `Tinkercad_Setup.png` - circuit setup screenshot
- `Serial_Monitor.png` - output showing Timer1 and PCINT activity
- `Reflection_and_D_Questions.md` - reflection notes and Distinction follow-up answers
- `Demo_Link.txt` - demonstration video link

## Pin-change interrupt configuration

Pins D8, D9 and D10 are part of PORTB on the Arduino Uno.

The program enables the PORTB pin-change interrupt group with `PCIE0` and unmasks PCINT0, PCINT1 and PCINT2 through `PCMSK0`.

All three pins share the `PCINT0_vect` ISR.

Inside the ISR, the program reads `PINB` and compares the current value with the previous PORTB value using XOR. The changed-bit result identifies which input changed.

The ISR only sets volatile flags. Serial output and the remaining processing occur in the main loop.

## Timer1 configuration

Timer1 uses CTC mode.

The Arduino Uno clock is 16 MHz. A 1024 prescaler is used and `OCR1A` is set to 15624.

This creates one Timer1 compare-match interrupt each second.

The Timer1 ISR only sets `flag_timer`.

The main loop then:

1. Increments the heartbeat counter.
2. Toggles the onboard D13 LED.
3. Prints the heartbeat message to the Serial Monitor.

This keeps the time-based interrupt work separate from the sensor event processing.

## Running the system

1. Open the circuit in Tinkercad or on an Arduino Uno.
2. Load `uninterrupted_cir1.ino`, or use the equivalent `TaskM1.cpp` source.
3. Start the simulation.
4. Open Serial Monitor at 9600 baud.
5. Press the D8, D9 and D10 sensor buttons.
6. Confirm the correct PCINT sensor event is printed.
7. Confirm the Timer1 heartbeat continues once per second while sensor events occur.

## Example output

```text
TIMER [Timer1]: Periodic Heartbeat #18 | System Active
TIMER [Timer1]: Periodic Heartbeat #19 | System Active
EVENT [PCINT0]: Sensor 1 (Pin 8) triggered!
TIMER [Timer1]: Periodic Heartbeat #20 | System Active
```

## Demonstration video

Google Drive:

https://drive.google.com/file/d/1QtW7P5DFdDCfbmkPxNT3EewparffDMZI/view?usp=drive_link

Make sure the marking tutor has viewing access to the video.

## Distinction follow-up questions

The answers requested by the marker are included in `Reflection_and_D_Questions.md`.

## Repository access

The assessment sheet requires the repository to be private. Add the marking tutor as a collaborator before submitting the GitHub repository link through OnTrack.
