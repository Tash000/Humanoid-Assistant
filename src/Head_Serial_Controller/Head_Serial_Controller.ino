#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

// PCA9685 setup
Adafruit_PWMServoDriver pca9685 = Adafruit_PWMServoDriver();

#define DEBUG 1

// Motor channel assignments
struct CHANNEL {
  static const int JAW = 0;
  static const int SMILE_LEFT = 1;
  static const int SMILE_RIGHT = 2;
  static const int UPPER_LIP = 3;
  static const int EYELID_LEFT_UPPER = 4;
  static const int EYELID_LEFT_LOWER = 5;
  static const int EYELID_RIGHT_UPPER = 6;
  static const int EYELID_RIGHT_LOWER = 7;
  static const int EYE_HORIZONTAL = 8;
  static const int EYE_VERTICAL = 9;
  static const int EYEBROW_LEFT_UPPER = 10;
  static const int EYEBROW_LEFT_LOWER = 11;
  static const int EYEBROW_RIGHT_UPPER = 12;
  static const int EYEBROW_RIGHT_LOWER = 13;
};

// Servo and movement ranges
namespace RANGE {
  static const int SERVO_MIN = 150;
  static const int SERVO_MAX = 600;

  // Jaw range
  struct JAW {
    static const int MAX = 90;
    static const int NORMAL = 35;
    static const int MIN = 33;
  };

  // Smile range
  struct SMILE {
    static const int WIDE = 70;
    static const int SAD = 50;
    static const int NORMAL = 90;
  };

  // Upper Lip range
  struct LIP {
    static const int UP = 110;
    static const int NORMAL = 90;
    static const int DOWN = 70;
  };

  // Eyelid ranges
  struct EYELID {
    static const int LEFT_UPPER_OPEN = 95;
    static const int LEFT_UPPER_NORMAL = 90;
    static const int LEFT_UPPER_CLOSED = 85;
    static const int LEFT_LOWER_OPEN = 85;
    static const int LEFT_LOWER_NORMAL = 90;
    static const int LEFT_LOWER_CLOSED = 95;
    static const int RIGHT_UPPER_OPEN = 85;
    static const int RIGHT_UPPER_NORMAL = 90;
    static const int RIGHT_UPPER_CLOSED = 95;
    static const int RIGHT_LOWER_OPEN = 95;
    static const int RIGHT_LOWER_NORMAL = 90;
    static const int RIGHT_LOWER_CLOSED = 85;
  };

  // Eye movement ranges
  struct EYE {
    static const int HORIZONTAL_LEFT = 85;
    static const int HORIZONTAL_NORMAL = 90;
    static const int HORIZONTAL_RIGHT = 95;
    static const int VERTICAL_UP = 95;
    static const int VERTICAL_NORMAL = 90;
    static const int VERTICAL_DOWN = 85;
  };

  // Eyebrow ranges
  struct EYEBROW {
    static const int LEFT_UPPER_RAISE = 100;
    static const int LEFT_UPPER_NORMAL = 90;
    static const int LEFT_UPPER_LOWER = 80;
    static const int LEFT_LOWER_RAISE = 98;
    static const int LEFT_LOWER_NORMAL = 90;
    static const int LEFT_LOWER_LOWER = 83;
    static const int RIGHT_UPPER_RAISE = 80;
    static const int RIGHT_UPPER_NORMAL = 90;
    static const int RIGHT_UPPER_LOWER = 100;
    static const int RIGHT_LOWER_RAISE = 82;
    static const int RIGHT_LOWER_NORMAL = 90;
    static const int RIGHT_LOWER_LOWER = 97;
  };
}

void setup() {
  Serial.begin(115200);
  pca9685.begin();
  pca9685.setPWMFreq(60); // Set frequency to 60 Hz for servos
  normalizeServos();
}

void loop() {
  if (Serial.available()) {
    serialCMD();  // Handle serial commands
  }
}

// Set servo position using mapped angle to PWM range
void setServoPosition(int channel, int angle) {
  int pulseLength = map(angle, 0, 180, RANGE::SERVO_MIN, RANGE::SERVO_MAX);
  pca9685.setPWM(channel, 0, pulseLength);
}

// Set all servos to neutral positions
void normalizeServos() {
  setServoPosition(CHANNEL::JAW, RANGE::JAW::NORMAL);
  setServoPosition(CHANNEL::SMILE_LEFT, RANGE::SMILE::NORMAL);
  setServoPosition(CHANNEL::SMILE_RIGHT, RANGE::SMILE::NORMAL);
  setServoPosition(CHANNEL::UPPER_LIP, RANGE::LIP::NORMAL);
  setServoPosition(CHANNEL::EYE_HORIZONTAL, RANGE::EYE::HORIZONTAL_NORMAL);
  setServoPosition(CHANNEL::EYE_VERTICAL, RANGE::EYE::VERTICAL_NORMAL);
  setServoPosition(CHANNEL::EYEBROW_LEFT_UPPER, RANGE::EYEBROW::LEFT_UPPER_NORMAL);
  setServoPosition(CHANNEL::EYEBROW_LEFT_LOWER, RANGE::EYEBROW::LEFT_LOWER_NORMAL);
  setServoPosition(CHANNEL::EYEBROW_RIGHT_UPPER, RANGE::EYEBROW::RIGHT_UPPER_NORMAL);
  setServoPosition(CHANNEL::EYEBROW_RIGHT_LOWER, RANGE::EYEBROW::RIGHT_LOWER_NORMAL);
  setServoPosition(CHANNEL::EYELID_LEFT_UPPER, RANGE::EYELID::LEFT_UPPER_NORMAL);
  setServoPosition(CHANNEL::EYELID_LEFT_LOWER, RANGE::EYELID::LEFT_LOWER_NORMAL);
  setServoPosition(CHANNEL::EYELID_RIGHT_UPPER, RANGE::EYELID::RIGHT_UPPER_NORMAL);
  setServoPosition(CHANNEL::EYELID_RIGHT_LOWER, RANGE::EYELID::RIGHT_LOWER_NORMAL);

  if (DEBUG) {
    Serial.println("Servos normalized to default positions.");
  }
}

