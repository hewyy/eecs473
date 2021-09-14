import msvcrt
import serial
ser = serial.Serial('COM38', 9600)
while 1:
  # Poll keyboard
  if msvcrt.kbhit():
    key = msvcrt.getch()
  if key == 'f':
    ser.write('C21FE')
  elif key == 's':
    ser.write('C21SE')
