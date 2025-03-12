## ReadMe

##  Aim of the project:
	This project aim a simple communication protocol between Raspberry Pi computers with the Gertboard-extension board, by using its ATMEL-processor for the communication. The protocol uses a unidirectional logical token-ring infrastructure.

## How to install this project?
	For installing, you just need to use the command line(Windows) or terminal(Mac os). Navigate to the project folder and just follow the below mentioned steps.

## Commands to run this project:
	- make all : This command will create the build files and flash the executable to the micro-controller.

	- make doc : This command will generate project documentation. Here after executing the command 'make doc' is extratcs two versions of documents one is html 
		     and the other one is latex. This document contains details about structure of the project, APIs, macros etc. To run this command, you should
		     have Doxygen software running on the computer.

## other commands:
      - make clean : This command is to clear the files which are built while flasing and clear doxygen documentation files.
 
## To check running output of the project:
	Use the command line tool "MINICOM" to view the receiving data. Here you can see the receiving data on the MINICOM console. 

## Documentation:
	To view the document created by Doxygen, go the path \DoxygenDocuments\html and open index.html

	If folder or files not found, follow the below steps:  

	Step1: Navigate to the project folder

	Step2: Run the command -> make doc