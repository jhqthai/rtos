	
    int fifofd;
    /* Open Fifo for writing */
	if((fifofd = open(data->fifoname, O_WRONLY)) < 0)
    {
        perror("open");
		exit(2);
    }
	
	/* Write to fifo */
	write(fifofd, &averageWaitTimeAsString, strlen(averageWaitTimeAsString));
	
	/* Write to fifo */
	write(fifofd, &averageTurnaroundTimeAsString, strlen(averageTurnaroundTimeAsString));

	/* Close the fifo */
	close(fifofd);