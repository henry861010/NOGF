'''
    run: uvicorn server:app --host 0.0.0.0 --port 8000 --reload
    install:
        (1) pip install "uvicorn[standard]"
        (2) pip install fastapi 
        (3) pip install uvicorn 
'''

from fastapi import FastAPI, Request, WebSocket, WebSocketDisconnect, Request, Header, Query, HTTPException, Response
from pydantic import BaseModel
from fastapi.middleware.cors import CORSMiddleware

app = FastAPI()
app.add_middleware(
    CORSMiddleware,
    allow_origins=["*"],  # or specify your frontend URL e.g., ["http://localhost:3000"]
    allow_credentials=True,
    allow_methods=["*"],  # Allows POST, GET, OPTIONS, etc.
    allow_headers=["*"],  # Allows custom headers like Authorization
)

class ConnectionManager:
    def __init__(self):
        self.pair = {}

    async def start_session(self, websocket: WebSocket, token: str):
        await websocket.accept()
        self.pair[token] = websocket

        try:
            while True:
                data = await websocket.receive_text()
                #await websocket.send_text(f"Echo: {data}")
                print(data)
        except WebSocketDisconnect:
            del self.pair[token]

    async def push(self, token: str, message: str):
        if token in self.pair:
            message = message.decode('utf-8')
            await self.pair[token].send_text(message)
            
class Message(BaseModel):
    message: str

manager = ConnectionManager()

### http://127.0.0.1:8000/sender
@app.post("/sender")
async def handle_post(request: Request, authorization: str = Header(...), ):
    ### verify the token
    #if verify_token(authorization):
    message = await request.body()
    await manager.push(authorization, message)
    return Response(status_code=204)


### ws://yourserver.com/ws?token=CLIENT_ABC_123
@app.websocket("/receiver")
async def gps_relay_server(websocket: WebSocket, token: str = Query(...)):
    ### verify the token
    #if verify_token(token):
    
    await manager.start_session(websocket, token)

if __name__ == "__main__":
    import uvicorn
    uvicorn.run(app, host="0.0.0.0", port=8000)

