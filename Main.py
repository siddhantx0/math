from flask import Flask, request
# from flask_socketio import SocketIO, emit, join_room
import time
import tiktoken

app = Flask(__name__)
while (app.secret_key):
    app.secret_key = time.time()

while True:
    print("hello world")
    print(time.time())

# while True:
#     enc = tiktoken.get_encoding("o200k_base")
#     assert enc.decode(enc.encode("hello world")) == "hello world"
#     # To get the tokeniser corresponding to a specific model in the OpenAI API:
#     enc = tiktoken.encoding_for_model("gpt-4o")


if __name__ == '__main__':
    socketio.run(app, host="0.0.0.0", port=9000)

# https://x.com/panopstor/status/1798487147915149347
# correct context
# persistence
# colloquial
# makes sense?
# ALL KV.
