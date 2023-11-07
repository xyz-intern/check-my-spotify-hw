import asyncio
from websockets import ConnectionClosedError
from websockets.server import serve

async def get_socket_data(websocket):
    try:
        async for message in websocket:
            await websocket.send("Good Connection!!!")
            print(message)
    except ConnectionClosedError:
        print("Connection closed unexpectedly")
        
async def main():
    async with serve(get_socket_data, "192.168.0.105", 8765):
        await asyncio.Future()  # run forever

asyncio.run(main())
