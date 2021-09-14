/**
 * @brief Control commands available for the robot.
 */
#define FORWARD     'F'
#define LEFT        'L'
#define BACKWARD    'B'
#define RIGHT       'R'
#define STOP        'S'
#define IDL         'I'
#define SPEED_UP    'A'
#define SPEED_DN    'D'

/**
 * @brief Start and end delimiters
 * @details Ideally these would be high ASCII characters and MUST
 * be something that doesn't occur inside of a legal packet!
 */
const char SoP = 'C';
const char EoP = 'E';

/**
 * @brief Other constants and variables for communication
 */
const char nullTerminator = '\0';
unsigned char inByte;
#define MESSAGE_MAX_SIZE 5
char message[MESSAGE_MAX_SIZE];
char command;


/************************/
/**** LCD INTERFACE ****/
/************************/
class LCD {
  public:
  int read_write_pin;
  int enable_pin;
  int reg_sel_pin;
  int data_pins[4] = {4, 6, 10, 11};

  LCD(){};

  LCD(int rs, int rw, int en) {
    reg_sel_pin = rs;
    read_write_pin = rw;
    enable_pin = en;
    pinMode(reg_sel_pin, OUTPUT);
    digitalWrite(reg_sel_pin, LOW);
    
    pinMode(read_write_pin, OUTPUT);
    digitalWrite(read_write_pin, LOW);
    
    pinMode(enable_pin, OUTPUT);
    for(int i = 0; i < 4; i++) {
      pinMode(data_pins[i], OUTPUT);
      digitalWrite(data_pins[i], LOW);
    }
    
    // wait 40 ms for power on
    delay(40);
    // send function set (4-bit)
    send_data(0, 0b0011, 4);
    // wait 4.1 ms
    delay(5);
    send_data(0, 0b0011, 4);
    delay(1);
    send_data(0, 0b0011, 4);
    delay(1);
    send_data(0, 0b0010, 4);
    delay(1);
    // possibly set display size
    send_data(0, 0b00101000); // set to 2 lines, 5x8 dot display
    // turn display on with send_data
    delay(1);
    send_data(0, 0b00001000); // turn off display
    // set entry mode pag 42 step 5
    delay(1);
    send_data(0, 0b00000001); // clear the display
    delay(1);
    send_data(0, 0b00000110); // cursor moves to the right with each char. no display shift
    delay(1);

    send_data(0, 0b00001111); // display on, cursor on, blink on
    send_data(0, 0b00000010); // return home
    // then you can start writing data
  }

  // display message
  void display(const char* message_in, double display_time = 0) {
    // for loop to write each character
    for(int i = 0; i < strlen(message_in); i++) {
      send_data(1, message_in[i]);
    }
  }
  
  // display character
  void display(char char_in, int row, int column, double display_time = 0) {
    // move cursor to row, col
    set_cursor(row, column);
    send_data(1, static_cast<int>(char_in)); 
    if (display_time > 0 ) {
      delay(display_time);
    }
  }
  
  void move_cursor(int shift) {
    if (shift < 0) { //shift left
      for (int i = 0; i < shift*(-1); i++) {
        send_data(0, 0b00010000); 
      } 
    }
    else { //shift right
      for (int i = 0; i < shift; i++) {
        send_data(0, 0b00010100); 
      }   
    }
  }
  
  void cursor_off() {
    send_data(0, 0b00001100);
  }
  
  void cursor_on() {
    send_data(0, 0b00001110);
  }
  
  private:
  
