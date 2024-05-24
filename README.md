##### Multithreaded_Server-Client_Mapping system API('C')
This project is completed by Anson Jacob, Niyaz Murshed and Minita Dabhi as part of coursework ECE 650 - Tools and Methods of Software Engineering at University of Waterloo. 

- Project goal: To prepare the mapping system API to determing the shortest distance between 2 locations saved by name. This project has 3 major sections - multithreaded server code, client code and trip plannar API which finds the shortest distance between 2 locations. The locations should already be stored in the system when accessing them to find the distance. 
The server code has multitheaded feature to satisfy the requests coming from multiple client PCs. Mutex has been used to provide the protection againt being accessed and updated by multiple threads.

- Communication Protocol: TCP/IP

Makefile is used to run all the codes- multiserver.c, clients.c and roadmap.c and create exacutable file runmap.
roadmap.c contains the trip plannar API which updates the graphInfo.txt file and reads the saved map locations from graphInfo.txt. roadmap.c uses roadmap.h headerfile which has all the function definitions and description.

- Learning:
  1. Implement TCP/IP protocol, craeting sockets
  2. Server-client architecture
  3. Craete API and call the API in software application
  4. Write and execute test cases for embedded project using multiple terminals to understand client-server concept
  5. Create interactive menu for UI/UX 
  
