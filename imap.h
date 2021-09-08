#include <WinSock2.h>
#include <iostream>
#include <string>
#include <vector>
#include <map>
using namespace std;
#pragma comment(lib, "ws2_32.lib")
#define imapport 143
#pragma once

class imap
{
public:
	map<int, string> storeFlages = { {0,"\\Seen"},{1,"\\Answered"},{2,"\\Flagged"},{3,"\\Deleted"},{4,"\\Draft"},{5,"\\Recent"} };
	imap();
	//���ӷ�����
	void connectToSever(string severName);
	//��¼
	void login(string user, string passWord);
	//��ȡ�ļ����б�
	vector<string> list(string base, string temp);
	//����һ���ʼ���ָ���ļ��У����籣���ʼ����ݸ���
	void append(string floadName, string mimmessage,int lineCount);      
	//�½��ļ���
	void createFolder(string path);
  //ɾ���ļ��� 
	void delteFolder(string folderName);
	//�ļ���������
	void renameFolder(string oldName, string newName);
	//ѡ��Ҫ�������ļ���
	void select(string folderName);
	//�����ʼ����
	void store(vector<int> id, char active, string flag);   
	//����ļ���Ϊ�����
	void subscribe(string floadName);       
	//ȡ�������
	void unsubscribe(string floadName);
	//��ȡ������б�
	vector<string> lsub(string fload, string mailBox); 
	//�����ʼ���ָ���ļ��� �ƶ�����ʱ�Ƚ��ʼ�����Ȼ��ԭ�ʼ�����ɾ����־
	void copy(vector<int> id, string toFloaderName);
	//�ƶ��ʼ�֧�������ƶ�
	void moveMailTo(vector<int> id,string toFloader);
	//�������İ���matchStr�ʼ����  
	vector<int> searchFromBody(string matchStr);    
	//�ӷ����˲���
	vector<int> searchFromSender(string sender);
	//�����ռ���
	vector<int> seachFromTo(string recUser);
	//�ض��������
	vector<int> searchFromSubject(string matchStr);
	//��ȡ�ʼ�uid
	vector<string> getuid(string id);
	//��ȡ�ʼ�ͷ��RFC822��ʽ���
	string getHeaders(int id);
	//��ȡ�ʼ�body ����ʼ���ŵĲ���������uid���
	string getBody(int id);
	//��ȡ������
	vector<string> getAttchmentName(int id);
	//��ȡ�ʼ�ָ����������
	string getBodyPart(string partId);
	//CLOSE�����ʾClient�����Ե�ǰFolder���ļ���/���䣩�ķ��ʣ�
	//�ر���������������б�־Ϊ��DELETED���ʼ��ͱ���������ɾ��
	void close();  
	//EXPUNGE�����ڲ��ر�����������ɾ�����еı�־Ϊ��DELETED���ʼ�
	void expunge();   
	//�����в�����ֹ��ʱ��û�в�����������Ͽ�
	void check();
	void noop();
	//�˳��˺�
	void logout();
	~imap();
private:
	bool strStartWith(string str, string start);
	vector<string> splitStr(string str, string delim);
	int msgId = 1001;
	SOCKET client;
	char buff[5000];
};

