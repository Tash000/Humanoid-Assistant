#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

// Define the analog pins for the joysticks and potentiometers
#define JOYSTICK1_X_PIN  34
#define JOYSTICK1_Y_PIN  35
#define JOYSTICK2_X_PIN  32
#define JOYSTICK2_Y_PIN  33
#define JOYSTICK3_X_PIN  25
#define JOYSTICK3_Y_PIN  26
#define JOYSTICK4_X_PIN  27
#define JOYSTICK4_Y_PIN  14

#define POT1_PIN  12
#define POT2_PIN  13

#define DEBUG 1

// Initialize the PCA9685 driver
Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();

#include <map>
String srin, key;
int n, i;
std::map<String, int> pos;

struct CHANNEL {
  struct FOREHEAD {
    static const int LEFT = 0;
    static const int RIGHT = 1;
  } FOREHEAD;
  struct EYEBROW {
    static const int LEFT = 2;
    static const int RIGHT = 3;
  } EYEBROW;
  struct EYELID {
    struct LEFT {
      static const int TOP = 4;
      static const int BOTTOM = 5;
    } LEFT;
    struct RIGHT {
      static const int TOP = 6;
      static const int BOTTOM = 7;
    } RIGHT;
  } EYELID;
  struct EYE {
    struct LEFT {
      static const int HORIZONTAL = 8;
      static const int VERTICAL = 9;
    } LEFT;
    struct RIGHT {
      static const int HORIZONTAL = 10;
      static const int VERTICAL = 11;
    } RIGHT;
  } EYE;
  struct LIP {
    static const int UPPER = 12;
  } LIP;
  struct MOUTH {
    static const int LEFT = 13;
    static const int RIGHT = 14;
  } MOUTH;
  static const int JAW = 15;
};

namespace RANGE {
  struct GENERAL {
    static const int MIN = 0;
    static const int MAX = 100;
  } GENERAL;

  struct SERVO {
    static const int MIN = 150;
    static const int MAX = 600;
  } SERVO;

  struct FOREHEAD {
    struct LEFT {
      static const int SQUINCH = 15;
      static const int NORMAL = 0;
      static const int WIDEN = 0;
    } LEFT;
    struct RIGHT {
      static const int SQUINCH = 0;
      static const int NORMAL = 20;
      static const int WIDEN = 30;
    } RIGHT;
  } FOREHEAD;

  struct EYEBROW {
    struct LEFT {
      static const int RAISE = 40;
      static const int NORMAL = 20;
      static const int LOWER = 0;
    } LEFT;
    struct RIGHT {
      static const int RAISE = 20;
      static const int NORMAL = 40;
      static const int LOWER = 60;
    } RIGHT;
  } EYEBROW;

  struct EYELID {
    struct LEFT {
      struct TOP {
        static const int RAISE = 50;
        static const int NORMAL = 30;
        static const int LOWER = 0;
      } TOP;
      struct BOTTOM {
        static const int RAISE = 10;
        static const int NORMAL = 0;
      } BOTTOM;
    } LEFT;
    struct RIGHT {
      struct TOP {
        static const int RAISE = 20;
        static const int NORMAL = 50;
        static const int LOWER = 90;
      } TOP;
      struct BOTTOM {
        static const int RAISE = 5;
        static const int NORMAL = 0;
      } BOTTOM;
    } RIGHT;
  } EYELID;

  struct EYE {
    struct LEFT {
      struct HORIZONTAL {
        static const int LEFT = 0;
        static const int NORMAL = 3;
        static const int RIGHT = 9;
      } HORIZONTAL;
      struct VERTICAL {
        static const int UP = 20;
        static const int NORMAL = 10;
        static const int DOWN = 0;
      } VERTICAL;
    } LEFT;
    struct RIGHT {
      struct HORIZONTAL {
        static const int LEFT = 0;
        static const int NORMAL = 7;
        static const int RIGHT = 20;
      } HORIZONTAL;
      struct VERTICAL {
        static const int UP = 0;
        static const int NORMAL = 10;
        static const int DOWN = 20;
      } VERTICAL;
    } RIGHT;
  } EYE;

  struct LIP {
    struct UPPER {
      static const int UP = 15;
      static const int NORMAL = 0;
    } UPPER;
  } LIP;

  struct MOUTH {
    struct LEFT {
      static const int UP = 47;
      static const int NORMAL = 30;
      static const int DOWN = 10;
    } LEFT;
    struct RIGHT {
      static const int UP = 0;
      static const int NORMAL = 17;
      static const int DOWN = 35;
    } RIGHT;
  } MOUTH;

