#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>

char* readLine(FILE* inFile, int symbolCount, bool* lineIsLast) // чтение очередной строки
{
	int symbol = getc(inFile);
	char* line;

	if (symbol != '\n' && symbol != EOF)
	{
		line = readLine(inFile, symbolCount + 1, lineIsLast);
		line[symbolCount] = symbol;
	}
	else
	{
		line = (char*)malloc(symbolCount + 1);
		line[symbolCount] = '\0';
		if (symbol == EOF)  *lineIsLast = true;
		else  *lineIsLast = false;
	}
	return line;
}

bool parseLine(FILE* inFile, char** timeStr, char** requestStr, char** statusStr) // проверка, последняя ли строка
{
	bool lineIsLast;
	char* line = readLine(inFile, 0, &lineIsLast);

	strtok(line, "[");
	*timeStr = strtok(NULL, "]");
	strtok(NULL, "\"");
	*requestStr = strtok(NULL, "\"");
	*statusStr = strtok(NULL, " ");

	free(line);
	return lineIsLast;
}


int parseTime(char* timeStr) // парсинг времени(возвращает время в секундах с помощью следующей функции)
{
	struct tm time;
	char* readStr;

	readStr = strtok(timeStr, "/: ");
	time.tm_mday = atoi(readStr);

	readStr = strtok(NULL, "/: ");
	if (strcmp(readStr, "Jan") == 0)  time.tm_mon = 0;
	else if (strcmp(readStr, "Jan") == 0)  time.tm_mon = 0;
	else if (strcmp(readStr, "Feb") == 0)  time.tm_mon = 1;
	else if (strcmp(readStr, "Mar") == 0)  time.tm_mon = 2;
	else if (strcmp(readStr, "Apr") == 0)  time.tm_mon = 3;
	else if (strcmp(readStr, "May") == 0)  time.tm_mon = 4;
	else if (strcmp(readStr, "Jun") == 0)  time.tm_mon = 5;
	else if (strcmp(readStr, "Jul") == 0)  time.tm_mon = 6;
	else if (strcmp(readStr, "Aug") == 0)  time.tm_mon = 7;
	else if (strcmp(readStr, "Sep") == 0)  time.tm_mon = 8;
	else if (strcmp(readStr, "Oct") == 0)  time.tm_mon = 9;
	else if (strcmp(readStr, "Nov") == 0)  time.tm_mon = 10;
	else if (strcmp(readStr, "Dec") == 0)  time.tm_mon = 11;

	readStr = strtok(NULL, "/: ");
	time.tm_year = atoi(readStr) - 1900;

	readStr = strtok(NULL, "/: ");
	time.tm_hour = atoi(readStr);

	readStr = strtok(NULL, "/: ");
	time.tm_min = atoi(readStr);

	readStr = strtok(NULL, "/: ");
	time.tm_sec = atoi(readStr);

	return mktime(&time);
}


int secondsIn(char* periodType)
{
	if (strcmp(periodType, "sec") == 0 || strcmp(periodType, "secs") == 0) return 1;
	else if (strcmp(periodType, "min") == 0 || strcmp(periodType, "mins") == 0) return 60;
	else if (strcmp(periodType, "hour") == 0 || strcmp(periodType, "hours") == 0) return 3600;
	else if (strcmp(periodType, "day") == 0 || strcmp(periodType, "days") == 0) return 3600 * 24;
	else return 0;
}

int inputPeriod()
{
	int period = 0;
	char* inputStr[100];
	printf("введите период времени в формате : 1 day 5 hours 30 sec ): ");
	scanf("%99[^\n]", inputStr);

	char* readStr;
	int value, coefficenttime;
	bool firstWord = true;

	while (1) 
	{
		if (firstWord)
		{
			readStr = strtok(inputStr, " ");
			firstWord = false;
		}
		else  readStr = strtok(NULL, " ");
		if(readStr == NULL)  break;
		value = atoi(readStr);
		if (value == 0)  return 0;

		readStr = strtok(NULL, " ");
		if (readStr == NULL)  return 0;
		coefficenttime = secondsIn(readStr);
		if (coefficenttime == 0)  return 0;

		period += value * coefficenttime;
	}
	return period;
}


typedef struct TimeElement {
	int time;
	int index;
	struct TimeElement* prev;
	struct TimeElement* next;
} TimeElement;

typedef struct {
	TimeElement* first;
	TimeElement* last;
} TimeList;

void push(TimeList* timeList, int timeValue, int index)
{
	TimeElement* elementToPush = (TimeElement*)malloc(sizeof(TimeElement));
	elementToPush->time = timeValue;
	elementToPush->index = index;
	elementToPush->prev = NULL;
	elementToPush->next = NULL;

	if (timeList->first == NULL)  timeList->last = elementToPush;
	else
	{
		elementToPush->next = timeList->first;
		timeList->first->prev = elementToPush;
	}
	timeList->first = elementToPush;
}

void popTail(TimeList* timeList)
{
	TimeElement* elementToPop = timeList->last;
	if (elementToPop != NULL)
	{
		if (elementToPop->prev != NULL)
		{
			timeList->last = elementToPop->prev;
			timeList->last->next = NULL;
		}
		else
		{
			timeList->first = NULL;
			timeList->last = NULL;
		}
		free(elementToPop);
	}
}

struct GreatPeriod {
	int max;
	int beginTime;
	int endTime;
};

int main()
{
	int period = inputPeriod();

	FILE* inFile;
	inFile = fopen("access_log_Jul95", "r");

	char* timeStr;
	char* requestStr;
	char* statusStr;

	TimeList timeList = { NULL, NULL };
	int failedRequests = 0;
	struct GreatPeriod Global = { 0,0,0 };
	bool lineIsLast = 0;

	printf("\nFailed requests:\n");

	for (int lineCount = 0; !lineIsLast; lineCount++)
	{

		lineIsLast = parseLine(inFile, &timeStr, &requestStr, &statusStr);
		if (timeStr != NULL) 
		{
			int time = parseTime(timeStr);
			push(&timeList, time, lineCount);
			
			while (time - timeList.last->time > period)  popTail(&timeList);
			int requestsInPeriod = lineCount - timeList.last->index + 1;
			if (requestsInPeriod > Global.max)
			{
				Global.max = requestsInPeriod;
				Global.beginTime = timeList.last->time;
				Global.endTime = time;
			}
		}
		if (statusStr != NULL && statusStr[0] == '5')
		{
			printf("%s \n", requestStr);
			failedRequests++;
		}
	}
	printf("\nколичество запросов с ошибками 5xx: %d", failedRequests);
	printf("\nнаибольшее количество запросов за указанный период времени: %d", Global.max);

	time_t rawtime = Global.beginTime;
	struct tm* timeinfo;
	timeinfo = localtime(&rawtime);
	printf("\nНачало периода: %s", asctime(timeinfo));

	rawtime = Global.endTime;
	timeinfo = localtime(&rawtime);
	printf("Конец периода: %s\n", asctime(timeinfo));

	return 0;
}