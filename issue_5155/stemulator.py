# original
import BaseHTTPServer
import time
from datetime import datetime

class SimpleRequestHandler(BaseHTTPServer.BaseHTTPRequestHandler):
  def do_GET(self):
    print "incoming request: " + self.path
    self.wfile.write('HTTP-1.0 200 Okay\r\n\r\n')
    self.wfile.write(modes(self.path))

def run(server_class=BaseHTTPServer.HTTPServer,
  handler_class=SimpleRequestHandler):
  server_address = ('', 80)
  httpd = server_class(server_address, handler_class)
  httpd.serve_forever()

def modes(argument):
  # returns value based on time of day to simulate tally data
  # /tally/[address]/status
  curr_time = datetime.now()
  seconds = int(curr_time.strftime('%S'))
  tally_state = (seconds)//10 # do some math so the state changes every 10 seconds

  if tally_state == 0 or tally_state == 3 :
    return "unselected"
  elif tally_state == 1 or tally_state == 4:
    return "selected"
  else:
    return "onair"

run ( )