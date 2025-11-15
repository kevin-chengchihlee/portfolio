#Kevin Lee 10/11/2025
import time
import threading
import UDP_v01 as UDP
import state_machine_v01 as STATE
    
if __name__=='__main__':
    
    STATE.gpio_setup()
    receiver_thread = threading.Thread(target=UDP.UDP_Receive, daemon=True)
    receiver_thread.start()# Put UDP listening to thread to ensuring listening
    print("####################################################")
    print("System Up! Press the button to start com with ESP32!")
    print("####################################################")
    
    try:
        while True:
            time.sleep(0.5)  # keep main thread alive
    except KeyboardInterrupt:
        print("Exiting...")
