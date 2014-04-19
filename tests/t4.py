#!/usr/bin/env python
from TacoDevice import TacoDevice, Dev_Exception

try:
    d2 = TacoDevice('//localhost/test/test/test')
except Dev_Exception as e:
    print e
