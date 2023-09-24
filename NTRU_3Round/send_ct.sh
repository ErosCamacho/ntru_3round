#!/bin/bash
echo "Enviando ct ... "
read -p "IP: " IP

if [[ $IP == "192.168.2.101" ]] 
then 
	name="eros"
	dir="/home/eros/Share/NTRU_3Round/data_in"
else
	name="xilinx"
	dir="/home/xilinx/NTRU_3Round/data_in"
fi

scp data_out/ct.bin $name@$IP:$dir

echo "Completado ... "