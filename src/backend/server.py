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
            await websocket.send_text(f"there is no {role2} for pair {pair_id}")
        else:
            await websocket.send_text(f"{role2} is ready !!!")
            await self.pair[pair_id][role2].send_text(f"{role} is ready !!!")

    async def disconnect(self, websocket: WebSocket, pair_id: str, role: str):
        self.pair[pair_id].pop(role)
        if len(self.pair[pair_id]) == 0:
            self.pair.pop(pair_id)
        else:
            if role == "receiver":
                role2 = "sender"
            else:
                role2 = "receiver"
            await self.pair[pair_id][role2].send_text(f"{role} leave the cnannel")

    async def transmit(self, message: str, pair_id: str, role: str):
        if role == "receiver":
            role2 = "sender"
        else:
            role2 = "receiver"

        if role2 in self.pair[pair_id]:
            await self.pair[pair_id][role2].send_text(message)
        else:
            await self.pair[pair_id][role].send_text(f"the {role2} is not in the channel")

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

