import msvcrt
import serial
ser = serial.Serial('COM38', 9600)
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
      ser.write('C21SE')

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
        message = input("type message: ")
        if len(message) > 4:
          print("message supplied is too long must be < 4 chars)")
        else:
          total_len = len(message) + 1
          val = "C" + char(total_len) + "4" + message + "E"
          print(val) # just for testing
          ser.write(val)
