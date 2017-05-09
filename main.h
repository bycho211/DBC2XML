#include <map>
#include <string>
#include <vector>

#include <iostream>
#include <fstream>
#include <stdlib.h>

using namespace std;

typedef struct {
	int id;
	char name[40];
	int data_len;
	char node[40];
	int period;
} msg_info;

std::map<int, msg_info> msg_map;
map<int, msg_info>::iterator iter;
map<int, msg_info>::iterator iter2;

int N;		// the number of msgs

void parsing(char* path){

	FILE *finput;

	char* parsing_word = (char*)malloc(sizeof(char)* 100);
	char* parsing_word2 = (char*)malloc(sizeof(char)* 100);

	bool found_BO = false;
	bool found_BA = false;

	char ch = 0;

	fopen_s(&finput, path, "r");

	while (1) {

		fscanf_s(finput, "%s", parsing_word, 100);

		if (feof(finput))
			break;
		
		if (!strcmp(parsing_word, "BO_")) {
			
			found_BO = true;

			msg_info tmp;

			fscanf_s(finput, "%s", parsing_word, 100);
			tmp.id = stoi(parsing_word);

			fscanf_s(finput, "%s", parsing_word, 100);
			parsing_word[strlen(parsing_word) - 1] = 0;
			strcpy_s(tmp.name, sizeof(tmp.name), parsing_word);
						
			fscanf_s(finput, "%s", parsing_word, 100);
			tmp.data_len = stoi(parsing_word);

			fscanf_s(finput, "%s", parsing_word, 100);
			strcpy_s(tmp.node, sizeof(tmp.node), parsing_word);

			msg_map[tmp.id] = tmp;
			
		}
		else if (!strcmp(parsing_word, "BA_")) {
			
			if (found_BO) {

				found_BA = true;
				
				fscanf_s(finput, "%s", parsing_word, 100);
				
				if (!strcmp(parsing_word, "\"GenMsgCycleTime\"")) {
				
					fscanf_s(finput, "%s", parsing_word, 100);

					fscanf_s(finput, "%s", parsing_word, 100);
					fscanf_s(finput, "%s", parsing_word2, 100);

					msg_map[stoi(parsing_word)].period = stoi(parsing_word2);
									
				}
				else {
					while ((ch = fgetc(finput)) != '\n');
				}
			}
			else{
			}
		}
		else 
			while ((ch = fgetc(finput)) != '\n');
	}

	for (iter = msg_map.begin(); iter != msg_map.end(); ++iter) {
		if (iter->second.period < 0)
			iter->second.period = 0;
	}

	N = msg_map.size();

	free(parsing_word);
	free(parsing_word2);

	fclose(finput);
}
	
void smoothing() {
	
	bool flag = true;

	while (flag) {

		flag = false;

		for (iter = msg_map.begin(); iter != msg_map.end(); ++iter) {

			if (iter->second.period > 10) {
		
				++iter;
			
				if (iter == msg_map.end())
					break;

				for (iter2 = iter; iter2 != msg_map.end(); ++iter2) {

					if (iter2 == iter)
						--iter;

					if (iter->second.period == iter2->second.period) {
					
						strcat_s(iter->second.name, sizeof(iter->second.name), " + ");
						strcat_s(iter->second.name, sizeof(iter->second.name), iter2->second.name);

						iter->second.data_len = (iter->second.data_len + iter2->second.data_len) / 2;

						strcpy_s(iter->second.node, sizeof(iter->second.node), "aggregated");
					
						iter->second.period /= 2;

						iter2 = msg_map.erase(iter2);
						N--;
						flag = true;
						
						break;
					}
				}
			}

			while (iter->second.period == 0) {
			
				iter = msg_map.erase(iter);
				N--;
				flag = true;
			}
		}
	}
}

