#include<iostream>
#include<string>
#include<string.h>
using namespace std;
int main() {
	std::string str = "abbbbb";
	int maxlen = 1;
	int len = str.length();
	int start = 0;
	for (int i = 0; i < len; i++)
	{
		for (int j = i + 1; j < len; j++)
		{
			int tmp1 = i;
			int tmp2 = j;
			while (tmp1 < tmp2 && str[tmp1] == str[tmp2])
			{
				tmp1++;
				tmp2--;
			}

			if (tmp1 >= tmp2 && maxlen < j - i + 1)
			{
				maxlen = j - i + 1;
				start = i;
			}
		}
	}
	printf("%d", maxlen);
	return 0;
}