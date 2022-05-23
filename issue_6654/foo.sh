while true
do
    curl -s --connect-timeout 5 http://192.168.31.65/index.html > /dev/null
    if [ $? == 0 ]; then
        echo OK
    else
        echo ERROR
    fi
    sleep 3
done