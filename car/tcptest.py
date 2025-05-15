import socket
import requests
import threading

# TCP服务器配置
TCP_HOST = '0.0.0.0'  # 接收任意IP连接
TCP_PORT = 8080       # 使用不同端口避免与Flask冲突

# Flask主服务器配置
FLASK_SERVER_URL = "http://localhost:8080/api/sensor-data"

def forward_to_flask(data_str):
    """将接收到的数据转发到Flask主服务器"""
    try:
        # 检查数据是否包含有效的传感器数据字段
        if any(key in data_str for key in ["TEMP:", "PRESS:", "TRACK:", "AVOID:", "COLOR:", "DIST:"]):
            response = requests.post(FLASK_SERVER_URL, 
                                   data=data_str.encode('utf-8'),
                                   headers={'Content-Type': 'text/plain'})
            print(f"转发到主服务器: HTTP {response.status_code}")
        else:
            print("忽略非传感器数据:", data_str)
    except Exception as e:
        print("转发数据时出错:", str(e))

def handle_client(conn, addr):
    """处理客户端连接"""
    print(f"客户端 {addr} 已连接")
    with conn:
        while True:
            data = conn.recv(1024)
            if not data:
                break
            
            # 接收并处理数据
            data_str = data.decode('utf-8', errors='ignore').strip()
            print(f"接收到原始数据: {data_str}")
            
            # 转发到主服务器（在新线程中执行避免阻塞）
            threading.Thread(target=forward_to_flask, args=(data_str,)).start()
            
            # 发送响应
            conn.sendall(b"ACK: Data received and forwarded\n")

def main():
    """主服务器函数"""
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as server:
        server.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        server.bind((TCP_HOST, TCP_PORT))
        server.listen(5)  # 允许最多5个等待连接
        print(f"TCP中继服务器运行在 {TCP_HOST}:{TCP_PORT}...")
        
        while True:
            conn, addr = server.accept()
            # 为每个客户端创建新线程
            threading.Thread(target=handle_client, args=(conn, addr)).start()

if __name__ == '__main__':
    main()