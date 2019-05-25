BUILD_FOLDER=./build

${BUILD_FOLDER}/rio-profile-create & 
SERVER_PID=$!
echo executing server with pid ${SERVER_PID}
sleep 3
${BUILD_FOLDER}/test
kill ${SERVER_PID}
sleep 2
