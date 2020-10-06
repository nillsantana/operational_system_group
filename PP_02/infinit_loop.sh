while true; do
  ping 8.8.8.8 -c 1 >/dev/null
  if [ "$?" = "0" ]; then
  #echo "";
    echo "Ping disparado em: $(date)Status: ${STTS}" >> log_server_on.txt;
    sleep 10;
  else
    #echo "Desligando o server";
    echo "Server desligado em: $(date)Status: ${STTS}" >>log_server_off.txt
#    shutdown -h now
  fi
done
