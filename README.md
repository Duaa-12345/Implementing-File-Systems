# Implementing-File-Systems
Implementing our own file systems. The first time your utility runs, it will ask user following arguments
• Block size of the virtual hard drive you will create
• Name of the file which will be treated as the new virtual hard drive
• Path of a directory which you will import as a root file system into your virtual 
hard drive

In the subsequent runs, if the virtual hard drive should already be present in the same directory, utility should not ask any of the above things.
In the virtual hard drive there will be two portions
• Volume Control block which will store the directory tree along with the FCBs. It will also have the FAT table to keep track of the data and free space
• All other blocks will be treated as Data blocks. Number of data blocks should be 1000 regardless of the size of the block.

Implementing following five commands
• ls lists the contents of a directory
• rm removes a directory or a path
• mkdir creates a directory
• cd changes the present working directory
• import imports a file from the host system to the path specified in the 
arguments.
Directory tree and FCBs can be implemented using JSON. In the JSON there will be three types of keys:
• Keys with no value: they will represent empty directories
• Keys with dictionary value: they will represent directories containing one or 
more directories or files
• Keys with integer value: they will represent a file control block, integer being 
the starting data block number.
