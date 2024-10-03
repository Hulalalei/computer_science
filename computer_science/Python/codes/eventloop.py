from queue import deque
import sys
import signal
import select
import socket


def signal_handler(signum, frame):
    print("catch a signal: {}".format(signum))
    sys.exit()

class Future:


    def __init__(self, loop):
        self.loop = loop
        self.done = False
        self.co = None

    def set_coroutine(self, co):
        self.co = co

    def set_done(self): 
        self.done = True

    # operator await()
    def __await__(self):
        if not self.done:
            yield self
        return


# socket封装协程
class SocketWrapper:

    def __init__(self, sock, loop):
        sock.setblocking(False)
        self.sock = sock
        self.loop = loop


    def fileno(self):
        return self.sock.fileno()


    # 注册epoll
    def create_future_for_events(self, events):
        future = self.loop.create_future()

        def handler():
            # 可等待对象完成
            future.set_done()
            self.loop.unregister_handler(self.fileno())
            # 继续调度
            if future.co:
                self.loop.add_coroutine(future.co)
        
        self.loop.register_handler(self.fileno(), events, handler)
        return future
 

    async def accept(self):
        while True:
            try:
                sock, addr = self.sock.accept()
                return SocketWrapper(sock = sock, loop = self.loop), addr
            except BlockingIOError:
                future = self.create_future_for_events(select.EPOLLIN)
                await future

    async def recv(self, backlog):
        while True:
            try:
                return self.sock.recv(backlog)
            except BlockingIOError:
                future = self.create_future_for_events(select.EPOLLIN)
                await future

    async def send(self, data):
        while True:
            try:
                return self.sock.send(data)
            except BlockingIOError:
                future = self.create_future_for_events(select.EPOLLOUT)
                await future



class EventLoop:
    current = None
    runnables = deque()
    epoll = select.epoll()
    handlers = {}

    # 单例模式
    @classmethod
    def instance(self):
        if not EventLoop.current:
            EventLoop.current = EventLoop()
        return EventLoop.current


    def create_future(self):
        return Future(loop = self)


    def register_handler(self, fileno, events, handler):
        self.handlers[fileno] = handler
        self.epoll.register(fileno, events)


    def unregister_handler(self, fileno):
        self.epoll.unregister(fileno)
        self.handlers.pop(fileno)


    def create_listen_socket(self, ip = "127.0.0.1", port = 8089):
        sock = socket.socket()
        sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        sock.bind( (ip, port) )
        sock.listen()
        return SocketWrapper(sock = sock, loop = self)


    def run_coroutine(self, co):
        try:
            # resume
            future = co.send(None)
            # 55 lines对应
            future.set_coroutine(co)
        except Exception as e:
            print(e)
            print("coroutine {} stopped".format(co.__name__))


    def run_forever(self):
        signal.signal(signal.SIGINT, signal_handler)
        while True:
            # 循环等待epoll事件
            print("waiting")
            while self.runnables:
                print("poping")
                self.run_coroutine(co = self.runnables.popleft())

            events = self.epoll.poll(1)
            for fileno, event in events:
                # 回调，启动协程
                handler = self.handlers.get(fileno)
                handler()


    def add_coroutine(self, co):
        self.runnables.append(co)



class TCPServer:


    def __init__(self, loop):
        self.loop = loop
        # SocketWrapper listen_sock
        self.listen_sock = self.loop.create_listen_socket()
        self.loop.add_coroutine(self.serve_forever())


    async def handle_client(self, sock):
        while True:
            data = await sock.recv(1024)
            if not data:
                print("client close connect")
                break
            await sock.send(data.upper())


    async def serve_forever(self):
        while True:
            sock, addr = await self.listen_sock.accept()
            print("client connect addr: {}".format(addr))
            self.loop.add_coroutine(self.handle_client(sock))








if __name__ == "__main__":
    loop = EventLoop.instance()
    server = TCPServer(loop)
    loop.run_forever()

