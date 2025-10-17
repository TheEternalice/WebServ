#!/bin/bash
echo "Content-Type: text/plain"
echo ""
echo "Hello from Bash CGI!"
echo "This is a simple CGI script executed by the web server."
echo "I'm creating a file named test.sh in cgi_test."
touch ./cgi_test/get/test.sh
cat > ./cgi_test/get/test.sh << 'EOF'
#!/bin/bash
echo "Content-Type: text/plain"
echo ""
echo "Hello from the newly created test.sh script!"
echo "This script was created by hello.sh."
EOF
chmod +x ./cgi_test/get/test.sh
echo "The script test.sh has been created as following:"
cat ./cgi_test/get/test.sh
echo "You can execute it by accessing /cgi_test/get/test.sh via the web server."
