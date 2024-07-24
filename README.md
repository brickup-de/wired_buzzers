# Arduino Wired Buzzer System

Coding for a wired buzzer system, usable for games or quizzes. Features:

**Demonstration video**
* https://youtu.be/WlllEnodv78

**Simple three button control**
* Red button: Lock all buzzers
* Green button: Unlock active buzzers
* Yellow: Pick single active buzzer

**Advanced context-aware functions**
* "Wrong answer": Red button after buzz keeps current buzzer disabled
* "Right answer": Green button after buzz reactivates all disabled buzzers
* "Noone knows": Green button when buzzers are already unlocked reactivates all buzzers
* "Skip sounds": Yellow button after buzz/pick skips the sound / countdown

**Customizable**
* Pins, tones, countdown length and more are defined in the main *.ino file on the top
