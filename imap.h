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
	//链接服务器
	void connectToSever(string severName);
	//登录
	void login(string user, string passWord);
	//获取文件夹列表
	vector<string> list(string base, string temp);
	//上载一份邮件到指定文件夹，例如保存邮件到草稿箱
	void append(string floadName, string mimmessage,int lineCount);      
	//新建文件夹
	void createFolder(string path);
  //删除文件夹 
	void delteFolder(string folderName);
	//文件夹重命名
	void renameFolder(string oldName, string newName);
	//选择将要操作的文件夹
	void select(string folderName);
	//设置邮件标记
	void store(vector<int> id, char active, string flag);   
	//添加文件夹为活动邮箱
	void subscribe(string floadName);       
	//取消活动邮箱
	void unsubscribe(string floadName);
	//获取活动邮箱列表
	vector<string> lsub(string fload, string mailBox); 
	//复制邮件到指定文件夹 移动操作时先将邮件复制然后将原邮件设置删除标志
	void copy(vector<int> id, string toFloaderName);
	//移动邮件支持批量移动
	void moveMailTo(vector<int> id,string toFloader);
	//返回正文包含matchStr邮件序号  
	vector<int> searchFromBody(string matchStr);    
	//从发件人查找
	vector<int> searchFromSender(string sender);
	//查找收件人
	vector<int> seachFromTo(string recUser);
	//特定主题查找
	vector<int> searchFromSubject(string matchStr);
	//获取邮件uid
	vector<string> getuid(string id);
	//获取邮件头以RFC822格式输出
	string getHeaders(int id);
	//获取邮件body 设计邮件标号的操作都可用uid替代
	string getBody(int id);
	//获取附件名
	vector<string> getAttchmentName(int id);
	//获取邮件指定部分内容
	string getBodyPart(string partId);
	//CLOSE命令表示Client结束对当前Folder（文件夹/邮箱）的访问，
	//关闭邮箱该邮箱中所有标志为、DELETED的邮件就被从物理上删除
	void close();  
	//EXPUNGE命令在不关闭邮箱的情况下删除所有的标志为、DELETED的邮件
	void expunge();   
	//不进行操作防止长时间没有操作与服务器断开
	void check();
	void noop();
	//退出账号
	void logout();
	~imap();
private:
	bool strStartWith(string str, string start);
	vector<string> splitStr(string str, string delim);
	int msgId = 1001;
	SOCKET client;
	char buff[5000];
};

