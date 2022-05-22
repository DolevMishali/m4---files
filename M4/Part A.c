#define _CRT_SECURE_NO_WARNINGS
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
//#define _CRTDBG_MAP_ALLOC
//#include <crtdbg.h>

typedef struct StudentCourseGrade
{
	char courseName[35];
	int grade;
}StudentCourseGrade;

typedef struct Student
{
	char name[35];
	StudentCourseGrade* grades; //dynamic array of courses
	int numberOfCourses;
}Student;

int countPipes(const char* lineBuffer, int maxCount)
{
	if ( lineBuffer == NULL) return -1;
	if (maxCount <= 0) return 0;

	int count = 0;
	int i = 0;

	while(lineBuffer[i] != '\0')
	{
		if (lineBuffer[i] ==  '|') count++;
		i++;
	}
	return count;
}

void countStudentsAndCourses(const char* fileName, int** coursesPerStudent,int* numberOfStudents)
{
	FILE* fp = fopen(fileName, "r");
	assert(fp);

	fseek(fp, 0, SEEK_SET);
	char c = 0;
	int countStud = 0;
	char tmp[100] = "";

	while (!feof(fp))
	{
		c = fgetc(fp);
		if (c == '\n')
		{
			countStud++;
		}
	}
	countStud++;// Because the txt don't finish with \n.
	*numberOfStudents = countStud;

	*coursesPerStudent = (int*)calloc(countStud, sizeof(int));

	fseek(fp, 0, SEEK_SET);
	int countCourses = 0;
	for(int i = 0; i < *numberOfStudents;i++)
	{
		fgets(tmp, 100, fp);
		countCourses = countPipes(tmp,100);
		(*coursesPerStudent)[i] = countCourses;
	}
	fclose(fp);
}

char*** makeStudentArrayFromFile(const char* fileName, int** coursesPerStudent, int* numberOfStudents)
{
	FILE* fp = fopen(fileName, "r");
	assert(fp);

	fseek(fp, 0, SEEK_SET);
	char tmp[100] = "";
	int countCourses = 0;

	char*** list = (char***)calloc((*numberOfStudents), sizeof(char**));

	const char s[2] = "|";
	const char s2[2] = ",";
	char* token;

	for (int i = 0; i < (*numberOfStudents);i++)
	{
		countCourses = (*coursesPerStudent)[i];
		char** students = (char**)calloc((countCourses*2)+1,sizeof(char*));
		list[i] = students;

		fgets(tmp, 100, fp);
		token = strtok(tmp, s);
		//Put the name first, than the courses.
		char* name = (char*)calloc((strlen(token) + 1), sizeof(char));
		list[i][0] = name;
		strcpy(list[i][0], token);

		for(int j = 1; j <= (countCourses*2+1);j++)
		{
			// -- Course name --
			token = strtok(NULL, s2);
			if (token == NULL) break;

			char* course = (char*)calloc((strlen(token)), sizeof(char));
			list[i][j] = course;
			strcpy(list[i][j], token);

			// -- Grade --
			token = strtok(NULL, s);
			if (token == NULL) break;

			char* grade = (char*)calloc((strlen(token)), sizeof(char));
			list[i][++j] = grade;
			strcpy(list[i][j], token);
		}
	}
	fclose(fp);
	return list;
}

void factorGivenCourse(char** const* students, const int* coursesPerStudent,int numberOfStudents, const char* courseName, int factor)
{
	if ( factor > -20 && factor < 20 ) 
	{
		for (int i = 0; i < numberOfStudents; i++)
		{
			for (int j = 0; j < (coursesPerStudent[i] * 2) + 1; j++)
			{
				if(!strcmp(students[i][j],courseName))
				{
					int tmp = atoi(students[i][j + 1]);
					tmp = tmp + factor;
					if (tmp > 100) tmp = 100;
					if (tmp < 0) tmp = 0;
					_itoa(tmp,students[i][j + 1],10);
				}
			}
		}
	}
	else
		printf("No change.");
}

void studentsToFile(char*** students, int* coursesPerStudent, int numberOfStudents)
{
	FILE* fp = fopen("studentList - New.txt","w");
	assert(fp);

	fseek(fp, 0, SEEK_SET);
	for (int i = 0; i < numberOfStudents; i++)
	{
		for (int j = 0; j < (coursesPerStudent[i] * 2) + 1; j++)
		{
			fwrite(students[i][j],strlen(students[i][j]),sizeof(char), fp);
			fwrite(" ", 1, sizeof(char), fp);
		}
		fwrite("\n", 1, sizeof(char), fp);
	}

	for(int i = 0;i < numberOfStudents; i++)
	{
		for(int j=0;j < (coursesPerStudent[i] * 2) + 1;j++)
		{
		free(students[i][j]);
		}
	free(students[i]);
	}
	free(students);

	fclose(fp);
}

