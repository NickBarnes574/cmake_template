#!/usr/bin/python3

import socket
import struct

SHUTDOWN = 6


def send_message(opcode, message, server_ip="127.0.0.1", server_port=17337):
    # Create a socket
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as client_socket:
        # Connect to the server
        client_socket.connect((server_ip, server_port))

        # Prepare the packet
        message_bytes = message.encode("utf-8")
        message_len = len(message_bytes)
        packet_format = "!B3sI{}s".format(message_len)
        padding = b"\x00" * 3

        # Pack the data
        print(f"message_len: {message_len}")
        packet = struct.pack(packet_format, opcode, padding, message_len, message_bytes)

        # Print raw packet bytes
        print(f"Packed packet: {packet}")

        # Send the packet
        client_socket.sendall(packet)
        print(f'Sent: opcode={opcode}, message="{message}"')


# Example usage
if __name__ == "__main__":
    opcode = 2
    message = "Hello, Server!"
    send_message(opcode, message)
