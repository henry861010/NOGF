from fastapi import FastAPI, WebSocket, WebSocketDisconnect
from typing import List

app = FastAPI()

class ConnectionManager:
    """Manages multiple WebSocket connections."""
    def __init__(self):
        self.pair = {}

    async def connect(self, websocket: WebSocket, pair_id: str, role: str):
        await websocket.accept()
        if not pair_id in self.pair:
            self.pair[pair_id] = {}
        self.pair[pair_id][role] = websocket

        if role == "receiver":
            role2 = "sender"
        else:
            role2 = "receiver"

        if role2 not in self.pair[pair_id]:
            message = f'there is no {role2} for pair {pair_id}'
            await websocket.send_text(f'{{"message": "{message}"}}')
            
        else:
            message = f"{role2} is ready !!!"
            await websocket.send_text(f'{{"message": "{message}"}}')

            message = f"{role} is ready !!!"
            await self.pair[pair_id][role2].send_text(f'{{"message": "{message}"}}')

    async def disconnect(self, websocket: WebSocket, pair_id: str, role: str):
        self.pair[pair_id].pop(role)
        if len(self.pair[pair_id]) == 0:
            self.pair.pop(pair_id)
        else:
            if role == "receiver":
                role2 = "sender"
            else:
                role2 = "receiver"
            
            message = f"{role} leave the cnannel"
            await self.pair[pair_id][role2].send_text(f'{{"message": "{message}"}}')

    async def transmit(self, message: str, pair_id: str, role: str):
        if role == "receiver":
            role2 = "sender"
        else:
            role2 = "receiver"

        if role2 in self.pair[pair_id]:
            await self.pair[pair_id][role2].send_text(message)
        else:
            message = f"the {role2} is not in the channel"
            await self.pair[pair_id][role].send_text(f'{{"message": "{message}"}}')

manager = ConnectionManager()

@app.websocket("/ws/{client_id}/{role}")
async def websocket_endpoint(websocket: WebSocket, client_id: str, role: str):
    """Handles WebSocket connections for clients."""
    await manager.connect(websocket, client_id, role)
    try:
        while True:
            message = await websocket.receive_text()
            await manager.transmit(message, client_id, role)
    except WebSocketDisconnect:
       await  manager.disconnect(websocket, client_id, role)

if __name__ == "__main__":
    import uvicorn
    uvicorn.run(app, host="0.0.0.0", port=8000)

