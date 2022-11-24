int inputLength = strlen(query_string);
		char *QueryfirstWord,*context;
		char *inputCopy = (char*) calloc(inputLength + 1, sizeof(char));
		strncpy(inputCopy, query_string, inputLength);
		QueryfirstWord = strtok_r (inputCopy, delimiter, &context);

		//insert into table()
		//insert into table values()

		tolower(QueryfirstWord);
		getchar();
		free(inputCopy);
		

		

		if(strcmp("insert",QueryfirstWord) == 0)		// if the first word of the query is "insert", only then we are checking for functional dependency violation
		{
			char *inputCopy = (char*) calloc(inputLength + 1, sizeof(char));
			strncpy(inputCopy, query_string, inputLength);
			char* token = strtok(inputCopy,delimiter);
			int count = 0;
			while (token != NULL) {
        		token = strtok(NULL, delimiter);
				count += 1;
    		}
    		
			getchar();
			free(inputCopy);
			elog(NOTICE,"table length %d",count);
			char table_name[100];

			if (count == 3)
			{
				//insert into table()
				char *inputCopy = (char*) calloc(inputLength + 1, sizeof(char));
				strncpy(inputCopy, query_string, inputLength);
				char*  token = strtok(inputCopy,delimiter);
				char *fw, *sw, *tw;
				int count = 0;
				
				while (token != NULL) {
        			token = strtok(NULL, delimiter);
					count += 1;
					if (count == 2)
					{
						tw = token;
					}					
    			}
				char* newToken = strtok(tw,"(");
				strcpy(table_name,newToken);

    		
				getchar();
				free(inputCopy);
				elog(NOTICE,"table length %d",count);

			}
			else if (count==4)
			{ 
			char *inputCopy = (char*) calloc(inputLength + 1, sizeof(char));
			strncpy(inputCopy, query_string, inputLength);
			char* token = strtok(inputCopy,delimiter);
			int count = 0;
			while (token != NULL) {
        		token = strtok(NULL, delimiter);
				count += 1;
				if(count==2)
				{
					strcpy(table_name,token );
				}
    		}
    		
			getchar();
			free(inputCopy);
			
			}
			elog(NOTICE,"table name issss %s",table_name);
			elog(NOTICE,"input query %s",inp_query);
