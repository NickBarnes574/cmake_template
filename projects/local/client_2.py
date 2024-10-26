#!/usr/bin/env python3

import cmd
import socket
import struct

HOST = "127.0.0.1"
# HOST = "192.168.86.150"  # Replace with your server IP if needed
PORT = 17337  # Must match the port your server is listening on


class TCPClient(cmd.Cmd):
    intro = "Welcome to the LED control client. Type help or ? to list commands.\n"
    prompt = "(client) "

    def __init__(self):
        super().__init__()
        self.client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        try:
            self.client_socket.connect((HOST, PORT))
            print(f"Connected to server at {HOST}:{PORT}")
        except ConnectionRefusedError:
            print(f"Unable to connect to server at {HOST}:{PORT}")
            exit(1)

    def send_command(self, opcode):
        """Send a single-byte opcode to the server."""
        try:
            self.client_socket.sendall(struct.pack("B", opcode))
            # response = self.client_socket.recv(1024)
            # print(f"Received: {response}")
        except (ConnectionResetError, BrokenPipeError):
            print("Connection lost with the server.")
            self.client_socket.close()
            exit(1)

    def do_green_on(self, arg):
        """Turn on the green light."""
        self.send_command(0x04)  # GREEN_ON

    def do_green_off(self, arg):
        """Turn off the green light."""
        self.send_command(0x05)  # GREEN_OFF

    def do_red_on(self, arg):
        """Turn on the red light."""
        self.send_command(0x06)  # RED_ON

    def do_red_off(self, arg):
        """Turn off the red light."""
        self.send_command(0x07)  # RED_OFF

    def do_yellow_on(self, arg):
        """Turn on the yellow light."""
        self.send_command(0x08)  # YELLOW_ON

    def do_yellow_off(self, arg):
        """Turn off the yellow light."""
        self.send_command(0x09)  # YELLOW_OFF

    def do_exit(self, arg):
        """Close the connection and exit the client."""
        print("Closing connection...")
        self.send_command(0x03)  # CLOSE_CONNECTION
        self.client_socket.close()
        return True


if __name__ == "__main__":
    TCPClient().cmdloop()
