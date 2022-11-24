80a81
> #include "nodes/nodes.h"
944a946
> 
995c997,998
< 	
---
> 
> 	//elog(INFO, "hello");
1001c1004
< 	
---
> 
1145a1149,1306
> 
> 		                              /*************************************
> 						 * DB project changes starts here
> 						 ************************************/
> 						bool fd_failed = false;
> 						char error_msg[500];
> 						char *relname; // attributes;
> 						Query *query = (Query*) querytree_list->head->data.ptr_value;
> 
> 						if (query->commandType == CMD_INSERT) {
> 							char attributes[50][50];
> 							int values[50];
> 							int n = 0;
> 			      			RangeTblEntry *rte = query->rtable->head->data.ptr_value;
> 							// Table name
> 							relname = rte->eref->aliasname;
> 							ListCell *entries = query->targetList->head;
> 							while (entries) {
> 								TargetEntry *entry = entries->data.ptr_value;
> 								if (entry->expr->type == T_Const) {
> 									Const *const_value = (Const*) entry->expr;
> 									if (const_value->constisnull) {
> 										// Value is null, we are ignoring it
> 									} else if (const_value->constbyval) {
> 										// It has a constant value in constvalue field
> 										// we could check actual type using consttype file in oid attribute of pg_type table
> 										// possible values => 26
> 										// But for this project, we assume it will be int4
> 										// printf("Attribute %s has constant by value %d\n", entry->resname, const_value->constvalue);
> 										strcpy(attributes[n], entry->resname);
> 										values[n++] = (int) const_value->constvalue;
> 									} else {
> 
> 										// Const value is pointed by constvalue (datum) field
> 										// Again for simply assume it to be text/char/varchar/other variants
> 										// check consttype in pg_type table
> 										// possible values => 25, 1043
> 										// printf("Attribute %s has value %s\n", entry->resname, ((text*)const_value->constvalue)->vl_dat);
> 										// strcpy(attributes[n], entry->resname);
> 									}
> 			    				} else {
> 									// printf("No appropriate type handler defined for %s\n",entry->resname);
> 									// Ignore
> 								}
> 								entries = entries->next;
> 							}
> 							for(int i=0;i<n;i++)
> 							{
> 								elog(INFO , "attribute name %s attribute value %d",attributes[i],values[i]);
> 							}
> 
> 							char fd_check_query[500];
> 							char fd_check2_query[500];
> 
> 							sprintf(error_msg, "Found violations in: ");
> 
> 							SPI_connect();
> 							for (int i = 0; i < n; i++) {
> 								bzero(fd_check_query, 500);
> 								sprintf(fd_check_query,
> 										"select * from fd where table_name='%s' and determinant='%s';",
> 										relname, attributes[i]);
> 								SPI_exec(fd_check_query, 0);
> 								if (SPI_processed > 0) {
> 									bzero(fd_check_query, 500);
> 									sprintf(fd_check_query,
> 											"select dependent1 from fd where table_name='%s' and determinant='%s';",
> 											relname, attributes[i]);
> 									bzero(fd_check2_query, 500);
> 											sprintf(fd_check2_query,
> 													"select dependent2 from fd where table_name='%s' and determinant='%s';",
> 															relname, attributes[i]);
> 									SPI_exec(fd_check_query, 0);
> 									char *dependent1_name;
> 									if(SPI_processed > 0){
> 
> 										dependent1_name = SPI_getvalue(SPI_tuptable->vals[0],SPI_tuptable->tupdesc, 1);
> 
> 									}
> 
> 									SPI_exec(fd_check2_query,0);
> 									if (SPI_processed > 0) {
> 										char *dependent2_name;
> 										dependent2_name = SPI_getvalue(SPI_tuptable->vals[0],
> 												SPI_tuptable->tupdesc, 1);
> 
> 
> 
> 										// check for violation
> 										int determinant, dependent1,dependent2, id1,id2 = 0;
> 										bool isnull;
> 
> 										// find index for dependent_name, and then take the value from values[] array
> 										for (id1= 0; id1< n; id1++) {
> 
> 											if (strcmp(attributes[id1], dependent1_name) == 0)
> 											{
> 												dependent1 = (int)values[id1];
> 												break;
> 											}
> 
> 										}
> 
> 										for (id2 = 0; id2 < n; id2++) {
> 											if (strcmp(attributes[id2], dependent2_name) == 0)
> 												break;
> 										}
> 
> 
> 										dependent2 = values[id2];
> 
> 										determinant = values[i];
> 
> 
> 
> 										bzero(fd_check_query, 500);
> 										sprintf(fd_check_query,
> 												"select %s  from %s where %s= %u ;",
> 												dependent1_name ,relname, attributes[i],
> 												values[i]);
> 										SPI_exec(fd_check_query, 0);
> 										int j;
> 										if (SPI_processed > 0) {
> 												j = DatumGetInt64(
> 											SPI_getbinval(SPI_tuptable->vals[0],
> 										    SPI_tuptable->tupdesc, 1, &isnull));
> 										}
> 										bzero(fd_check_query, 500);
> 
> 										sprintf(fd_check_query,"select %s  from %s where %s= %u ;",
> 															dependent2_name ,relname, attributes[i],
> 																		values[i]);
> 																		SPI_exec(fd_check_query, 0);
> 										if (SPI_processed > 0) {
> 											int k = DatumGetInt64(
> 													SPI_getbinval(SPI_tuptable->vals[0],
> 															SPI_tuptable->tupdesc, 1, &isnull));
> 
> 											if (j!=dependent1 || k != dependent2) {
> 												fd_failed = true;
> 												sprintf(error_msg + strlen(error_msg),
> 														"%s depends on %s,%s conflicting with %s=%d and %s=%d and %s=%d\n",
> 														attributes[i], dependent1_name,dependent2_name,
> 														attributes[i], determinant,
> 														dependent1_name, j,dependent2_name,k);
> 											}
> 										}
> 									}
> 								}
> 
> 							}SPI_finish();
> 						}
> 						if (fd_failed)
> 							ereport(ERROR, (errcode(ERRCODE_INTERNAL_ERROR ), errmsg(error_msg)));
> 
> 						/*************************************
> 						 * DB project changes ends here
> 						 ************************************/
