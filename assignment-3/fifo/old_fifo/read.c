/* Declare and initialise local variables */
	int fifofd;
	char datafromFIFO[data->messageLength];
	
	/* Open Fifo for reading */
	if((fifofd = open(data->fifoname, O_RDONLY)) < 0)
    {
        perror("open");
		exit(2);
    }
    
    /* Read from fifo */
	int n = read(fifofd, datafromFIFO, data->messageLength);
	
	/* Loop until fifo has been closed */
	while(n>0)
	{
		/* Put a null char at the end */
		datafromFIFO[n] = '\0'; 
		
		/* Let thread other thread know that they can put more data into fifo */
		sem_post(data->sem);
	
		/* write data to file */
		appendToFile(data->outFileName,datafromFIFO);
		
		/* Read from fifo */
		n = read(fifofd, datafromFIFO, data->messageLength);
	}
	
	/* Close the fifo */
	close(fifofd);