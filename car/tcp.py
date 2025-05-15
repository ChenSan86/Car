import socket
import requests
HOST = '0.0.0.0'  # 接收任意IP连接
PORT = 8081
def forward_to_server(data_str):
    """简单的数据转发函数"""
    try:
        requests.post('http://localhost:8080/api/sensor-data', 
                     data=data_str.encode('utf-8'),
                     timeout=1)
    except:
        pass  # 静默失败，不干扰主程序

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
                # 简单地将数据转发到服务器
                forward_to_server(data.decode(errors='ignore'))

if __name__ == '__main__':
    main()