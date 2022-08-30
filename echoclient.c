/*
 * echoclient.c - An echo client
 */
/* $begin echoclientmain */
#include "csapp.h"
#include "time.h"

/* Checks if a filename is a cloud file */
int isCloudFile(char Filename[]){
	char temp[10];
	strncpy(temp, Filename, 2); 
	if (strstr(temp, "c:") != NULL){
		return 1;
	} else {
		return 0;
	}
}

/* Attempts to copy contents of Filename into Filename2 */
int copyFile(char Filename[], char Filename2[]){
	FILE *file, *file2;
	char buffer[1];
	
	/* Remove \n from the end of the arguments */
	Filename[strcspn(Filename, "\n")] = 0;
	Filename2[strcspn(Filename2, "\n")] = 0;
	
	if ((file = fopen(Filename, "r")) == NULL){
		printf("File to be copied from does not exist.\n");
		return 0;
	} else {
		file2 = fopen(Filename2, "w");
		while(fread(buffer, 1, 1, file) > 0){
    			fwrite(buffer, 1, 1, file2);	
    		}

    		fclose(file);
    		fclose(file2);
    		return 1;
	}
}
/* These two functions calculate the time the processor takes to run the program */
clock_t startTimer(){
	clock_t begin = clock();
	return begin;
}

double endTimer(clock_t startTime){
	clock_t end = clock();
	double time = (double)(end - startTime)/CLOCKS_PER_SEC;
	printf("Time (processor): %fs\n", time);
	return time;
}
/* These two functions calculate the real time it takes to run the program */
time_t startClock(){
	time_t begin = time(NULL);
	return begin;
}

void endClock(time_t startTime){
	time_t end = time(NULL);
	double time = (double)(end - startTime);
	printf("Time (real): %fs\n", time);
}

