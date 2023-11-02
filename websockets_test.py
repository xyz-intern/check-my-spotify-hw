#!/usr/bin/env python

import asyncio
from websockets import ConnectionClosedError
from websockets.server import serve

async def echo(websocket):
    try:
        async for message in websocket:
            await websocket.send(message)
            print(message)
    except ConnectionClosedError:
        print("Connection closed unexpectedly")
        
async def main():
    async with serve(echo, "192.168.0.105", 8765):
        await asyncio.Future()  # run forever

asyncio.run(main())
