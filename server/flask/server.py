#!/usr/bin/python

from flask import Flask
from flask import Response
from flask import send_file
#from io import BytesIO
import io

app = Flask(__name__)   # Create an instance of flask called "app"

@app.route("/")         # This is our default handler, if no path is given
def index():
    return "...Webserver running..."


@app.route("/getpicture", methods=['POST', 'GET'])
def test():
    with open('./devices/test.img','rb') as file:
        return send_file(
                io.BytesIO(file.read()),
                mimetype='image/bmp')
        #testStr = file.read()
    #response = Response(testStr ,content_type="charset=utf-8" )
    #return response





# The magic happens here. When some http request comes in with a path of
#  gpio/x/y, the Flask app will attempt to parse that as x=pin and y=level.
#  Note that there is no error handling here! Failure to properly specify the
#  route will result in a 404 error.
@app.route('/gpio/<string:id>/<string:level>')
def setPinLevel(id, level):
    GPIO.output(int(id), int(level))
    return "OK"

# If we're running this script directly, this portion executes. The Flask
#  instance runs with the given parameters. Note that the "host=0.0.0.0" part
#  is essential to telling the system that we want the app visible to the
#  outside world.
if __name__ == "__main__":
    app.run(host='0.0.0.0', port=80)