void make_XML(char* path) {
	
	FILE *finput;
	FILE *foutput;

	char* dst_path = (char*)malloc(sizeof(char)* 100);
	char* context;
	char ch;

	char* tmp = (char*)malloc(sizeof(char)* 100);
				
	tmp = strtok_s(path, "\\", &context); 
	tmp = strtok_s(NULL, ".", &context);
	
	strcpy_s(dst_path, 100, tmp);
	strcat_s(dst_path, 100, ".xml");
	
//	printf("%s", dst_path);
		
	
	fopen_s(&foutput, dst_path, "w");
	
	/******************** [1] XML version and encoding definition *********************/
	
	fopen_s(&finput, "lib\\1_version_encoding_info", "r");	// copy p1
	
	while ( (ch = fgetc(finput)) != EOF) 
		fputc(ch, foutput);
	
	fclose(finput);	// p1 copy done

	/******************** [2] global declaration *************/
	
	fprintf(foutput, "<nta>\n");
	fprintf(foutput, "\t<declaration>\n");
	fprintf(foutput, "clock global_clock;\n\n");
	fprintf(foutput, "const int N = %d; \t// the number of messages\n", N);

	fopen_s(&finput, "lib\\2_global_decl", "r");	// copy p2

	while ((ch = fgetc(finput)) != EOF)
		fputc(ch, foutput);

	fclose(finput);	// p2 copy done

	fprintf(foutput, "const int id[%d] = {", N);	// define id

	for (iter = msg_map.begin(); iter != msg_map.end(); ++iter)
		fprintf(foutput, "%d, ", iter->second.id);

	fseek(foutput, -2, SEEK_CUR); 
	fputs("};\t// in decimal\n", foutput);

	fprintf(foutput, "// in hexadecimal, id[%d] = {", N);	

	for (iter = msg_map.begin(); iter != msg_map.end(); ++iter)
		fprintf(foutput, "0x%x, ", iter->second.id);

	fseek(foutput, -2, SEEK_CUR);
	fputs("};\n\n", foutput);	// id definition done
	
	fprintf(foutput, "const int period[%d] = {", N);	// define period

	for (iter = msg_map.begin(); iter != msg_map.end(); ++iter)
		fprintf(foutput, "%d, ", (iter->second.period * 1000) / 24);
	
	fseek(foutput, -2, SEEK_CUR);
	fputs("};\t// in UPPAAL Clock\n", foutput);

	fprintf(foutput, "// in ms, period[%d] = {", N);

	for (iter = msg_map.begin(); iter != msg_map.end(); ++iter)
		fprintf(foutput, "%d, ", iter->second.period);

	fseek(foutput, -2, SEEK_CUR);
	fputs("};\n\n", foutput);	// period definition done
		
	fprintf(foutput, "const int max_latency[%d] = {", N);	// define max_latency

	for (iter = msg_map.begin(); iter != msg_map.end(); ++iter)
		fprintf(foutput, "%d, ", (iter->second.period * 1000) / 48);

	fseek(foutput, -2, SEEK_CUR);
	fputs("};\t// in UPPAAL Clock\n", foutput);	
	
	fprintf(foutput, "// in ms, max_latency[%d] = {", N);

	for (iter = msg_map.begin(); iter != msg_map.end(); ++iter)
		fprintf(foutput, "%.2f, ", (iter->second.period) / 2.0);

	fseek(foutput, -2, SEEK_CUR);
	fputs("};\n\n", foutput);	// max_latency definition done
	
	fprintf(foutput, "const int Tt[%d] = {", N);	// define transmission_time

	for (iter = msg_map.begin(); iter != msg_map.end(); ++iter)
		fprintf(foutput, "%d, ", (34 + (iter->second.data_len * 8)) / 12);

	fseek(foutput, -2, SEEK_CUR);
	fputs("};\t// in UPPAAL Clock\n", foutput);	
	
	fprintf(foutput, "// in ns, Tt[%d] = {", N);

	for (iter = msg_map.begin(); iter != msg_map.end(); ++iter)
		fprintf(foutput, "%d, ", (34 + (iter->second.data_len * 8)) * 2);

	fseek(foutput, -2, SEEK_CUR);
	fputs("};", foutput);	// transmission_time definition done

	/******************** [3] template definition *****************************/

	fopen_s(&finput, "lib\\3_template_def", "r");	// copy p3

	while ((ch = fgetc(finput)) != EOF)
		fputc(ch, foutput);

	fclose(finput);	// p3 copy done

	/******************** [4] system declaration  ****************************/

	fprintf(foutput, "\t<system>\n");

	for (int i = 0; i < N; i++) {
		fprintf(foutput, "msg%d = Message(%d, id[%d], period[%d]);\n", i, i, i, i);
		fprintf(foutput, "tra%d = Transmit(%d, id[%d], period[%d]);\n\n", i, i, i, i, i);
	}
	
	fprintf(foutput, "system Bus, ");

	for (int i = 0; i < N; i++) {
		fprintf(foutput, "msg%d, ", i);
		fprintf(foutput, "tra%d, ", i);
	}

	fseek(foutput, -2, SEEK_CUR);
	fputs(";</system>\n", foutput);

	/******************** [5] queries  *****************************/

	fopen_s(&finput, "lib\\4_queries", "r");	// copy p4

	while ((ch = fgetc(finput)) != EOF)
		fputc(ch, foutput);

	fclose(finput);	// p4 copy done

	fputs("</nta>", foutput);

	fclose(foutput);
}