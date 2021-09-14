//LCD Interface 473 lab 2 in lab new as of 9/13/21 2:50pm


class LCD {
  public:
  int read_write_pin;
  int enable_pin;
  int reg_sel_pin;
  int data_pins[8];
  bool four_bit_mode = false;

  LCD(){};

  LCD(int rs, int rw, int en, int *data_pins_in): reg_sel_pin(rs), read_write_pin(rw), enable_pin(en) {
    
    // determine the number of datapins specifed
    if (sizeof(data_pins_in)/(sizeof(data_pins_in[0]) == 4) {
      four_bit_mode = true; 
    }
    
    // copy data_pins_in
    copy(begin(data_pins_in), end(data_pins_in), begin(data_pins));
    
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
  
  void clear_display() {
    // TODO: implement this
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
    
  void init_eight_bit() {
    // NOT IMPLEMENTED
    return;
  }
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

// global LCD screen
LCD lcd1;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
 // int d[4] = {4, 6, 10, 11};

  lcd1 = LCD(A5, A4, 2);
  lcd1.cursor_off();
  
  // non-ascii char
  lcd1.display(0b10110111, 0, 0, 0);
  
  delay(2000);
  
  lcd1.display("ARDUINO RULES!", 0);
  Serial.println("arduino rules!");
  
  // move to next line (40 spaces to get to next line)
  lcd1.move_cursor(29);
}
void loop() {
  // put your main code here, to run repeatedly
  lcd1.display("Roomba!  ", 0);
  Serial.println("roomba!");
  delay(1500);
  
  // move the cursor backwards
  lcd1.move_cursor(-9);
  
  lcd1.display("BLE 4ever", 0);
  Serial.println("BLE 4ever");
  delay(1500);
  lcd1.move_cursor(-9);
}
