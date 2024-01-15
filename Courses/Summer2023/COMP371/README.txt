COMP371 - Computer Graphics

OpenGL is set up using a Docker container provided at https://github.com/tiperiu/COMP371_dockerimage

To open up a OpenGL program, do the following instructions:

1. Open Docker Desktop and start up the COMP371 container
2. Head to root dir of /COMP371_dockerimage/
3. Run the following command as an example: .\windows\run.ps1 capsule1 ${pwd}\COMP371_all\Lab_capsules\capsule1\code

for lab3

1. use interactive_run.ps1 instead of run.ps1
2. mkdir build
3. cd build
4. cmake ../
5. make
6. ./capsule03 ../assets/teapot1.obj

1. .\windows\interactive_run.ps1 ${pwd}\COMP371_all\Lab_capsules\capsule3
2. cd build
3. ./capsule03 ../assets/teapot1.obj

SHORTCUT WAY OF RUNNING COMMAND:

1. Add C:\Users\{username}\OneDrive\Concordia\Courses\Summer2023\COMP371\COMP371_dockerimage\windows\ to environment variables
2. Now you can use "run.ps1 capsule1 ." and "interactive_run.ps1 ." inside of the \code\ folder

BUG: MUST DELETE /build/ FOLDER INSIDE INTERACTIVE SHELL IF YOU WANT TO RUN "run.ps1" OTHERWISE IT WILL GIVE "build_and_run.sh" ERROR
AT HOME, TRY TO USE RUN.PS1 EVEN WITH /build/ INSIDE INTERACTIVE SHELL

POTENTIAL FIX FOR INTERACTIVE SHELL:
- "/scripts/build_and_run.sh lab01" where lab01 is the name of your project

JC'S SOLUTION WORKS! NO NEED TO CREATE YOUR OWN /build/ FOLDER INSIDE LABS AND PROJECTS