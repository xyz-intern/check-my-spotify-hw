#!/usr/bin/env python

import asyncio
from websockets.sync.client import connect

def hello():
    with connect("ws://192.168.0.133:8765") as websocket:
        websocket.send(
            '''
            {
                "event": "events",
                "data": "test"
            }
            '''
        )
        message = websocket.recv()
        print(f"Received: {message}")

hello()

#!/usr/bin/env python

# import asyncio
# from websockets.sync.client import connect

# def hello():
#     with connect("ws://192.168.0.105:8765") as websocket:
#         websocket.send(
#             '''
# testestestestest
#             '''
#         )
#         message = websocket.recv()
#         print(f"Received: {message}")

# hello()