  struct JAW {
    static const int NORMAL = 25;
    static const int DOWN = 140;
  } JAW;
}

// Function to convert pulse length to angle
int pulseLengthToAngle(int pulseLength) {
  return map(pulseLength, RANGE::SERVO::MIN, RANGE::SERVO::MAX, 0, 180);
}

// Function to gradually move the servo
void smoothServoMove(uint8_t servoNum, int currentPos, int targetPos, int stepDelay = 5) {
  if (currentPos < targetPos) {
    for (int pos = currentPos; pos <= targetPos; pos++) {
      pwm.setPWM(servoNum, 0, pos);
      delay(stepDelay);
    }
  } else {
    for (int pos = currentPos; pos >= targetPos; pos--) {
      pwm.setPWM(servoNum, 0, pos);
      delay(stepDelay);
    }
  }
}

void servoWrite(int channel, int angle, int range) {
  int pulseLength = map(angle, 0, 180, RANGE::SERVO::MIN, RANGE::SERVO::MAX);
  pwm.setPWM(channel, 0, pulseLength);
  if (channel == CHANNEL::FOREHEAD::LEFT || channel == CHANNEL::FOREHEAD::RIGHT)
    pos["forehead"] = range;
  else if (channel == CHANNEL::EYEBROW::LEFT || channel == CHANNEL::EYEBROW::RIGHT)
    pos["eyebrow"] = range;
  else if (channel == CHANNEL::EYELID::LEFT::TOP || channel == CHANNEL::EYELID::LEFT::BOTTOM || channel == CHANNEL::EYELID::RIGHT::TOP || channel == CHANNEL::EYELID::RIGHT::BOTTOM)
    pos["eyelid"] = range;
  else if (channel == CHANNEL::EYE::LEFT::HORIZONTAL || channel == CHANNEL::EYE::RIGHT::HORIZONTAL)
    pos["eyeHorizontal"] = range;
  else if (channel == CHANNEL::EYE::LEFT::VERTICAL || channel == CHANNEL::EYE::RIGHT::VERTICAL)
    pos["eyeVertical"] = range;
  else if (channel == CHANNEL::LIP::UPPER)
    pos["lip"] = range;
  else if (channel == CHANNEL::MOUTH::LEFT || channel == CHANNEL::MOUTH::RIGHT)
    pos["mouth"] = range;
  else if (channel == CHANNEL::JAW)
    pos["jaw"] = range;
}

void setup() {
  // Start the serial communication
  Serial.begin(115200);

  // Initialize the PCA9685 driver
  pwm.begin();
  pwm.setPWMFreq(60);  // Analog servos run at ~60 Hz
}

