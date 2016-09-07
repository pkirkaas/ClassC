struct lst { lst * next;
	lst() {next = (lst *) 0;};};
main()
{
 lst first , *current;
 current = &first;
 char s[99];
  while (1)
    { 
          //sprintf (s,"Here");
	  current->next = new lst;
	  current =  (current->next);
	 if (!(current)) {printf("No more new space.\n"); exit();}
	//if (!((int)current%10000)) 
		printf ("Current # of k = %d\n",((int)current)/1000);
		
	}
	}
