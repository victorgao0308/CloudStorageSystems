# CloudStorageSystems
This repository contains a program allowing a user to perform file functionalities such as creating files, retrieving files, copying files, and deleting files. These functionalities can be performed on and between a local storage and a cloud storage. There is also a built-in timer feature to let the user know how long a certain action took. 

**This program was written in C, and was built to be run on Linux, specifically Ubuntu.**

Files in this repository were obtained or based off of files located here: http://csapp.cs.cmu.edu/3e/code.html

# Running the Program
In order to run the program, create two directories and place the files from this repository into those directories. One directory will serve as your local storage, and one will serve as the cloud storage. You do not need both the `echoclient.c` file and the `echoserveri.c` file in both directories; one directory should contain `echoclient.c` and the other should contain `echoserveri.c`. Just make sure to keep track of which one is which. However, both directories should contain `MakeFile`, `csapp.c`, `csapp.h`, and `echo.c`.

**You do not necessarily need two directories for this program; you can run both the server and the client from one directory, but they will share the same files.**

**You do not need a specified cloud storage point to run this program; you can have the local and cloud storages on the same machine.**

Then, in the command prompt, navigate to the directories you created and type in `make`. This will create executable files that can then be run. You will need to do this for both directories. Then, navigate to the directory containing `echoserveri.c` and type into the command prompt `./echoserveri 8000 x`. The 8000 stands for the port number, and 8000 is the default. The `x` stands for a secret key, which can be any integer, and will be needed later.

**If you run into file permission problems, you need to run `chmod +x <FileName>` in the command prompt to give execution permission to the files.**

Then, navigate to the directory containing `echoclient.c` and type into the command prompt `./echoclient ipAddress 8000 x`. To connect the client to a server on a different machine, type in the ip address of the machine the server is on in place of `ipAddress`. To host the server and client on the same machine, type in `localhost` in place of `ipAddress`. Again, the `x` stands for the secret key. If the secret key doesn't match the one given when running `echoserveri.c`, the program will exit.

Once the client and server are connected, a hello message should be sent from the client and be received by the server. Check the command prompt on the client side to see if the message went through.

After the hello message has been sent, you are free to use the different file functionalities of the program. 

The documentation for the different functionalities are listed below. They can also be accessed by typing `commands` into the command prompt on the client side.

## Documentation
### STORE
Creates a file `<FileName>` with data `<Filedata>`.

Format: `s <Filename> <Filedata>` 

Type `c:` before `<Filename>` to store the file to the cloud. 

Add a space after `<Filename>` to create an empty file.

**`<FileName>` is assumed to have no spaces.**

**WARNING: Creating a new file with a name that matches an already existing file will override the previous file without warning.**

### RETRIEVE

Retrieves and prints the contents of `<FileName>`.

Format: `r <Filename>`

Type `c:` before `<Filename>` to retrieve data from a file stored in the cloud.

Returns an error if `<Filename>` does not exist.

### COPY

Copies a file.

Format: `c <Filename> <Filename2>`

Type `c:` before `<Filename>` to copy a file from the cloud to local storage, type `c`: before `<Filename2>` to copy a file from local storage to the cloud, and type `c`: before both `<Filename>` and `<Filename2>` to copy a file within the cloud. Do not type c: before `<Filename>` or `<Filename2>` to copy a file locally.

Returns an error if `<Filename>` does not exist.

**WARNING: Like the store function, copying to a file whose name matches with one that already exists will override the previous file without warning.**

### DELETE

Deletes a file.

Format: `d <Filename>`

Type `c:` before `<Filename>` to delete a file in the cloud.

Returns error if `<Filename>` does not exist.

### LIST

Lists the contents of the directory.

Format: `l(c)`

Type `l` to list files in local storage and `lc` to list files stored in the cloud.

### QUIT 
Type `QUIT` to exit the program.
