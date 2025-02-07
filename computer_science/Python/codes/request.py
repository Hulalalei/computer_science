import socket

client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

# client_socket.connect(('www.baidu.com', 80))
client_socket.connect(('127.0.0.1', 8080))

request = (
    "GET / HTTP/1.1\r\n"
        "Host: co_http\r\n"
        "Connection: close\r\n"
#        "User-Agent: User-Agent: Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/128.0.0.0 Safari/537.36 Edg/128.0.0.0\r\n"
        "\r\n"
)

print(request)
client_socket.sendall(request.encode())

response = b""
while True:
    part = client_socket.recv(4096)
    if not part:
        break;
    response += part

print(response.decode())
client_socket.close()
