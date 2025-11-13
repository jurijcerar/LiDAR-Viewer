import struct

# Output filename
filename = "test.las"

# LAS 1.2 header fields
header = bytearray(227)  # LAS 1.2 header is typically 227 bytes

# Signature: "LASF"
header[0:4] = b'LASF'

# File source ID (2 bytes)
header[4:6] = (0).to_bytes(2, byteorder='little')

# Global encoding (2 bytes)
header[6:8] = (0).to_bytes(2, byteorder='little')

# Project ID GUID data (16 bytes)
header[8:24] = bytes(16)

# Version major and minor
header[24] = 1  # version major
header[25] = 2  # version minor

# System identifier (32 bytes)
header[26:58] = b'Python LAS Test'.ljust(32, b'\0')

# Generating software (32 bytes)
header[58:90] = b'ChatGPT Generator'.ljust(32, b'\0')

# Header size (2 bytes)
header[94:96] = (227).to_bytes(2, byteorder='little')

# Offset to point data (4 bytes)
header[96:100] = (227).to_bytes(4, byteorder='little')

# Number of variable length records (4 bytes)
header[100:104] = (0).to_bytes(4, byteorder='little')

header[104] = 1  # point data format 1
header[105:107] = (28).to_bytes(2, byteorder='little')  # length of format 1

# Number of point records (4 bytes) -> 3 points
header[107:111] = (3).to_bytes(4, byteorder='little')

# Scale factors for X,Y,Z
header[131:139] = struct.pack('<ddd', 0.01, 0.01, 0.01)
# Offsets for X,Y,Z
header[139:151] = struct.pack('<ddd', 0.0, 0.0, 0.0)

# Max/Min X,Y,Z (just placeholder)
header[151:175] = struct.pack('<dddddd', 100.0, 100.0, 100.0, 0.0, 0.0, 0.0)

# Write header and points
with open(filename, 'wb') as f:
    f.write(header)
    
    # Point format 0: X,Y,Z,intensity,bitfields,class,scan angle,scan dir, user data, point source ID
    # We'll write 3 simple points
    points = [
        (1000, 2000, 3000),  # raw coordinates, will be scaled by 0.01
        (1500, 2500, 3500),
        (2000, 3000, 4000)
    ]
    
    for x, y, z in points:
        gps_time = 0.0
        f.write(struct.pack('<iiiHBBbBHd', x, y, z, 100, 0, 0, 0, 0, 0, gps_time))

