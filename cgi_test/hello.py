#!/usr/bin/env python3
import sys
import os

print("Content-Type: text/plain")
print()

length = int(os.environ.get("CONTENT_LENGTH", 0))
data = sys.stdin.read(length)
print("STDIN:", repr(data))
print("ENV CONTENT_LENGTH:", os.environ.get("CONTENT_LENGTH"))

import sys
sys.stdout.flush()