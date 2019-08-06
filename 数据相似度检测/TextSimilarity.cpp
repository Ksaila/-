#include"TextSimilarity.h"
#include "cppjieba/Jieba.hpp"
using namespace std;

//1�����Ȼ�ȡͣ�ô�
//2�����ļ��е����½��зִʣ�������ؼ��ֵ���������������
//3���Թؼ��ְ�����Ŀ��������
//4������˳��ѡ�������ļ��еĺ�ѡ��
//5���ú�ѡ�ʵ���Ŀ����������
//6�������������ƶȵĹ�ʽ��������ƶ�


string TextSimilarity::GBKToUTF8(string p)
{
	int len = MultiByteToWideChar(CP_ACP, 0, p.c_str(), -1, NULL, 0);
	wchar_t *uft16 = new wchar_t[len];
	MultiByteToWideChar(CP_ACP, 0, p.c_str(), -1, uft16, len);
	len = WideCharToMultiByte(CP_UTF8, 0, uft16, -1, NULL, 0, NULL, NULL);
	char *a = new char[len];
	WideCharToMultiByte(CP_UTF8, 0, uft16, -1, a, len, NULL, NULL);
	string str(a);
	return str;
}

string TextSimilarity::UTF8ToGBK(string p)
{
	int len = MultiByteToWideChar(CP_UTF8, 0, p.c_str(), -1, NULL, 0);
	wchar_t *uft16 = new wchar_t[len];
	MultiByteToWideChar(CP_UTF8, 0, p.c_str(), -1, uft16, len);
	len = WideCharToMultiByte(CP_ACP, 0, uft16, -1, NULL, 0, NULL, NULL);
	char *a = new char[len];
	WideCharToMultiByte(CP_ACP, 0, uft16, -1, a, len, NULL, NULL);
	string str(a);
	return str;
}

//��ȡͣ�ô�
//1�����ļ�����ȡ�ļ��е�ÿһ�е�ͣ�ô�
//2����ͣ�ôʱ�����set��
void TextSimilarity::getStopWordTable(const char* stopWordFile)
{
	ifstream fin(stopWordFile);
	if (!fin.is_open())
	{
		cout << "open file2 error" << endl;
		return;
	}
	string ptr;
	while (!fin.eof())
	{
		//��Ϊ�ļ���û��ͣ�ô�ռһ�У����Զ�ȡһ��ͣ�ô�
		getline(fin, ptr);
		//����ȡ��ͣ�ôʱ�����set��
		_stopWordSet.insert(ptr);
	}
	fin.close();
}
//���캯��
TextSimilarity::TextSimilarity(string dict)
	:DICT(dict)
	, DICT_PATH(dict + "/jieba.dict.utf8")
	, HMM_PATH(dict + "/hmm_model.utf8")
	, USER_DICT_PATH(dict + "/user.dict.utf8")
	, IDF_PATH(dict + "/idf.utf8")
	, STOP_WORD_PATH(dict + "/stop_words.utf8")
	, _jieba(DICT_PATH, HMM_PATH, USER_DICT_PATH, IDF_PATH, STOP_WORD_PATH)
	, _maxWordNumber(10)
{
	//��ʼ��ͣ�ô�
	getStopWordTable(STOP_WORD_PATH.c_str());
}
//�ȽϺ���
bool cmpREserve(pair<string, int>lp, pair<string, int>rp)
{
	return lp.second > rp.second;
}

