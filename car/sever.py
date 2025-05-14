from flask import Flask, request, jsonify
from flask_cors import CORS
from flask_socketio import SocketIO
import re
import threading
import time
from datetime import datetime

app = Flask(__name__)
CORS(app)
socketio = SocketIO(app, cors_allowed_origins="*")

# 存储传感器数据
sensor_data = {
    'temperature': 0.0,
    'pressure': 0.0,
    'tracking': [0, 0, 0, 0, 0, 0, 0, 0],  # 8路循迹状态
    'avoidance': [0, 0],  # 2路避障状态
    'color': {'r': 0, 'g': 0, 'b': 0},  # RGB颜色
    'distance': 0.0,  # 超声波距离
    'timestamp': None
}

def parse_sensor_data(data_str):
    """解析传感器数据字符串"""
    try:
        data_parts = data_str.split(',')
        parsed_data = {}
        
        for part in data_parts:
            if ':' not in part:
                continue
                
            key, value = part.split(':', 1)
            
            if key == "TEMP":
                parsed_data['temperature'] = float(value)
            elif key == "PRESS":
                parsed_data['pressure'] = float(value)
            elif key == "TRACK":
                # 将8位二进制字符串转换为8个数字的列表
                parsed_data['tracking'] = [int(bit) for bit in value]
            elif key == "AVOID":
                # 将2位二进制字符串转换为2个数字的列表
                parsed_data['avoidance'] = [int(bit) for bit in value]
            elif key == "COLOR":
                # 解析RGB颜色值
                r, g, b = map(int, value.split('-'))
                parsed_data['color'] = {'r': r, 'g': g, 'b': b}
            elif key == "DIST":
                parsed_data['distance'] = float(value)
                
        return parsed_data
        
    except Exception as e:
        print(f"Error parsing sensor data: {e}")
        return None

@app.route('/api/sensor-data', methods=['GET'])
def get_sensor_data():
    """获取当前传感器数据"""
    return jsonify(sensor_data)

@app.route('/api/sensor-data', methods=['POST'])
def receive_sensor_data():
    """接收小车发送的传感器数据"""
    global sensor_data
    
    # 获取原始数据
    raw_data = request.data.decode('utf-8').strip()
    print(f"Received raw data: {raw_data}")

    # 解析数据
    parsed_data = parse_sensor_data(raw_data)
    if not parsed_data:
        return jsonify({"status": "error", "message": "Invalid data format"})
    
    # 更新传感器数据
    sensor_data.update(parsed_data)
    sensor_data['timestamp'] = datetime.now().strftime('%Y-%m-%d %H:%M:%S')
    
    # 通过WebSocket广播新数据
    socketio.emit('sensor_update', sensor_data)
    
    return jsonify({"status": "success", "message": "Data received"})

def background_thread():
    """模拟数据更新线程"""
    while True:
        socketio.sleep(1)
        # 这里可以添加模拟数据生成的逻辑
        pass

if __name__ == '__main__':
    # 启动后台线程
    threading.Thread(target=background_thread, daemon=True).start()
    # 启动服务器
    socketio.run(app, host='0.0.0.0', port=8080, debug=True)