int main(int argc, char **argv) 
{
    	int clientfd;
    	char *host, *port, buf[MAXLINE], buffer[MAXLINE]; 
    	rio_t rio;

    	if (argc != 4) {
		fprintf(stderr, "usage: %s <host> <port>\n", argv[0]);
		exit(0);
    	}
    	host = argv[1];
    	port = argv[2];
    
    	int secretkey = atoi(argv[3]);
    	int numSpaces = 0;
    	int locSpace1 = 0;
    	int locSpace2 = 0;
    	int locSpace3 = 0;
    	char Filename[40];
    	char Filename2[40];
    	char tempStr[40];
    	char Filedata[1000];
    	int numFiles = 0;
    	int byte = 0;
    	char buff [2];
    	double time = 0.0;
    	double ptime = 0.0;
    	int isCloud = 0;
    	int length = 0;
    	clock_t begin;
    	time_t begin2;
    
    	clientfd = Open_clientfd(host, port);
    	Rio_readinitb(&rio, clientfd);
    	FILE *file;
    
    	struct helloMessage{
    	int seqNo;
    	unsigned int type;
   	int SecretKey;
    	} h1;
    
    	struct helloMessage2{
    	int seqNo;
    	int status;
    	} h2;
    
   
    	struct message {
    	unsigned int type;
    	int seqNo;
    	unsigned int numbytes;
    	char filename[40];
    	char filename2[40];
    	char filedata [1000];
    	} m1;
    
    	struct message2 {
    	int seqNo;
    	int status;
    	unsigned int numbytes;
    	char filename[40];
    	char filename2[40];
   	char filedata [1000];
    	} m2;
    
    	struct listMessage{
    	unsigned int type;
    	int seqNo;
    	} l1;
    
    	struct listMessage2{
    	int seqNo;
    	int status;
    	int numFiles;
    	} l2;
    
   
	h1.seqNo = 1;
	m1.seqNo = 1;
	h1.SecretKey = secretkey;
		    	
    /* HELLO message */
	h1.type = 1;
	Rio_writen(clientfd, &h1, 12);
	printf("HELLO message sent!\n");
	m1.seqNo += 1;
	
	/* Receive */
	Rio_readn(clientfd, &h2, 8);
	printf("Seqno: %d\n", h2.seqNo);
	printf("Status: %d\n", h2.status);
	
	if (h2.status == 0){
		printf(">\n");
	} else {
		printf("Secret Keys Do Not Match\n");
		Close(clientfd);
		exit(0);
	}
		
    while (Fgets(buf, MAXLINE, stdin) != NULL) {
    
    	/* Remove \n from end of input */
    	buf[strcspn(buf, "\n")] = 0; 
    	
    	begin = startTimer();
    	begin2 = startClock();
    	
    	
    	/* In case nothing gets typed in the console and enter is pressed */
    	if (strlen(buf) == 0){
    		printf("INVALID COMMAND\n");
    		continue;
    	}
    	
    	/* List of commands */
    	if (strstr(buf, "commands") != NULL){
    		printf("COMMAND LIST:\n\n ***STORE***\n Creates a file.\n Format: s [Filename] [Filedata]\n Type c: before [Filename] to store file to the cloud. Add a space after [Filename] to create an empty file.\n\n ***RETRIEVE***\n Retrieves and prints the contents of a file. \n Format: r [Filename]\n Type c: before [Filename] to retrieve data from a file stored in the cloud.\n Returns an error if [Filename] does not exist.\n\n ***COPY***\n Copies a file.\n Format: c [Filename] [Filename2]\n Type c: before [Filename] to copy a file from the cloud to local storage, type c: before [Filename2] to copy a file from local storage to the cloud, and type c: before both [Filename] and [Filename2] to copy a file within the cloud. Do not type c: before [Filename] or [Filename2] to copy a file locally.\n Returns error if [Filename] does not exist.\n\n ***DELETE***\n Deletes a file.\n Format: d [Filename]\n Type c: before [Filename] to delete a file in the cloud.\n Returns error if [Filename] does not exist.\n\n***LIST***\n Lists the contents of the directory.\n Format:l(c)\n Type l to list files in local storage and lc to list files stored in the cloud.\n\n***QUIT***\n Type QUIT to exit the program.\n\n");
    		continue;
    	}
    	if (strstr(buf, "QUIT") != NULL){
    		m1.type = 7;
    		Rio_writen(clientfd, &m1, 4);
    		exit(0);
    	}
    	/* Check formatting by determining number of spaces in buf */
    	for (int i = 0; i < strlen(buf); i++){
    		if (isspace(buf[i])){
    			numSpaces += 1;
    			/* Location of first space, helps determine Filename later on */
    			if(numSpaces == 1){
    				locSpace1 = i; 
    			}
    			/* Location of second space, helps determine Filename later on */
    			if(numSpaces == 2){
    				locSpace2 = i; 
    			}
    			/* Location of third space, helps determine Filename later on */
    			if(numSpaces == 3){
    				locSpace3 = i; 
    			}
    		}
    	}
    	
    	/* These help prevent buffer overflow if command is invalid */
    	if (locSpace2 == 0){
    		locSpace2 = strlen(buf);
    	}
    	if (locSpace3 == 0){
    		locSpace3 = strlen(buf);
    	}
    	
    	/* Get Filename & Filedata */  
  	memcpy(Filename, &buf[locSpace1 + 1], locSpace2 - locSpace1 - 1);
    	memcpy(Filedata, &buf[locSpace2 + 1], strlen(buf) - 1);
    	m1.numbytes = strlen(Filedata);  
    	 	
    	/* STORE message */
    	/* Format: s Filename Filedata */
    	/* Assume first word is s, second is Filename, and the rest is all Filedata */	
    	strncpy(tempStr, buf, 1);
    	if (strstr(tempStr, "s") != NULL && locSpace1 == 1){
    		if (numSpaces < 2){
    			printf("Invalid STORE message. Use format 's Filename Filedata'. If you would like to create an empty file, add a space after Filename.\n");
    		} else {
    			printf("***STORE MESSAGE***\n");
    			if (isCloudFile(Filename)) {
    			isCloud = 1;
    				memmove(Filename, &Filename[2], strlen(Filename));
    				m1.type = 2;
    				Rio_writen(clientfd, &m1, 4);
    				
    				memcpy(m1.filename, Filename, 40);
    				memcpy(m1.filedata, Filedata, 1000);
    				Rio_writen(clientfd, &m1, 1092);
				printf("STORE request sent!\n");
				m1.seqNo += 1;
				
				/* Receive STORE message */
				Rio_readn(clientfd, &m2, 8);
				printf("Seqno: %d\n", m2.seqNo);
				printf("Status: %d\n", m2.status);
  		
    			} else {
    				file = fopen(Filename, "w");
    				fwrite(Filedata, strlen(Filedata), 1, file);
    				fclose(file);
    			}
    		ptime = endTimer(begin);
		endClock(begin2);
    		printf("Filename:***%s***\n", Filename);
    		printf("Filedata:***%s***\n", Filedata);
    		}
    	
    	} 
    	
    	/* RETRIEVE message */
    	/* Format: r Filename */
    	/* Only one space is allowed for format; Filename is assumed to have no spaces */
    	else if(strstr(tempStr, "r") != NULL && locSpace1 == 1){
    		printf("***RETRIEVE MESSAGE***\n");
    		if (numSpaces != 1){
    			printf("Invalid RETRIEVE message. Use format 'r Filename'.\n");
    		} else {
    			if (isCloudFile(Filename)) {
    				isCloud = 1;
    				memmove(Filename, &Filename[2], strlen(Filename));
    				m1.type = 3;
    				Rio_writen(clientfd, &m1, 4);
    				
    				memcpy(m1.filename, Filename, 40);
    				Rio_writen(clientfd, &m1, 52);
    				printf("RETRIEVE request sent!\n");
    				m1.seqNo += 1;
    				
    				/* Receive RETRIEVE message */
    				Rio_readn(clientfd, &m2, 12);
    				printf("Length:%d\n", m2.numbytes);
    				while (byte < m2.numbytes){
    					Rio_readn(clientfd, buff, 1);
    					printf("%s", buff);
    					
    					byte += 1;
    					
    				}
    				printf("\n");
    				
				Rio_readn(clientfd, &m2, 8);
				printf("Seqno: %d\n", m2.seqNo);
				printf("Status: %d\n", m2.status);
				
				
    			} else {
    				if ((file = fopen(Filename, "r")) == NULL){
    					printf("File does not exist!\n");
    				} else {
    					printf("File does exist!\n");
    					printf("File Contents:\n");
    					while (fread(buffer, 1, 1, file) > 0){
    						printf("%s", buffer);
    					}
    					printf("\n");
    					}
    		
    				}
    				
    				ptime = endTimer(begin);
				endClock(begin2);
    			}
    		}
    		
    	/* COPY message */
    	/* Format: c Filename1 Filename2 */
    	/* Only two spaces are allowed for format; Filename and Filename2 are assumed to have no spaces */
    	else if(strstr(tempStr, "c") != NULL && locSpace1 == 1){
    		if (numSpaces != 2){
    			printf("Invalid COPY message. Use format 'c Filename1 Filename2'.\n");
    		} else {
    		printf("***COPY MESSAGE***\n");
    		m1.type = 4;
    		/* Get Filename2 */
    		memcpy(Filename2, &buf[locSpace2 + 1], strlen(buf) - 1);
    		
    		/* CASE 1: Both files are cloud files */
    		/* Send both files over to server and copy file there */
    		if (isCloudFile(Filename) && isCloudFile(Filename2)){
    			isCloud = 1;
    			Rio_writen(clientfd, &m1, 4);
    			
    			memcpy(m1.filename, Filename, 40);
    			memcpy(m1.filename2, Filename2, 40);
    			Rio_writen(clientfd, &m1, 92);
    			printf("CASE 1, COPY message sent!\n");
    			m1.seqNo += 1;
    			
    			/* Receive */
    			Rio_readn(clientfd, &m2, 8);
			printf("Seqno: %d\n", m2.seqNo);
			printf("Status: %d\n", m2.status);
    		}
    		
    		/* CASE 2: Filename is a cloud file, Filename2 is not */
    		/* Read information from Filename on the server and send that info back */
    		else if (isCloudFile(Filename)){
    			isCloud = 1;
    			Rio_writen(clientfd, &m1, 4);
    			
    			memcpy(m1.filename, Filename, 40);
    			Rio_writen(clientfd, &m1, 92);
    			printf("CASE 2, COPY message sent!\n");
    			m1.seqNo += 1;
    			
    			/* Receive */
    			Rio_readn(clientfd, &m2, 12);
    			printf("Length:%d\n", m2.numbytes);
    			
    			if ((file = fopen(Filename2, "w")) != NULL){
    				while (byte < m2.numbytes){
    					Rio_readn(clientfd, buff, 1);
    					fwrite(buff, 1, 1, file);
    					byte += 1;	
    					}
    			
    				Rio_readn(clientfd, &m2, 8);
				printf("Seqno: %d\n", m2.seqNo);
				printf("Status: %d\n", m2.status);
    				fclose(file);
    			}
    		}
    		
    		/* CASE 3: Filename2 is a cloud file, Filename is not */
    		/* Read information from Filename on the client and send that info to the server */
    		else if (isCloudFile(Filename2)){
    			isCloud = 1;
    			if ((file = fopen(Filename, "r")) == NULL){
    			printf("File does not exist!\n");
    			} else {
    				printf("File does exist!\n");
    				
    				Rio_writen(clientfd, &m1, 4);
    				
    				memcpy(m1.filename2, Filename2, 40);
    				Rio_writen(clientfd, &m1, 92);
    				printf("CASE 3, COPY message sent!\n");	
    				while(fread(buff, 1, 1, file) > 0){
    					length += 1;
    				}
    				fclose(file);
    				m2.numbytes = length;	
    				Rio_writen(clientfd, &m2, 12);
    			
    				file = fopen(Filename, "r");
    				
    				while(fread(buff, 1, 1, file) > 0){
    					Rio_writen(clientfd, buff, 1);
    				}
    				
    				fclose(file);

    				m1.seqNo += 1;
    				
    				/* Receive */
    				Rio_readn(clientfd, &m2, 8);
    				printf("Seqno: %d\n", m2.seqNo);
				printf("Status: %d\n", m2.status);
    				}
		}
		
		/* CASE 4: Both are not cloud files */
		else {
			printf("CASE 4\n");
			if (copyFile(Filename, Filename2)){
				printf("File copied successfully\n");
			} else {
				printf("ERROR\n");
			}
		}
			
	}
		ptime = endTimer(begin);
		endClock(begin2);
		printf("***FILENAME:%s***\n", Filename);
		printf("***FILENAME2:%s***\n", Filename2);
		
	
	/* DELETE message */
	/*Format: d Filename */	
    	} else if (strstr(tempStr, "d") != NULL && locSpace1 == 1){
    		printf("***DELETE MESSAGE***\n");
    		if (numSpaces != 1){
    			printf("Invalid DELETE message. Use format 'd Filename'.\n");
    			}
    		else {
    			if (isCloudFile(Filename)) {
    				printf("CLOUD FILE\n");
    				isCloud = 1;
    				m1.type = 5;
    				Rio_writen(clientfd, &m1, 4);
    				
    				memmove(Filename, &Filename[2], strlen(Filename));
    				memcpy(m1.filename, Filename, 40);
    				Rio_writen(clientfd, &m1, 52);
    				printf("DELETE message sent!\n");
    				m1.seqNo += 1;
    				
    				/* Receive */
    				Rio_readn(clientfd, &m2, 8);
				printf("Seqno: %d\n", m2.seqNo);
				printf("Status: %d\n", m2.status);
 
    			} else {
    				printf("LOCAL FILE\n");
    				if (remove(Filename) == 0) {
    					printf("File deleted successfully.\n");
    					} else {
    						printf("Error in deleting file.\n");
    						}
    			}
    			ptime = endTimer(begin);
			endClock(begin2);
    		}
    	
    /* LIST message */
	/*Format: l(c)*/		
    	} else if (strstr(tempStr, "l") != NULL){
    		printf("LIST message\n");
    		if (strlen(buf) == 1){
    			printf("LOCAL files\n");
  
    			DIR *d;
    			struct dirent *dir;
    			d = opendir(".");
    			if (d) {
    				while ((dir = readdir(d)) != NULL) {
    					if (dir->d_type == DT_REG){
    						printf("%s\n", dir->d_name);
    						numFiles += 1;
    					}
    				}
    				printf("%d total files\n", numFiles);
    				closedir(d);
    			}
    		/* Cloud request */
    		} else if (strstr(buf, "lc") != NULL && strlen(buf) == 2){
    			isCloud = 1;
    			m1.type = 6;
    			Rio_writen(clientfd, &m1, 4);
    			
    			l1.seqNo = m1.seqNo;
    			l1.type = 6;
    			Rio_writen(clientfd, &l1, 8);
    			m1.seqNo += 1;
    			printf("LIST message sent!\n");
    			
    			/*Recieve*/
    			Rio_readn(clientfd, &l2, 12);
    			printf("Status: %d\n", l2.status);
    			printf("NumFiles: %d\n", l2.numFiles);
    			char fileNames[(l2.numFiles * 40)];
    			
    			Rio_readn(clientfd, &fileNames, (l2.numFiles * 40));
    			printf("Cloud Files:\n%s\n", fileNames);
    			
    			} 
    			
    			ptime = endTimer(begin);
			endClock(begin2);
    			
    	}
    
    	  	
    	/* Input does not match a command */
    	else {
    		printf("INVALID COMMAND. Type 'commands' to see a valid list of commands.\n");
    	}
    	
    	if (isCloud){
    		Rio_readn(clientfd, &time, 8);
    		printf("%fs\n", time);
    		printf("Total:%fs\n", (time + ptime));
    	}
   	
   	printf("\n");
	/* Reset variables */
	numSpaces = 0;
	locSpace1 = 0;
	locSpace2 = 0;
	locSpace3 = 0;
	isCloud = 0;
	numFiles = 0;
	begin = 0.0;
	byte = 0;
	time = 0.0;
	ptime = 0.0;
	length = 0;
	
	memset(Filename, 0, strlen(Filename));
	memset(Filename2, 0, strlen(Filename2));
	memset(m1.filename, 0, strlen(m1.filename));
	memset(m1.filename2, 0, strlen(m1.filename2));
	memset(m1.filedata, 0, strlen(m1.filedata));
    memset(tempStr, 0, strlen(tempStr));
    memset(Filedata, 0, strlen(Filedata));
    memset(buff, 0, strlen(buff));
    file = NULL;	
	}
    	Close(clientfd); //line:netp:echoclient:close
    	exit(0);
}

/* $end echoclientmain */

