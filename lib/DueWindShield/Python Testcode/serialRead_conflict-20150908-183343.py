'''
Python 3.4 code !!!
'''
import serial,json,time,csv,sys



# ser = serial.Serial(
	# port = 'COM28',
	# baudrate = 115200, 
	# parity = serial.PARITY_NONE,
	# stopbits = serial.STOPBITS_ONE,
	# bytesize = serial.EIGHTBITS
	# )
	
ser = serial.Serial('COM28',250000)

message=''

time.sleep(5.0)
timestamp_last=0
timestamp=0
diff_last=0
timestamp_start=0;

ser.write(b'NI')
time.sleep(2.0)
ser.write(b'AA')

def readlineCR(port):
    rv = ""
    start=False
    
    while True:
        # print(start)
        ch = port.read()
        # print(len(ch))
        # print(type(ch))
        # print("ch: ")
        # print(ch)
        try: 
            if ch.decode('utf-8')=='{':
                start=True
        except:
            pass
        if start:
            try:
                rv+=ch.decode('utf-8')
                
            except:
                print('Fail to decode')
                pass
            # print('rv: {0}'.format(rv))
            try:
                if rv.find('}\r\nEOL',0)!=-1:
                    start=False
                    # print('rv: {0}'.format(rv))
                    return rv[:-3]
            except:
                print('Fail to find EOL')
                pass

def validate_json(text):
    try:
        if text.__contains__('timestamp') & text.__contains__('voltage') &text.__contains__('current')&text.__contains__('windspeed') &text.__contains__('pitch') &text.__contains__('rpm') &text.__contains__('temperature') &text.__contains__('pressure_amb') &text.__contains__('humidity') &text.__contains__('g_x') &text.__contains__('g_y') &text.__contains__('g_z') &text.__contains__('dummy_a') &text.__contains__('dummy_b') &text.__contains__('dummy_c') &text.__contains__('dummy_d') &text.__contains__('dummy_e'):
            return True
    except:
        return False
with open ('data/Data.csv','w', newline='') as csvfile:
    print(time.strftime('%X %x'))
    fieldnames=['timestamp','voltage','current','windspeed','pitch','rpm','temperature','pressure_amb','humidity','g_x','g_y','g_z','dummy_a','dummy_b','dummy_c','dummy_d','dummy_e']
    writer = csv.DictWriter(csvfile, fieldnames=fieldnames,dialect='excel-tab')
    writer.writeheader()
    while ser.isOpen():
        # print(timestamp)
        # print(ser.isOpen())
        
        # if ser.readline() is not 'EOL':
            # message+=str(ser.readline())
            # print(message)
        message = readlineCR(ser)
        # print('Message: {0}'.format(message))
        # print('count open: {0} count close: {1}'.format(message.count('{'),message.count('}')))
        # print(message)
        if message.count('{')==1 & message.count('}')==1 :
            try:
                text= json.loads(message)
                # print(text)
                if validate_json(text):
                    if timestamp==0:
                        timestamp=int(text['timestamp'])
                        timestamp_start=timestamp
                    else:
                        timestamp=int(text['timestamp'])
                    # print(text['timestamp'])
                    if timestamp>0:             #2*60*1000000:
                        writer.writerow(text)
                else:
                    message=''
                    # timestamp=0
                    ser.flushOutput()
            except ValueError as err:
                print('JSON Error')
                print (err)
                print('Faulty Message: {0}'.format(message))
                message=''
                # timestamp=0
                ser.flushOutput()
                pass
        else:
            message=''
            # timestamp=0
            ser.flushOutput()
        
        if (timestamp-timestamp_start)>1*60*1000000:# 1 minutes
            print(time.strftime('%X %x'))
            ser.write(b'S')
            sys.exit()

        
        
        # diff=(timestamp-timestamp_last)
        # diff_diff=abs(diff-diff_last)
        # freq=1/(diff/1000000)
        # print(round(freq,2))

        # print(diff)
        # timestamp_last=timestamp
        # diff_last=diff

