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

        ### sender
        if "sender" == role:
            if "receiver" not in self.pair[pair_id]:
                await websocket.send_text(f"there is no receiver for pair {pair_id}")
            else:
                await websocket.send_text(f"receiver is ready !!!")
        ### receiver
        elif "receiver" == role:
            if "sender" not in self.pair[pair_id]:
                await websocket.send_text(f"there is no sender for pair {pair_id}")
            else:
                await websocket.send_text(f"sender is ready !!!")

    def disconnect(self, websocket: WebSocket, pair_id: str, role: str):
        self.pair[pair_id][role] = None

    async def transmit(self, message: str, pair_id: str):
        if "receiver" not in self.pair[pair_id]:
            await self.pair[pair_id]["sender"].send_text(f"there is no receiver for pair {pair_id}")
        else:
            await self.pair[pair_id]["receiver"].send_text(message)
manager = ConnectionManager()

@app.websocket("/ws/{client_id}/{role}")
async def websocket_endpoint(websocket: WebSocket, client_id: str, role: str):
    """Handles WebSocket connections for clients."""
    await manager.connect(websocket, client_id, role)
    try:
        while True:
            message = await websocket.receive_text()
            await manager.transmit(message, client_id)
    except WebSocketDisconnect:
        manager.disconnect(websocket, client_id, role)

if __name__ == "__main__":
    import uvicorn
    uvicorn.run(app, host="0.0.0.0", port=8000)

