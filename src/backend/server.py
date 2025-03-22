'''
    run: uvicorn server:app --host 0.0.0.0 --port 8000 --reload
    install:
        (1) pip install "uvicorn[standard]"
        (2) pip install fastapi 
        (3) pip install uvicorn 
'''

from fastapi import FastAPI, Request, WebSocket, WebSocketDisconnect, Header, Query, HTTPException, Response
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

    async def push(self, token: str, location: str):
        if token in self.pair:
            await self.pair[token].send_text(location)
            
class Message(BaseModel):
    message: str

manager = ConnectionManager()

@app.post("/sender")
async def handle_post(payload: Message, authorization: str = Header(...), ):
    ### verify the token
    #if verify_token(authorization):
    
    await manager.push(authorization, payload.message)
    return Response(status_code=204)


### http: ws://yourserver.com/ws?token=CLIENT_ABC_123
@app.websocket("/receiver")
async def gps_relay_server(websocket: WebSocket, token: str = Query(...)):
    ### verify the token
    #if verify_token(token):
    
    await manager.start_session(websocket, token)

if __name__ == "__main__":
    import uvicorn
    uvicorn.run(app, host="0.0.0.0", port=8000)

