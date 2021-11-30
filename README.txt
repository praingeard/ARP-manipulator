[GENERAL DESCRIPTION]

authors:
  - Paul Raingeard de la Bletiere
  - Elea Papin
  If there is any problem while running this code, we can be contacted with the email adresses p.raingeard.37@gmail.com and elea.papin@eleves.ec-nantes.fr

usage: 

  prerequisites : 
    - konsole
    - gcc compiler 

  scripts :

    - install.sh : unzip the archive and compile the executables.
    - run.sh : run the executables once they have been installed.
    - cleanup.sh : remove the executables. You need to run install.sh again to recompile. Also clean logs.
    - uninstall.sh : delete the added files and the unziped archive.
    - help.sh : shows this document and the description of the other processes if they have been installed

  processes : 

    - master : run all the other processes and manage signals.
    - cmd_shell : get and send commands to motors.
    - motor1 and motor2 : control an axis by simulating a motor output. Simulate the manipulator's position.
    - display : show the output on screen and control Reset and Pause.
    - logarp : manage logs.
    - watchdog : reset the system if nothing happened for a long time.
    
  description:
    
    Implements a 2D manipulator which can be controlled by keyboard input. Shows the output on konsole. Several actions can be made such as a global reset, an emergency stop or a pause.
    
  commands:

    On the cmd_shell Konsole : 

      Q : Go left for the x axis.
      S : Stop the x axis. 
      D : Go right for the x axis.
      8 : Go up for the z axis.
      5 : Stop the z axis. 
      2 : Go down for the z axis.
      CTRL^C or CTRL^Z : Quit the session.

    On the display Konsole:

      P : pause the processes. A resume signal can resume the processes on their previous state.
      S : Emergency stop. A resume signal can resume the processes but thay will be stopped until a command is received.
      R : Resume the processes.
      CTRL^C : Reset. The manipulator will go back to its beginning value (0, 0). 
      CTRL^Z : Quits the session.

    On the watchdog shell:

      CTRL^C or CTRL^Z : Quit the session.