void loop() {
  // Read the values from the joysticks and potentiometers
  int joystick1X = analogRead(JOYSTICK1_X_PIN);
  int joystick1Y = analogRead(JOYSTICK1_Y_PIN);
  int joystick2X = analogRead(JOYSTICK2_X_PIN);
  int joystick2Y = analogRead(JOYSTICK2_Y_PIN);
  int joystick3X = analogRead(JOYSTICK3_X_PIN);
  int joystick3Y = analogRead(JOYSTICK3_Y_PIN);
  int joystick4X = analogRead(JOYSTICK4_X_PIN);
  int joystick4Y = analogRead(JOYSTICK4_Y_PIN);
  int pot1 = analogRead(POT1_PIN);
  int pot2 = analogRead(POT2_PIN);

  // Map the joystick and potentiometer values to servo positions
  int jawPos = map(joystick4Y, 0, 4095, RANGE::SERVO::MIN, RANGE::SERVO::MAX);
  int eyebrowPosLeft = map(joystick1X 0, 4095, RANGE::SERVO::MIN, RANGE::SERVO::MAX);
  int eyebrowPosRight = map(joystick1Y 0, 4095, RANGE::SERVO::MIN, RANGE::SERVO::MAX);
  int eyeHPos = map(joystick3X, 0, 4095, RANGE::SERVO::MIN, RANGE::SERVO::MAX);
  int eyeVPos = map(joystick3Y, 0, 4095, RANGE::SERVO::MIN, RANGE::SERVO::MAX);
  int eyelidPosLeft = map(joystick2X, 0, 4095, RANGE::SERVO::MIN, RANGE::SERVO::MAX);
  int eyelidPosRight = map(joystick2Y, 0, 4095, RANGE::SERVO::MIN, RANGE::SERVO::MAX);
  int foreheadPos = map(pot1, 0, 4095, RANGE::SERVO::MIN, RANGE::SERVO::MAX);
  int mouthPos = map(pot2, 0, 4095, RANGE::SERVO::MIN, RANGE::SERVO::MAX);
  int lipPos = map(joystick4X, 0, 4095, RANGE::SERVO::MIN, RANGE::SERVO::MAX);

void normalizeServos() {
  servoWrite(CHANNEL::FOREHEAD::LEFT, RANGE::FOREHEAD::LEFT::NORMAL, map(RANGE::FOREHEAD::LEFT::NORMAL, RANGE::GENERAL::MIN, RANGE::GENERAL::MAX, RANGE::FOREHEAD::LEFT::SQUINCH, RANGE::FOREHEAD::LEFT::WIDEN));
  servoWrite(CHANNEL::FOREHEAD::RIGHT, RANGE::FOREHEAD::RIGHT::NORMAL, map(RANGE::FOREHEAD::RIGHT::NORMAL, RANGE::GENERAL::MIN, RANGE::GENERAL::MAX, RANGE::FOREHEAD::RIGHT::SQUINCH, RANGE::FOREHEAD::RIGHT::WIDEN));
  servoWrite(CHANNEL::EYEBROW::LEFT, RANGE::EYEBROW::LEFT::NORMAL, map(RANGE::EYEBROW::LEFT::NORMAL, RANGE::GENERAL::MIN, RANGE::GENERAL::MAX, RANGE::EYEBROW::LEFT::LOWER, RANGE::EYEBROW::LEFT::RAISE));
  servoWrite(CHANNEL::EYEBROW::RIGHT, RANGE::EYEBROW::RIGHT::NORMAL, map(RANGE::EYEBROW::RIGHT::NORMAL, RANGE::GENERAL::MIN, RANGE::GENERAL::MAX, RANGE::EYEBROW::RIGHT::LOWER, RANGE::EYEBROW::RIGHT::RAISE));
  servoWrite(CHANNEL::EYELID::LEFT::TOP, RANGE::EYELID::LEFT::TOP::NORMAL, map(RANGE::EYELID::LEFT::TOP::NORMAL, RANGE::GENERAL::MIN, RANGE::GENERAL::MAX, RANGE::EYELID::LEFT::TOP::LOWER, RANGE::EYELID::LEFT::TOP::RAISE));
  servoWrite(CHANNEL::EYELID::RIGHT::TOP, RANGE::EYELID::RIGHT::TOP::NORMAL, map(RANGE::EYELID::RIGHT::TOP::NORMAL, RANGE::GENERAL::MIN, RANGE::GENERAL::MAX, RANGE::EYELID::RIGHT::TOP::LOWER, RANGE::EYELID::RIGHT::TOP::RAISE));
  servoWrite(CHANNEL::EYELID::LEFT::BOTTOM, RANGE::EYELID::LEFT::BOTTOM::NORMAL, map(RANGE::EYELID::LEFT::BOTTOM::NORMAL, RANGE::GENERAL::MIN, RANGE::GENERAL::MAX, RANGE::EYELID::RIGHT::TOP::LOWER, RANGE::EYELID::RIGHT::TOP::RAISE));
  servoWrite(CHANNEL::EYELID::RIGHT::BOTTOM, RANGE::EYELID::RIGHT::BOTTOM::NORMAL, map(RANGE::EYELID::RIGHT::BOTTOM::NORMAL, RANGE::GENERAL::MIN, RANGE::GENERAL::MAX, RANGE::EYELID::RIGHT::BOTTOM::RAISE, RANGE::EYELID::RIGHT::BOTTOM::NORMAL));
  servoWrite(CHANNEL::EYE::LEFT::HORIZONTAL, RANGE::EYE::LEFT::HORIZONTAL::NORMAL, map(RANGE::EYE::LEFT::HORIZONTAL::NORMAL, RANGE::GENERAL::MIN, RANGE::GENERAL::MAX, RANGE::EYE::LEFT::HORIZONTAL::LEFT, RANGE::EYE::LEFT::HORIZONTAL::RIGHT));
  servoWrite(CHANNEL::EYE::RIGHT::HORIZONTAL, RANGE::EYE::RIGHT::HORIZONTAL::NORMAL, map(RANGE::EYE::RIGHT::HORIZONTAL::NORMAL, RANGE::GENERAL::MIN, RANGE::GENERAL::MAX, RANGE::EYE::RIGHT::HORIZONTAL::LEFT, RANGE::EYE::RIGHT::HORIZONTAL::RIGHT));
  servoWrite(CHANNEL::EYE::LEFT::VERTICAL, RANGE::EYE::LEFT::VERTICAL::NORMAL, map(RANGE::EYE::LEFT::VERTICAL::NORMAL, RANGE::GENERAL::MIN, RANGE::GENERAL::MAX, RANGE::EYE::LEFT::VERTICAL::UP, RANGE::EYE::LEFT::VERTICAL::DOWN));
  servoWrite(CHANNEL::EYE::RIGHT::VERTICAL, RANGE::EYE::RIGHT::VERTICAL::NORMAL, map(RANGE::EYE::RIGHT::VERTICAL::NORMAL, RANGE::GENERAL::MIN, RANGE::GENERAL::MAX, RANGE::EYE::RIGHT::VERTICAL::UP, RANGE::EYE::RIGHT::VERTICAL::DOWN));
  servoWrite(CHANNEL::LIP::UPPER, RANGE::LIP::UPPER::NORMAL, map(RANGE::LIP::UPPER::NORMAL, RANGE::GENERAL::MIN, RANGE::GENERAL::MAX, RANGE::LIP::UPPER::NORMAL, RANGE::LIP::UPPER::UP));
  servoWrite(CHANNEL::MOUTH::LEFT, RANGE::MOUTH::LEFT::NORMAL, map(RANGE::MOUTH::LEFT::NORMAL, RANGE::GENERAL::MIN, RANGE::GENERAL::MAX, RANGE::MOUTH::LEFT::DOWN, RANGE::MOUTH::LEFT::UP));
  servoWrite(CHANNEL::MOUTH::RIGHT, RANGE::MOUTH::RIGHT::NORMAL, map(RANGE::MOUTH::RIGHT::NORMAL, RANGE::GENERAL::MIN, RANGE::GENERAL::MAX, RANGE::MOUTH::RIGHT::DOWN, RANGE::MOUTH::RIGHT::UP));
  servoWrite(CHANNEL::JAW, RANGE::JAW::NORMAL, map(RANGE::JAW::NORMAL, RANGE::GENERAL::MIN, RANGE::GENERAL::MAX, RANGE::JAW::NORMAL, RANGE::JAW::DOWN));
  if (DEBUG)
    Serial.println("\nNORMALIZED SERVOS");
}

  // Write the positions to the servos
  servoWrite(CHANNEL::JAW, pulseLengthToAngle(jawPos), jawPos);
  servoWrite(CHANNEL::EYEBROW::LEFT, pulseLengthToAngle(eyebrowPos), eyebrowPos);
  servoWrite(CHANNEL::EYEBROW::RIGHT, pulseLengthToAngle(eyebrowPos), eyebrowPos);
  servoWrite(CHANNEL::EYE::LEFT::HORIZONTAL, pulseLengthToAngle(eyeHPos), eyeHPos);
  servoWrite(CHANNEL::EYE::LEFT::VERTICAL, pulseLengthToAngle(eyeVPos), eyeVPos);
  servoWrite(CHANNEL::EYE::RIGHT::HORIZONTAL, pulseLengthToAngle(eyeHPos), eyeHPos);
  servoWrite(CHANNEL::EYE::RIGHT::VERTICAL, pulseLengthToAngle(eyeVPos), eyeVPos);
  servoWrite(CHANNEL::EYELID::LEFT::TOP, pulseLengthToAngle(eyelidPos), eyelidPos);
  servoWrite(CHANNEL::EYELID::RIGHT::TOP, pulseLengthToAngle(eyelidPos), eyelidPos);
  servoWrite(CHANNEL::EYELID::LEFT::BOTTOM, pulseLengthToAngle(eyelidPos), eyelidPos);
  servoWrite(CHANNEL::EYELID::RIGHT::BOTTOM, pulseLengthToAngle(eyelidPos), eyelidPos);
  servoWrite(CHANNEL::FOREHEAD::LEFT, pulseLengthToAngle(foreheadPos), foreheadPos);
  servoWrite(CHANNEL::FOREHEAD::RIGHT, pulseLengthToAngle(foreheadPos), foreheadPos);
  servoWrite(CHANNEL::MOUTH::LEFT, pulseLengthToAngle(mouthPos), mouthPos);
  servoWrite(CHANNEL::MOUTH::RIGHT, pulseLengthToAngle(mouthPos), mouthPos);

  // Delay before the next loop iteration
  delay(50);
}
