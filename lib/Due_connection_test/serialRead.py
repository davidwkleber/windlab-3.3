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
    
ser = serial.Serial('COM17',250000)
time.sleep(0.5)
ser.flushInput()
ser.flushOutput()
record_minutes=5 #minutes

message=''

time.sleep(5.0)
timestamp_last=0
timestamp=0
diff_last=0
timestamp_start=0;

#ser.write(b'NI')
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
        if text.__contains__('timestamp') & text.__contains__('voltage') &text.__contains__('current')&text.__contains__('windSpeed') &text.__contains__('pitchAngle') &text.__contains__('rpm') &text.__contains__('temperature') &text.__contains__('ambientPressure') &text.__contains__('humidity') &text.__contains__('accelerationX') &text.__contains__('accelerationY') &text.__contains__('accelerationZ') &text.__contains__('currentSink'):
            return True #&text.__contains__('dummy_a') &text.__contains__('dummy_b') &text.__contains__('dummy_c') &text.__contains__('dummy_d') &text.__contains__('dummy_e')
    except:
        return False
with open ('data/Data.csv','w', newline='') as csvfile:
    print(time.strftime('%X %x'))
    fieldnames=['timestamp','voltage','current','windSpeed','pitchAngle','rpm','temperature','ambientPressure','humidity','accelerationX','accelerationY','accelerationZ','currentSink'] #,'dummy_a','dummy_b','dummy_c','dummy_d','dummy_e'
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
        #print(message)
        if message.count('{')==1 & message.count('}')==1 :
            try:
                text = json.loads(message)
                # print(text)
                #print(text['timestamp'])
                if validate_json(text):
                    if timestamp==0:
                        # print(text['timestamp'])
                        timestamp=int(text['timestamp'])
                        timestamp_start=timestamp
                    else:
                        timestamp=int(text['timestamp'])
                    # print(text['timestamp'])
                    if timestamp>0:             #2*60*1000000:
                        writer.writerow(text)
                else:
                    # print("no JSON")
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
        #print(timestamp-timestamp_start)
        
        if (timestamp-timestamp_start)>record_minutes*60*1000000:# 1 minutes
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

