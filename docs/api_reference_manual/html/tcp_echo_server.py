#
# (c) 2025, Infineon Technologies AG, or an affiliate of Infineon
# Technologies AG. All rights reserved.
# This software, associated documentation and materials ("Software") is
# owned by Infineon Technologies AG or one of its affiliates ("Infineon")
# and is protected by and subject to worldwide patent protection, worldwide
# copyright laws, and international treaty provisions. Therefore, you may use
# this Software only as provided in the license agreement accompanying the
# software package from which you obtained this Software. If no license
# agreement applies, then any use, reproduction, modification, translation, or
# compilation of this Software is prohibited without the express written
# permission of Infineon.
# 
# Disclaimer: UNLESS OTHERWISE EXPRESSLY AGREED WITH INFINEON, THIS SOFTWARE
# IS PROVIDED AS-IS, WITH NO WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
# INCLUDING, BUT NOT LIMITED TO, ALL WARRANTIES OF NON-INFRINGEMENT OF
# THIRD-PARTY RIGHTS AND IMPLIED WARRANTIES SUCH AS WARRANTIES OF FITNESS FOR A
# SPECIFIC USE/PURPOSE OR MERCHANTABILITY.
# Infineon reserves the right to make changes to the Software without notice.
# You are responsible for properly designing, programming, and testing the
# functionality and safety of your intended application of the Software, as
# well as complying with any legal requirements related to its use. Infineon
# does not guarantee that the Software will be free from intrusion, data theft
# or loss, or other breaches ("Security Breaches"), and Infineon shall have
# no liability arising out of any Security Breaches. Unless otherwise
# explicitly approved by Infineon, the Software may not be used in any
# application where a failure of the Product or any consequences of the use
# thereof can reasonably be expected to result in personal injury.
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