Student* transformStudentArray(char*** students, const int* coursesPerStudent, int numberOfStudents)
{
	Student* newStudent = (Student*)malloc(numberOfStudents * sizeof(Student));
	assert(newStudent);

	for (int i = 0; i < numberOfStudents; i++)
	{
		strcpy(newStudent[i].name, students[i][0]);

		newStudent[i].grades = (StudentCourseGrade*)malloc(coursesPerStudent[i] * sizeof(StudentCourseGrade));
		assert(newStudent[i].grades);

		newStudent[i].numberOfCourses = coursesPerStudent[i];

		for (int j=0,k=1; j < coursesPerStudent[i]; j++,k++)
		{
			strcpy(newStudent[i].grades[j].courseName, students[i][k]); // Cpy Name.
			int tmp = atoi(students[i][++k]);
			newStudent[i].grades[j].grade = tmp; //Cpy Grade.
		}
	}
	return newStudent;
}

void writeToBinFile(const char* fileName, Student* students, int numberOfStudents)
{
	FILE* fp = fopen(fileName,"wb");
	assert(fp);
	fseek(fp,0,SEEK_SET);

	fwrite(&numberOfStudents,sizeof(int),1,fp);

	for (int i = 0; i < numberOfStudents; i++)
	{
		fwrite(&students[i].name,sizeof(students[i].name), 1, fp);
		fwrite(&students[i].numberOfCourses,sizeof(students[i].numberOfCourses),1,fp);
		for(int j = 0;j<students[i].numberOfCourses;j++)
		{
			fwrite(&students[i].grades[j].courseName, sizeof(students[i].grades[j].courseName), 1, fp);
			fwrite(&students[i].grades[j].grade, sizeof(students[i].grades[j].grade), 1, fp);
		}
	}
	fclose(fp);
}

Student* readFromBinFile(const char* fileName)
{
	FILE* fp = fopen(fileName, "rb");
	assert(fp);
	fseek(fp, 0, SEEK_SET);

	int numOfStudents;
	fread(&numOfStudents, 1, sizeof(int), fp);

	Student* newlist = (Student*)malloc(numOfStudents*sizeof(Student));
	assert(newlist);

	for (int i = 0; i < numOfStudents; i++)
	{
		fread(newlist[i].name,1,sizeof(newlist[i].name),fp);

		int numOfCourses;
		fread(&numOfCourses, 1, sizeof(int), fp);

		newlist[i].grades = (StudentCourseGrade*)malloc(numOfCourses * sizeof(StudentCourseGrade));
		assert(newlist[i].grades);
		newlist[i].numberOfCourses = numOfCourses;
		for (int j = 0; j < numOfCourses; j++)
		{
			fread(&newlist[i].grades[i].courseName,1, sizeof(newlist[i].grades[i].courseName),fp);
			fread(&newlist[i].grades[i].grade, 1, sizeof(newlist[i].grades[i].grade), fp);
		}
	}
	fclose(fp);
	return newlist;
}

void main()
{
	//                     --- Part A ---
	//                     --- Question 2 ---
	char fileName[17] = "studentList.txt";
	int* coursesPerStudent = NULL;
	int numberOfStudents = 0;
	countStudentsAndCourses(fileName, &coursesPerStudent, &numberOfStudents);
	//_CrtDumpMemoryLeaks();

	//                     --- Question 3 ---
	char*** students = makeStudentArrayFromFile("studentList.txt", &coursesPerStudent, &numberOfStudents);

	//                     --- Question 4 ---
	factorGivenCourse(students, coursesPerStudent, numberOfStudents, "Advanced Topics in C", +5);

	//                     --- Question 5 ---
	//studentsToFile(students,coursesPerStudent,numberOfStudents);


	//                     --- Part B ---
	//                     --- Question 1 ---
	Student* transformedStudents = transformStudentArray(students, coursesPerStudent, numberOfStudents);

	//                     --- Question 2 ---
	writeToBinFile("students.bin", transformedStudents, numberOfStudents);

	//                     --- Question 3 ---
	Student* testReadStudents = readFromBinFile("students.bin");
}