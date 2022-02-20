#!/usr/bin/env python3

# some easy python to do basic control of a stepper motor
# written by grey@christoforo.net

import serial

# some details related to the firmware
cmd_term = '\t'
fw_prompt = '>>> '

# some details related to the setup
port = '/dev/ttyUSB0'
timeout = 30  # in s, no motion (including homing) should take longer than this

# a list of commands to issue to the firmware (see Basic.ino sketch)
command_q = ['hr', '250000', '-100000', '50000']
# home counter clockwise
# take 250k steps
# take -100k steps
# take 50k steps

print("Connecting, initializing...")
with serial.Serial(port, 9600, timeout=timeout) as ser:
    # wait for firmware to come up (appearance of hello message)
    ser.read_until("begin.".encode())
    ser.write('b'.encode())  # send begin signal (hardware inits now)

    # wait for hardware  init to complete and command prompt to appear
    ser.read_until(fw_prompt.encode())
    print("Ready for commands!\n")

    command_q.append('hang')
    for cmd in command_q:
        msg = cmd+cmd_term
        print(f"Sending cmd = '{cmd}'...")
        ser.write(msg.encode())
        if (cmd in ['reboot', 'hang']):
            break

        response = ser.read_until(fw_prompt.encode()).decode().rstrip(fw_prompt).strip()
        print(f"Response = '{response}'\n")

print("Done!")
