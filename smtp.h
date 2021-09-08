#include <WinSock2.h>
#include <string>
#include <bitset>
#include <iostream>
#include <vector>
#include <fstream>
#include <map>
using namespace std;
#pragma comment(lib, "ws2_32.lib")
#define port 25

class smtp
{
public:
	smtp();
	void connectToSever(string severName);
	void authLogin(string user, string passWord);
	void sendMailFromTo(string from, vector<string> to);
	string readFileAsByte(string fileName);
	string sendTextContents(int Type, string content);  //type text������0Ϊtext/plain 1Ϊtext/html
	void setMultipartHeaders(string contentType);  //multipart����ͷ��  �����ͨ���ͻ��������Զ��ж�����
	void addAttchment(string attchmentPath);  //multipart��Ӹ���
	void addTextPlain(string content);   //multipart����ı�
	void addTextHtml(string html);       //multipart���html
	void addInline(string content);      //���������Դ
	void addMultipartContentType(string contentType);     //multipartǶ���������
	string getSubTypeAttachment(string fileName);
	void setSubject(string str) { subject = str; };  //��������
	void sendMultipart();   //����multipart����
	void vrfy(string name);  //��֤�����Ƿ����
	void rset();  //�������������ֹ�ʼ�����mail transaction�����κ��Ѿ��洢�ķ����ߡ������ߡ��ʼ����ݣ�mail data����Ϣ���������������������㡣
	void quit();
private:
	map<string, string>boundary = { {"multipart/mixed","mailmaster-612de2e5_0_18be"},
		{"multipart/related","mailmaster-612de2e5_0_4823"},{"multipart/alternative","mailmaster-612de2e5_0_29"}};
	string sendContent;   //multipart����
	vector<std::string> split(std::string str, std::string delim);
	string subject="";
	string from;
	vector<string> to;
	bool isContainInline = false;
	bool isContainAttachment = false;
	string toBase64(string str);
	string toQuotedPrintable(string str);
	SOCKET client;
	char buff[5000];
};

