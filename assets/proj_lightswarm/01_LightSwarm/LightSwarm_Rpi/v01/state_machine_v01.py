#Kevin Lee 10/11/2025
#Kevin Lee 10/20/2025 LED bug fix

import RPi.GPIO as GPIO
import time
import UDP_v01 as UDP
import threading
import re

# Pin definitions (BOARD numbering)
led_r = 37   # Red LED
led_y = 33   # Yellow LED
led_g = 31   # Green LED
led_w = 29   # White LED
btn   = 36   # Push button

led_state = False
sys_state = 0 #0 init #1 operation #2 err #3 pwr_dwn
new_msg_cnt = 0
pre_msg_cnt = 0
non_msg_cnt = 0

blink_stop   = threading.Event() #for stop led blink
photosns_stop   = threading.Event() #for stop photo sense

def button_callback(channel):
    state_machine()

def state_machine():
    global sys_state
    
    if sys_state == 0: #from init to operation
        m_operation()
        
    elif sys_state == 1:  #from norm down
        photosns_stop.set()
        m_pwr_dwn() #to pwr down
    
    elif sys_state == 2: #from err
        # Stop blinking
        blink_stop.set()
        photosns_stop.clear() #clear the stop event
        sys_state = 0
        m_operation() #to norm
        
    elif sys_state == 3: #from pwr down
        sys_state = 0
        m_operation() #to norm

def photo_sns():
    global new_msg_cnt
    global pre_msg_cnt
    value = 0
    print("################")
    print("In photo sns now")
    print("################")
    while sys_state==1 and not photosns_stop.is_set():
        pre_msg_cnt = new_msg_cnt
        msg = UDP.get_message_r()
        new_msg_cnt = UDP.get_new_msg_cnt()
        #Checking UDP incoming status
        if pre_msg_cnt == new_msg_cnt:
            non_msg_cnt+=1 #count no msg incoming
            if non_msg_cnt%2 == 0:
                print("No UDP Time:", non_msg_cnt/2, "s")
        else:
            non_msg_cnt=0 #reset whenever msg income
       
        nums = re.findall(r"[-+]?\d*\.\d+|\d+", msg)  # find floats or ints
        
        if non_msg_cnt == 20:
            print("NO INCOMING UDP >10SEC")
            photosns_stop.set() #stop this thread
            nums[0] = 0
            time.sleep(0.5)
            m_err() #goto err state
        #Checking UDP incoming status-----ends
        if nums:
           value = int(nums[0])
       
        if value>0 and value < 1500:
            GPIO.output(led_r, GPIO.LOW)
            GPIO.output(led_y, GPIO.LOW)
            GPIO.output(led_g, GPIO.HIGH)
        elif value>=1500 and value<3000:
            GPIO.output(led_r, GPIO.LOW)
            GPIO.output(led_y, GPIO.HIGH)
            GPIO.output(led_g, GPIO.HIGH)
        elif value>=3000:
            GPIO.output(led_r, GPIO.HIGH)
            GPIO.output(led_y, GPIO.HIGH)
            GPIO.output(led_g, GPIO.HIGH)
        elif sys_state == 2: #Turn LEDs off for err mode
            GPIO.output(led_r, GPIO.LOW)
            GPIO.output(led_y, GPIO.LOW)
            GPIO.output(led_g, GPIO.LOW)
        time.sleep(0.5)
    
def m_operation():
    global sys_state
    global message_r
    err = 0
    
    sys_state = 1
    GPIO.output(led_w, GPIO.HIGH)
    print("#################")
    print("In operation mode")
    print("#################")
    msg = "RPi:TRIG"

        
    for i in range(12): # > 10sec so it cover the limit
        
        if UDP.get_message_r() == "ESP:WORKING":
            print("Operation State UDP incoming:", UDP.get_message_r())
            # Start photo sensing in a thread
            photosns_thread = threading.Thread(target=photo_sns, daemon=True)
            photosns_thread.start()
            break
            
        elif i == 10:
            print("Time OUT!!")
            m_err()
            break
        
        if msg.strip():
            UDP.UDP_Send(msg)
        print("Waiting Time:", i, "s")
        time.sleep(1)


def m_pwr_dwn():
    global sys_state
    sys_state = 3
    photosns_stop.clear() #clear the stop event
    
    msg = "RPi:STOP"
    if msg.strip():
        UDP.UDP_Send(msg)
        
    GPIO.output(led_r, GPIO.HIGH)
    GPIO.output(led_y, GPIO.HIGH)
    GPIO.output(led_g, GPIO.HIGH)
    GPIO.output(led_w, GPIO.HIGH)
    time.sleep(0.5)
    GPIO.output(led_r, GPIO.LOW)
    time.sleep(0.5)
    GPIO.output(led_y, GPIO.LOW)
    time.sleep(0.5)
    GPIO.output(led_g, GPIO.LOW)
    time.sleep(0.5)
    GPIO.output(led_w, GPIO.LOW)#Turn LED_W off for m_pwr_dwn
    print("##################")
    print("Power down mode")
    print("##################")
    #Enter pwr_dwn procedure

def m_err():
    global sys_state
    print("###########")
    print("In err mode")
    print("Press Button to Re-try >o<")
    print("###########")
    GPIO.output(led_r, GPIO.LOW)
    GPIO.output(led_y, GPIO.LOW)
    GPIO.output(led_g, GPIO.LOW)
    time.sleep(0.5)
    
    #Enter err procedure
    sys_state = 2
    blink_stop.clear() #clear the event
    
    def blink_led():
        while not blink_stop.is_set():
            GPIO.output(led_w, not GPIO.input(led_w))
            time.sleep(0.5)
    # Start blinking in a thread
    blink_thread = threading.Thread(target=blink_led, daemon=True)
    blink_thread.start()

def gpio_setup():
    # GPIO setup
    GPIO.setmode(GPIO.BOARD)  # use BOARD numbering, not physical pins
    GPIO.setwarnings(False)
    
    # Set LEDs as outputs
    GPIO.setup(led_r, GPIO.OUT)
    GPIO.setup(led_y, GPIO.OUT)
    GPIO.setup(led_g, GPIO.OUT)
    GPIO.setup(led_w, GPIO.OUT)
    #Blink LEDs
    GPIO.output(led_r, GPIO.LOW)
    GPIO.output(led_y, GPIO.LOW)
    GPIO.output(led_g, GPIO.LOW)
    GPIO.output(led_w, GPIO.LOW)
    time.sleep(0.5)
    GPIO.output(led_r, GPIO.HIGH)
    GPIO.output(led_y, GPIO.HIGH)
    GPIO.output(led_g, GPIO.HIGH)
    GPIO.output(led_w, GPIO.HIGH)
    time.sleep(0.5)
    GPIO.output(led_r, GPIO.LOW)
    GPIO.output(led_y, GPIO.LOW)
    GPIO.output(led_g, GPIO.LOW)
    GPIO.output(led_w, GPIO.LOW)
    
    # Set button as input with pull-down resistor
    GPIO.setup(btn, GPIO.IN, pull_up_down=GPIO.PUD_DOWN)
    # Add interrupt to the push btn
    GPIO.add_event_detect(btn, GPIO.RISING, callback=button_callback, bouncetime=300)
