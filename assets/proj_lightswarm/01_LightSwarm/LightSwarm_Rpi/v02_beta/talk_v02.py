#Kevin Lee 10/11/2025
import time
import threading
import UDP_v02 as UDP
import state_machine_v02 as STATE
import LightSwarm as LS

if __name__=='__main__':
    
    STATE.gpio_setup()
    receiver_thread = threading.Thread(target=UDP.UDP_Receive, daemon=True)
    receiver_thread.start()# Put UDP listening to thread to ensuring listening
    processPacket_thread = threading.Thread(target=LS.processPacket, daemon=True)
    processPacket_thread.start()
    print("####################################################")
    print("System Up! Listening to LightSwarm Packets!!")
    print("####################################################")
    
    try:
        while True:
            time.sleep(0.5)  # keep main thread alive
    except KeyboardInterrupt:
        print("Exiting...")
