import bluetooth
import serial
import time
import requests
import json

# Bluetooth settings
target_address = "98:DA:60:0B:B9:CC"
port = 1

# Serial settings
serial_port = '/dev/ttyUSB0'
baud_rate = 9600
                         
# Variables to track window status
window_open = False
window_closed = True

sensor_url = 'http://220.68.27.103:5000/api'
control_url = 'http://220.68.27.103:5000/window_status'
environment_url = 'http://220.68.27.103:5000/environment'

def receive_Uno_message(sock):
    buffer = ""  # Buffer to accumulate data
    try:
        while True:
            data = sock.recv(1024).decode()
            if data:
                buffer += data
                if '\n' in buffer:
                    messages = buffer.split('\n')
                    print(messages)
                    for message in messages[:-1]:  # Process all complete messages
                        values = message.split(',')
                        if len(values) == 6:  # Ensure the message has 6 values
                            data_dict = {
                                "temp": float(values[0]),
                                "humid": float(values[1]),
                                "pm": float(values[2]),
                                "rain": float(values[3]),
                                "gas": float(values[4]),
                                "light": float(values[5])
                            }
                            json_data = json.dumps(data_dict, indent=4)
                            print("From Ras to Flask server: sending json data:", json_data)
                            # Send data to the server
                            response = requests.post(sensor_url, json=data_dict)
                            print("Data sent to server, waiting for response...")
                            requests.post(environment_url, json=data_dict)
                            response_data = response.json()
                            print("From Flask server to Ras: Receiving json data:", response_data)
                            return data_dict
    except Exception as e:
        print("Failed to receive message:", e)
    return None

def send_Uno_message(sock, message):
    try:
        sock.send(message.encode('utf-8'))
        print("Sent:", message)
    except Exception as e:
        print("Failed to send message:", e)

def send_serial_command(ser, command):
    try:
        ser.write(command.encode())
        print("Sent command to Arduino:", command)
    except Exception as e:
        print("Failed to send command:", e)

def check_server_for_window_command():
    try:
        response = requests.get(control_url)
        if response.status_code == 200:
            data = response.json()
            print('창문 개방 여부:', data)
            return data['status']
        else:
            print("Failed to get window status from server")
            return None
    except Exception as e:
        print("Failed to get window status from server:", e)
        return None

def main():
    global window_open, window_closed

    # Bluetooth connection
    nearby_devices = bluetooth.discover_devices(duration=8, lookup_names=True, flush_cache=True)

    if target_address not in [addr for addr, _ in nearby_devices]:
        print("Cannot search target device")
        return

    print("Target device searched successfully")

    sock = bluetooth.BluetoothSocket(bluetooth.RFCOMM)
    try:
        sock.connect((target_address, port))
        print("Bluetooth connection succeeded")
    except Exception as e:
        print("Failed to connect Bluetooth:", e)
        return

    # Serial port setup
    try:
        ser = serial.Serial(serial_port, baud_rate, timeout=1)
        time.sleep(2)  # Wait for port initialization
    except Exception as e:
        print("Serial port setup failed:", e)
        return

    try:
        while True:
            print("Ready to receive Bluetooth message")
            sensor_data = receive_Uno_message(sock)
            print(sensor_data,1)
            time.sleep(5)
            if sensor_data:
                # Parse sensor data
                temp, humid, pm, rain, gas, light = sensor_data['temp'], sensor_data['humid'], sensor_data['pm'], sensor_data['rain'], sensor_data['gas'], sensor_data['light']
                print(f"temp: {temp}, humid: {humid}, pm: {pm}, rain: {rain}, gas: {gas}, light: {light}")

                # Check window status from server
                window_command = check_server_for_window_command()
                if window_command == "open" and not window_open:
                    print('창문 열기 명령이 전송됨')
                    send_serial_command(ser, 'o')  # Send window open command
                    window_open = True
                    window_closed = False
                elif window_command == "closed" and not window_closed:
                    print('창문 닫기 명령이 전송됨')
                    send_serial_command(ser, 'c')  # Send window close command
                    window_open = False
                    window_closed = True
                else:
                    print('창문 멈춤 명령이 전송됨')
                    send_serial_command(ser, 's')  # Send stop command

    except Exception as e:
        print("Error:", e)
    finally:
        sock.close()
        ser.close()

if __name__ == "__main__":
    main()