// Serial command handler
void serialCMD() {
  String cmd = Serial.readStringUntil('\n'); // Read input till newline
  int value = 0;

  // Jaw control (e.g., "JAW 45")
  if (cmd.startsWith("JAW")) {
    value = cmd.substring(4).toInt();  // Get the value after the command
    setServoPosition(CHANNEL::JAW, value);
    Serial.println("Jaw position set to " + String(value));
  }

  // Smile motor control (left: "SMLL 70", right: "SMLR 70")
  else if (cmd.startsWith("SML")) {
    if (cmd.endsWith("L")) {  // Left smile motor control
      value = cmd.substring(4, cmd.length() - 1).toInt();  // Extract the value
      setServoPosition(CHANNEL::SMILE_LEFT, value);
      Serial.println("Smile left position set to " + String(value));
    } 
    else if (cmd.endsWith("R")) {  // Right smile motor control
      value = cmd.substring(4, cmd.length() - 1).toInt();  // Extract the value
      setServoPosition(CHANNEL::SMILE_RIGHT, value);
      Serial.println("Smile right position set to " + String(value));
    }
  }

  // Upper lip control (e.g., "UPL 30")
  else if (cmd.startsWith("UPL")) {
    value = cmd.substring(4).toInt();  // Upper lip control
    setServoPosition(CHANNEL::UPPER_LIP, value);
    Serial.println("Upper Lip position set to " + String(value));
  }

  // Upper eyelid control (left: "EYUL 50", right: "EYUR 50")
  else if (cmd.startsWith("EYU")) {
    if (cmd.endsWith("L")) {  // Left upper eyelid
      value = cmd.substring(4, cmd.length() - 1).toInt();
      setServoPosition(CHANNEL::EYELID_LEFT_UPPER, value);
      Serial.println("Left Upper Eyelid position set to " + String(value));
    } 
    else if (cmd.endsWith("R")) {  // Right upper eyelid
      value = cmd.substring(4, cmd.length() - 1).toInt();
      setServoPosition(CHANNEL::EYELID_RIGHT_UPPER, value);
      Serial.println("Right Upper Eyelid position set to " + String(value));
    }
  }

  // Lower eyelid control (left: "EYLL 40", right: "EYLR 40")
  else if (cmd.startsWith("EYL")) {
    if (cmd.endsWith("L")) {  // Left lower eyelid
      value = cmd.substring(4, cmd.length() - 1).toInt();
      setServoPosition(CHANNEL::EYELID_LEFT_LOWER, value);
      Serial.println("Left Lower Eyelid position set to " + String(value));
    } 
    else if (cmd.endsWith("R")) {  // Right lower eyelid
      value = cmd.substring(4, cmd.length() - 1).toInt();
      setServoPosition(CHANNEL::EYELID_RIGHT_LOWER, value);
      Serial.println("Right Lower Eyelid position set to " + String(value));
    }
  }

  // Eye horizontal movement (e.g., "EYEH 60")
  else if (cmd.startsWith("EYEH")) {  // Horizontal eye movement
    value = cmd.substring(5).toInt();  // Extract the value
    setServoPosition(CHANNEL::EYE_HORIZONTAL, value);
    Serial.println("Horizontal Eye position set to " + String(value));
  }

  // Eye vertical movement (e.g., "EYEV 70")
  else if (cmd.startsWith("EYEV")) {  // Vertical eye movement
    value = cmd.substring(5).toInt();  // Extract the value
    setServoPosition(CHANNEL::EYE_VERTICAL, value);
    Serial.println("Vertical Eye position set to " + String(value));
  }

  // Upper eyebrow control (left: "EBUL 30", right: "EBUR 30")
  else if (cmd.startsWith("EBU")) {
    if (cmd.endsWith("L")) {  // Left upper eyebrow
      value = cmd.substring(4, cmd.length() - 1).toInt();
      setServoPosition(CHANNEL::EYEBROW_LEFT_UPPER, value);
      Serial.println("Left Upper Eyebrow position set to " + String(value));
    } 
    else if (cmd.endsWith("R")) {  // Right upper eyebrow
      value = cmd.substring(4, cmd.length() - 1).toInt();
      setServoPosition(CHANNEL::EYEBROW_RIGHT_UPPER, value);
      Serial.println("Right Upper Eyebrow position set to " + String(value));
    }
  }

  // Lower eyebrow control (left: "EBLL 20", right: "EBLR 20")
  else if (cmd.startsWith("EBL")) {
    if (cmd.endsWith("L")) {  // Left lower eyebrow
      value = cmd.substring(4, cmd.length() - 1).toInt();
      setServoPosition(CHANNEL::EYEBROW_LEFT_LOWER, value);
      Serial.println("Left Lower Eyebrow position set to " + String(value));
    } 
    else if (cmd.endsWith("R")) {  // Right lower eyebrow
      value = cmd.substring(4, cmd.length() - 1).toInt();
      setServoPosition(CHANNEL::EYEBROW_RIGHT_LOWER, value);
      Serial.println("Right Lower Eyebrow position set to " + String(value));
    }
  }
}
