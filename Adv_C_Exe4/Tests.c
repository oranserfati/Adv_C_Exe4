#define _CRT_SECURE_NO_WARNINGS
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

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

int countPipes(const char* lineBuffer, int maxCount);
void countStudentsAndCourses(const char* fileName, int** coursesPerStudent, int* numberOfStudents);
char*** makeStudentArrayFromFile(const char* fileName, int** coursesPerStudent, int* numberOfStudents);
void factorGivenCourse(char** const* students, const int* coursesPerStudent, int numberOfStudents, const char* courseName, int factor);
void studentsToFile(char*** students, int* coursesPerStudent, int numberOfStudents);
Student* transformStudentArray(char*** students, const int* coursesPerStudent, int numberOfStudents);
void writeToBinFile(const char* fileName, Student* students, int numberOfStudents);
Student* readFromBinFile(const char* fileName);

void main()
{
	int* coursesPerStudent = NULL;
	int numberOfStudents = 0;
	char*** students = makeStudentArrayFromFile("studentList.txt", &coursesPerStudent, &numberOfStudents);
	Student* transformedStudents = transformStudentArray(students, coursesPerStudent, numberOfStudents);
	writeToBinFile("students.bin", transformedStudents, numberOfStudents);
	Student* testReadStudents = readFromBinFile("students.bin");
	printf("\n\n#################################\n\n");


}

int countPipes(const char* lineBuffer, int maxCount)
{
	
	if (lineBuffer == NULL)
		return -1;
	if (maxCount < 1)
		return 0;

	int pipeCount = 0;
	int generalCount = 0;
	while ((*(lineBuffer) != '\0') && (generalCount != maxCount))
	{
		if (*(lineBuffer++) == '|')
			pipeCount++;
		generalCount++;
	}

	return pipeCount;
}

void countStudentsAndCourses(const char* fileName, int** coursesPerStudent, int* numberOfStudents)
{
	int studentCount = 0;
	FILE* fin = fopen(fileName, "r");
	char oneline[1023];

	if (fin == NULL)
	{
		printf("Unable to open file\n");
		exit(1);
	}
	while (!feof(fin))
		while (fgets(oneline, 1023, fin) != NULL)
			studentCount++;
	*numberOfStudents = studentCount;
		

	rewind(fin);

	int* tmpPtr = (int*)malloc(sizeof(int)* (studentCount));//Malloc
	if (tmpPtr == NULL)
	{
		printf("Allocation faild.\n");
		return -1;
	}

	int i = 0;
	while (!feof(fin))//Assign  num of courses value
		if (fgets(oneline, 1023, fin) != NULL)
		{
			*(tmpPtr+i) = countPipes(oneline, 1023);
			i++;
		}
	*coursesPerStudent = tmpPtr;

	fclose(fin);
}

char*** makeStudentArrayFromFile(const char* fileName, int** coursesPerStudent, int* numberOfStudents)
{
	char oneline[1023];
	FILE* fin = fopen(fileName, "r");
	if (fin == NULL)
	{
		printf("Unable to open file\n");
		exit(1);
	}
	
	countStudentsAndCourses(fileName, coursesPerStudent, numberOfStudents);
	char*** studentArray = (char***)malloc(sizeof(char**) * (*numberOfStudents));
	if (studentArray == NULL)
	{
		printf("Allocation faild.\n");
		return -1;
	}
	for (int i = 0; i < *numberOfStudents; i++)
	{
		*(studentArray + i) = (char**)malloc(sizeof(char*) * (*(*(coursesPerStudent)+i) * 2 + 1));
		if (*(studentArray + i) == NULL)
		{
			printf("Allocation faild.\n");
			return -1;
		}

		int buffer = 0;
		int lastBuffer = 0;
		if (!feof(fin))
			if (fgets(oneline, 1023, fin) != NULL)
		for (int j = 0; j < (*(*(coursesPerStudent)+i) * 2 + 1); j++)
		{
			char tmpStr[50];
			while (oneline[buffer] != '|' && oneline[buffer] != '\0' && oneline[buffer] != ',')
				buffer++;

			*(*(studentArray + i) + j) = (char*)malloc(sizeof(char) * (buffer - lastBuffer)+1);
			if (*(*(studentArray + i) + j) == NULL)
			{
				printf("Allocation faild.\n");
				return -1;
			}
			strncpy(*(*(studentArray + i) + j), oneline + lastBuffer, buffer - lastBuffer);
			*(*(*(studentArray + i) + j) + buffer - lastBuffer) = '\0';
			buffer++;
			lastBuffer = buffer;
		}
	}

	fclose(fin);
	return studentArray;
}

void factorGivenCourse(char** const* students, const int* coursesPerStudent, int numberOfStudents, const char* courseName, int factor)
{
	if (factor > 20 || factor < -20)
	{
		printf("Unaccepted factor.\n");
		return -1;
	}
		
	for (int i = 0; i < numberOfStudents; i++)
		for (int j = 0; j < *(coursesPerStudent+i) * 2 + 1; j++)
		{
			if (!strcmp(students[i][j], courseName))
			{
				j++;
				if (atoi(students[i][j]) + factor > 100)
					_itoa(100, students[i][j], 10);
				else if (atoi(students[i][j]) + factor < 0)
					_itoa(0, students[i][j], 10);
				else
					_itoa(atoi(students[i][j]) + factor, students[i][j], 10);
			}
		}
}

