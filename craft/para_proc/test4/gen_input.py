import random
import socket
import struct

def ip_to_int(ip):
    return struct.unpack("!I", socket.inet_aton(ip))[0]

def int_to_ip(num):
    # 使用 inet_ntoa 将二进制数据转换为IP字符串
    packed = struct.pack("!I", num)
    return socket.inet_ntoa(packed)

def generate_ip_range():
    start_a = 192
    start_b = 168
    start_c = random.randint(0, 255)
    start_d = random.randint(1, 254)
    start_ip = f"{start_a}.{start_b}.{start_c}.{start_d}"
    start_num = ip_to_int(start_ip)
    
    end_c = start_c + random.randint(0, 3)
    end_d = random.randint(start_d, 255)
    end_ip = f"{start_a}.{start_b}.{min(end_c, 255)}.{end_d}"
    end_num = ip_to_int(end_ip)
    
    return (start_num, end_num)

num_entries = 5000
entries = []

# 插入明确的测试用例
target_start = ip_to_int("192.168.1.1")
target_end = ip_to_int("192.168.1.255")
entries.append((target_start, target_end, "test_region"))

# 生成其他条目
for _ in range(num_entries - 1):
    start, end = generate_ip_range()
    region = f"region_{random.randint(1, 1000)}"
    entries.append((start, end, region))

# 按起始IP排序
entries.sort(key=lambda x: x[0])

# 构造命令行参数
command = ["./a.out", str(num_entries)]
for entry in entries:
    start_ip = int_to_ip(entry[0])  # 直接返回字符串，无需解码
    end_ip = int_to_ip(entry[1])
    command.append(f'"{start_ip} {end_ip} {entry[2]}"')

# 添加查询IP
command.append("192.168.56.100")

# 输出到文件
with open("test_input.txt", "w") as f:
    f.write(" ".join(command))