//���ļ��еĹؼ��ְ�����Ŀ��������
vector<std::pair<std::string, int>> TextSimilarity::sortByValueReverse(TextSimilarity::wordFreq& wf)
{
	//��Ϊsortֻ�ܶ�����ʽ����������������Խ�map�е����ݱ����ڶ�ά�����н�������
	vector<std::pair<std::string, int>> wfvector(wf.begin(), wf.end());
	sort(wfvector.begin(), wfvector.end(), cmpREserve);
	return wfvector;
}
//�ִʡ��������Ƶ
TextSimilarity::wordFreq TextSimilarity::getWordFreq(const char *file)
{
	ifstream fin(file);
	if (!fin.is_open())
	{
		cout << "open file1 error" << endl;
		return wordFreq();
	}
	string line;
	wordFreq wf;
	while (!fin.eof())
	{
		getline(fin, line);
		//���Ƚ��ļ��ĸ�ʽת��ΪŶUTF8�ĸ�ʽ��Ȼ����зִ�
		line = GBKToUTF8(line);
		vector<string> words;
		//�ִʡ����ؼ��ֱ�����word�ĵ���
		_jieba.Cut(line, words, true);
		for (const auto & e : words)
		{
			//����ǵ�ǰ����ͣ�ôʵĻ���ֱ������
			if (_stopWordSet.count(e))
				continue;
			else
			{
				//�������ؼ����Ѿ�����Ļ�����������Ŀ++
				if (wf.count(e))
					wf[e]++;
				//�������ؼ����ǵ�һ�γ��ֵĻ�����������Ŀ���ó�1
				else
					wf[e] = 1;
			}
		}
	}
	return wf;
}

//ѡ���ѡ��
void TextSimilarity::selectAimWords(std::vector<std::pair<std::string, int>>& wfvec, TextSimilarity::wordSet& wset)
{
	int len = wfvec.size();
	int num = _maxWordNumber;
	//�������ļ��еĹؼ��ֵ�����С�ں�ѡ�ʵĹ涨��Ŀ��ʱ�򣬴�ʱ�����չؼ��ֵ���Ŀ��ѡ���ѡ��
	if (_maxWordNumber > len)
		num = len;
	//����ѡ�ʵ����ֱ�����set��
	for (int i = 0; i < num; i++)
	{
		wset.insert(wfvec[i].first);
	}
}

//��ȡ������
vector<double> TextSimilarity::getOneHot(TextSimilarity::wordSet& wset, TextSimilarity::wordFreq& wf)
{
	vector<double> str;
	//���ݺ�ѡ�ʵ�������ѡ�ʵ���Ŀ��������������
	for (auto & e : wset)
	{
		if (wf.count(e))
		{
			str.push_back(wf[e]);
		}
		else
		{
			str.push_back(0);
		}
	}
	return str;
}

//�������ƶ�
double TextSimilarity::cosine(std::vector<double> oneHot1, std::vector<double> oneHot2)
{
	double ptr = 0.0;
	double deniminator = 0;
	double deniminator2 = 0;
	for (int i = 0; i < oneHot1.size(); i++)
	{
		ptr += (oneHot1[i] * oneHot2[i]);
	}
	for (int i = 0; i < oneHot1.size(); i++)
	{
		deniminator += (oneHot1[i] * oneHot1[i]);
	}
	for (int i = 0; i < oneHot1.size(); i++)
	{
		deniminator2 += (oneHot2[i] * oneHot2[i]);
	}
	//cout << ptr <<" "<< sqrt(deniminator) * sqrt(deniminator2) << endl;
	return (ptr / (sqrt(deniminator) * sqrt(deniminator2)));
}


double TextSimilarity::getTextSimilarity(const char* file1, const char* file2)
{
	TextSimilarity wf("dict");
	std::unordered_map<std::string, int> ptr = wf.getWordFreq(file1);//��ȡ����1�еĴ��Ｐ�����
	std::unordered_map<std::string, int> ptr2 = wf.getWordFreq(file2);//��ȡ����2�еĴ���ļ������
	std::vector<std::pair<std::string, int>> wfvec = wf.sortByValueReverse(ptr);//�����ﰴ����Ŀ��������
	std::vector<std::pair<std::string, int>> wfvec2 = wf.sortByValueReverse(ptr2);
	std::unordered_set<std::string> str;//������ȡ����1������2�еĺ�ѡ��
	wf.selectAimWords(wfvec, str);
	wf.selectAimWords(wfvec2, str);
	std::vector<double> oneHont = wf.getOneHot(str, ptr);//������1���ҳ�str��ÿ���ʳ��ֵĴ���
	std::vector<double> oneHont2 = wf.getOneHot(str, ptr2);//������2���ҳ�strÿ���ʳ��ֵĴ���
	return wf.cosine(oneHont, oneHont2);//�����������ƶȼ�������ƶ�
}