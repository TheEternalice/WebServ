#!/bin/bash
echo "Content-Type: text/plain"
echo ""
echo "Hello from Bash CGI!"
echo "This is a simple CGI script executed by the web server."
cat > ./cgi-bin/test.sh
code ./cgi-bin/test.sh