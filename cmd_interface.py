import msvcrt
import serial

def message_mode():
  message = raw_input("type message: ")

  num_of_chars = 0
  message_packet = ""

  for char in message:
    message_packet += char
    num_of_chars += 1

    if num_of_chars == 3:
      val = "C" + "4" + "4" + message_packet + "E"
      print("packet sent", val) # just for testing
      ser.write(val)
      num_of_chars = 0
      message_packet = ""

  # if there are still chars to send
  if num_of_chars != 0:
    total_len = num_of_chars + 1
    val = "C" + str(total_len) + "4" + message_packet + "E"
    print("packet sent", val) # just for testing
    ser.write(val)


# main
ser = serial.Serial('COM40', 9600)
while 1:
  # Poll keyboard
  if msvcrt.kbhit():
    key = msvcrt.getch()
    print(key)
    # stop
    if key == ' ':
      ser.write('C21SE')

    # forward
    elif key == 'w':
      ser.write('C21FE')

    # backwards
    elif key == 's':
      ser.write('C21BE')
    # left
    elif key == 'a':
      ser.write('C21LE')

    # right
    elif key == 'd':
      ser.write('C21RE')

    # speed up
    elif key == ']':
      ser.write('C21AE')

    # slow down
    elif key == '[':
      ser.write('C21DE')

    # message for display
    elif key == '`':
	  message_mode()