  // takes 8 bit and breaks into 2 4s
  void send_data(int val_of_reg_sel, int data, int bits = 8) {
      
    // Read Busy Flag and Address, page 29 of second manual
    /** WAIT FOR BF **/
    Serial.println("start");
    /*** SET R/W PIN ****/
    digitalWrite(read_write_pin, LOW);
  
    /*** SET REG SEL ***/
    digitalWrite(reg_sel_pin, val_of_reg_sel);
  
    /***** SET DATA PINS ****/
    int cover = 0b1 << (bits - 1);
    int bit_to_write;
  
    // 4-bit mode
    //if (sizeof(data_pins)/sizeof(data_pins[0]) == 4) {
    int pin_index = 3;
    for(int i = 0; i < bits; i++) {
      bit_to_write = (data & cover);
      bit_to_write = bit_to_write >> (bits - 1 - i);
      Serial.println(bit_to_write, BIN);
    //  Serial.println(data_pins[pin_index]);
      digitalWrite(data_pins[pin_index], bit_to_write);
      cover = cover >> 1;
      if(pin_index == 0) {
            /*** SET EN PIN ***/
        Serial.println("enable");
        digitalWrite(enable_pin, LOW);
        delay(1);
        digitalWrite(enable_pin, HIGH);
        delay(1);
        digitalWrite(enable_pin, LOW);
        pin_index = 3;
      } else {
        pin_index = pin_index - 1;
      }
    }
  }
};
/***** END OF INTERFACE *****/


LCD lcd1;

/**
 * @brief Definitions of different speed levels.
 * @details Here we define idle as 0 and full speed as 150. For now,
 * we don't have intermediate speed levels in the program. But you
 * are welcome to add them and change the code.
 */
const int IDLE  = 0;
int SPEED = 50;

/**
 * @brief H bridge (SN754410) pin connections to Arduino.
 * @details The main ideas is to drive two enable signals with PWMs to
 * control the speed. Connect the 4 logic inputs to any 4 Arduino
 * digital pins. We can also connect enables to vcc to save pins, but
 * then we need 2 or 4 pwm pins. These methods are all feasible, but you
 * need to adjust the setup and the function 'motorControl' accordingly.
 */
/// pwm pin, control left motor
const int EN1 = 3;
/// pwm pin, control right motor
const int EN2 = 5;
/// control Y1 (left motor positive)
const int A_1 = 7;
/// control Y2 (left motor negative)
const int A_2 = 8;
/// control Y3 (right motor positive)
const int A_3 = 9;
/// control Y4 (right motor negative)
const int A_4 = 13;
#define LEFT_MOTOR  true
#define RIGHT_MOTOR false

/**
 * @brief Packet parser for serial communication
 * @details Called in the main loop to get legal message from serial port.
 * @return true on success, false on failure
 */
bool parsePacket();

/**
 * @brief Control the robot
 * @param command FORWARD, LEFT, BACKWARD or STOP.
 */
void moveRobot(char command);

/**
 * @brief Control motor
 * @details Called by `moveRobot` to break down high level command
 * to each motor.
 *
 * @param ifLeftMotor Either Left motor or right motor.
 * @param command FORWARD, STOP or BACKWARD
 */
void motorControl(bool ifLeftMotor, char command);

/**
 * @brief Pin setup and initialize state
 * @details Enable serial communication, set up H bridge connections,
 * and make robot stop at the beginning.
 */
void setup() {
    Serial.begin(9600);
    Serial.println("START");
    pinMode(EN1, OUTPUT);
    pinMode(EN2, OUTPUT);
    pinMode(A_1, OUTPUT);
    pinMode(A_2, OUTPUT);
    pinMode(A_3, OUTPUT);
    pinMode(A_4, OUTPUT);

    moveRobot(STOP);
    delay(10);
    Serial.println("Ready, Steady, Go");
    delay(10);
}

/**
 * @brief Main loop of the program
 * @details In this function, we get messages from serial port and
 * execute them accordingly.
 */
void loop() {
    /// 1. get legal message
    if (!parsePacket())
        return;

    /// 2. action, for now we only use option 1
    if (message[0] == '1') {
        // Move command
        command = message[1];
        moveRobot(command);
    }
    else if (message[0] == '2') {
        // Display Read
        // ...
    }
    else if (message[0] == '3') {
        // Distance Read
        // ...
    }
    else if (message[0] == '4') {
        // Display Write
        lcd1.display(message);
        return;
    }
    else {
        Serial.println("ERROR: unknown message");
        return;
    }
}


