#include"TextSimilarity.h"
#include"FindFiles.h"
using namespace std;

void Select()
{
	cout << "**********************" << endl;
	cout << "***** 1.�ļ����� *****" << endl;
	cout << "***** 2.�ı����� *****" << endl;
	cout << "**********************" << endl;
}

int main()
{
	TextSimilarity wf("dict");
	//cout << "�������ļ������ƶ�Ϊ" << wf.getTextSimilarity("test.txt", "test2.txt") << endl;
	int n = 0;
	Select();
	cin >> n;
	switch (n)
	{
	case 1:
		Find();
		break;
	case 2:
		cout << "�������ļ������ƶ�Ϊ" << wf.getTextSimilarity("test.txt", "test2.txt") << endl;
		break;
	default:
		break;
	}
	return 0;
}
