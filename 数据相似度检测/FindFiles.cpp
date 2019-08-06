#include"FindFiles.h"

//��һ��Ŀ¼�±������е��ļ���
void find_fold(string mainDir, std::vector<string> &files)
{
	files.clear();
	intptr_t hFile; //win10 need long long or intptr_t, long will show error    
	_finddata_t fileinfo;
	char findDir[250];
	strcpy_s(findDir, mainDir.c_str());
	strcat_s(findDir, "\\*.*");
	if ((hFile = _findfirst(findDir, &fileinfo)) != -1)
	{
		do
		{
			if ((fileinfo.attrib & _A_SUBDIR))//find fold            
			{
				if (fileinfo.name[0] == '.') //avoid . ..         
					continue;
				char filename[_MAX_PATH];
				strcpy_s(filename, mainDir.c_str());
				strcat_s(filename, "\\");
				strcat_s(filename, fileinfo.name);
				string temfilename = filename;
				files.push_back(temfilename);
				//cout << temfilename << endl;
			}
		} while (_findnext(hFile, &fileinfo) == 0);
		_findclose(hFile);
	}
}

//��Ŀ¼�±������е��ļ�
void find_file(const char* mainDir, std::vector<string> &files, string findName)
{
	files.clear();
	intptr_t hFile; //win10 need long long or intptr_t, long will show error   
	_finddata_t fileinfo;
	char findDir[250];
	strcpy_s(findDir, mainDir);
	strcat_s(findDir, "\\*.*");
	if ((hFile = _findfirst(findDir, &fileinfo)) != -1)
	{
		do
		{
			if (!(fileinfo.attrib & _A_SUBDIR))//find fold    
			{
				if (fileinfo.name[0] == '.') //avoid . ..   
					continue;
				char filename[_MAX_PATH];
				strcpy_s(filename, mainDir);
				strcat_s(filename, "\\");
				strcat_s(filename, fileinfo.name);
				string temfilename = filename;
				string sptr(fileinfo.name);
				if (sptr.find(findName, 0) != string::npos)
				{
					cout << temfilename << endl;
				}
				files.push_back(temfilename);
			}
		} while (_findnext(hFile, &fileinfo) == 0);
		_findclose(hFile);
	}
}

void Find(string d, vector<string> &filesSum)
{
	std::vector<string> jpgs;
	find_fold(d.c_str(), jpgs);
	if (jpgs.size() == 0)
		return;
	for (size_t i = 0; i < jpgs.size(); i++)
	{
		filesSum.push_back(jpgs[i]);
		Find(jpgs[i], filesSum);
	}
}

void Find() {
	cout << "����������Ŀ¼(����E:������E:/home)" << endl;
	string path;
	string FilesName;
	cin >> path;
	cout << "����Ҫ�������ļ������֣�������ȫ����Ҳ���������е�һ���֣�" << endl;
	cin >> FilesName;
	cout << "�����Ľ��Ϊ��" << endl;
	std::vector<string> files;
	find_fold(path, files);  //find folder first 
	vector<string> filesSum;
	filesSum = files;
	for (auto d : files)
	{
		Find(d, filesSum);
	}
	for (auto d : filesSum)
	{
		std::vector<string> jpgs;
		find_file(d.c_str(), jpgs, FilesName);
	}
}