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
		printf("에러! 파일을 찾을 수 없습니다.\n");
		system("pause");

		return 0;
	}

	while (bResult) {

		if (strcmp(wfd.cFileName, ".") && strcmp(wfd.cFileName, "..")) {
		
			strcpy_s(path, 30, "DBC\\");
			strcat_s(path, 30, wfd.cFileName);
			
			cout << "\n\n *************************** [" << ++n << "] " << path << " ***************************"<< endl;

			parsing(path);
			cout << "\n\n1. DBC로부터 추출한 메시지들에 대한 정보는 아래와 같습니다.\n" << endl;

			for (iter = msg_map.begin(); iter != msg_map.end(); ++iter) {
				printf("ID : 0x%x     NAME : %-30s DLC : %2d      Transmitter : %-15s Cycle Time : %3d\n", iter->first, iter->second.name, iter->second.data_len, iter->second.node, iter->second.period);
			}

			smoothing();

			cout << "\n\n2. XML 모델로 만들어진 메시지들에 대한 정보는 아래와 같습니다.\n" << endl;

			for (iter = msg_map.begin(); iter != msg_map.end(); ++iter) {
				printf("ID : 0x%x     NAME : %-30s DLC : %2d      Transmitter : %-15s Cycle Time : %3d\n", iter->first, iter->second.name, iter->second.data_len, iter->second.node, iter->second.period);
			}

			make_XML(path);
		}
	
		bResult = FindNextFile(hSrch, &wfd);
	}

	FindClose(hSrch);

	cout << "\n\n작업이 완료되었습니다. \n\n" << endl;
	
	system("pause");
}
