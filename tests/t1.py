#!/usr/bin/env python
import ResDatabase
db = ResDatabase.ResDatabase('//localhost/test/python/test1')
print (db.res_path)
print (db.read('value'))
db.write('value', 100)
print (db.read('value'))
# db.delete('value')
print (db.read('value'))
db.write('value', 98.9)
print (db.read('value'))
print (db.read('xvalue'))
