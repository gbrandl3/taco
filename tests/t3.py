#!/usr/bin/env python
import TacoDevice

TacoDevice.dev_debug(0)
d = TacoDevice.TacoDevice('//localhost/test/a/1')
print (sorted(TacoDevice.dev_query('//localhost/test/a/1').keys()))
