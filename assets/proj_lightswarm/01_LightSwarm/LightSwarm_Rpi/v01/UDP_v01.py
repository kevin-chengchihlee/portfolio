#Kevin Lee 10/11/2025
import socket
import state_machine_v01 as STATE
print("####################################")
print("!!!!!!!!!!!!!!!!!!Hi!!!!!>0<!!!!!!!!")
print("####################################")
UDP_IP = input("Enter Listening IP or Enter to default 0.0.0.0: ") or "0.0.0.0"
UDP_PORT = 8386
Laptop_UDP_IP = input("Enter Laptop IP or Enter to default 172.20.10.4: ") or "172.20.10.4"
Laptop_UDP_PORT = 823
ESP32_UDP_IP = input("Enter ESP32 IP or Enter to default 172.20.10.2: ") or "172.20.10.2"
ESP32_UDP_PORT = 1996

sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
sock.bind((UDP_IP, UDP_PORT))
message_r = None
new_msg_cnt = 0

def UDP_Send(message):
    #send to both ESP and my Laptop
    sock.sendto(message.encode(), (Laptop_UDP_IP, Laptop_UDP_PORT))
    sock.sendto(message.encode(), (ESP32_UDP_IP, ESP32_UDP_PORT))

    print(f"Sending UDP Message:'{message}' to {Laptop_UDP_IP}:{Laptop_UDP_PORT}")
    print(f"Sending UDP Message:'{message}' to {ESP32_UDP_IP}:{ESP32_UDP_PORT}")

def UDP_Receive():
    global message_r
    global new_msg_cnt

    while True:
        data, addr = sock.recvfrom(1024)
        message = data.decode()#errors="ignore"
        message_r = message
        new_msg_cnt +=1#UDP incoming count up
        print(f"Received from {addr}: {message_r}")


def get_message_r():
    #for getting the received UDP message
    return message_r

def get_new_msg_cnt():
    #for getting the UDP message incoming status
    return new_msg_cnt
