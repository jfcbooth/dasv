./winscap.exe 2 48000 16 > /tmp/cava.fifo  &
stty 9600 echo brkint imaxbel -F /dev/ttyS10
cava-master/cava -p dasv.conf
