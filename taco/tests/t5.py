#!/usr/bin/env python
from TacoDevice import TacoDevice, Dev_Exception

try:
    d = TacoDevice('//localhost/test/a/1')
    ret = d.SendByteArray()
    print ret[:10] + ret[-10:]
except Dev_Exception as e:
    print e
