#include "strings.h"
#include <string>

void print_vector_words(char **array, int size)
{
	int i;
	for(i = 0; i<(size-1);i++)
	{
		printf("%s\n",array[i]);
	}
	printf("%s\n",array[i]);
}

int get_char_words(char **a, int size_vector_max)
{
	int i = 0;
	for(; scanf("%s",a[i])!= EOF && i<size_vector_max;i++);
	return i;
}

void strings_sort(char **a, int n)
{
  for (int i = 2; i <= n; i++)
  {
    int b = n-1;
  	while (b > 0 && strcmp(a[b-1], a[b]) > 0)
  	{
  	    char *m = a[b-1];
  		a[b-1] = a[b];
  		a[b] = m;
  	  	b--;
  	}
  }
}

int get_str_len(char* a)
{
	for(int i = 0;1;i++)
	{
		if(a[i]=='\0')
			return i;
	}
}


int find(const char* source, int size_s, const char* to_compare,int size_c)
{
	for(int i = 0; i<size_s;i++)
	{
		int b = i;
		if(source[i] == to_compare[0])
		{
			for(int j = 0;1;j++,b++)
			{
				if(j>=size_c || b>=size_s)
					return i;
				if(source[b] == to_compare[j]);
				else
				{
					break;
				}

			}

		}
		i = b;
	}
	return -1;
}