void studentsToFile(char*** students, int* coursesPerStudent, int numberOfStudents)
{
	FILE* fin = fopen("studentListCopy.txt", "w");
	if (fin == NULL)
	{
		printf("Unable to create file\n");
		exit(1);
	}
	for (int i = 0; i < numberOfStudents; i++)
	{
		int swap = 0;
		for (int j = 0; j < coursesPerStudent[i] * 2 + 1; j++)
		{
			fputs(students[i][j], fin);

			if (j == coursesPerStudent[i] * 2 + 1 - 1)
			{
				if (!strstr(students[i][j], "\n"))
				{
					fputs("\n", fin);
				}
			}
			else
				if (!swap)
				{
					fputs("|", fin);
					swap = 1;
				}
				else
				{
					fputs(",", fin);
					swap = 0;
				}
		}
	}
	for (int i = 0; i < numberOfStudents; i++)
	{
		for (int j = 0; j < *(coursesPerStudent + i) * 2 + 1; j++)
			free(students[i][j]);
		free(students[i]);
	}	
	free(students);
}

Student* transformStudentArray(char*** students, const int* coursesPerStudent, int numberOfStudents)
{
	Student* studentArray = (Student*)malloc(sizeof(Student) * numberOfStudents);
	if (studentArray == NULL)
	{
		printf("Allocation faild.\n");
		return -1;
	}

	for (int i = 0; i < numberOfStudents; i++)
	{
		(studentArray + i)->numberOfCourses = coursesPerStudent[i];
		(studentArray + i)->grades = (StudentCourseGrade*)malloc(sizeof(StudentCourseGrade) * coursesPerStudent[i]);
		if ((studentArray + i)->grades == NULL)
		{
			printf("Allocation faild.\n");
			return -1;
		}
		int k = 0;
		for (int j = 0; j < coursesPerStudent[i] * 2 + 1; j++)
		{
			if (j == 0)
				strcpy((studentArray + i)->name , students[i][j]);
			else if (j % 2)
				strcpy(studentArray[i].grades[k].courseName, students[i][j]);
			else
			{
				studentArray[i].grades[k].grade = atoi(students[i][j]);
				k++;
			}
				
		}
	}

	for (int i = 0; i < numberOfStudents; i++)
	{
		printf("Name: %s.\nNumber of courses: %d.\n", (studentArray + i)->name, (studentArray + i)->numberOfCourses);
		for (int j = 0; j < (studentArray + i)->numberOfCourses; j++)
			printf("Course Name: %s.\nGrade: %d.\n", studentArray[i].grades[j].courseName, studentArray[i].grades[j].grade);
	}
	return studentArray;
}

void writeToBinFile(const char* fileName, Student* students, int numberOfStudents)
{
	FILE* fin = fopen(fileName, "w");
	if (fin == NULL)
	{
		printf("Unable to open file\n");
		exit(1);
	}

	fwrite(&numberOfStudents, 4, 1, fin);//V
	for (int i = 0; i < numberOfStudents; i++)
	{
		fwrite(students[i].name, 1, strlen(students[i].name), fin);
		fwrite("|", 1, 1, fin);
		fwrite(&students[i].numberOfCourses, 4, 1, fin);
		for (int  j = 0; j < students[i].numberOfCourses; j++)
		{
			fwrite(students[i].grades[j].courseName, 1, strlen(students[i].grades[j].courseName), fin);
			fwrite("|", 1, 1, fin);
			fwrite(&students[i].grades[j].grade, 4, 1, fin);
		}
	}
	fclose(fin);
}

Student* readFromBinFile(const char* fileName)
{
	FILE* fin = fopen(fileName, "r");
	if (fin == NULL)
	{
		printf("Unable to open file\n");
		exit(1);
	}

	int numberOfStudents;
	fread(&numberOfStudents, 4, 1, fin);

	Student* studentArray = (Student*)malloc(sizeof(Student) * numberOfStudents);
	if (studentArray == NULL)
	{
		printf("Allocation faild.\n");
		return -1;
	}

	for (int i = 0; i < numberOfStudents; i++)
	{
		int tmpNumOfCourses;
		int k = 0;
		char tempChar = "";
		int tmpNum;

		//Get student name
		while (tempChar != '|')
		{
			fread(&tempChar, 1, 1, fin);
			studentArray[i].name[k] = tempChar;
			k++;
		}
		studentArray[i].name[k-1] = '\0';
		tempChar = "";
		k = 0;

		//Get number of courses
		fread(&tmpNumOfCourses, 4, 1, fin);
		(studentArray + i)->numberOfCourses = tmpNumOfCourses;
		(studentArray + i)->grades = (StudentCourseGrade*)malloc(sizeof(StudentCourseGrade) * tmpNumOfCourses);
		if ((studentArray + i)->grades == NULL)
		{
			printf("Allocation faild.\n");
			return -1;
		}

		for (int j = 0; j < tmpNumOfCourses; j++)
		{
			//Course name
			while (tempChar != '|')
			{
				fread(&tempChar, 1, 1, fin);
				studentArray[i].grades[j].courseName[k] = tempChar;
				k++;
			}
			studentArray[i].grades[j].courseName[k-1] = '\0';
			tempChar = "";
			k = 0;

			//Course grade
			fread(&tmpNum, 4, 1, fin);
			studentArray[i].grades[j].grade = tmpNum;
		}
	}
	return studentArray;
}

void printStudentArray(Student* students)
{
	if (students == NULL)
		exit(1);

	for (int i = 0; i < students; i++)
	{
		printf("Name: %s.\nNumber of courses: %d.\n", (students + i)->name, (students + i)->numberOfCourses);
		for (int j = 0; j < (students + i)->numberOfCourses; j++)
			printf("Course Name: %s.\nGrade: %d.\n", students[i].grades[j].courseName, students[i].grades[j].grade);
	}
}
