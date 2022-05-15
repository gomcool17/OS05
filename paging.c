#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define BUFFER_SIZE 100

int page_frame;
int pages[30];
int N;
bool check[31];
bool second[31];
int count;

void print_intro()
{
    printf("\t%8s  ", "frame");
    for (int i = 0; i < page_frame; i++)
    {
        printf("%-5d", i + 1);
    }
    printf("%s\n", "page fault");
    printf("time\n");
}

void print_result(int p[], bool page_fault)
{
    for (int i = 0; i < page_frame; i++)
    {
        if (p[i] == -1)
        {
            printf("%-5s", " ");
        }
        else
            printf("%-5d", p[i]);
    }

    if (page_fault)
    {
        count++;
        printf("F\n");
    }
    else
        printf("\n");
}

void OPT()
{
    print_intro();

    bool page_fault = false;
    int *p;
    p = malloc(sizeof(int) * page_frame);

	for(int i=0;i<page_frame;i++)
		p[i] = -1;

    int z = 0;
    for (int i = 0; i < N; i++)
    {
        int notUse;
        int len = 0;
        printf("%-8d%8s  ", i + 1, " ");

        if (!check[pages[i]]) // 현재 올라와 있지 않으면
        {
            page_fault = true; // 페이비폴트 발생
            for (int j = 0; j < page_frame; j++) 
            {
                if (z < page_frame) // fram개수보다 작을시
                {
                    if (p[j] == -1) // 채워준다
                    {
                        p[j] = pages[i];
                        z++;
                        check[p[j]] = true;
                        break;
                    }

                    continue;
                }

                for (int k = i + 1; k < N; k++) // 프레임 개수를 넘어가면
                {
                    if (pages[k] == p[j]) // 현재 메모리의 페이지 번호와 이후 일어날 페이지들을 비교
                    {
                        if (len < k) // 현재 저장된 최댓값보다 크다면 갱신
                        {
                            len = k;
                            notUse = j;
                        }
                        break;
                    }

                    if (k == N - 1 && len != N - 1) // 이후에 아예 일어날 일이 없는 경우
                    {
                        len = k;
                        notUse = j;
                    }
                }
            }

            if (len != 0)
            {
                check[p[notUse]] = false;
                p[notUse] = pages[i];
                check[pages[i]] = true;
            }
        }

        print_result(p, page_fault); // 출력
        page_fault = false;
    }

    printf("Number of page faults : %d times\n", count);
}

void FIFO()
{
    print_intro();

    bool page_fault = false;
    int *p;
    
	p = malloc(sizeof(int) * page_frame);

	for(int i=0;i<page_frame;i++)
		p[i] = -1;
   
	int j = 0;

    for (int i = 0; i < N; i++)
    {

        if (j == page_frame)
            j = 0;

        printf("%-8d%8s  ", i + 1, " ");
        
		if (!check[pages[i]])
        {
			if(p[j] != -1)
            	check[p[j]] = false;
            
			p[j] = pages[i];
            j++;
            check[pages[i]] = true;
            page_fault = true;
        }

        print_result(p, page_fault);
        page_fault = false;
    }

    printf("Number of page faults : %d times\n", count);
}

void LRU()
{
    print_intro();

    bool page_fault = false;
    int *p;
    p = malloc(sizeof(int) * page_frame);

	for(int i=0;i<page_frame;i++)
		p[i] = -1;
    
	int j = 0;

    int z = 0;
    for (int i = 0; i < N; i++) // OPT랑 비슷하다.
    {
        int notUse;
        int len = 100;
        printf("%-8d%8s  ", i + 1, " ");

        if (!check[pages[i]])
        {
            page_fault = true;
            for (int j = 0; j < page_frame; j++)
            {
                if (z < page_frame)
                {
                    if (p[j] == -1)
                    {
                        notUse = j;
                        z++;
                        break;
                    }

                    continue;
                }

                for (int k = i - 1; k >= 0; k--) // 더 작은 것을 갱신
                {
                    if (pages[k] == p[j])
                    {
                        if (len > k)
                        {
                            len = k;
                            notUse = j;
                        }
                        break;
                    }
                }
            }

            check[p[notUse]] = false;
            check[pages[i]] = true;
            p[notUse] = pages[i];
        }

        print_result(p, page_fault);
        page_fault = false;
    }

    printf("Number of page faults : %d times\n", count);
}

void Second_Chance()
{
    print_intro();

    bool page_fault = false;
    int *p;
    p = malloc(sizeof(int) * page_frame);
	
	for(int i=0;i<page_frame;i++)
		p[i] = -1;
   
	int j = 0;
    bool flag = false;

    for (int i = 0; i < N; i++)
    {

        if (j == page_frame)
            j = 0;

        if (!flag) {
            printf("%-8d%8s  ", i + 1, " ");
            flag = true;
        }

		if(p[j] == -1 && !check[pages[i]]) {
			p[j] = pages[i];
			check[pages[i]] = true;
			page_fault = true;
			j++;
		}
		else if (!check[pages[i]])
        {
            if (second[p[j]]) // 한번의 기회가 더 있다면 넘어가기
            {
                second[p[j]] = false;
                i--;
                j++;
                continue;
            }
            
			check[p[j]] = false;
           
			p[j] = pages[i];
            j++;
            check[pages[i]] = true;
            page_fault = true;
        }
        else
            second[pages[i]] = true;

        print_result(p, page_fault);
        page_fault = false;
        flag = false;
    }

    printf("Number of page faults : %d times\n", count);
}

int main()
{
    char method[BUFFER_SIZE];

    printf("Used method : ");
    scanf("%s", method);

    FILE *fp = fopen("input.txt", "r");
    
    if ((fp = fopen("input.txt", "r")) == NULL )
    {
        fprintf(stderr, "File open error!.!\n");
        exit(0);
    }
    
    char str[BUFFER_SIZE];
    fgets(str, BUFFER_SIZE, fp);
    page_frame = atoi(str);

    fgets(str, BUFFER_SIZE, fp);
    printf("page reference string : %s\n", str);
    str[strlen(str)] = '\0';
    char *ptr = strtok(str, " ");

    while (ptr != NULL)
    {
        pages[N] = atoi(ptr);
        N++;

        ptr = strtok(NULL, " ");
    }

    if (!strcmp("OPT", method))
        OPT();
    else if (!strcmp("FIFO", method))
        FIFO();
    else if (!strcmp("LRU", method))
        LRU();
    else if (!strcmp("Second-Chance", method))
        Second_Chance();
    else
    {
        printf("OPT, FIFO, LRU, Second-Chance중 하나만 입력해 주세요. (대소문자 정확히\n");
        exit(0);
    }
}
