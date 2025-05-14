import socket
HOST = '0.0.0.0'  # 接收任意IP连接
PORT = 8080

def main():
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as server:
        server.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        server.bind((HOST, PORT))
        server.listen(1)
        print(f"Server listening on port {PORT}...")
        conn, addr = server.accept()
        with conn:
            print(f"Connected by {addr}")
            while True:
                data = conn.recv(1024)
                if not data:
                    break
                print(f"Received: {data.decode(errors='ignore')}")
                conn.sendall(b"MOVE_FORWARD\n")

if __name__ == '__main__':
    main()