import bitarray
import MySQLdb

conn = MySQLdb.connect(user='root', db='test')
cursor = conn.cursor()
cursor.execute("select bitarray(id) from ids group by 'a'")
r = cursor.fetchone()
bitarray_bytes = r[0]

bits = bitarray.bitarray()
bits.frombytes(bitarray_bytes)

print len(bits)
print bits.any()
print bits.count()

