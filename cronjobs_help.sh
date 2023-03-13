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


##Make sure that the following  lines are added to the cron job list using the following command to open and edit cron jobs:
#crontab -e

#@reboot /usr/bin/python3 -u /home/pi/Desktop/adc_test.py & > /home/pi/Desktop/cronlog.txt
#@reboot sleep 20 && sudo kill -kill $(ps aux | grep 'adc_test.py' | awk '{print $2}')
#@reboot sleep 30 && sudo gpsd -n /dev/ttyS0
#@reboot sleep 60 && nohup /usr/bin/python3 -u /home/pi/Desktop/data_collect.py >> /home/pi/Desktop/SA_log.out &
#*/15 * * * * /home/pi/Desktop/cronjobs_help.sh >> /home/pi/Desktop/cronlog.out &
