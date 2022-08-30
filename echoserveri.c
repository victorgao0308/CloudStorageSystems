/* 
 * echoserveri.c - An iterative echo server 
 */ 
/* $begin echoserverimain */
#include "csapp.h"

void echo(int connfd);

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
	int listenfd, connfd;
    	socklen_t clientlen;
    	struct sockaddr_storage clientaddr;  /* Enough space for any address */  //line:netp:echoserveri:sockaddrstorage
    	char client_hostname[MAXLINE], client_port[MAXLINE];
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
	
    	if (argc != 3) {
		fprintf(stderr, "usage: %s <port>\n", argv[0]);
		exit(0);
	}

    	listenfd = Open_listenfd(argv[1]);
    	int secretkey = atoi(argv[2]);
    	char tempData [1000];
    	int i = 0;
    	int x = 0;
    	int length = 0;
    	double time = 0.0;
    	char buff[2];
    	int byte = 0;
    	clock_t begin;
    	time_t begin2;
    
	clientlen = sizeof(struct sockaddr_storage); 
	connfd = Accept(listenfd, (SA *)&clientaddr, &clientlen);
	Getnameinfo((SA *) &clientaddr, clientlen, client_hostname, MAXLINE, client_port, MAXLINE, 0);        
	printf("Connected to (%s, %s)\n", client_hostname, client_port);
        
    	Rio_readn(connfd, &h1, 12);    
    	/* HELLO message */
    	if (h1.type == 1){
      		printf("HELLO message received!\n");
        if(secretkey != h1.SecretKey){
        	h2.status = -1;
        } else {
        	h2.status = 0;
        	h2.seqNo = h1.seqNo;
        }
        Rio_writen(connfd, &h2, 8);
        printf("SeqNo: %d\n", h2.seqNo);
        printf("SecretKey: %d\n", h1.SecretKey);
        printf("Status: %d\n", h2.status);
        printf("\n");
        }
        /* Keys do not match */
    	if(secretkey != h1.SecretKey){
    		exit(0);
   	}
     
    while (1) {
    	/* Only read the first 4 bytes, to determine the type of request */
	Rio_readn(connfd, &m1, 4);
	
	begin = startTimer();
    	begin2 = startClock();
	
	/* STORE message */
	if (m1.type == 2){
		Rio_readn(connfd, &m1, 1092);
        	printf("STORE message received!\n");
        	file = fopen(m1.filename, "w");
    		if (fwrite(m1.filedata, strlen(m1.filedata), 1, file)){
    			m2.status = 0;
    		} else {
    			m2.status = -1;
    		}
    		fclose(file);
    		/* If file is empty */
    		if (strlen(m1.filedata) == 0){
    			m2.status = 0;
    		}
    		
    		m2.seqNo = m1.seqNo;
    		memcpy(m2.filedata, m1.filedata, strlen(m1.filedata));

    		Rio_writen(connfd, &m2, 8);
    		time = endTimer(begin);
		endClock(begin2);
	 }
	
	/* RETRIEVE message */
	else if (m1.type == 3){
		Rio_readn(connfd, &m1, 52);
		printf("RETRIEVE message received!\n");
		m2.seqNo = m1.seqNo;
        	if ((file = fopen(m1.filename, "r")) == NULL){
    			printf("File does not exist!\n");
    			m2.status = -1;
    			} else {
    				printf("File does exist!\n");
    				m2.status = 0;
    				
    				while(fread(buff, 1, 1, file) > 0){
    					length += 1;
    				}
    				
    				fclose(file);
    				m2.numbytes = length;	
    				Rio_writen(connfd, &m2, 12);
    			
    				file = fopen(m1.filename, "r");
    				
    				while(fread(buff, 1, 1, file) > 0){
    					Rio_writen(connfd, buff, 1);
    				}
    				
    				fclose(file);	
        		}
        	Rio_writen(connfd, &m2, 8);
        	time = endTimer(begin);
		endClock(begin2);
        }
        
        /* COPY message */
        else if (m1.type == 4){
        	Rio_readn(connfd, &m1, 92);
        	m2.seqNo = m1.seqNo;
        	printf("COPY message received!\n");
        	/* To differentiate between the cases: the c: before cloud files is not removed when the client sends them over. On the server, the program checks which files are cloud files, then assigns the case number accordingly. */
        
        	/* Case 1: Both files are cloud files */
        	if (isCloudFile(m1.filename) && isCloudFile(m1.filename2)){
        		printf("CASE ONE\n");
        		
        		memmove(m1.filename, &m1.filename[2], strlen(m1.filename));
        		memmove(m1.filename2, &m1.filename2[2], strlen(m1.filename2));
        			
        		if (copyFile(m1.filename, m1.filename2)){
        			m2.status = 0;
        		} else {
        			m2.status = -1;
        		}
        		Rio_writen(connfd, &m2, 8);
        	}
        	
        	/* Case 2: Filename is a cloudfile, Filename2 is not */
        	else if (isCloudFile(m1.filename)){
        		printf("CASE TWO\n");
        		memmove(m1.filename, &m1.filename[2], strlen(m1.filename));
        		
        	if ((file = fopen(m1.filename, "r")) == NULL){
    			printf("File does not exist!\n");
    			m2.status = -1;
    			} else {
    				printf("File does exist!\n");
    				m2.status = 0;
    				
    				while(fread(buff, 1, 1, file) > 0){
    					length += 1;
    				}
    				
    				fclose(file);
    				m2.numbytes = length;	
    				Rio_writen(connfd, &m2, 12);
    			
    				file = fopen(m1.filename, "r");
    				
    				while(fread(buff, 1, 1, file) > 0){
    					Rio_writen(connfd, buff, 1);
    				}
    				
    				fclose(file);
    				Rio_writen(connfd, &m2, 8);	
    				
    				}
    		}
    
        	/* Case 3: Filename2 is a cloudfile, Filename1 is not */
        	else if (isCloudFile(m1.filename2)){
        		printf("CASE THREE\n");
        		Rio_readn(connfd, &m2, 12);
        		printf("Length:%d\n", m2.numbytes);
        		
        		memmove(m1.filename2, &m1.filename2[2], strlen(m1.filename2));
        		if ((file = fopen(m1.filename2, "w")) != NULL){
    				while (byte < m2.numbytes){
    					Rio_readn(connfd, buff, 1);
    					fwrite(buff, 1, 1, file);
    					byte += 1;	
    					}
    				m2.status = 0;
    			}
    			m2.seqNo = m1.seqNo;
 			Rio_writen(connfd, &m2, 8);
    			fclose(file);
        	}
        time = endTimer(begin);
        endClock(begin2);	
        memcpy(m2.filename, m1.filename, strlen(m1.filename));
        memcpy(m2.filename2, m1.filename2, strlen(m1.filename2));
        
        /* DELETE message */
        } else if (m1.type == 5){
        	Rio_readn(connfd, &m1, 52);
        	m2.seqNo = m1.seqNo;
        	printf("DELETE message received!\n");
        	if (remove(m1.filename) == 0) {
    			printf("File deleted successfully.\n");
    			m2.status = 0;
    			} else {
    				printf("Error in deleting file\n");
    				m2.status = -1;
    			}
    		time = endTimer(begin);
		endClock(begin2);
    		Rio_writen(connfd, &m2, 8);
      
        /* LIST message */
        } else if (m1.type == 6){
        	Rio_readn(connfd, &l1, 8);
        	printf("LIST message received!\n");
        	m1.seqNo = l1.seqNo;
        	DIR *d;
    			struct dirent *dir;
    			d = opendir(".");
    			if (d) {
    			
    				/* Count number of files in directory */
    				while ((dir = readdir(d)) != NULL){
    					if (dir->d_type == DT_REG){
    						i ++;
    					}
    				}
    				
    			 	char *fileList[i];
    				char fileNames[i*40];
    				
    				/* Store file names in an array of pointers*/
    				d = opendir(".");    
    				while ((dir = readdir(d)) != NULL){
    					if (dir->d_type == DT_REG){
    						fileList[x] = (dir->d_name);
    						x ++;
    					}
    				}
    				
    				/* Transfer data from array of pointers to a normal array */
    				strcat(fileList[0], "\n");
    				memcpy(&fileNames[0], fileList[0], strlen(fileList[0]));
    				for (int j = 1; j < i; j ++){
    					strcat(fileList[j], "\n");
    					strcat(fileNames, fileList[j]);
    				}
    				
    				l2.numFiles = i;
    				printf("NumFiles:%d\n", l2.numFiles);
    				l2.status = 0;
    				Rio_writen(connfd, &l2, 12);
    				Rio_writen(connfd, &fileNames, (i * 40));
    				
    				/* Clear strings*/
    				memset(fileNames, 0, strlen(fileNames));	
    				for (int k = 0; k < x; k ++){
    					fileList[k] = "\n";
    				}
    				
    			} else {
    				l2.status = -1;
    				Rio_writen(connfd, &l2, 12);
    			}
    			closedir(d);
    			time = endTimer(begin);
			endClock(begin2);	
    	/* QUIT */
        } else if (m1.type == 7){
        	exit(0);
        }
           
        if (m1.type > 0 && m1.type < 7){
		printf("SeqNo: %d\n", m1.seqNo);
		Rio_writen(connfd, &time, 8);
		printf("Filename: %s\n", m1.filename);
		printf("Filename2: %s\n", m1.filename2);
		printf("Numbytes: %d\n", m1.numbytes);
		printf("Filedata: %s\n", m2.filedata);
		printf("Status: %d\n", m2.status);
		printf("\n");
	}
	
	/* Reset variables */
	memset(m1.filename, 0, strlen(m1.filename));
	memset(m1.filename2, 0, strlen(m1.filename2));
	memset(m1.filedata, 0, strlen(m1.filedata)); 	
    	memset(m2.filedata, 0, strlen(m2.filedata)); 	
    	memset(tempData, 0, strlen(tempData));
    	memset(buff, 0, strlen(buff));
	m1.numbytes = 0;
	i = 0;
	x = 0;
	byte = 0;
	length = 0;
	time = 0.0;
       }
      
    exit(0);
}
/* $end echoserverimain */

