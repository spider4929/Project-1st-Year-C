#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mysql.h>
#include <conio.h>
void creates();
void reads();
void finds();
void updates();
void deletes();
void user_manual();
//for debugging purposes
void finish_with_error(MYSQL *con) {
	fprintf(stderr, "%s\n", mysql_error(con));
	mysql_close(con);
	exit(1);
}
int main() {
    //initializes MySQL
	MYSQL *con = mysql_init(NULL);
	if (con == NULL) {
		finish_with_error(con);
	}
	//connects to a database that corresponds to the details in mysql_real_connect
    //if being used at another computer, export and import cpe_project database then change username and password in mysql_real_connect function according to settings of other computer
	if (mysql_real_connect(con, "localhost", "root", "8700", "cpe_project", 0, NULL, 0) == NULL) {
		finish_with_error(con);
	}
	int x;
	while (1) {
		printf("-------------------------------------\n");
		printf("       Student Grades Recorder       \n");
		printf("-------------------------------------\n");
		printf("Please choose an operation.\n");
		printf("1. Create or modify a student record.\n");
		printf("2. Display all student record.\n");
		printf("3. Display one student record.\n");
		printf("4. Remove student record.\n");
		printf("5. Close program.\n");
        x = getch();
		switch (x) {
			case '1':
				creates(con);
				break;
			case '2':
				reads(con);
				break;
			case '3':
				finds(con);
				break;
			case '4':
				deletes(con);
				break;
            case '5':
				mysql_close(con);
				printf("\nThe program will close now.\n");
				exit(0);
			default:
				printf("\nIncorrect input.\n\n");
				break;
		}
	}
}
//function that creates or edits an entry in the database
void creates(MYSQL *con) {
	int y = '1';
	char name[50];
	int student_num, grading_period;
	float pt_grade, ww_grade, pe_grade, final_grade;
	while (y == '1') {
		system("CLS");
		printf("---------------------------------\n");
		printf("Create or update a student record\n");
		printf("---------------------------------\n");
		printf("In order to create a record, input student number on grading period that is not inputted yet.\n");
		printf("In order to update a record, input student number on grading period that is already inputted previously.\n");
		printf("\nEnter student number or class number.\n");
		fflush(stdin);
		scanf("%d", &student_num);
		printf("Enter name of student.\n");
		fflush(stdin); //used because of fgets
		fgets(name, 50, stdin);
		name[strcspn(name, "\n")] = 0;
		printf("Enter the grading period.\n");
		scanf("%d", &grading_period);
		printf("Enter the student's Performance Tasks grade (in percent).\n");
		scanf("%f", &pt_grade);
		printf("Enter the student's Written Works grade.\n");
		scanf("%f", &ww_grade);
		printf("Enter the student's Periodical Exam grade.\n");
		scanf("%f", &pe_grade);
		final_grade = (pt_grade*0.3)+(ww_grade*0.5)+(pe_grade*0.2); //grades are multiplied according to how much they comprise the final grade, the percentage is taken from the client herself
		printf("\nThe student's final grade for the grading period is %0.2f.\n", final_grade);
		char buf[1024]={}; //the array that will hold the query
		//the long query is due to it being a create and update query. if it doesn't find a duplicate, which is based on the student number, it will enter the data inputted by the user, otherwise it will overwrite the duplicate
		char query[]={"INSERT INTO grade_%d(student_num,name,pt_grade,ww_grade,pe_grade,final_grade) VALUES('%d','%s','%f','%f', '%f', '%f') ON DUPLICATE KEY UPDATE student_num=VALUES(student_num),name=VALUES(name),pt_grade=VALUES(pt_grade),ww_grade=VALUES(ww_grade),pe_grade=VALUES(pe_grade), final_grade=VALUES(final_grade)"};
		sprintf(buf,query,grading_period,student_num,name,pt_grade,ww_grade,pe_grade,final_grade); //using sprintf, a query can be written with user input values from C
		if (mysql_query(con,buf)) {
			finish_with_error(con);
		}
		else {
			printf("\nData successfully recorded!\n");
		}
		printf("\nEnter 1 to enter another record. Enter any other character otherwise.\n");
		y = getch();
	}
}
//function that reads all entries for one grading period
void reads(MYSQL *con) {
	system("CLS");
    printf("----------------------------\n");
	printf("Display all student records.\n");
    printf("----------------------------\n");
    printf("Enter grading period.\n");
    int period;
    scanf("%d", &period);
    char buf[1024]={};
    sprintf(buf,"SELECT * FROM grade_%d",period);//retrieves all values from rows that corresponds to grading period
    if (mysql_query(con,buf)) {
        finish_with_error(con);
    }
	MYSQL_ROW row;
 	MYSQL_RES *res = mysql_store_result(con);
	while (row=mysql_fetch_row(res)) {//this will print out the data it receives as long as there are rows detected
		printf("\n");
		printf("Student number: %s\n",row[1]);
		printf("Student name: %s\n",row[2]);
		printf("Grading Period: %d\n",period);
		printf("Performance Task Grade: %s\n",row[3]);
		printf("Written Work Grade: %s\n",row[4]);
		printf("Periodical Exam Grade: %s\n",row[5]);
		printf("Final Grade: %s\n",row[6]);
		printf("\n");
	}
}
//function that reads one specific record that matches the user inputted student number and grading period
void finds(MYSQL *con) {
	system("CLS");
	int y='1';
    printf("-----------------------------\n");
	printf("Display one student's record.\n");
	printf("-----------------------------\n");
	while (y=='1') {
		printf("Enter student number.\n");
		int student;
		scanf("%d", &student);
		printf("Enter grading period.\n");
		int period;
		scanf("%d", &period);
		char buf[1024]={};
		sprintf(buf,"SELECT * FROM grade_%d WHERE student_num=%d", period, student);//retrieves all values from row that corresponds to student number and grading period
		if (mysql_query(con,buf)) {
			finish_with_error(con);
		}
		else {
            MYSQL_ROW row;
            MYSQL_RES *res = mysql_store_result(con);
            while(row=mysql_fetch_row(res)) {
                printf("\n");
                printf("Student number: %s\n",row[1]);
                printf("Student name: %s\n",row[2]);
                printf("Grading Period: %d\n",period);
                printf("Performance Task Grade: %s\n",row[3]);
                printf("Written Work Grade: %s\n",row[4]);
                printf("Periodical Exam Grade: %s\n",row[5]);
                printf("Final Grade: %s\n",row[6]);
                printf("\n");
		}
		}
		printf("\nEnter '1' to find another record. Enter any other character otherwise.\n");
		y = getch();
	}
}
//function that deletes one specific record that matches the user inputted student number and grading period
void deletes(MYSQL *con) {
    system("CLS");
    int y='1';
    while (y=='1') {
        printf("----------------------\n");
        printf("Remove student record.\n");
        printf("----------------------\n");
        printf("Enter student number of the entry you want to delete.\n");
        int student;
        scanf("%d", &student);
		printf("Enter grading period.\n");
		int period;
		scanf("%d", &period);
        char buf[1024]={};
        sprintf(buf,"SELECT * FROM grade_%d WHERE student_num=%d",period,student);
        if (mysql_query(con,buf)) {
            finish_with_error(con);
        }
        else {
            MYSQL_ROW row;
            MYSQL_RES *res = mysql_store_result(con);
            if ((row=mysql_fetch_row(res))) {
                printf("\nRecord found!\n");
                char buf_delete[1024]={};
                char query_delete[]={"DELETE FROM grade_%d WHERE student_num=%d"};
                sprintf(buf_delete,query_delete,period,student);
                if (mysql_query(con,buf_delete)) {
                    finish_with_error(con);
                }
                else {
                    printf("\nRecord successfully deleted!\n");
                }
            }
            else {
                printf("\nNo such record found!\n");
            }
        }
        printf("\nEnter '1' to delete another record. Enter any other character otherwise.\n\n");
		y = getch();
    }
}
