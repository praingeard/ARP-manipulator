CC = gcc
LIBFLAG = -lm
DEP = logarp/logarp.o

logarp/logarp.o: logarp/logarp.c logarp/logarp.h
	$(CC) logarp/logarp.c -c -o logarp/logarp.o

cmd_shell/cmd_shell: cmd_shell/cmd_shell.c $(DEP)
	$(CC) $(DEP) cmd_shell/cmd_shell.c -o cmd_shell/cmd_shell

display/display: display/display.c $(DEP)
	$(CC) $(DEP) display/display.c -o display/display $(LIBFLAG)

watchdog/watchdog: watchdog/watchdog.c $(DEP)
	$(CC) $(DEP) watchdog/watchdog.c -o watchdog/watchdog
	
motor1/motor1: motor1/motor1.c $(DEP)
	$(CC) $(DEP) motor1/motor1.c -o motor1/motor1
	
motor2/motor2: motor2/motor2.c $(DEP)
	$(CC) $(DEP) motor2/motor2.c -o motor2/motor2	
	
master/master: master/master.c $(DEP)
	$(CC) $(DEP) master/master.c -o master/master
	
clean:
	rm -f master/master cmd_shell/cmd_shell display/display watchdog/watchdog motor1/motor1 motor2/motor2 logarp/logarp.o
