#!/usr/bin/env python

# imported modules
import os                   # portable way of using operating system dependent functionality
# provides access to some variables used or maintained by the interpreter
import sys
import time                 # provides various time-related functions
import fcntl                # performs file control and I/O control on file descriptors
import serial               # encapsulates the access for the serial port
import threading
#SQL
import datetime
import glob
import MySQLdb
from time import strftime


if __name__ == '__main__':
    serial = serial.Serial()
    serial.port = '/dev/ttyUSB0'
    serial.baudrate = 9600
    serial.timeout = 1
    serial.writeTimeout = 1
    serial.open()
    #Variables for MySQL
    db = MySQLdb.connect(host="localhost", user="root", passwd="5522", db="temp_database")
    cur = db.cursor()

    # make stdin a non-blocking file
    fcntl.fcntl(sys.stdin, fcntl.F_SETFL, os.O_NONBLOCK)

    # post startup message to other XBee's and at stdout
    serial.writelines("RPi #1 is up and running.\r\n")
    print "RPi #1 is up and running."
    try:
        while True:
            # read a line from XBee and convert it from b'xxx\r\n' to xxx and
            # print at stdout
            line = serial.readline().decode('utf-8')
	    pieces = line.split("\t") #split the data by tab
            if line:
		datetimeWrite = (time.strftime("%Y-%m-%d ") + time.strftime("%H:%M:%S"))
                print line
		sql = ("""INSERT INTO tempLog (datetime,temperature,humidity,dewpoint) VALUES (%s,%s,%s,%s)""",(datetimeWrite,pieces[0],pieces[1],pieces[2]))
		print "Writing to database.."
		cur.execute(*sql)
		db.commit()
            # read data from the keyboard (i.e. stdin) and send via the XBee
            # modem
            try:
                 line = sys.stdin.readline()
                 serial.writelines(line)
            except IOError:
                time.sleep(0.1)
                continue

    except KeyboardInterrupt:
        printc("\n*** Ctrl-C keyboard interrupt ***", ERROR_TEXT)
        serial.writelines("RPi #1 is going down.\r\n")