/**
 * @brief FUNCTION IMPLEMENTATIONS BELOW
 */
bool parsePacket() {
    /// step 1. get SoP
    while (Serial.available() < 1) {};
    inByte = Serial.read();
    if (inByte != SoP) {
        Serial.print("ERROR: Expected SOP, got: ");
        Serial.write((byte)inByte);
        Serial.print("\n");
        return false;
    }

    /// step 2. get message length
    while (Serial.available() < 1) {};
    inByte = Serial.read();
    if (inByte == EoP || inByte == SoP) {
        Serial.println("ERROR: SoP/EoP in length field");
        return false;
    }
    int message_size = inByte - '0';
    if (message_size > MESSAGE_MAX_SIZE || message_size < 0) {
        Serial.println("ERROR: Packet Length out of range");
        return false;
    }

    /// step 3. get message
    for (int i = 0; i < message_size; i++) {
        while (Serial.available() < 1) {};
        inByte = Serial.read();
        if ((inByte == EoP || inByte == SoP)) {
            Serial.println("ERROR: SoP/EoP in command field");
            return false;
        }
        message[i] = (char)inByte;
    }
    message[message_size] = nullTerminator;

    /// step 4. get EoP
    while (Serial.available() < 1) {};
    inByte = Serial.read();
    if (inByte != EoP) {
        Serial.println("EoP not found");
        return false;
    } else {
        return true;
    }
}

void moveRobot(char command) {
    switch(command) {
        case FORWARD:
            Serial.println("FORWARD");
            motorControl(LEFT_MOTOR, FORWARD);
            motorControl(RIGHT_MOTOR, FORWARD);
            break;
        case LEFT:
            Serial.println("LEFT");
            motorControl(LEFT_MOTOR, STOP);
            motorControl(RIGHT_MOTOR, FORWARD);
            break;
        case BACKWARD:
            Serial.println("BACKWARD");
            motorControl(LEFT_MOTOR, BACKWARD);
            motorControl(RIGHT_MOTOR, BACKWARD);
            break;
        case RIGHT:
            Serial.println("RIGHT");
            motorControl(LEFT_MOTOR, FORWARD);
            motorControl(RIGHT_MOTOR, STOP);
            break;
        case STOP:
            Serial.println("STOP");
            motorControl(LEFT_MOTOR,STOP);
            motorControl(RIGHT_MOTOR,STOP);
            break;
        case IDL:
            // TODO: make work
            Serial.println("IDEL");
            digitalWrite(EN1, LOW);
            digitalWrite(EN2, LOW);
            break;
        case SPEED_UP:
            SPEED = SPEED > 100 ? 100 : SPEED + 5;
            analogWrite(EN1, SPEED);
            analogWrite(EN2, SPEED);
            break;
        case SPEED_DN:
            SPEED = SPEED < 0 ? 0 : SPEED - 5;
            analogWrite(EN1, SPEED);
            analogWrite(EN2, SPEED);
            break;
        default:
            Serial.println("ERROR: Unknown command in legal packet");
            break;
    }
}

/**
 * @brief Please rewrite the function if you change H bridge connections.
 */
void motorControl(bool ifLeftMotor, char command) {
    int enable   = ifLeftMotor ? EN1 : EN2;
    int motorPos = ifLeftMotor ? A_1 : A_3;
    int motorNeg = ifLeftMotor ? A_2 : A_4;
    int new_speed = ifLeftMotor ? SPEED: SPEED + SPEED*0.16;  
    switch (command) {
        case FORWARD:
            analogWrite(enable, new_speed);
            digitalWrite(motorPos, HIGH);
            digitalWrite(motorNeg, LOW);
            break;
        case BACKWARD:
            analogWrite(enable, new_speed);
            digitalWrite(motorPos, LOW);
            digitalWrite(motorNeg, HIGH);
            break;
        case STOP:
            digitalWrite(motorPos, LOW);
            digitalWrite(motorNeg, LOW);
            break;
        default:
            break;
    }
}
