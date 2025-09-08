#!/bin/bash
echo "Content-Type: text/plain"
echo ""
echo "Hello from Bash CGI!"
echo "This is a simple CGI script executed by the web server."
echo "I'm creating a file named test.sh in cgi_test."
touch ./cgi_test/test.sh
cat > ./cgi_test/test.sh << 'EOF'
#!/bin/bash
echo "Content-Type: text/plain"
echo ""
echo "Hello from the newly created test.sh script!"
echo "This script was created by hello.sh."
EOF
chmod +x ./test.sh
echo "Created test.sh script:"
cat ./test.sh
echo "You can execute it by accessing /cgi_testt/test.sh via the web server."
