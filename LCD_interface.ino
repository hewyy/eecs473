//LCD Interface 473 lab 2 in lab new as of 9/13/21 2:50pm

/** START OF IMPLEMENTATION **/

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

    // some error checking
    if (num_of_dps != 8 || num_of_dps != 4) {
      exit(1);
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
    // move cursor to row, col
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
/** END OF IMPLEMENTATION **/



// LCD screen instance
LCD lcd1;

void setup() {

  Serial.begin(9600);
  
  // lcd setup
  int d[4] = {4, 6, 10, 11};
  lcd1 = LCD(A5, A4, 2, d);
  lcd1.cursor_off();
  
  // display non-ascii char
  lcd1.display(0b10110111, 0);
  delay(2000);
  
  // display messages (char arrays)
  lcd1.display("ARDUINO RULES!", 0);
  Serial.println("arduino rules!");
  
  // move to the first column of the next line
  lcd1.set_cursor(2, 0);
}
void loop() {
  // change message every 2 seconds
  lcd1.display("Roomba!", 2);
  lcd1.display("BLE 4ever", 2);
}