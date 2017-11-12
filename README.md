##### Multithreaded_Server-Client_Mapper#####
This project is completed by Anson Jacob, Niyaz Murshed and Minita Dabhi as part of coursework ECE 650 - Tools and Methods of Software Engineering at University of Waterloo. 

The project aims to prepare the mapping system to determing the shortest distance between 2 locations saved by name. This project has 3 major sections - multithreaded server code, client code and trip plannar API which finds the shortest distance between 2 locations. The locations should already be stored in the system when accessing them to find the distance. 

The server code has multitheaded feature to satisfy the requests coming from multiple client PCs. Mutex has been used to provide the protection againt being accessed and updated by multiple threads.

Makefile is used to run all the codes- multiserver.c, clients.c and roadmap.c and create exacutable file runmap.
roadmap.c contains the trip plannar API which updates the graphInfo.txt file and reads the saved map locations from graphInfo.txt. roadmap.c uses roadmap.h headerfile which has all the function definitions and description.
