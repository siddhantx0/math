from flask import Flask, request
# from flask_socketio import SocketIO, emit, join_room
import time

app = Flask(__name__)
app.secret_key = time.time()
# socketio = SocketIO(app, cors_allowed_origins="*")

# while True:
#     print(time.time())

# if __name__ == '__main__':
# socketio.run(app, host="0.0.0.0", port=9000)

# https://x.com/panopstor/status/1798487147915149347
