#!/usr/bin/python3

import socket
import struct
import cmd

SHUTDOWN = 3


class ClientCLI(cmd.Cmd):
    prompt = "client> "
    intro = "Welcome to the client. Type help or ? to list commands.\n"

    def __init__(self):
        super().__init__()
        self.client_socket = None
        self.default_server_ip = "127.0.0.1"
        self.default_server_port = 17337

    def do_connect(self, arg):
        "Connect to the server: connect [server_ip] [server_port]"
        args = arg.split()
        if len(args) == 0:
            server_ip = self.default_server_ip
            server_port = self.default_server_port
        elif len(args) == 1:
            server_ip = args[0]
            server_port = self.default_server_port
        elif len(args) == 2:
            server_ip = args[0]
            server_port = int(args[1])
        else:
            print("Invalid arguments. Usage: connect [server_ip] [server_port]")
            return

        self.client_socket = connect_to_server(server_ip, server_port)
        if self.client_socket:
            print(f"Connected to {server_ip}:{server_port}")

    def do_send(self, arg):
        "Send a message to the server: send <message>"
        if not self.client_socket:
            print("Not connected to any server. Use connect to establish a connection.")
            return

        opcode = 2
        message = arg
        send_message(opcode, message, self.client_socket)

    def do_shutdown(self, arg):
        "Send shutdown opcode to the server and close the connection: shutdown"
        if not self.client_socket:
            print("Not connected to any server. Use connect to establish a connection.")
            return

        send_shutdown(self.client_socket)
        self.client_socket.close()
        self.client_socket = None
        print("Connection closed.")

    def do_exit(self, arg):
        "Exit the client"
        if self.client_socket:
            self.client_socket.close()
        print("Goodbye!")
        return True


def connect_to_server(server_ip="127.0.0.1", server_port=17337):
    try:
        client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        client_socket.connect((server_ip, server_port))
        return client_socket
    except socket.error as e:
        print(f"Failed to connect to server: {e}")
        return None


def send_message(opcode, message, client_socket):
    try:
        message_bytes = message.encode("utf-8")
        message_len = len(message_bytes)
        packet_format = "!B3sI{}s".format(message_len)
        padding = b"\x00" * 3
        print(f"message_len: {message_len}")
        packet = struct.pack(packet_format, opcode, padding, message_len, message_bytes)
        print(f"Packed packet: {packet}")
        client_socket.sendall(packet)
        print(f'Sent: opcode={opcode}, message="{message}"')
    except socket.error as e:
        print(f"Failed to send message: {e}")


def send_shutdown(client_socket):
    try:
        opcode = SHUTDOWN
        padding = b"\x00" * 3
        message_len = 0
        packet_format = "!B3sI"
        packet = struct.pack(packet_format, opcode, padding, message_len)
        client_socket.sendall(packet)
        print("Sent shutdown opcode.")
    except socket.error as e:
        print(f"Failed to send shutdown opcode: {e}")


if __name__ == "__main__":
    ClientCLI().cmdloop()
