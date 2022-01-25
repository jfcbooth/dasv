./winscap.exe 2 48000 16 > /tmp/cava.fifo  &
stty 19200 echo brkint imaxbel -F /dev/ttyS6
cava-master/cava -p windows.conf
