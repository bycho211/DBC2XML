#include <stdio.h>
#include <Windows.h>

#include "main.h"

using namespace std;

int main() {

	char filepath[30] = "dbc\\*.dbc";
	char path[30];
	int n = 0;	// the number of DBCs

	HANDLE hSrch;
	WIN32_FIND_DATA wfd;
	BOOL bResult = TRUE;
	hSrch = FindFirstFile(filepath, &wfd);

	if (hSrch == INVALID_HANDLE_VALUE) {
		printf("����! ������ ã�� �� �����ϴ�.\n");
		system("pause");

		return 0;
	}

	while (bResult) {

		if (strcmp(wfd.cFileName, ".") && strcmp(wfd.cFileName, "..")) {
		
			strcpy_s(path, 30, "DBC\\");
			strcat_s(path, 30, wfd.cFileName);
			
			cout << "\n\n *************************** [" << ++n << "] " << path << " ***************************"<< endl;

			parsing(path);
			cout << "\n\n1. DBC�κ��� ������ �޽����鿡 ���� ������ �Ʒ��� �����ϴ�.\n" << endl;

			for (iter = msg_map.begin(); iter != msg_map.end(); ++iter) {
				printf("ID : 0x%x     NAME : %-30s DLC : %2d      Transmitter : %-15s Cycle Time : %3d\n", iter->first, iter->second.name, iter->second.data_len, iter->second.node, iter->second.period);
			}

			smoothing();

			cout << "\n\n2. XML �𵨷� ������� �޽����鿡 ���� ������ �Ʒ��� �����ϴ�.\n" << endl;

			for (iter = msg_map.begin(); iter != msg_map.end(); ++iter) {
				printf("ID : 0x%x     NAME : %-30s DLC : %2d      Transmitter : %-15s Cycle Time : %3d\n", iter->first, iter->second.name, iter->second.data_len, iter->second.node, iter->second.period);
			}

			make_XML(path);
		}
	
		bResult = FindNextFile(hSrch, &wfd);
	}

	FindClose(hSrch);

	cout << "\n\n�۾��� �Ϸ�Ǿ����ϴ�. \n\n" << endl;
	
	system("pause");
}
