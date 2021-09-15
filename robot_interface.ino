/** START OF LCD INTERFACE IMPLEMENTATION **/

class LCD {
  public:
  int read_write_pin;
  int enable_pin;
  int reg_sel_pin;
  int data_pins[8];
  bool four_bit_mode = false;

  LCD(){};


  /**
   * @brief init for LCD, sets up pins and modes
   * @param rs: physical pin # for register selec, rw: physical pin # for read/write, 
   * en: physical pin # for enable, data_pins_in: physical pin #'s for data pins
   * num_of_dps: how many data pins are specified
   */  
  LCD(int rs, int rw, int en, int *data_pins_in, int num_of_dps = 4): reg_sel_pin(rs), read_write_pin(rw), enable_pin(en) {
    
    // determine the number of datapins specifed
    if (num_of_dps == 4) {
      four_bit_mode = true; 
    } 
    
    // copy data_pins_in
    for (int i = 0; i < num_of_dps; i++) {
      data_pins[i] = data_pins_in[i];
    }
    
    // pin init, set all pin values to be low
    pinMode(reg_sel_pin, OUTPUT);
    digitalWrite(reg_sel_pin, LOW);
    
    pinMode(read_write_pin, OUTPUT);
    digitalWrite(read_write_pin, LOW);
    
    pinMode(enable_pin, OUTPUT);
    digitalWrite(enable_pin, LOW);
    
    for(int i = 0; i < 4; i++) {
      pinMode(data_pins[i], OUTPUT);
      digitalWrite(data_pins[i], LOW);
    }
    
    // Initialize for either 8-bit or 4-bit mode
    if (four_bit_mode) {
      init_four_bit();
    } else {
      init_eight_bit();
    }
  }


  /**
   * @brief For displaying a single character on the LCD
   * @param char_in: the char to be displayed, display_time: amount of time the char 
   * is displayed before it is cleared. If display_time = 0, the char will not be cleared
   */  
  void display(const char* message_in, double display_time = 0) {
    // for loop to write each character
    for(int i = 0; i < strlen(message_in); i++) {
      send_data(1, message_in[i]);
    }

    if(display_time > 0) {
      delay(display_time*1000);
      clear_message(strlen(message_in));
    }
  }
  

  /**
   * @brief For displaying a single character on the LCD
   * @param char_in: the char to be displayed, display_time: amount of time the char 
   * is displayed before it is cleared. If display_time = 0, the char will not be cleared
   */  
  void display(char char_in, double display_time = 0) {
    send_data(1, static_cast<int>(char_in)); 
    if (display_time > 0) {
      delay(display_time*1000);
      clear_message(1);
    }
  }
  

  /**
   * @brief For shifting the cursor from its current position
   * @param shift left by amount if negitive, shift right by amount if positive.
   */  
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
  

  /**
   * @brief for clearing only a portion of the screen of length message_len. 
   * Note: it is assumed that the cursor is in location directly after message was written
   * @param message_len: length of message to clear from screen
   */  
  void clear_message(int message_len) {
    // move cursor to the left of where it started
    move_cursor(-1*message_len);

    // write spaces
    for(int i = 0; i < message_len; i++) {
      send_data(1, 0b00100000);
    }

    // move cursor back again
    move_cursor(-1*message_len);
  }


  /**
   * @brief For moving the cursor to a given position based on row and col
   * @param row: the row cursor to be set to (usally 1 or 2). 
   * col: the column that the cursor should be set to
   */
  void set_cursor(int row, int col) {
    // reset the cursor to go home
    send_data(0, 0b00000010); // pg 24 of datasheet

    if (row == 2) {
      // need to move the cursor over 40 times to get to row 2
      for (int i = 0; i < 40; i++) {
        send_data(0, 0b00010100); 
      } 
    }

    // move the correct column
    for (int i = 0; i < col; i++) {
        send_data(0, 0b00010100); 
    } 
    return;
  }


  /**
   * @brief turns the cursor off
   */  
  void cursor_off() {
    send_data(0, 0b00001100);
  }
  

  /**
   * @brief turns the cursor on
   */  
  void cursor_on() {
    send_data(0, 0b00001110);
  }
  

  /**
   * @brief clears all characters from display
   */  
  void clear_display() {
    send_data(0, 0b00000001);
  }
  

  private:

  /**
   * @brief sends data to the lcd
   * @param val_of_reg_sel: the value to set the reg_select pin to, data: the data to send, 
   * bits_to_send: either sending 8 bits or 4 bits (this has nothing to do with 
   * the 4-bit or 8-bit modes, this is if the data to be send is 8bits or 4 bits)
   */  
  void send_data(int val_of_reg_sel, int data, int bits_to_send = 8) {   
    
    /*** SET PINS ****/
    digitalWrite(read_write_pin, LOW);
    digitalWrite(reg_sel_pin, val_of_reg_sel);
    
    int cover = 0b1 << (bits_to_send - 1);
    int bit_to_write;
  
    
    if (four_bit_mode) {
      // 4-bit data_pin mode
      int pin_index = 3;
      for(int i = 0; i < bits_to_send; i++) {
        bit_to_write = (data & cover);
        bit_to_write = bit_to_write >> (bits_to_send - 1 - i);
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
    } else {
      // 8-bit data_pin mode
      // NOT IMPLEMENTED
    }
  }

  /**
   * @brief helper function for initilizing an LCD in 8-bit mode
   */  
  void init_eight_bit() {
    // NOT IMPLEMENTED
    return;
  }

  /**
   * @brief helper function for initilizing an LCD in 4-bit mode
   */  
  void init_four_bit() {
    // FOLLOWING STEPS ON PAGE 46 OF DATASHEET (Initializing by Instruction)
    delay(40); // wait 40ms
    send_data(0, 0b0011, 4); // send function set (4-bit)
    delay(5);
    send_data(0, 0b0011, 4);
    delay(1);
    send_data(0, 0b0011, 4);
    delay(1);
    send_data(0, 0b0010, 4);
    delay(1);
    send_data(0, 0b00101000); // set to 2 lines, 5x8 dot display
    delay(1);
    send_data(0, 0b00001000); // turn off display
    delay(1);

    // set entry mode pag 42 step 5
    send_data(0, 0b00000001); // clear the display
    delay(1);
    send_data(0, 0b00000110); // cursor moves to the right with each char. no display shift
    delay(1);

    send_data(0, 0b00001111); // display on, cursor on, blink on
    send_data(0, 0b00000010); // return home
  }
  
};
/** END OF LCD INTERFACE IMPLEMENTATION **/






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
    
    int d[4] = {4, 6, 10, 11};
    lcd1 = LCD(A5, A4, 2, d);
    lcd1.cursor_off();
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

        // we don't want that first char in the message
        char message_to_print[MESSAGE_MAX_SIZE - 1];
        for (int i = 1; i < strlen(message) - 1; i++){
            message_to_print[i - 1] = message[i];
        }

        if (!strcmp(message_to_print, "CLR")) {
            lcd1.clear_display();
        } else {
            lcd1.display(message_to_print);
        }
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
            SPEED = SPEED <= 0 ? 0 : SPEED - 5;
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
