#!/usr/bin/bash


do_the_action ()
{
  /usr/bin/python3 -u /home/pi/Desktop/adc_test.py > /home/pi/Desktop/cronlog.txt &
  sleep 20 && sudo kill -kill $(ps aux | grep 'adc_test.py' | awk '{print $2}')
  sleep 30 && sudo gpsd -n /dev/ttyS0
  sleep 60 && nohup /usr/bin/python3 -u /home/pi/Desktop/data_collect.py > /home/pi/Desktop/SA_log.out &
  date +"Restarted: Action started at %H:%M:%S"
}

previous_instance_active () {
  pgrep -a python3 | grep -v "^$$ " | grep 'data_collect.py' 
}

if previous_instance_active
then 
  date +'Previous instance is still active at %H:%M:%S, Letting it be... '
else 
  do_the_action
fi

#PIDFILE="/tmp/action1.pid"

#create_pidfile ()
#{
#  echo $$ > "$PIDFILE"
#  date + "PIDFILE CREATED at %H:%M:%S"
#}

#remove_pidfile ()
#{
#  [ -f "$PIDFILE" ] && rm "$PIDFILE"
#  date + "PIDFILE REMOVED AT %H:%M:%S"
#}


#previous_instance_active ()
#{
#  local prevpid
#  if [ -f "$PIDFILE" ]
#    "PID FILE IS NOT HERE"
#  then
#    prevpid=$(cat "$PIDFILE")
#    kill -0 $prevpid
#    date + "DATA STILL COLLECTING AT %H:%M:%S"
#  else
#    false
#  fi
#}


#if previous_instance_active
#then
#  echo date + 'PID: $$ Previous instance is still active at %H:%M:%S'
#else
#  trap remove_pidfile EXIT
#  create_pidfile
#  /usr/bin/python3 -u /home/pi/Desktop/adc_test.py > /home/pi/Desktop/cronlog.txt &
#  sleep 20 && sudo kill -kill $(ps aux | grep 'adc_test.py' | awk '{print $2}')
#  sleep 30 && sudo gpsd -n /dev/ttyS0
#  sleep 60 && nohup /usr/bin/python3 -u /home/pi/Desktop/data_collect.py > /home/pi/Desktop/SA_log.out &
#  date +"Restarted: PID: $$ Action started at %H:%M:%S"
#fi

