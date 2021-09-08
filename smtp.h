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
	string sendTextContents(int Type, string content);  //type text子类型0为text/plain 1为text/html
	void setMultipartHeaders(string contentType);  //multipart公共头部  或许可通过客户端输入自动判断类型
	void addAttchment(string attchmentPath);  //multipart添加附件
	void addTextPlain(string content);   //multipart添加文本
	void addTextHtml(string html);       //multipart添加html
	void addInline(string content);      //添加内联资源
	void addMultipartContentType(string contentType);     //multipart嵌套添加类型
	string getSubTypeAttachment(string fileName);
	void setSubject(string str) { subject = str; };  //设置主题
	void sendMultipart();   //发送multipart内容
	void vrfy(string name);  //验证邮箱是否存在
	void rset();  //这个命令用来终止邮件事务（mail transaction），任何已经存储的发送者、接收者、邮件数据（mail data）信息都被丢弃，缓存区被清零。
	void quit();
private:
	map<string, string>boundary = { {"multipart/mixed","mailmaster-612de2e5_0_18be"},
		{"multipart/related","mailmaster-612de2e5_0_4823"},{"multipart/alternative","mailmaster-612de2e5_0_29"}};
	string sendContent;   //multipart内容
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

