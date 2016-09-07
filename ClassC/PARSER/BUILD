if [ ! -f SwitchTest ]
then 
	if  cc SwitchTest.c -o SwitchTest
	then
		echo \\c
	else
		echo "Error trying to compile SwitchTest.c"
		exit 1
	fi
fi
if [ `SwitchTest` = "5" ]
then 
	echo "Compiling for standard (right to left) argument evaluation"
	SWITCH=
elif [ `SwitchTest` = "22" ]
then
	echo "Compiling for Reverse (left to right) argument evaluation"
	SWITCH=-DREVERSE
else
	echo "Error: SwitchTest returned unexpected result"
	exit 1
fi
export SWITCH
make -e $*

