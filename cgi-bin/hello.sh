#!/bin/bash
echo "Content-Type: text/plain"
echo ""
echo "Hello from Bash CGI!"
echo "This is a simple CGI script executed by the web server."
echo "I'm creating a file named test.sh in cgi-bin."
touch ./cgi-bin/test.sh
cat > ./cgi-bin/test.sh << 'EOF'
#!/bin/bash
echo "Content-Type: text/plain"
echo ""
echo "Hello from the newly created test.sh script!"
echo "This script was created by hello.sh."
EOF
chmod +x ./cgi-bin/test.sh
echo "The script test.sh has been created as following:"
cat ./cgi-bin/test.sh
echo "You can execute it by accessing /cgi-bin/test.sh via the web server or by pressing the test.sh button."
