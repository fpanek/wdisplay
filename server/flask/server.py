#!/usr/bin/python

from flask import Flask
from flask import Response
from flask import make_response
from flask import send_file
import io
import hashlib

app = Flask(__name__)   # Create an instance of flask called "app"

@app.route("/")         # This is our default handler, if no path is given
def index():
    return "...Webserver running..."


#@app.route("/getpicture", methods=['POST', 'GET'])
#def test():
#   with open('./devices/test.img','rb') as file:
#       return send_file(
#               io.BytesIO(file.read()),
#               mimetype='image/bmp'), 204

@app.route("/getpicture", methods=['POST', 'GET'])
def test():
    file_data = open('./devices/test.img', 'rb').read()
    file_hash = hashlib.sha256(file_data).hexdigest()
    print("checksum of file:" ,  file_hash)
    response = make_response()
    response.data = file_data
    response.headers['checksum'] = file_hash
    return response

# If we're running this script directly, this portion executes. The Flask
#  instance runs with the given parameters. Note that the "host=0.0.0.0" part
#  is essential to telling the system that we want the app visible to the
#  outside world.
if __name__ == "__main__":
    app.run(host='0.0.0.0', port=80)