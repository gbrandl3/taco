#!/usr/bin/env python
from TacoDevice import TacoDevice, Dev_Exception
from DEVSTATES import DEVON, DEVOFF

try:
    d = TacoDevice('//localhost/test/a/1')
    if d.DevState() == DEVOFF:
        d.DevOn()
    d.DevOff()
    if d.DevState() != DEVOFF:
        print d.DevStatus()
    d.DevOn()
    if d.DevState() != DEVON:
        print d.DevStatus()
    d.DevReset()
    if d.DevState() != DEVOFF:
        print d.DevStatus()
except Dev_Exception as e:
    print e
