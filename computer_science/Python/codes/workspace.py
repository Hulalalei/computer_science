import asyncio
import time


def sample_task():
    print("one")
    x = yield
    print("x: {}".format(x))

try:
    co = sample_task()
    next(co)
    co.send(42)
except Exception as e:
    print("except: {}".format(e))
