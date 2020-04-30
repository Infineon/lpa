#
# $ Copyright Cypress Semiconductor $
#

#!/usr/bin/python

"""
A simple "tcp echo server" for demonstrating TCP usage.
The server listens for TCP packets and echoes any received
packets back to the originating host.

"""

import socket
import optparse
import time
import sys


def echo_server(host, port):
    print "TCP echo server"
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    s.setsockopt(socket.IPPROTO_TCP, socket.TCP_NODELAY, 1)
    try:
        s.bind((host, port))
        s.listen(1)
    except socket.error, msg:
        print "ERROR: ", msg
        s.close()
        s = None

    if s is None:
        sys.exit(1)

    while 1:
        print "Listening on: %s:%d"%(host, port)
        data_len = 0
        index = 0
        try:
            conn, addr = s.accept()
        except KeyboardInterrupt:
            print "Closing Connection"
            s.close()
            s = None
            sys.exit(1)

        print 'Incoming connection accepted: ', addr
       
        try:
            while 1:
                conn.settimeout(30.0)            
                try:
                    data = conn.recv(4096)                    
                except:
                    # conn.send("Hello")
                    continue
                print time.strftime("%b %d %H:%M:%S ", time.localtime()), addr[0], ":", repr(data)
                print ""                        
                               
        except KeyboardInterrupt:
            print "Closing Connection"
            s.close()
            s = None
            sys.exit(1)

        conn.close()

if __name__ == '__main__':
    parser = optparse.OptionParser()
    parser.add_option("-p", "--port", dest="port", type="int", default=50007, help="Port to listen on [default: %default].")
    parser.add_option("--hostname", dest="hostname", default="", help="Hostname to listen on.")

    (options, args) = parser.parse_args()

    echo_server(options.hostname, options.port)
